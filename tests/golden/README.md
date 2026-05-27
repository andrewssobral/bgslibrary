# Golden-master regression tests (Phase 0 safety net)

These tests pin the **per-frame foreground-mask output of every algorithm** so that any
refactor, modernization, or OpenCV bump that *changes algorithm behaviour* is caught
immediately. They are the prerequisite for the riskier modernization work (porting the
`DP*`/`T2F*` family to OpenCV 4, fixing warnings, consolidating the wrapper registries —
see `MODERNIZATION_ROADMAP.md`).

It runs at **two levels** over the same frames and the same token format:
- **Python (`pybgs`) wrapper** — `golden_test.py` (+ `run_algorithm.py`), goldens `goldens/<cell>.json`.
- **C++ core via `BGS_Factory`** — `golden_cpp.py` driving the `cpp/golden_cpp` emitter, goldens
  `goldens/<cell>-cpp.json`. Because the tokens match byte-for-byte, the C++ `--check` also
  **cross-checks** its masks against the Python golden for the algorithms common to both.

## What it does

For each registered algorithm, the harness runs it over the fixed frames in
[`dataset/frames`](../../dataset/frames) (51 PNGs, 320×240 — PNG decoding is deterministic
across OpenCV backends, unlike AVI), hashes each foreground mask, and compares the
sequence against a committed reference. `--check` exits non-zero (CI-failing) if any
deterministic algorithm's output differs, naming the first frame that diverged.

```bash
# from the repo root, in an environment where `import pybgs` works (e.g. the pixi env):
# --- Python level ---
python tests/golden/golden_test.py --check    --opencv 4.13      # compare (default)
python tests/golden/golden_test.py --generate --opencv 4.13      # (re)create the golden

# --- C++ level (build the emitter once, then drive it) ---
cmake -S tests/golden/cpp -B tests/golden/cpp/build -G Ninja
cmake --build tests/golden/cpp/build
python tests/golden/golden_cpp.py  --check    --opencv 4.13      # compare + cross-check vs Python
python tests/golden/golden_cpp.py  --generate --opencv 4.13      # (re)create the C++ golden
```

The C++ emitter runs **one algorithm per process** (driven by `golden_cpp.py`), mirroring the
Python subprocess isolation — so `rand()`/config are clean per algorithm and a teardown crash in
one (e.g. VuMeter) can't take the others down.

### Notes on the two levels

- The **C++ factory exposes `MyBGS`** (the `_template_` skeleton), which the Python wrapper does
  not bind — so the C++ golden has one extra algorithm; it is simply absent from the cross-check.
- The cross-check on `linux-64/OpenCV4.13` finds **24/25 shared algorithms byte-identical**; **KDE**
  differs. That is expected build-sensitivity (KDE is float-heavy + threshold-based, so FP rounding
  differs between the separately-configured `pybgs` and harness builds) — which is exactly why each
  level keeps its **own** golden and the cross-check is informational, not a gate.

## Design notes / determinism

- **Exact-match, per `(OS, arch, OpenCV)` cell.** The core never seeds its RNG, so C
  `rand()` and PBAS's `cv::RNG` are deterministic *run-to-run* — but the `rand()`
  sequence is platform-specific (glibc / macOS / MSVC differ), so each cell gets its own
  reference under [`goldens/`](goldens), named e.g. `linux-64-opencv4.13.json`.
- **One subprocess per algorithm.** The global C `rand()` is shared, so running
  algorithms in one process would make each one's output order-dependent. `run_algorithm.py`
  runs a single algorithm in a fresh process (clean `rand()` default seed) and a temp cwd
  (so `./config/<Name>.xml` is recreated with the algorithm's *default* parameters — the
  golden is independent of your local `config/`).
- **Nondeterministic algorithms are auto-detected.** `--generate` runs each algorithm
  twice; if the two clean runs disagree it is recorded as `nondeterministic` and excluded
  from exact-match (with no hardcoding).
- **Some are force-excluded** via `KNOWN_NONDETERMINISTIC` (excluded everywhere; `--check`
  skips them even if a baseline marks them `ok`), because two fast runs on one machine can
  coincidentally agree yet differ elsewhere: `IndependentMultimodal`/IMBS (wall-clock timing
  at `fps == 0`), and `KDE` + `SigmaDelta` (float/threshold output that FP rounding flips
  across CPUs — caught in CI when a dev-box baseline was checked on a different runner;
  `SigmaDelta`'s frame-1 diff also points to a likely uninitialized read).
- **Teardown-crash safe.** Results are flushed/fsync'd before the algorithm is destroyed,
  so the known VuMeter `munmap_chunk()` crash at interpreter exit does not lose data; the
  driver tolerates a nonzero exit once a valid result file exists.

## Why `--opencv` is passed explicitly

The OpenCV version `pybgs` was *compiled* against is not discoverable from Python:
`cv2.__version__` is the `opencv-python` version, which is **decoupled** from the C++
OpenCV that decides the available algorithm set (26 on OpenCV 4 vs ~39 on OpenCV 2/3).
In CI the version is known from the matrix cell; locally, pass `--opencv` (or set
`BGS_GOLDEN_OPENCV`). On `--check` it may be omitted if exactly one golden matches the
current platform.

## Regenerating after an *intended* output change

If a change is *meant* to alter an algorithm's output, regenerate the affected cell's
golden (`--generate`) and review the JSON diff in the PR so the behaviour change is
explicit and reviewed — never edit the golden by hand.
