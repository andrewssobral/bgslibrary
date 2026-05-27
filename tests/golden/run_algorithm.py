#!/usr/bin/env python3
"""Run ONE BGSLibrary algorithm over the fixed golden frames and emit per-frame
foreground-mask hashes.

Run in its own fresh subprocess by ``golden_test.py`` so that:
  * the shared global C ``rand()`` starts from its default seed (no algorithm run
    before this one has perturbed it), and
  * the current working directory is a throwaway temp dir, so ``./config/<Name>.xml``
    is recreated with the algorithm's *default* parameters (goldens are independent of
    the developer's local config).

Results are written to ``--out`` and flushed/fsync'd *before* the algorithm object is
destroyed, so a crash in a destructor at interpreter shutdown (e.g. the known VuMeter
``munmap_chunk(): invalid pointer``) does not lose already-computed hashes.
"""
import argparse
import hashlib
import json
import os
import sys


def _frames(frames_dir):
    """PNG frames sorted numerically by stem (1, 2, ... 10, 11 — not lexically)."""
    names = [f for f in os.listdir(frames_dir)
             if f.lower().endswith(".png") and os.path.splitext(f)[0].isdigit()]
    return sorted(names, key=lambda f: int(os.path.splitext(f)[0]))


def _write(out_path, payload):
    with open(out_path, "w") as fh:
        json.dump(payload, fh)
        fh.flush()
        os.fsync(fh.fileno())


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--algorithm", required=True, help="pybgs class name, e.g. FrameDifference")
    ap.add_argument("--frames-dir", required=True, help="absolute path to dataset/frames")
    ap.add_argument("--out", required=True, help="absolute path of the JSON result file")
    args = ap.parse_args()

    import numpy as np
    import cv2
    import pybgs as bgs

    frames = _frames(args.frames_dir)
    if not frames:
        _write(args.out, {"algorithm": args.algorithm, "status": "no-frames"})
        return 2

    cls = getattr(bgs, args.algorithm, None)
    if not isinstance(cls, type):
        _write(args.out, {"algorithm": args.algorithm, "status": "not-found"})
        return 3

    try:
        algo = cls()
    except Exception as exc:  # constructor failed (should not happen for real algos)
        _write(args.out, {"algorithm": args.algorithm, "status": "init-error", "error": repr(exc)})
        return 4

    if not hasattr(algo, "apply"):
        _write(args.out, {"algorithm": args.algorithm, "status": "no-apply"})
        return 5

    per_frame = []
    try:
        for fn in frames:
            img = cv2.imread(os.path.join(args.frames_dir, fn), cv2.IMREAD_COLOR)
            fg = np.ascontiguousarray(algo.apply(img))
            sha = hashlib.sha256(fg.tobytes()).hexdigest()[:16]
            # One compact, self-describing token per frame: "<frame> <WxHxC> <dtype> <sha>".
            # Keeps goldens small + diff-friendly while still tolerating per-frame shape
            # changes (some algorithms emit a 1-channel mask before the model warms up).
            dims = "x".join(str(d) for d in fg.shape)
            per_frame.append(f"{fn} {dims} {fg.dtype} {sha}")
    except Exception as exc:
        _write(args.out, {"algorithm": args.algorithm, "status": "apply-error",
                          "error": repr(exc), "frames": per_frame})
        return 6

    _write(args.out, {"algorithm": args.algorithm, "status": "ok", "frames": per_frame})
    # The result is on disk and fsync'd. A teardown crash from here on is harmless.
    return 0


if __name__ == "__main__":
    sys.exit(main())
