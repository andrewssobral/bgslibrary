# Algorithm Families and Taxonomy

Here is a structured synthesis of the algorithms categorized by their mathematical approach.

### 1. Basic Temporal & Statistical Filters
These algorithms rely on simple statistics (mean, median, variance) or frame differences. They are computationally inexpensive but generally lack the ability to handle complex, dynamic backgrounds (like waving trees).

* **Frame Difference Methods:**
    * **FrameDifference:** Thresholds the absolute difference between the current and previous frame.
    * **StaticFrameDifference:** Compares the current frame to a permanently fixed initial frame.
    * **WeightedMovingMean / WeightedMovingVariance:** Computes a background model on the fly using a weighted average or variance of the last 3 frames.
* **Running Averages:**
    * **AdaptiveBackgroundLearning:** Updates a running average background model using a learning rate $\alpha$.
    * **DPMean:** Maintains a running floating-point mean and classifies based on Euclidean distance.
    * **SigmaDelta:** Uses a non-linear, increment/decrement feedback loop to update background and variance models.
* **Median & Histogram:**
    * **DPAdaptiveMedian:** Approximates a running median by incrementing/decrementing the model by 1 pixel value.
    * **VuMeter:** Maintains a per-pixel probability histogram of intensity values.

### 2. Gaussian Mixture Models (GMM)
These algorithms model the color distribution of each pixel as a mixture of $K$ Gaussian distributions. This allows them to handle **multimodal backgrounds**, such as a pixel that alternates between a swaying leaf and the sky behind it.

* **Classic GMMs:**
    * **DPGrimsonGMM / MixtureOfGaussianV1:** The standard Stauffer-Grimson approach where Gaussians are sorted by weight/variance.
    * **DPWrenGA / LBSimpleGaussian:** Simpler models that use a single Gaussian per pixel rather than a mixture.
* **Adaptive GMMs:**
    * **DPZivkovicAGMM / MixtureOfGaussianV2:** Advanced versions that dynamically determine the number of Gaussian components per pixel, pruning unnecessary ones.
    * **LBMixtureOfGaussians:** A per-pixel GMM that explicitly manages weights and sorts by significance.
* **Fuzzy GMMs:**
    * **T2FGMM (UM/UV) / LBFuzzyGaussian:** Introduces fuzzy logic to the matching process, using "fuzzy degrees" rather than binary thresholds for smoother updates.


### 3. Non-Parametric & Sample-Based Methods
Instead of assuming a mathematical shape (like a Gaussian), these methods maintain a history buffer of raw pixel values. They are currently considered some of the most robust methods for dynamic scenes.

* **Randomized/Stochastic Updates:**
    * **ViBe:** A landmark algorithm that updates its model by stochastically replacing values in its own history *and* the history of neighboring pixels.
    * **PBAS (Pixel-Based Adaptive Segmenter):** Similar to ViBe but introduces feedback loops to dynamically adjust the learning rate and decision threshold per pixel.
    * **SuBSENSE / LOBSTER / PAWCS:** Highly advanced derivatives of ViBe/PBAS that fuse Local Binary Patterns (LBSP) with color information and use complex feedback mechanisms to adapt to local noise and stability.
* **Density Estimation:**
    * **KDE (Kernel Density Estimation):** Estimates probability density using a kernel function over a history of samples.
    * **KNN:** Uses a K-Nearest Neighbor approach to determine if a pixel belongs to the background history distribution.
    * **IndependentMultimodal (IMBS):** Clusters samples into "bins" to form a multimodal background model without assuming Gaussian shapes.

### 4. Neural, Texture, and Subspace Methods
These algorithms use specialized features or learning architectures.

* **Texture-Based:**
    * **DPTexture:** Compares Local Binary Pattern (LBP) histograms rather than raw color, making it robust to illumination changes.
    * **LBP_MRF / T2FMRF:** Combines background modeling with Markov Random Fields (MRF) to enforce spatial coherence (smoothing) in the output mask.
* **Neural Networks:**
    * **LBAdaptiveSOM / LBFuzzyAdaptiveSOM:** Uses Self-Organizing Maps (SOM) to learn the topology of the background color space for each pixel.
* **Subspace Learning:**
    * **DPEigenbackground:** Uses Principal Component Analysis (PCA) to learn an "eigenspace" of the background during a training phase.

---

### Comparative Summary

| Feature | Algorithms | Strengths | Weaknesses |
| :--- | :--- | :--- | :--- |
| **Speed** | FrameDifference, WeightedMovingMean, SigmaDelta | Extremely fast, low CPU usage. | Cannot handle non-static backgrounds; "ghosting" artifacts. |
| **Multimodal Support** | GMMs (Grimson, Zivkovic), ViBe, PBAS | Can handle waving trees, water, and repetitive motion. | GMMs have high computational cost; ViBe/PBAS require memory for samples. |
| **Adaptability** | SuBSENSE, PAWCS, PBAS | Auto-tunes thresholds per pixel; robust to cam jitter & lighting. | Very complex implementation; high parameter count. |
| **Robustness to Light** | DPTexture, LOBSTER, MultiCue | Texture features (LBP) ignore shadow/light changes. | Fails on flat, uniform colors (like a blank wall). |
