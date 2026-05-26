# Golden-master regression tests (Phase 0 safety net)

These tests pin the **per-frame foreground-mask output of every algorithm** so that any
refactor, modernization, or OpenCV bump that *changes algorithm behaviour* is caught
immediately. They are the prerequisite for the riskier modernization work (porting the
`DP*`/`T2F*` family to OpenCV 4, fixing warnings, consolidating the wrapper registries —
see `MODERNIZATION_ROADMAP.md`).

This directory currently covers the **Python (`pybgs`)** level. A C++ (`BGS_Factory`)
harness using the same frames is planned next.

## What it does

For each algorithm registered in `pybgs`, the harness runs it over the fixed frames in
[`dataset/frames`](../../dataset/frames) (51 PNGs, 320×240 — PNG decoding is deterministic
across OpenCV backends, unlike AVI), hashes each foreground mask, and compares the
sequence against a committed reference.

```bash
# from the repo root, in an environment where `import pybgs` works (e.g. the pixi env):
python tests/golden/golden_test.py --check    --opencv 4.13      # compare (default)
python tests/golden/golden_test.py --generate --opencv 4.13      # (re)create the golden
```

`--check` exits non-zero (CI-failing) if any deterministic algorithm's output differs
from its golden, naming the first frame that diverged.

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
  from exact-match (this is how `IndependentMultimodal`/IMBS — wall-clock timing when
  `fps == 0` — is handled, with no hardcoding).
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
