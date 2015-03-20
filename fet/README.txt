FET - FOREGROUND EVALUATION TOOL

Implemented in Python 2.7

Available measurements
----------------------

TP - True positive:  Total number of true positive pixels.
FP - False positive: Total number of false positive pixels.
TN - True negative:  Total number of true negative pixels.
FN - False negative: Total number of false negative pixels.

Precision: Number of true positive pixels / (number of true positive pixels + number of false positive pixels).
Recall:    Number of true positive pixels / (number of true positive pixels + number of false negative pixels).
F-factor:  2 × (precision × recall)/(precision + recall). *** Also known as F1 score, F-score or F-measure.

Description
-----------
# TP - # of foreground pixels classified as foreground pixels.
# FP - # of background pixels classified as foreground pixels.
# TN - # of background pixels classified as background pixels.
# FN - # of foreground pixels classified as background pixels.
