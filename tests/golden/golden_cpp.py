#!/usr/bin/env python3
"""C++ golden-master driver (Phase 0 safety net) — the BGS_Factory counterpart of
golden_test.py.

It drives the ``golden_cpp`` emitter (built from tests/golden/cpp) once per algorithm — a
fresh process per algorithm, mirroring the Python subprocess isolation: clean global
``rand()`` (``srand(1)``) and a fresh ``./config`` (we launch it in a temp cwd), and a hard
crash in one algorithm's teardown can't take the others down. It compares per-frame
foreground-mask hashes against ``goldens/<cell>-cpp.json``.

Because ``golden_cpp`` emits the SAME token format as run_algorithm.py, ``--check`` also runs
a **cross-check** against the Python golden (``goldens/<cell>.json``) for the algorithms common
to both builds — proving the wrapper and the core produce byte-identical masks.

    python tests/golden/golden_cpp.py --check    [--opencv 4.13] [--binary PATH]
    python tests/golden/golden_cpp.py --generate  --opencv 4.13  [--binary PATH]

Shared logic (cell key, IMBS exclusion, hash-sequence comparison) is imported from
golden_test.py so the two levels stay in lockstep.
"""
import argparse
import glob
import json
import os
import re
import subprocess
import sys
import tempfile

import golden_test as gt

DEFAULT_BIN = os.path.join(gt.HERE, "cpp", "build", "golden_cpp")
NAME_RE = re.compile(r"^[A-Za-z][A-Za-z0-9_]*$")  # filter stray algorithm stdout from `list`


def resolve_binary(path):
    for cand in (path, path + ".exe"):
        if os.path.exists(cand):
            return cand
    raise SystemExit(f"golden_cpp binary not found at '{path}'. Build it first:\n"
                     f"  cmake -S tests/golden/cpp -B tests/golden/cpp/build -G Ninja\n"
                     f"  cmake --build tests/golden/cpp/build")


def list_algorithms(binpath):
    out = subprocess.run([binpath, "list"], capture_output=True, text=True)
    names = [ln.strip() for ln in out.stdout.splitlines() if NAME_RE.match(ln.strip())]
    return sorted(set(names))


def run_one(binpath, name):
    """Run a single algorithm in a fresh process + temp cwd; return its result dict."""
    with tempfile.TemporaryDirectory(prefix=f"golden_cpp_{name}_") as cwd:
        out = os.path.join(cwd, "result.json")
        proc = subprocess.run([binpath, "run", name, gt.FRAMES_DIR, out],
                              cwd=cwd, capture_output=True, text=True)
        if not os.path.exists(out):
            return {"algorithm": name, "status": "crash",
                    "returncode": proc.returncode, "stderr": proc.stderr[-500:]}
        with open(out) as fh:
            result = json.load(fh)
        result.setdefault("returncode", proc.returncode)  # nonzero after a valid result = teardown crash
        return result


def cpp_golden_path(opencv):
    return os.path.join(gt.GOLDENS_DIR, gt.cell_key(opencv) + "-cpp.json")


def _resolve_golden(opencv):
    if opencv:
        return cpp_golden_path(opencv)
    matches = glob.glob(os.path.join(gt.GOLDENS_DIR, gt.platform_tag() + "-opencv*-cpp.json"))
    if len(matches) == 1:
        return matches[0]
    raise SystemExit(f"--opencv not given and {len(matches)} C++ goldens match platform "
                     f"'{gt.platform_tag()}'. Specify --opencv (or BGS_GOLDEN_OPENCV).")


def generate(opencv, binpath):
    key = gt.cell_key(opencv)
    names = list_algorithms(binpath)
    print(f"[generate:cpp] cell={key}  algorithms={len(names)}")
    os.makedirs(gt.GOLDENS_DIR, exist_ok=True)
    entries = {}
    for name in names:
        if name in gt.KNOWN_NONDETERMINISTIC:
            entries[name] = {"status": "nondeterministic", "reason": gt.KNOWN_NONDETERMINISTIC[name]}
            print(f"  - {name}: nondeterministic (excluded; {gt.KNOWN_NONDETERMINISTIC[name]})")
            continue
        r1 = run_one(binpath, name)
        s1 = gt._seq(r1)
        if s1 is None:
            entries[name] = {"status": r1.get("status", "error"),
                             "error": r1.get("error") or r1.get("stderr")}
            print(f"  - {name}: {entries[name]['status']} (excluded)")
            continue
        s2 = gt._seq(run_one(binpath, name))
        if s2 != s1:
            entries[name] = {"status": "nondeterministic"}
            print(f"  - {name}: nondeterministic (excluded from exact-match)")
            continue
        entries[name] = {"status": "ok", "frames": r1["frames"]}
        print(f"  - {name}: ok ({len(r1['frames'])} frames)")
    payload = {"cell": key, "level": "cpp", "frames_dir": "dataset/frames", "algorithms": entries}
    path = cpp_golden_path(opencv)
    with open(path, "w") as fh:
        json.dump(payload, fh, indent=2, sort_keys=True)
        fh.write("\n")
    n_ok = sum(1 for e in entries.values() if e["status"] == "ok")
    print(f"[generate:cpp] wrote {os.path.relpath(path, gt.REPO_ROOT)}  ({n_ok} deterministic / {len(entries)} total)")
    return 0


def cross_check(opencv, cpp_golden):
    """Compare the C++ golden against the Python golden for algorithms common to both."""
    py_path = os.path.join(gt.GOLDENS_DIR, gt.cell_key(opencv) + ".json") if opencv else None
    if not py_path or not os.path.exists(py_path):
        print("[cross-check] no matching Python golden — skipped")
        return
    with open(py_path) as f_py, open(cpp_golden) as f_cpp:
        py = json.load(f_py)["algorithms"]
        cpp = json.load(f_cpp)["algorithms"]
    shared = sorted(set(py) & set(cpp))
    agree = differ = 0
    for name in shared:
        if py[name].get("status") != "ok" or cpp[name].get("status") != "ok":
            continue
        if py[name]["frames"] == cpp[name]["frames"]:
            agree += 1
        else:
            differ += 1
            first = next((c.split()[0] for c, r in zip(cpp[name]["frames"], py[name]["frames"]) if c != r),
                         "length differs")
            print(f"  cross DIFF {name}: C++ vs Python mask differs (first at {first})")
    print(f"[cross-check] vs Python golden: {agree} identical, {differ} differing "
          f"(of {len(shared)} shared)")


def check(opencv, binpath):
    path = _resolve_golden(opencv)
    if not os.path.exists(path):
        raise SystemExit(f"No C++ golden for this cell: {os.path.relpath(path, gt.REPO_ROOT)}\n"
                         f"Generate it with: python {os.path.relpath(__file__, gt.REPO_ROOT)} "
                         f"--generate --opencv <ver>")
    golden = json.load(open(path))
    print(f"[check:cpp] golden={os.path.relpath(path, gt.REPO_ROOT)}  cell={golden['cell']}")
    g_algos = golden["algorithms"]
    present = set(list_algorithms(binpath))
    failures, skipped, new = [], [], sorted(present - set(g_algos))

    for name in sorted(g_algos):
        g = g_algos[name]
        if name in gt.KNOWN_NONDETERMINISTIC:  # excluded everywhere, even if the baseline says "ok"
            skipped.append((name, "nondeterministic"))
            continue
        if g["status"] != "ok":
            skipped.append((name, g["status"]))
            continue
        if name not in present:
            failures.append((name, "missing from this build"))
            continue
        cur = gt._seq(run_one(binpath, name))
        if cur is None:
            failures.append((name, "did not produce output (status changed)"))
            continue
        if cur == g["frames"]:
            print(f"  ok   {name}")
        else:
            first = next((c.split()[0] for c, r in zip(cur, g["frames"]) if c != r),
                         "length/order differs")
            failures.append((name, f"output changed (first diff at frame {first})"))

    for name, status in skipped:
        print(f"  skip {name} ({status})")
    for name in new:
        print(f"  NEW  {name} (not in golden — regenerate to include it)")
    for name, why in failures:
        print(f"  FAIL {name}: {why}")
    print(f"[check:cpp] {len(g_algos) - len(skipped) - len(failures)} ok, "
          f"{len(skipped)} skipped, {len(new)} new, {len(failures)} failed")

    cross_check(opencv, path)
    return 1 if failures else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument("--generate", action="store_true", help="(re)generate the C++ golden for this cell")
    mode.add_argument("--check", action="store_true", help="compare this build vs the C++ golden (default)")
    ap.add_argument("--opencv", default=os.environ.get("BGS_GOLDEN_OPENCV"),
                    help="OpenCV major.minor the core was compiled against, e.g. 4.13")
    ap.add_argument("--binary", default=DEFAULT_BIN, help=f"path to golden_cpp (default: {DEFAULT_BIN})")
    args = ap.parse_args()

    binpath = resolve_binary(args.binary)
    if args.generate:
        if not args.opencv:
            raise SystemExit("--generate requires --opencv (or BGS_GOLDEN_OPENCV), e.g. --opencv 4.13")
        return generate(args.opencv, binpath)
    return check(args.opencv, binpath)


if __name__ == "__main__":
    sys.exit(main())
