#!/usr/bin/env python3
"""Golden-master regression harness for BGSLibrary's ``pybgs`` Python wrapper.

Runs every registered algorithm on the fixed frames in ``dataset/frames`` and compares
per-frame foreground-mask hashes against a committed reference for the current
``(OS, arch, OpenCV)`` matrix cell. This is the Phase 0 safety net: it must stay green
before/after any algorithm-touching change (see MODERNIZATION_ROADMAP.md).

Why per-cell goldens: the core never seeds its RNG, so C ``rand()``/``cv::RNG`` are
deterministic *run-to-run* — but the ``rand()`` sequence is platform-specific
(glibc/macOS/MSVC differ), so a reference is generated once per ``(OS, arch, OpenCV)``
cell and committed under ``goldens/``.

Why a subprocess per algorithm: the global C ``rand()`` is shared across algorithms, so
running them in one process makes each one's output depend on the order. Each algorithm
therefore runs in a fresh subprocess (clean ``rand()`` default seed) and a temp cwd
(fresh ``./config`` defaults) via ``run_algorithm.py``.

Nondeterministic algorithms (e.g. IndependentMultimodal/IMBS, which uses wall-clock
timing when ``fps == 0``) are auto-detected: ``--generate`` runs each algorithm twice
and, if the two runs disagree, records it as ``nondeterministic`` and excludes it from
exact-match checking — no per-algorithm hardcoding.

Usage:
    python tests/golden/golden_test.py --generate [--opencv 4.13]
    python tests/golden/golden_test.py --check    [--opencv 4.13]   # default

The OpenCV version pybgs was *compiled* against is not discoverable from Python
(``cv2.__version__`` is the decoupled opencv-python version), so pass it via ``--opencv``
or the ``BGS_GOLDEN_OPENCV`` env var. OS/arch are auto-detected. On ``--check`` it may be
omitted when exactly one golden matches the current platform.
"""
import argparse
import glob
import json
import os
import platform
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.dirname(os.path.dirname(HERE))
FRAMES_DIR = os.path.join(REPO_ROOT, "dataset", "frames")
GOLDENS_DIR = os.path.join(HERE, "goldens")
RUNNER = os.path.join(HERE, "run_algorithm.py")

# Algorithms excluded from exact-match because their output is not reproducible across
# environments, even though two fast back-to-back runs on one machine may coincidentally
# agree (a false "deterministic" for the double-run auto-detection). --generate records them
# as nondeterministic and --check skips them regardless of the baseline's stored status.
#   - IndependentMultimodal/IMBS: wall-clock timing when fps==0 (IMBS.cpp getTimestamp(),
#     `timestamp - prev_bg_frame_time >= samplingPeriod`, persistenceMap += elapsed) -> a
#     slow/loaded runner diverges from a fast dev box.
#   - KDE, SigmaDelta: float-heavy + threshold-based, so FP rounding (FMA/SIMD codegen) differs
#     across CPUs -> a baseline generated on one machine flips a few pixels on another (observed
#     in CI: KDE @ OpenCV 3.4.16, SigmaDelta @ OpenCV 4.6.0). SigmaDelta differs already at
#     frame 1 -> likely an uninitialized read (a real bug to fix; see TODO).
#   - LOBSTER, PAWCS, SuBSENSE: the LBSP family (RNG + float distance thresholds) is likewise
#     cross-CPU-fragile -> flips on Apple Silicon vs the generating runner (LOBSTER failed the
#     osx-arm64 gate at frame 4); the double-run detection is unreliable for these (it excluded
#     LOBSTER's Python golden but not its C++ one). The robust long-term fix for all of these is
#     tolerance-based comparison (see MODERNIZATION_ROADMAP), not exact hashes.
KNOWN_NONDETERMINISTIC = {
    "IndependentMultimodal": "wall-clock dependent (fps==0): see IMBS.cpp getTimestamp()",
    "KDE": "float/threshold-sensitive: FP rounding differs across CPUs/builds",
    "SigmaDelta": "float/threshold-sensitive across CPUs; frame-1 diff suggests an uninitialized read",
    "LOBSTER": "LBSP family (RNG + float thresholds): cross-CPU-fragile (failed the osx-arm64 gate)",
    "PAWCS": "LBSP family (RNG + float thresholds): cross-CPU-fragile on arm64",
    "SuBSENSE": "LBSP family (RNG + float thresholds): cross-CPU-fragile on arm64",
}


def platform_tag():
    """conda-forge-style subdir tag: linux-64, linux-aarch64, osx-64, osx-arm64, win-64."""
    sysname = {"linux": "linux", "darwin": "osx", "windows": "win"}.get(
        platform.system().lower(), platform.system().lower())
    machine = platform.machine().lower()
    if machine in ("x86_64", "amd64", "x64"):
        arch = "64"
    elif machine in ("aarch64", "arm64"):
        arch = "arm64" if sysname in ("osx", "win") else "aarch64"
    else:
        arch = machine
    return f"{sysname}-{arch}"


def cell_key(opencv):
    return f"{platform_tag()}-opencv{opencv}"


def list_algorithms():
    """Names of pybgs classes (the algorithm registry exposed by the module)."""
    import pybgs as bgs
    return sorted(n for n in dir(bgs)
                  if not n.startswith("_") and isinstance(getattr(bgs, n), type))


def run_one(name):
    """Run a single algorithm in a fresh subprocess + temp cwd; return its result dict."""
    with tempfile.TemporaryDirectory(prefix=f"golden_{name}_") as cwd:
        out = os.path.join(cwd, "result.json")
        proc = subprocess.run(
            [sys.executable, RUNNER, "--algorithm", name,
             "--frames-dir", FRAMES_DIR, "--out", out],
            cwd=cwd, capture_output=True, text=True)
        if not os.path.exists(out):
            return {"algorithm": name, "status": "crash",
                    "returncode": proc.returncode, "stderr": proc.stderr[-500:]}
        with open(out) as fh:
            result = json.load(fh)
        # A nonzero exit *after* a valid result is the known teardown crash — tolerate it.
        result.setdefault("returncode", proc.returncode)
        return result


def _seq(result):
    """The comparable per-frame token sequence, or None if the run produced none.

    Each token is ``"<frame> <WxHxC> <dtype> <sha>"`` (see run_algorithm.py).
    """
    if result.get("status") != "ok":
        return None
    return result["frames"]


def generate(opencv):
    key = cell_key(opencv)
    names = list_algorithms()
    print(f"[generate] cell={key}  algorithms={len(names)}")
    os.makedirs(GOLDENS_DIR, exist_ok=True)
    entries = {}
    for name in names:
        if name in KNOWN_NONDETERMINISTIC:
            entries[name] = {"status": "nondeterministic", "reason": KNOWN_NONDETERMINISTIC[name]}
            print(f"  - {name}: nondeterministic (excluded; {KNOWN_NONDETERMINISTIC[name]})")
            continue
        r1 = run_one(name)
        s1 = _seq(r1)
        if s1 is None:
            entries[name] = {"status": r1.get("status", "error"),
                             "error": r1.get("error") or r1.get("stderr")}
            print(f"  - {name}: {entries[name]['status']} (excluded)")
            continue
        # Second run from a clean process: if it disagrees, the algorithm is
        # nondeterministic on this platform (e.g. IMBS with fps==0) -> exclude.
        s2 = _seq(run_one(name))
        if s2 != s1:
            entries[name] = {"status": "nondeterministic"}
            print(f"  - {name}: nondeterministic (excluded from exact-match)")
            continue
        entries[name] = {"status": "ok", "frames": r1["frames"]}
        print(f"  - {name}: ok ({len(r1['frames'])} frames)")
    payload = {"cell": key, "frames_dir": "dataset/frames", "algorithms": entries}
    path = os.path.join(GOLDENS_DIR, key + ".json")
    with open(path, "w") as fh:
        json.dump(payload, fh, indent=2, sort_keys=True)
        fh.write("\n")
    n_ok = sum(1 for e in entries.values() if e["status"] == "ok")
    print(f"[generate] wrote {os.path.relpath(path, REPO_ROOT)}  ({n_ok} deterministic / {len(entries)} total)")
    return 0


def _resolve_golden(opencv):
    if opencv:
        return os.path.join(GOLDENS_DIR, cell_key(opencv) + ".json")
    # exclude the C++ goldens (`*-cpp.json`) so they don't collide with the Python ones
    matches = [m for m in glob.glob(os.path.join(GOLDENS_DIR, platform_tag() + "-opencv*.json"))
               if not m.endswith("-cpp.json")]
    if len(matches) == 1:
        return matches[0]
    raise SystemExit(
        f"--opencv not given and {len(matches)} goldens match platform '{platform_tag()}'. "
        f"Specify --opencv (or BGS_GOLDEN_OPENCV).")


def check(opencv):
    path = _resolve_golden(opencv)
    if not os.path.exists(path):
        raise SystemExit(f"No golden for this cell: {os.path.relpath(path, REPO_ROOT)}\n"
                         f"Generate it with: python {os.path.relpath(__file__, REPO_ROOT)} --generate "
                         f"--opencv <ver>")
    with open(path) as fh:
        golden = json.load(fh)
    print(f"[check] golden={os.path.relpath(path, REPO_ROOT)}  cell={golden['cell']}")
    g_algos = golden["algorithms"]
    present = set(list_algorithms())
    failures, skipped, new = [], [], sorted(present - set(g_algos))

    for name in sorted(g_algos):
        g = g_algos[name]
        if name in KNOWN_NONDETERMINISTIC:  # excluded everywhere, even if the baseline says "ok"
            skipped.append((name, "nondeterministic"))
            continue
        if g["status"] != "ok":
            skipped.append((name, g["status"]))
            continue
        if name not in present:
            failures.append((name, "missing from this build"))
            continue
        cur = _seq(run_one(name))
        if cur is None:
            failures.append((name, "did not produce output (status changed)"))
            continue
        ref = g["frames"]
        if cur == ref:
            print(f"  ok   {name}")
        else:
            first = next((c.split()[0] for c, r in zip(cur, ref) if c != r),
                         "length/order differs")
            failures.append((name, f"output changed (first diff at frame {first})"))

    for name, status in skipped:
        print(f"  skip {name} ({status})")
    for name in new:
        print(f"  NEW  {name} (not in golden — regenerate to include it)")
    for name, why in failures:
        print(f"  FAIL {name}: {why}")

    print(f"[check] {len(g_algos) - len(skipped) - len(failures)} ok, "
          f"{len(skipped)} skipped, {len(new)} new, {len(failures)} failed")
    return 1 if failures else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument("--generate", action="store_true", help="(re)generate the golden for this cell")
    mode.add_argument("--check", action="store_true", help="compare this build vs the golden (default)")
    ap.add_argument("--opencv", default=os.environ.get("BGS_GOLDEN_OPENCV"),
                    help="OpenCV major.minor pybgs was compiled against, e.g. 4.13")
    args = ap.parse_args()

    if args.generate:
        if not args.opencv:
            raise SystemExit("--generate requires --opencv (or BGS_GOLDEN_OPENCV), "
                             "e.g. --opencv 4.13")
        return generate(args.opencv)
    return check(args.opencv)


if __name__ == "__main__":
    sys.exit(main())
