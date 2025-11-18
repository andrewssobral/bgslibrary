# Quick Reference: Algorithm Comparison Matrix

The table below provides a comprehensive comparative summary of all algorithms detailed in the provided file. They are categorized by **Family** to help you quickly identify relationships between different methods (e.g., all Gaussian Mixture Models vs. all Neural methods).

### Comprehensive Comparative Summary

| Algorithm | Family | Strengths | Weaknesses |
| :--- | :--- | :--- | :--- |
| **AdaptiveBackgroundLearning** | Running Average | Simple, fast, low memory (1 frame). Adapts to gradual changes. | "Ghosting" on moving objects; cannot handle multimodal backgrounds (e.g., waving trees). |
| **AdaptiveSelectiveBackgroundLearning** | Selective Running Average | Prevents stationary foreground from corrupting the model; reduces noise with median filter. | Discards color (grayscale); "dead background" issue if object moves; no multimodal support. |
| **CodeBook** | Codebook / Clustering | Handles multimodal backgrounds and waving trees; efficient cache system for new objects. | Discards color (grayscale); requires tuning many parameters; no visual model image. |
| **DPAdaptiveMedian** | Median Filtering | Simple integer math; selective update prevents corruption; uses RGB. | Very slow adaptation (1 step/frame); sensitive to single-channel noise; no multimodal support. |
| **DPEigenbackground** | Subspace / PCA | Models correlated background motion (e.g., waves); compact model size. | **Non-adaptive** (static after training); high training cost; requires clean training frames. |
| **DPGrimsonGMM** | Gaussian Mixture (GMM) | Handles multimodal backgrounds; unsupervised learning. | High memory/CPU cost; sensitive to parameter tuning (learning rate, threshold). |
| **DPMean** | Running Average | Very fast; simple implementation; uses RGB. | No multimodal support; slow to adapt to sudden lighting changes. |
| **DPPratiMediod** | Non-Parametric / Mediod | Robust to outliers; makes no Gaussian assumptions; clean mask via hysteresis. | High memory (stores history buffer); computationally expensive mediod calculation. |
| **DPTexture** | Texture (LBP) | Robust to illumination changes due to LBP features. | Computationally expensive; fails on untextured surfaces; high memory footprint. |
| **DPWrenGA** | Single Gaussian | Fast; low memory; adaptive to gradual changes. | Single mode only (no waving trees); assumes spherical variance (same for all channels). |
| **DPZivkovicAGMM** | Adaptive GMM | Auto-prunes unnecessary Gaussians; more stable/efficient than standard GMM. | Computationally expensive; requires careful parameter tuning; assumes shared variance. |
| **FrameDifference** | Temporal Difference | Extremely fast; minimal memory. | Only detects edges; stops detecting when object stops; sensitive to camera jitter. |
| **FuzzyChoquetIntegral** | Fuzzy / Fusion | Fuses color and texture for robustness; powerful evidence aggregation. | Very expensive; complex feature extraction; underlying model is simple running average. |
| **FuzzySugenoIntegral** | Fuzzy / Fusion | Fuses color/texture; slightly faster than Choquet integral. | Still expensive and complex; underlying model is simple running average. |
| **GMG** | Bayesian / Statistical | Robust to noise; uses priors for better decision making. | OpenCV "black box"; requires static training period; struggles with sudden lighting changes. |
| **IndependentMultimodal (IMBS)** | Non-Parametric / Clustering | Handles complex multimodal backgrounds; suppresses shadows; distinct "persistence" logic. | High memory (sample buffer); static between long sampling cycles; sensitive to sampling rate. |
| **KDE** | Density Estimation | Non-parametric (no Gaussian assumption); adapts bandwidth dynamically. | Very high memory and CPU cost (sums kernels for every sample). |
| **KNN** | K-Nearest Neighbors | Non-parametric; handles multimodal scenes; built-in shadow detection. | High memory (history buffer); OpenCV "black box"; expensive distance calculations. |
| **LBAdaptiveSOM** | Neural Network (SOM) | Models topology of color space; robust to jitter/noise; multimodal. | High memory (3x3 SOM per pixel); expensive; slow adaptation after training phase. |
| **LBFuzzyAdaptiveSOM** | Fuzzy Neural Network | Smoother learning than standard SOM due to fuzzy update logic. | Even higher cost/complexity; slow adaptation to large changes. |
| **LBFuzzyGaussian** | Fuzzy Single Gaussian | Smooth, adaptive learning robust to noise; handles per-channel variance. | Single mode only; slow to adapt to sudden lighting changes. |
| **LBMixtureOfGaussians** | Per-Pixel GMM | Handles multimodal backgrounds; per-channel variance; adaptive. | High memory; expensive; fixed small number of Gaussians (3). |
| **LBP\_MRF** | Texture + MRF | Extremely robust spatial consistency (MRF) + illumination invariance (LBP). | Massive computational cost (Graph Cut); complex parameters; high memory. |
| **LBSimpleGaussian** | Single Gaussian | Very fast; continuous update adapts quickly. | **Unconditional update** absorbs stopped objects immediately; no multimodal support. |
| **LOBSTER** | Sample-based (Color+LBSP) | Robust to light (LBSP) and camouflage (Color); spatial propagation fills holes. | High memory/CPU cost (storing N samples); sensitive to many thresholds. |
| **MixtureOfGaussianV1** | GMM (Grimson) | Standard adaptive multimodal modeling. | Wrapper for OpenCV (limited control); redundant post-thresholding. |
| **MixtureOfGaussianV2** | Adaptive GMM (Zivkovic) | Auto-selects component count; shadow detection included. | Wrapper for OpenCV; post-thresholding may oversimplify shadow mask. |
| **MultiCue** | Codebook (Color + Texture) | Very robust verification; Hausdorff distance removes "ghost" shapes. | Extremely complex/slow; operates on downsampled image (loss of detail). |
| **MultiLayer** | Multi-Feature Mixture | Fuses Color+LBP; handles shadows; "Layer" concept for background priority. | Huge parameter set; computationally heavy; complex state management. |
| **PAWCS** | Adaptive Dictionary | Highly adaptive (self-tuning learning rates); uses local & global dictionaries. | Immensely complex feedback loops; high memory; hard to predict/tune. |
| **PBAS** | Adaptive Sample-based | Auto-tunes threshold & update rate per pixel; robust to dynamic scenes. | Expensive feedback loops; complex meta-parameters. |
| **SigmaDelta** | Adaptive Filter | Extremely fast (integer math); low memory; adaptive. | Slow convergence (step-by-step); no multimodal support; sensitive to `ampFactor`. |
| **StaticFrameDifference** | Static Difference | Simplest/Fastest possible method. | **Non-adaptive**; permanently broken by lighting changes or moved objects. |
| **SuBSENSE** | Adaptive Sample-based | State-of-the-art adaptability via LBSP features + complex feedback loops. | High compute/memory cost; complex implementation. |
| **T2FGMM (UM/UV)** | Fuzzy GMM | Flexible "fuzzy" matching handles noise better than hard thresholds. | Expensive (sorting/fuzzy calc); complex fuzzy parameters; shared variance. |
| **T2FMRF (UM/UV)** | Fuzzy GMM + MRF | Excellent noise reduction and temporal stability (ICM optimization). | Extremely slow (Iterative MRF); risk of ghost persistence. |
| **TwoPoints** | Sample-based (2 points) | Very fast; low memory (2 bytes/pixel). | Too simplistic for complex/dynamic scenes; random updates cause instability. |
| **ViBe** | Sample-based | Fast; handles some dynamic background; spatial propagation fills holes. | Fixed threshold (sensitive to light); ghosting on initial frames; high memory. |
| **VuMeter** | Histogram | Captures full PDF of background; handles multimodal. | High memory (full histogram per pixel); precision depends on bin size. |
| **WeightedMovingMean** | Weighted Average | Fast; stable against noise (3-frame window). | Short memory (objects vanish in 3 frames); ghosting trails. |
| **WeightedMovingVariance** | Temporal Variance | Fast; detects *any* motion/change. | Objects vanish instantly when stopped; sensitive to camera noise. |

