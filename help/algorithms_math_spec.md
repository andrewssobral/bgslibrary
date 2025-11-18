# Background Subtraction: Mathematical Formulations and Specifications

Per-algorithm explanations (alphabetical)

## Quick links
- [AdaptiveBackgroundLearning](#adaptivebackgroundlearning)
- [AdaptiveSelectiveBackgroundLearning](#adaptiveselectivebackgroundlearning)
- [CodeBook](#codebook)
- [DPAdaptiveMedian](#dpadaptivemedian)
- [DPEigenbackground](#dpeigenbackground)
- [DPGrimsonGMM](#dpgrimsongmm)
- [DPMean](#dpmean)
- [DPPratiMediod](#dppratimediod)
- [DPTexture](#dptexture)
- [DPWrenGA](#dpwrenga)
- [DPZivkovicAGMM](#dpzivkovicagmm)
- [FrameDifference](#framedifference)
- [FuzzyChoquetIntegral](#fuzzychoquetintegral)
- [FuzzySugenoIntegral](#fuzzysugenointegral)
- [GMG](#gmg)
- [IndependentMultimodal (IMBS)](#independentmultimodal-imbs)
- [KDE](#kde)
- [KNN](#knn)
- [LBAdaptiveSOM](#lbadaptivesom)
- [LBFuzzyAdaptiveSOM](#lbfuzzyadaptivesom)
- [LBFuzzyGaussian](#lbfuzzygaussian)
- [LBMixtureOfGaussians](#lbmixtureofgaussians)
- [LBP_MRF](#lbp_mrf)
- [LBSimpleGaussian](#lbsimplegaussian)
- [LOBSTER](#lobster)
- [MixtureOfGaussianV1](#mixtureofgaussianv1)
- [MixtureOfGaussianV2](#mixtureofgaussianv2)
- [MultiCue](#multicue)
- [MultiLayer](#multilayer)
- [PAWCS (Pixel-based Adaptive Word Consensus Segmenter)](#pawcs-pixel-based-adaptive-word-consensus-segmenter)
- [PixelBasedAdaptiveSegmenter (PBAS)](#pixelbasedadaptivesegmenter-pbas)
- [SigmaDelta](#sigmadelta)
- [StaticFrameDifference](#staticframedifference)
- [SuBSENSE](#subsense)
- [T2FGMM_UM / T2FGMM_UV](#t2fgmm_um--t2fgmm_uv)
- [T2FMRF_UM / T2FMRF_UV](#t2fmrf_um--t2fmrf_uv)
- [TwoPoints](#twopoints)
- [ViBe](#vibe)
- [VuMeter](#vumeter)
- [WeightedMovingMean](#weightedmovingmean)
- [WeightedMovingVariance](#weightedmovingvariance)

---

## AdaptiveBackgroundLearning
---

### Algorithm Description

The `AdaptiveBackgroundLearning` algorithm creates a background model by computing a running average of the frames. The foreground is then detected by taking the absolute difference between the current frame and the learned background model. An optional threshold can be applied to this difference to create a binary foreground mask.

### Mathematical Formulation

Let the video sequence be represented by $I(x, y, t)$, where $(x, y)$ are the pixel coordinates and $t$ is the frame number (time). The algorithm computes a background model $B(x, y, t)$ and a foreground mask $F(x, y, t)$.

The pixel values are processed as floating-point numbers normalized to the range $[0.0, 1.0]$. Let $I'(x, y, t)$ be the normalized input frame at time $t$.

#### 1. Initialization

At time $t=0$, the background model $B'(x, y, 0)$ is initialized with the first frame of the video:

$B'(x, y, 0) = I'(x, y, 0)$

#### 2. Background Model Update

For each subsequent frame $t > 0$, the background model is updated using an exponential moving average. This update is governed by a learning rate $\alpha$. The update rule is defined as:

$B'(x, y, t) = \alpha \cdot I'(x, y, t) + (1 - \alpha) \cdot B'(x, y, t-1)$

This update is performed for each frame as long as the current frame number `currentLearningFrame` is less than `maxLearningFrames`, or if `maxLearningFrames` is set to -1 (indicating continuous learning).

#### 3. Foreground Detection

The foreground is determined by calculating the absolute difference between the current normalized input frame and the current normalized background model. Let $D'(x, y, t)$ be the normalized difference image:

$D'(x, y, t) = |I'(x, y, t) - B'(x, y, t)|$

This difference image $D'$ is then scaled from the range $[0.0, 1.0]$ to $[0, 255]$ to produce the 8-bit grayscale foreground image $F(x, y, t)$.

#### 4. Thresholding

If the `enableThreshold` parameter is true, a binary thresholding operation is applied to the foreground image $F(x, y, t)$. A pixel is classified as foreground if its intensity is above a certain `threshold`, $\tau$.

$F_{binary}(x, y, t) = \begin{cases} 255 & \text{if } F(x, y, t) > \tau \\ 0 & \text{otherwise} \end{cases}$

If `enableThreshold` is false, the foreground mask is simply the grayscale difference image $F(x, y, t)$.

### Parameters

The behavior of the algorithm is controlled by the following parameters found in the code:

*   `alpha` ($\alpha$): The learning rate, which determines how quickly the background model adapts to changes in the scene. It is a `double` value.
*   `maxLearningFrames`: An `int` that specifies the number of frames for the initial learning phase. If set to -1, the background model is updated indefinitely.
*   `enableThreshold`: A `bool` that, when true, applies a threshold to the difference image to create a binary foreground mask.
*   `threshold` ($\tau$): An `int` representing the value used to segment the difference image into foreground and background.

### Summary

* **Family:** Recursive averaging filter.
* **Idea (from code):** Maintains a background model as a running average of frames. For each new frame, it updates the background model using a weighted sum of the current frame and the previous model, controlled by a learning rate `alpha`. The absolute difference between the current frame and the model is then thresholded to produce the foreground mask.
* **Strengths:** Simple, fast, and requires low memory (only one background frame is stored). It can adapt to gradual changes in the scene.
* **Weaknesses:** A foreground object that becomes static will be slowly absorbed into the background model. It cannot handle multimodal backgrounds (e.g., waving trees) and adapts slowly to sudden, large-scale background changes, which can create ghost artifacts.

---

## AdaptiveSelectiveBackgroundLearning
---

### Algorithm Description

The `AdaptiveSelectiveBackgroundLearning` algorithm improves upon a simple adaptive model by introducing a selective update mechanism. It first performs an initial, unconditional background learning for a specified number of frames. Afterward, it only updates the background model for pixels that are classified as background. This prevents foreground objects from being incorporated into the background model. The algorithm operates on grayscale images.

### Mathematical Formulation

Let the video sequence be represented by $I(x, y, t)$, where $(x, y)$ are the pixel coordinates and $t$ is the frame number. The input is converted to grayscale. The algorithm computes a background model $B(x, y, t)$ and a foreground mask $F(x, y, t)$.

The pixel values are processed as floating-point numbers normalized to the range $[0.0, 1.0]$. Let $I'(x, y, t)$ and $B'(x, y, t)$ be the normalized input and background frames, respectively.

#### 1. Initialization

At time $t=0$, the background model is initialized with the first grayscale frame:

$B'(x, y, 0) = I'(x, y, 0)$

#### 2. Foreground Mask Generation

For each frame $t > 0$, a temporary foreground mask is generated before the background model is updated.

First, the absolute difference between the current frame and the *previous* background model is calculated:

$D'(x, y, t) = |I'(x, y, t) - B'(x, y, t-1)|$

This difference image is scaled to the range $[0, 255]$ and then thresholded using a value $\tau$ (`threshold`):

$F'_{t}(x, y) = \begin{cases} 255 & \text{if } (D'(x, y, t) \cdot 255) > \tau \\ 0 & \text{otherwise} \end{cases}$

A 3x3 median filter is applied to this binary mask to reduce noise and produce the final foreground mask for the current frame:

$F_t(x, y) = \text{median}_{3 \times 3}(F'_{t})(x, y)$

#### 3. Background Model Update

The update rule for the background model $B'(x, y, t)$ is divided into two distinct phases, controlled by the `learningFrames` parameter, denoted as $L$.

*   **Phase 1: Initial Learning (while $t \le L$)**
    During this phase, the background model is updated unconditionally for every pixel using the learning rate $\alpha_{learn}$ (`alphaLearn`):

    $B'(x, y, t) = \alpha_{learn} \cdot I'(x, y, t) + (1 - \alpha_{learn}) \cdot B'(x, y, t-1)$

*   **Phase 2: Selective Update (when $t > L$)**
    After the initial learning phase, the background model is updated selectively. Only pixels classified as background ($F_t(x, y) = 0$) are updated. Pixels classified as foreground are not used to update the model, preventing their absorption. The update uses a second learning rate, $\alpha_{detect}$ (`alphaDetection`):

    $B'(x, y, t) = \begin{cases} \alpha_{detect} \cdot I'(x, y, t) + (1 - \alpha_{detect}) \cdot B'(x, y, t-1) & \text{if } F_t(x, y) = 0 \\ B'(x, y, t-1) & \text{if } F_t(x, y) = 255 \end{cases}$

### Parameters

*   `learningFrames` ($L$): An `int` specifying the duration of the initial, unconditional learning phase.
*   `alphaLearn` ($\alpha_{learn}$): A `double` representing the learning rate during the initial learning phase.
*   `alphaDetection` ($\alpha_{detect}$): A `double` representing the learning rate during the selective update phase.
*   `threshold` ($\tau$): An `int` used to classify pixels as foreground or background based on the difference image.

### Summary

* **Family:** Selective averaging filter.
* **Idea (from code):** Calculates a foreground mask by thresholding the difference between the current frame and the background model, followed by a median filter. For an initial learning period, it updates the background model across all pixels. Afterward, it selectively updates only the pixels that were classified as background, preventing stationary objects from being absorbed into the model.
* **Strengths:** Prevents the background model from being corrupted by stationary foreground objects. The median filter reduces noise in the foreground mask. It remains simple and computationally fast.
* **Weaknesses:** Discards color information by converting to grayscale. Cannot handle multimodal backgrounds (e.g., waving trees). If a foreground object is removed, the newly exposed background may be misclassified if it changed during occlusion (the "dead background" problem).

---

## CodeBook
---

### Algorithm Description

The CodeBook algorithm models the background of each pixel as a "codebook," which is a collection of "codewords." Each codeword represents a range of intensity values and associated temporal information for a pixel. The algorithm operates in two main phases: an initial training phase where codebooks are built, and a subsequent detection phase where pixels are classified as foreground or background, and the codebooks are selectively updated. The implementation uses a main codebook for the background model and a cache codebook to stage new, potentially stable codewords. The algorithm operates on grayscale images.

### Mathematical Formulation

Let $I(p, t)$ be the intensity of a pixel $p=(x,y)$ at time (frame) $t$.
Each pixel $p$ is associated with a main codebook, $C_p$, and a cache codebook, $C'_p$.
A codebook is a set of codewords, e.g., $C_p = \{c_1, c_2, \dots, c_N\}$.
Each codeword $c_k$ is a tuple defined as $c_k = \langle min_k, max_k, f_k, l_k, first_k, last_k \rangle$, where:
*   $min_k, max_k$: The intensity range of the codeword.
*   $f_k$: The frequency, or the number of times the codeword has been matched.
*   $l_k$: The maximum negative run-length (MNRL), representing the number of frames since the codeword was last matched.
*   $first_k$: The frame number of the codeword's creation.
*   $last_k$: The frame number of the codeword's last match.

The algorithm's behavior is governed by the following parameters:
*   $L$: `learningFrames`, the duration of the initial training phase.
*   $\alpha$: `alpha`, a parameter defining the boundary slack for a pixel's intensity range.
*   $\beta$: `beta`, the update rate used during the detection phase.
*   $T_{del}$: `Tdel`, the staleness threshold to remove a codeword from the main codebook.
*   $T_{add}$: `Tadd`, the frequency threshold to promote a codeword from the cache to the main codebook.
*   $T_{h}$: `Th`, the staleness threshold to remove a codeword from the cache codebook.

#### **Phase 1: Initial Training ($t \le L$)**

During this phase, the foreground mask $F(p, t)$ is not computed and is effectively 0. For each pixel $p$ with intensity $I_p = I(p, t)$, the main codebook $C_{p,t-1}$ is updated to $C_{p,t}$.

1.  **Find Match:** Search for a matching codeword $c_m \in C_{p,t-1}$ such that $min_{m, t-1} \le I_p \le max_{m, t-1}$.

2.  **Update or Create:**
    *   **If a match $c_m$ is found:** The codeword is updated. For all other non-matching codewords $c_k$ ($k \ne m$), their staleness is incremented: $l_{k,t} = l_{k,t-1} + 1$. The matched codeword $c_m$ is updated as follows:
        *   $min_{m,t} = \frac{(I_p - \alpha) + f_{m,t-1} \cdot min_{m,t-1}}{f_{m,t-1} + 1}$
        *   $max_{m,t} = \frac{(I_p + \alpha) + f_{m,t-1} \cdot max_{m,t-1}}{f_{m,t-1} + 1}$
        *   $f_{m,t} = f_{m,t-1} + 1$
        *   $l_{m,t} = 0$
        *   $last_{m,t} = t$

    *   **If no match is found:** A new codeword $c_{new}$ is created and added to the codebook, $C_{p,t} = C_{p,t-1} \cup \{c_{new}\}$, with the following initial values:
        *   $min_{new} = \max(0, I_p - \alpha)$
        *   $max_{new} = \min(255, I_p + \alpha)$
        *   $f_{new} = 1$
        *   $l_{new} = 0$
        *   $first_{new} = t$
        *   $last_{new} = t$

#### **Phase 2: Detection and Update ($t > L$)**

For each pixel $p$ with intensity $I_p = I(p, t)$:

**1. Main Codebook Matching (Background Classification):**
Find a matching codeword $c_m \in C_{p,t-1}$ such that $min_{m, t-1} \le I_p \le max_{m, t-1}$.

*   **If a match $c_m$ is found (Pixel is Background):**
    *   Set foreground mask: $F(p, t) = 0$.
    *   Update the matched codeword $c_m$:
        *   $min_{m,t} = (1 - \beta)(I_p - \alpha) + \beta \cdot min_{m,t-1}$
        *   $max_{m,t} = (1 - \beta)(I_p + \alpha) + \beta \cdot max_{m,t-1}$
        *   $f_{m,t} = f_{m,t-1} + 1$
        *   $l_{m,t} = 0$
        *   $first_{m,t} = t$
    *   For all other codewords $c_k \in C_{p,t-1}$ ($k \ne m$), increment staleness: $l_{k,t} = l_{k,t-1} + 1$.
    *   Prune the main codebook: $C_{p,t} = \{c_k \in C_{p,t} \mid l_{k,t} < T_{del}\}$.
    *   The process for this pixel is complete.

*   **If no match is found (Pixel is Foreground):**
    *   Set foreground mask: $F(p, t) = 255$.
    *   Increment staleness for all codewords in the main codebook: $\forall c_k \in C_{p,t-1}, l_{k,t} = l_{k,t-1} + 1$.
    *   Prune the main codebook: $C_{p,t} = \{c_k \in C_{p,t-1} \mid l_{k,t} < T_{del}\}$.
    *   Proceed to handle the cache codebook.

**2. Cache Codebook Handling (for Foreground Pixels only):**
The cache codebook $C'_{p,t-1}$ is updated to $C'_{p,t}$ using the same logic as the training phase (Phase 1), but on the cache. A matching codeword in the cache is updated, or a new one is created if no match is found.

**3. Cache Pruning and Promotion:**
*   **Prune:** Remove any codeword $c'_k$ from the cache $C'_{p,t}$ if its staleness exceeds the threshold: $l'_{k,t} \ge T_{h}$.
*   **Promote:** Identify codewords in the cache that have become stable: $P = \{c'_k \in C'_{p,t} \mid f'_{k,t} > T_{add}\}$.
*   Move these codewords from the cache to the main codebook: $C_{p,t} = C_{p,t} \cup P$ and $C'_{p,t} = C'_{p,t} \setminus P$.

### Summary

*   **Family:** Clustering-based Codebook Modeling.
*   **Idea (from code):** Models each pixel with a "main codebook" (background) and a "cache codebook" (foreground candidates). A pixel is background if its intensity matches a codeword in the main book. If not, it's foreground and matched against the cache. New foreground pixels create new cache codewords. Cache codewords that are matched frequently are promoted to the main codebook. Infrequently used codewords in both books are eventually deleted.
*   **Strengths:** Can model multimodal backgrounds (e.g., waving leaves) by storing multiple codewords per pixel. The cache system allows it to adapt to new, stable background objects without long-term ghosting.
*   **Weaknesses:** Discards color information by using grayscale. Performance is highly dependent on tuning multiple parameters (`alpha`, `beta`, `Tdel`, `Tadd`, `Th`). This implementation does not generate a visual background model image.

---

## DPAdaptiveMedian
---

### Algorithm Description

The `DPAdaptiveMedian` algorithm is a median-based background subtraction method that operates in the RGB color space. It maintains a single background model representing the median color of each pixel. The foreground is detected by thresholding the per-channel absolute difference between the input frame and the background model. The background model is updated selectively for pixels classified as background, and this update is performed at a specified sampling rate. The update rule incrementally adjusts the background model's color values towards the current frame's values, which approximates a running median calculation.

### Mathematical Formulation

Let $\vec{I}(p, t) = (R_I(p, t), G_I(p, t), B_I(p, t))$ be the RGB color vector of a pixel $p=(r,c)$ at frame $t$.
Let $\vec{B}(p, t) = (R_B(p, t), G_B(p, t), B_B(p, t))$ be the background model's RGB color vector for pixel $p$ at frame $t$.

The algorithm's behavior is controlled by the following parameters:
*   $L$: `learningFrames`, the duration of the initial, unconditional update phase.
*   $S$: `samplingRate`, the interval at which the background model is updated.
*   $\tau_{low}$: `LowThreshold`, the threshold for foreground detection.
*   $\tau_{high}$: `HighThreshold`, a second, higher threshold (this implementation generates two masks, but the wrapper uses the low-threshold one).

#### **1. Initialization**

At time $t=0$, the background model is initialized with the pixel values from the first frame:
$\vec{B}(p, 0) = \vec{I}(p, 0)$

#### **2. Foreground Detection**

At each frame $t > 0$, the foreground mask $F(p, t)$ is computed. For each pixel $p$, the absolute difference for each color channel $k \in \{R, G, B\}$ is calculated:

$\Delta_k(p, t) = |I_k(p, t) - B_k(p, t-1)|$

A pixel is classified as background if the difference for all three channels is below the threshold $\tau_{low}$. The foreground mask $F(p, t)$ is defined as:

$F(p, t) = \begin{cases} 0 \text{ (Background)} & \text{if } \Delta_R(p, t) \le \tau_{low} \land \Delta_G(p, t) \le \tau_{low} \land \Delta_B(p, t) \le \tau_{low} \\ 255 \text{ (Foreground)} & \text{otherwise} \end{cases}$

*(Note: The code also computes a second mask using $\tau_{high}$, but it is not used by the parent class for the final output.)*

#### **3. Background Model Update**

The background model is updated periodically, not on every frame. The update is performed only if the frame number $t$ satisfies the condition:
$(t \pmod S) = 1$

If this condition is met, the model $\vec{B}(p, t-1)$ is updated to $\vec{B}(p, t)$. For each pixel $p$, the update is performed if the pixel is classified as background or if the algorithm is in its initial learning phase:

Let `is_background` be true if $F(p, t) = 0$.
Let `is_learning` be true if $t < L$.

If `is_background` $\lor$ `is_learning` is true, then for each channel $k \in \{R, G, B\}$, the background model is updated as follows:

$B_k(p, t) = \begin{cases} B_k(p, t-1) + 1 & \text{if } I_k(p, t) > B_k(p, t-1) \\ B_k(p, t-1) - 1 & \text{if } I_k(p, t) < B_k(p, t-1) \\ B_k(p, t-1) & \text{if } I_k(p, t) = B_k(p, t-1) \end{cases}$

If the update condition for the frame or the pixel is not met, the background model remains unchanged:
$\vec{B}(p, t) = \vec{B}(p, t-1)$

### Summary

*   **Family:** Approximated Median Filtering.
*   **Idea (from code):** Maintains a single RGB background model. A pixel is classified as foreground if the absolute difference between the frame and the model, on any color channel, exceeds a threshold. The model is updated only for pixels deemed to be background. The update rule is an approximation: if the current pixel is brighter than the model, the model's value is incremented by one; if darker, it's decremented by one. Updates are performed sparsely, controlled by a sampling rate.
*   **Strengths:** Fast and simple, using basic integer arithmetic. Utilizes RGB color information for more robust segmentation. The selective update prevents stationary foreground objects from corrupting the background model.
*   **Weaknesses:** The background adapts very slowly (one step at a time), making it unable to cope with sudden illumination changes. It cannot handle multimodal backgrounds (e.g., waving trees) as it only stores a single median color. It can be sensitive to color noise in a single channel.

---

## DPEigenbackground
---

### Algorithm Description

The `DPEigenbackground` algorithm is a static background subtraction method based on Principal Component Analysis (PCA). It learns a low-dimensional "eigenspace" from an initial sequence of training frames. This eigenspace represents the principal modes of variation within the training data, which is assumed to be the background. For subsequent frames, the algorithm projects the frame onto this eigenspace and then reconstructs it. The difference between the original frame and its reconstruction is used to identify foreground pixels. A large reconstruction error implies that the pixel contains variations not captured by the background model.

Crucially, this implementation is **non-adaptive**; once the background model is trained, it is never updated.

### Mathematical Formulation

Let an image frame at time $t$ be vectorized into a single column vector $\vec{x}_t \in \mathbb{R}^N$, where $N = \text{width} \times \text{height} \times 3$.

The algorithm's behavior is controlled by the following parameters:
*   $H$: `historySize`, the number of frames used for training.
*   $d$: `embeddedDim`, the dimensionality of the PCA subspace (the number of eigenvectors to use).
*   $\tau^2_{low}$: `LowThreshold`, the squared distance threshold for classifying foreground pixels.
*   $\tau^2_{high}$: `HighThreshold`, a second threshold for an alternate foreground mask (not typically used as the final output).

#### **Phase 1: History Collection ($t < H$)**

During the first $H$ frames, the algorithm populates a data matrix $D \in \mathbb{R}^{H \times N}$, where each row is a vectorized frame from the training sequence:

$D = \begin{bmatrix} \vec{x}_0^T \\ \vec{x}_1^T \\ \vdots \\ \vec{x}_{H-1}^T \end{bmatrix}$

In this phase, no foreground detection is performed, and the output mask is always zero (all background).

#### **Phase 2: PCA Model Computation (at $t = H$)**

At the exact moment when $t=H$, the PCA model is computed from the data matrix $D$.

1.  **Mean Vector:** The mean background vector $\vec{\mu} \in \mathbb{R}^N$ is calculated:
    $\vec{\mu} = \frac{1}{H} \sum_{i=0}^{H-1} \vec{x}_i$

2.  **Eigenspace:** The eigenvectors of the data's covariance matrix are computed. Let the matrix of the top $d$ eigenvectors (principal components) be $E = [\vec{e}_1, \vec{e}_2, \dots, \vec{e}_d] \in \mathbb{R}^{N \times d}$. These eigenvectors form an orthonormal basis for the background subspace.

The background model is now statically defined by the mean vector $\vec{\mu}$ and the eigenvector matrix $E$.

#### **Phase 3: Foreground Detection ($t \ge H$)**

For any new frame $\vec{x}_t$ after training is complete:

1.  **Centering:** The frame is centered by subtracting the mean background:
    $\vec{\phi}_t = \vec{x}_t - \vec{\mu}$

2.  **Projection:** The centered vector is projected onto the background eigenspace to obtain its low-dimensional coordinate representation $\vec{\omega}_t \in \mathbb{R}^d$:
    $\vec{\omega}_t = E^T \vec{\phi}_t$

3.  **Back-Projection (Reconstruction):** The low-dimensional representation is projected back into the original $N$-dimensional space to get the reconstruction of the frame, $\vec{x}'_t$, using only the background model:
    $\vec{x}'_t = E \vec{\omega}_t + \vec{\mu}$

4.  **Reconstruction Error:** The foreground mask $F(p, t)$ is determined by the reconstruction error at each pixel $p$. Let $I_k(p, t)$ be the value of channel $k \in \{R, G, B\}$ for pixel $p$ in the original frame $\vec{x}_t$, and $I'_k(p, t)$ be the corresponding value in the reconstructed frame $\vec{x}'_t$.

    The squared error for each channel of a pixel is calculated:
    $\delta_k^2(p, t) = (I_k(p, t) - I'_k(p, t))^2$

    A pixel is classified as foreground if the squared error in **any** of its color channels exceeds the threshold $\tau^2_{low}$:

    $F(p, t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } \delta_R^2(p, t) > \tau^2_{low} \lor \delta_G^2(p, t) > \tau^2_{low} \lor \delta_B^2(p, t) > \tau^2_{low} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **Update**

The `Update` function in this implementation is empty. The background model ($\vec{\mu}$ and $E$) is static and is never modified after its initial computation at $t=H$.

### Summary

*   **Family:** Principal Component Analysis (PCA) / Eigenspace Modeling.
*   **Idea (from code):** Learns a static background model by performing PCA on an initial sequence of frames. For each new frame, it projects the frame onto the learned "eigenspace" and then reconstructs it. A pixel is classified as foreground if the squared difference between the original and reconstructed pixel value, in any color channel, exceeds a threshold.
*   **Strengths:** Can effectively model correlated background variations (like waving trees) that are present in the initial training data. The model is compact, representing the background with a mean image and a set of eigenvectors.
*   **Weaknesses:** The background model is completely static and never updates after the initial training, which the code calls a "serious limitation." It requires a clean training period with no foreground objects. The initial training phase has high memory requirements and a significant one-time computational cost.

---

## DPGrimsonGMM
---

### Algorithm Description

For each pixel, the algorithm models the color distribution as a Mixture of Gaussians (GMM). Each Gaussian component has a mean, a variance (assumed to be spherical, i.e., the same for each color channel), and a weight representing the proportion of time it accounts for the pixel's color. The Gaussians are continuously sorted by their fitness ($\omega/\sigma$). The first few Gaussians that comprise a significant portion of the distribution's weight are considered the background model. When a new pixel arrives, it is checked against the existing Gaussians. If a match is found, the parameters of the matched Gaussian are updated; otherwise, a new Gaussian is created, potentially replacing the least significant one.

### Mathematical Formulation

For each pixel $p$, the color distribution is modeled as a mixture of up to $K_{max}$ (`MaxModes`) Gaussians:
$P(\vec{x}) = \sum_{k=1}^{K_p} \omega_{p,k} \cdot \eta(\vec{x}; \vec{\mu}_{p,k}, \sigma^2_{p,k}I)$
where, for the $k$-th Gaussian of pixel $p$:
*   $\vec{x}$ is the pixel's RGB color vector.
*   $\omega_{p,k}$ is the weight.
*   $\vec{\mu}_{p,k}$ is the mean RGB color.
*   $\sigma^2_{p,k}$ is the variance.
*   $\eta$ is the Gaussian probability density function.
*   $K_p$ is the current number of Gaussians for pixel $p$.

The algorithm's behavior is controlled by the following parameters:
*   $\alpha$: `Alpha`, the learning rate.
*   $T_B$: `m_bg_threshold`, the minimum proportion of the distribution that must be accounted for by the background model (e.g., 0.75).
*   $T_D$: `LowThreshold`, a factor determining the matching threshold. A pixel matches a Gaussian if its squared distance is less than $T_D \cdot \sigma^2$.
*   $\sigma^2_{init}$: `m_variance`, the initial variance for new Gaussians.

#### **Processing at Frame $t$ for Pixel $p$ with color $\vec{x}_t$**

**1. Background Model Identification:**
The existing $K_{p,t-1}$ Gaussians are sorted in descending order based on the value $\omega_{p,k}/\sigma_{p,k}$. The first $B$ distributions are chosen to represent the background, where $B$ is the smallest integer satisfying:
$\sum_{k=1}^{B} \omega_{p,k,t-1} > T_B$

**2. Pixel Matching:**
The algorithm finds the first Gaussian $m$ (from the sorted list) that matches the input pixel $\vec{x}_t$. A match is declared if the squared Euclidean distance is within a scaled variance:
$(\vec{x}_t - \vec{\mu}_{p,m,t-1})^T(\vec{x}_t - \vec{\mu}_{p,m,t-1}) < T_D \cdot \sigma^2_{p,m,t-1}$

**3. Foreground/Background Classification:**
*   If a match is found with a Gaussian $m$ that is part of the background model ($m \le B$), the pixel $p$ is classified as **Background**.
*   Otherwise (either the matched Gaussian is not in the background model, or no match is found at all), the pixel is classified as **Foreground**.

**4. Model Parameter Update:**

*   **Case 1: A match is found with Gaussian $m$.**
    The weights for all $K_{p,t-1}$ Gaussians are updated:
    $\omega_{p,k,t} = (1-\alpha)\omega_{p,k,t-1} + \alpha \cdot \delta_{k,m}$
    (where $\delta_{k,m}$ is 1 if $k=m$ and 0 otherwise).

    The mean and variance of the matched Gaussian $m$ are also updated. The update rate $\rho_t$ is defined as $\rho_t = \alpha / \omega_{p,m,t}$:
    *   $\vec{\mu}_{p,m,t} = (1-\rho_t)\vec{\mu}_{p,m,t-1} + \rho_t \vec{x}_t$
    *   $\sigma^2_{p,m,t} = (1-\rho_t)\sigma^2_{p,m,t-1} + \rho_t ((\vec{x}_t - \vec{\mu}_{p,m,t})^T(\vec{x}_t - \vec{\mu}_{p,m,t}))$
    The updated variance $\sigma^2_{p,m,t}$ is then clamped to the range $[4, 5 \cdot \sigma^2_{init}]$. The parameters of non-matching Gaussians are not changed.

*   **Case 2: No match is found.**
    The weights of all existing Gaussians are decayed: $\omega_{p,k,t} = (1-\alpha)\omega_{p,k,t-1}$.
    A new Gaussian is introduced. If $K_{p,t-1} < K_{max}$, it is added. If $K_{p,t-1} = K_{max}$, it replaces the Gaussian with the lowest $\omega/\sigma$ value. The new Gaussian is initialized with:
    *   $\vec{\mu}_{new} = \vec{x}_t$
    *   $\sigma^2_{new} = \sigma^2_{init}$
    *   $\omega_{new} = \alpha$ (or 1 if it is the first Gaussian for that pixel).

**5. Weight Renormalization:**
After the update step, the weights of all $K_{p,t}$ Gaussians for the pixel are normalized to ensure they sum to 1:
$\omega_{p,k,t} \leftarrow \frac{\omega_{p,k,t}}{\sum_{j=1}^{K_{p,t}} \omega_{p,j,t}}$

### Summary

*   **Family:** Per-pixel Mixture of Gaussians (GMM).
*   **Idea (from code):** Models each pixel's color distribution with a mixture of weighted Gaussians. These Gaussians are sorted by a weight/variance metric, and the top-ranked ones that exceed a weight threshold constitute the background model. For each new frame, if a pixel's color matches a background Gaussian, the model is updated. If it doesn't match any background Gaussian, it's marked as foreground. If it doesn't match any Gaussian at all, a new one is created, replacing the least significant one.
*   **Strengths:** Effectively models multimodal backgrounds (e.g., waving trees, water ripples). Continuously adapts to gradual lighting changes and background object movement. It is fully unsupervised.
*   **Weaknesses:** High memory footprint, as it stores multiple Gaussian models for every pixel. It is computationally more expensive than simpler methods. Performance is sensitive to the tuning of several parameters (learning rate, number of modes, background threshold).

---

## DPMean
---

### Algorithm Description

The `DPMean` algorithm models the background using a simple running average of pixel colors. It maintains a single, floating-point background model that represents the mean color for each pixel. Foreground detection is achieved by calculating the squared Euclidean distance between the current pixel's color and the mean background color, and comparing this distance to a threshold. The background model is updated selectively: only pixels classified as background are used to update the mean, preventing stationary foreground objects from being absorbed into the model.

### Mathematical Formulation

Let $\vec{I}(p, t) = (R_I(p, t), G_I(p, t), B_I(p, t))$ be the RGB color vector of a pixel $p=(r,c)$ at frame $t$.
Let $\vec{B}(p, t) = (R_B(p, t), G_B(p, t), B_B(p, t))$ be the floating-point vector representing the mean background color for pixel $p$ at time $t$.

The algorithm's behavior is controlled by the following parameters:
*   $L$: `learningFrames`, the duration of the initial, unconditional update phase.
*   $\alpha$: `Alpha`, the learning rate parameter that controls the update speed.
*   $\tau^2_{low}$: `LowThreshold`, the squared distance threshold for foreground classification.
*   $\tau^2_{high}$: `HighThreshold`, a second threshold for an alternate foreground mask.

---

#### **1. Initialization**

At time $t=0$, the background model is initialized with the pixel values from the first frame, cast to floating-point numbers:
$\vec{B}(p, 0) = \vec{I}(p, 0)$

---

#### **2. Foreground Detection**

At each frame $t > 0$, the foreground mask $F(p, t)$ is computed. For each pixel $p$, the squared Euclidean distance, $d^2(p, t)$, between the input pixel's color and the background model's mean color from the previous frame is calculated:

$d^2(p, t) = ||\vec{I}(p, t) - \vec{B}(p, t-1)||^2 = \sum_{k \in \{R,G,B\}} (I_k(p, t) - B_k(p, t-1))^2$

The pixel is classified as foreground if this distance exceeds the threshold $\tau^2_{low}$:

$F(p, t) = \begin{cases} 0 \text{ (Background)} & \text{if } d^2(p, t) \le \tau^2_{low} \\ 255 \text{ (Foreground)} & \text{if } d^2(p, t) > \tau^2_{low} \end{cases}$

*(Note: The code also computes a second mask using $\tau^2_{high}$, but it is not used by the parent class for the final output.)*

---

#### **3. Background Model Update**

The background model is updated for each pixel $p$ based on its classification in the current frame.

Let `is_background` be true if $F(p, t) = 0$.
Let `is_learning` be true if $t < L$.

If the condition (`is_background` $\lor$ `is_learning`) is met, the background model is updated using an exponential moving average:

$\vec{B}(p, t) = \alpha \cdot \vec{B}(p, t-1) + (1 - \alpha) \cdot \vec{I}(p, t)$

Note that in this formulation, a smaller $\alpha$ leads to faster adaptation. The code implements this with `alpha` weighting the old model, meaning a larger `alpha` in the code corresponds to slower adaptation. The equation above reflects the standard definition where `(1-alpha)` is the weight of the new observation.

If the update condition is not met, the background model for that pixel remains unchanged:
$\vec{B}(p, t) = \vec{B}(p, t-1)$

### Summary

*   **Family:** Recursive Averaging Filter.
*   **Idea (from code):** Maintains a running average of pixel colors in a floating-point background model. A pixel is classified as foreground if the squared Euclidean distance between its RGB value and the model's mean exceeds a threshold. The model is then updated using an exponential moving average, but only for pixels that were classified as background.
*   **Strengths:** Very fast, simple to implement, and has a low memory footprint. The selective update rule prevents stationary foreground objects from being absorbed into the background. It uses full RGB color information.
*   **Weaknesses:** Cannot handle multimodal backgrounds (e.g., waving trees), as it only stores a single mean color. It adapts slowly to sudden, large-scale illumination changes. The global distance threshold can be difficult to tune for all scene conditions.

---

## DPPratiMediod
---

### Algorithm Description

The `DPPratiMediod` algorithm is a non-parametric background subtraction method. For each pixel, it maintains a buffer of the last $N$ observed color values from previous frames. The background model for that pixel is not a statistical average, but the **mediod** of this sample buffer. The mediod is the specific sample within the buffer that has the minimum sum of distances to all other samples in the buffer. The distance metric used is the L-infinity norm ($L_\infty$). Foreground detection is performed by comparing the distance of the current pixel to the background mediod. The final foreground mask is produced using a two-threshold hysteresis method to improve robustness. The sample buffer is updated selectively for pixels classified as background.

### Mathematical Formulation

For each pixel $p$, the model consists of a circular buffer of the last $N$ (`historySize`) sampled pixel values, denoted as the set $S_p(t) = \{\vec{s}_{p,1}, \vec{s}_{p,2}, \dots, \vec{s}_{p,N}\}$.

**Distance Metric:**
The distance between two RGB color vectors $\vec{a}$ and $\vec{b}$ is the **L-infinity norm** ($L_\infty$), also known as the Chebyshev distance:
$d_\infty(\vec{a}, \vec{b}) = \max_{k \in \{R,G,B\}} |a_k - b_k|$

The algorithm's behavior is controlled by the following parameters:
*   $N$: `historySize`, the number of historical samples per pixel.
*   $S$: `samplingRate`, the interval at which the model is updated.
*   $\tau_{low}$: `LowThreshold`.
*   $\tau_{high}$: `HighThreshold`.

#### **1. Background Model Representation**

The background model for a pixel $p$ at time $t$ is the **mediod** of its sample buffer $S_p(t-1)$, denoted as $\vec{m}_p(t-1)$. The mediod is the sample point within the set that minimizes the sum of $L_\infty$ distances to all other points in the set:

$\vec{m}_p(t-1) = \underset{\vec{s}_k \in S_p(t-1)}{\arg\min} \sum_{j=1}^{N} d_\infty(\vec{s}_k, \vec{s}_j)$

#### **2. Foreground Detection**

This phase only occurs once the sample buffers are full (i.e., when frame number $t \ge N$).

For each pixel $p$ with input color $\vec{I}(p,t)$:

**a. Distance Calculation:**
The $L_\infty$ distance between the input pixel's color and the background mediod is calculated:
$d_p(t) = d_\infty(\vec{I}(p,t), \vec{m}_p(t-1))$

**b. Two-Level Thresholding:**
Two intermediate binary masks, $F_{low}$ and $F_{high}$, are generated:
$F_{low}(p,t) = \begin{cases} 1 \text{ (Foreground)} & \text{if } d_p(t) > \tau_{low} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

$F_{high}(p,t) = \begin{cases} 1 \text{ (Foreground)} & \text{if } d_p(t) > \tau_{high} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

**c. Hysteresis Combination:**
The final foreground mask $F(p,t)$ is produced by combining the two masks. A pixel is marked as foreground if it is above the high threshold, or if it is above the low threshold and is also connected to a high-threshold pixel.

$F(p,t) = F_{high}(p,t) \lor (F_{low}(p,t) \land (\exists q \in \mathcal{N}_8(p) \text{ s.t. } F_{high}(q,t)=1))$
where $\mathcal{N}_8(p)$ is the 8-connected neighborhood of pixel $p$.

#### **3. Background Model Update**

The model update is performed only on frames where $(t \pmod S) = 0$.

For each pixel $p$, the update is performed only if the pixel was classified as background in the final mask, i.e., $F(p,t) = 0$.

If the update condition is met:
1.  Let $\vec{s}_{old}$ be the oldest sample in the circular buffer $S_p(t-1)$.
2.  The new sample buffer is created by replacing the oldest sample with the current input pixel's color:
    $S_p(t) = (S_p(t-1) \setminus \{\vec{s}_{old}\}) \cup \{\vec{I}(p,t)\}$
3.  The mediod $\vec{m}_p(t)$ is then re-computed based on the updated sample set $S_p(t)$. The implementation does this efficiently by updating a stored table of summed distances for each sample.

If the update condition is not met for the frame or the pixel, the sample buffer and the mediod remain unchanged:
$S_p(t) = S_p(t-1)$
$\vec{m}_p(t) = \vec{m}_p(t-1)$

### Summary

*   **Family:** Non-parametric Sample-based Modeling.
*   **Idea (from code):** Maintains a buffer of recent color samples for each pixel. The background model is the "mediod" of this buffer—the one sample that is closest to all others, using the L-infinity (max channel) distance. A pixel is marked foreground if its distance to the mediod exceeds a threshold. The final mask is refined with a two-level hysteresis thresholding to reduce noise. The sample buffer is updated selectively for background pixels at a fixed sampling rate.
*   **Strengths:** Being non-parametric, it makes no assumptions about the background distribution (e.g., Gaussian). The mediod is robust to outliers in the sample history. Hysteresis thresholding produces a cleaner foreground mask than a single threshold.
*   **Weaknesses:** High memory usage, as it stores a history of pixel values for every pixel. The mediod calculation is computationally more expensive than simple averaging. The model adapts slowly, as its state is tied to the size of the sample history.

---

## DPTexture
---

### Algorithm Description

The `DPTexture` algorithm is a texture-based background subtraction method. It characterizes the appearance of each pixel by computing a histogram of Local Binary Pattern (LBP) codes in its local neighborhood. The background model for each pixel is a reference LBP histogram. Foreground detection is achieved by comparing the LBP histogram of the current frame with the background model's histogram. If the similarity between the two histograms is too low, the pixel is marked as foreground. The background model is then updated for pixels classified as background using a running average on the histogram bins. The entire process is performed independently for each of the R, G, and B color channels.

### Mathematical Formulation

Let $I(p, t)$ be the RGB color vector for a pixel $p=(x,y)$ at time $t$.

**Parameters:**
*   $P$: `TEXTURE_POINTS`, the number of LBP sampling points (hardcoded to 6).
*   $H_{LBP}$: `HYSTERSIS`, a value added during LBP calculation.
*   $R_{Hist}$: `REGION_R`, the radius of the square local region for histogram calculation.
*   $\alpha$: `ALPHA`, the learning rate for the model update.
*   $\tau$: `THRESHOLD`, the similarity threshold for foreground detection.

#### **1. LBP Image Creation**

For each pixel $p$ and each color channel $k \in \{R,G,B\}$, a 6-bit Local Binary Pattern code, $L_k(p,t)$, is computed. Let $c_k = I_k(p,t)$ be the center pixel's value and $\{n_{k,i}\}_{i=0}^{5}$ be the values of the 6 pre-defined neighboring pixels.

The LBP code is calculated as:
$L_k(p,t) = \sum_{i=0}^{5} s(c_k - n_{k,i} + H_{LBP}) \cdot 2^i$
where $s(x) = 1$ if $x \ge 0$ and $s(x) = 0$ if $x < 0$. This operation results in a 3-channel LBP image where each pixel value is an integer from 0 to 63.

#### **2. Local LBP Histogram Calculation**

For each pixel $p$, a local texture histogram, $H_p(t)$, is computed from the LBP image. This histogram consists of three separate histograms, one for each channel: $H_p(t) = \{H_{p,R}(t), H_{p,G}(t), H_{p,B}(t)\}$.

Each channel-specific histogram, $H_{p,k}(t)$, is a vector with $2^P=64$ bins. The value of the $j$-th bin, $H_{p,k,j}(t)$, is the number of occurrences of the LBP code $j$ within a square neighborhood $\mathcal{N}_{Hist}(p)$ of size $(2R_{Hist}+1) \times (2R_{Hist}+1)$ centered at pixel $p$ in the LBP image.

$H_{p,k,j}(t) = \sum_{q \in \mathcal{N}_{Hist}(p)} \delta(L_k(q,t), j)$
where $\delta(a,b)$ is the Kronecker delta (1 if $a=b$, 0 otherwise).

#### **3. Background Model**

The background model for each pixel $p$ is a single reference texture histogram, $B_p(t)$, which is structured identically to $H_p(t)$ and also consists of three channel-specific histograms: $\{B_{p,R}(t), B_{p,G}(t), B_{p,B}(t)\}$.

#### **4. Foreground Detection**

For each pixel $p$, the similarity between its current local histogram $H_p(t)$ and its background model $B_p(t-1)$ is calculated using the **histogram intersection** measure.

The intersection for a single channel $k$ is:
$\mathcal{P}_k(p,t) = \sum_{j=0}^{63} \min(H_{p,k,j}(t), B_{p,k,j}(t-1))$

The total proximity (similarity) is the sum of intersections over all three channels:
$\mathcal{P}_{total}(p,t) = \mathcal{P}_R(p,t) + \mathcal{P}_G(p,t) + \mathcal{P}_B(p,t)$

The pixel is classified as foreground if this total proximity is less than the global threshold $\tau$:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } \mathcal{P}_{total}(p,t) < \tau \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **5. Background Model Update**

The background model $B_p(t-1)$ is updated to $B_p(t)$ only if the pixel $p$ was classified as background ($F(p,t) = 0$). The update is a running average performed on each bin $j$ of each channel histogram $k$:

$B_{p,k,j}(t) = (1-\alpha) \cdot B_{p,k,j}(t-1) + \alpha \cdot H_{p,k,j}(t)$

If the pixel was classified as foreground, its background model is not updated:
$B_p(t) = B_p(t-1)$

### Summary

*   **Family:** Local Binary Pattern (LBP) Modeling.
*   **Idea (from code):** For each pixel, it computes a histogram of Local Binary Pattern (LBP) codes from a surrounding region, doing this for each RGB channel. The background model is a reference LBP histogram for each pixel. It measures the similarity between the current and background histograms using histogram intersection. If the similarity is below a threshold, the pixel is foreground. The background model is updated via a running average on the histogram bins, but only for background pixels.
*   **Strengths:** Robust to gradual, monotonic illumination changes due to the nature of LBP. It effectively uses local texture information, allowing it to differentiate objects in textured scenes where color-based methods might fail.
*   **Weaknesses:** Computationally expensive due to the multi-stage process (LBP, regional histogramming, comparison) for every pixel. It has a high memory footprint, storing multiple histograms per pixel. It performs poorly on untextured surfaces, where LBP features are not discriminative. The model is single-mode and cannot handle multiple background textures in the same region.

---

## DPWrenGA
---

### Algorithm Description

The `DPWrenGA` algorithm models the background of each pixel using a single Gaussian distribution over the RGB color space. This Gaussian is defined by a mean color vector and a single, shared variance value for all three color channels (i.e., a spherical covariance). A pixel is classified as foreground if its squared Euclidean distance from the mean of the background model is greater than the model's variance scaled by a threshold factor. The model's mean and variance are updated for pixels classified as background using a running average.

### Mathematical Formulation

For each pixel $p$, the background color is modeled by a single multi-variate Gaussian distribution, $B_p(t)$, with a mean vector $\vec{\mu}_p(t) \in \mathbb{R}^3$ and a spherical covariance matrix $\sigma_p^2(t)I$, where $I$ is the $3 \times 3$ identity matrix.

$B_p(t) \sim \mathcal{N}(\vec{\mu}_p(t), \sigma_p^2(t)I)$

The algorithm's behavior is controlled by the following parameters:
*   $L$: `learningFrames`, the duration of the initial, unconditional update phase.
*   $\alpha$: `Alpha`, the learning rate.
*   $T_D$: `LowThreshold`, the threshold factor for foreground classification.
*   $\sigma^2_{init}$: `m_variance`, the initial variance for new models (hardcoded to 36.0).

#### **1. Initialization**

At time $t=0$, for each pixel $p$:
*   The mean $\vec{\mu}_p(0)$ is initialized with the color of the input pixel from the first frame, $\vec{I}(p,0)$.
*   The variance $\sigma^2_p(0)$ is initialized to the global starting variance, $\sigma^2_{init}$.

#### **2. Foreground Detection**

At each frame $t > 0$, for each pixel $p$, the algorithm effectively calculates the squared Mahalanobis distance to the background model. Given the spherical covariance, this simplifies to the squared Euclidean distance scaled by the variance:

$d^2_M(p,t) = \frac{||\vec{I}(p,t) - \vec{\mu}_p(t-1)||^2}{\sigma^2_p(t-1)}$

A pixel is classified as foreground if this distance exceeds the threshold factor $T_D$. The foreground mask $F(p,t)$ is defined as:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } d^2_M(p,t) > T_D \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

This is equivalent to the implementation's comparison: $||\vec{I}(p,t) - \vec{\mu}_p(t-1)||^2 > T_D \cdot \sigma^2_p(t-1)$.

#### **3. Background Model Update**

The model for pixel $p$ is updated if the pixel is classified as background ($F(p,t)=0$) or if the algorithm is in its initial learning phase ($t < L$).

If the update condition is met:
*   **Mean Update:** The mean is updated via an exponential moving average:
    $\vec{\mu}_p(t) = (1-\alpha)\vec{\mu}_p(t-1) + \alpha \cdot \vec{I}(p,t)$

*   **Variance Update:** The variance is updated using a running average of the squared Euclidean distance between the input pixel and the *previous* mean. Let $d^2(p,t) = ||\vec{I}(p,t) - \vec{\mu}_p(t-1)||^2$.
    $\sigma^2_p(t) = (1-\alpha)\sigma^2_p(t-1) + \alpha \cdot d^2(p,t)$

    The newly calculated variance is then clamped to a fixed range:
    $\sigma^2_p(t) = \max(4, \min(\sigma^2_p(t), 5 \cdot \sigma^2_{init}))$

If the update condition is not met, the model's parameters remain unchanged:
*   $\vec{\mu}_p(t) = \vec{\mu}_p(t-1)$
*   $\sigma^2_p(t) = \sigma^2_p(t-1)$

### Summary

*   **Family:** Per-pixel Single Gaussian Model.
*   **Idea (from code):** Models each pixel's background with a single Gaussian distribution defined by a mean RGB color and a single shared variance. A pixel is classified as foreground if its squared Euclidean distance to the mean is greater than the model's variance scaled by a threshold. The mean and variance are updated with a running average, but only for pixels classified as background.
*   **Strengths:** Fast, simple, and has a low memory footprint. The model is adaptive to gradual changes and the selective update rule prevents stationary foreground objects from corrupting the background.
*   **Weaknesses:** Cannot handle multimodal backgrounds (e.g., waving trees) as it only stores one Gaussian per pixel. The use of a single variance for all color channels is a strong assumption that may not hold true. It is slow to adapt to sudden, large-scale scene changes.

---

## DPZivkovicAGMM
---

### Algorithm Description

This algorithm models each pixel's color distribution as a Mixture of Gaussians (GMM), similar to the Stauffer and Grimson method. Each Gaussian has a mean, a shared variance, and a weight. The key distinction of this implementation lies in the weight update rule. Instead of a simple learning rate update, it applies a constant penalty (`complexity_prior`) to all Gaussians in every frame, causing insignificant Gaussians to fade away more quickly. When a pixel matches a Gaussian, that Gaussian's weight gets a large positive update while all others are penalized. This mechanism helps in pruning unnecessary components from the model.

### Mathematical Formulation

For each pixel $p$, the color distribution is modeled as a mixture of up to $K_{max}$ (`MaxModes`) Gaussians:
$P(\vec{x}) = \sum_{k=1}^{K_p} \omega_{p,k} \cdot \eta(\vec{x}; \vec{\mu}_{p,k}, \sigma^2_{p,k}I)$
where, for the $k$-th Gaussian of pixel $p$:
*   $\vec{x}$ is the pixel's RGB color vector.
*   $\omega_{p,k}$ is the weight.
*   $\vec{\mu}_{p,k}$ is the mean RGB color.
*   $\sigma^2_{p,k}$ is a single, shared variance for all channels.
*   $K_p$ is the current number of Gaussians for the pixel.

The algorithm's behavior is controlled by the following parameters:
*   $\alpha$: `Alpha`, the learning rate.
*   $C$: `complexity_prior`, a constant used to penalize model components.
*   $T_B$: `m_bg_threshold`, the weight threshold to determine the background model.
*   $T_D$: `LowThreshold`, a factor for the matching threshold.
*   $\sigma^2_{init}$: `m_variance`, the initial variance for new Gaussians.

#### **1. Background Model Identification and Matching**

These steps are identical to the standard GMM approach.
*   **Sorting:** The Gaussians for pixel $p$ are sorted in descending order by weight, $\omega_{p,k,t-1}$.
*   **Background Selection:** The first $B$ Gaussians in the sorted list are chosen as the background model, where $B$ is the smallest integer satisfying $\sum_{k=1}^{B} \omega_{p,k,t-1} > T_B$.
*   **Matching:** A match is found with the first Gaussian $m$ in the sorted list that satisfies the condition: $||\vec{x}_t - \vec{\mu}_{p,m,t-1}||^2 < T_D \cdot \sigma^2_{p,m,t-1}$.

#### **2. Parameter Update**

Let the pruning term be $P_c = \alpha \cdot C$. The update rules for the weights (before normalization) differ based on whether a match was found.

*   **Case 1: A match is found with Gaussian $m$.**
    *   **Weight Update:**
        *   For the matched Gaussian $m$:
            $\omega'_{p,m,t} = (1-\alpha)\omega_{p,m,t-1} + \alpha - P_c$
        *   For all other Gaussians $k \ne m$:
            $\omega'_{p,k,t} = (1-\alpha)\omega_{p,k,t-1} - P_c$
    *   **Mean and Variance Update (for Gaussian $m$ only):**
        Let the update factor be $\rho_t = \alpha / \omega_{p,m,t-1}$ (using the weight *before* this frame's update).
        *   $\vec{\mu}_{p,m,t} = (1-\rho_t)\vec{\mu}_{p,m,t-1} + \rho_t \vec{x}_t$
        *   $\sigma^2_{p,m,t} = (1-\rho_t)\sigma^2_{p,m,t-1} + \rho_t ||\vec{x}_t - \vec{\mu}_{p,m,t-1}||^2$
        *   The variance is then clamped: $\sigma^2_{p,m,t} = \max(4, \min(\sigma^2_{p,m,t}, 5 \cdot \sigma^2_{init}))$.

*   **Case 2: No match is found.**
    *   **Weight Update:** All existing Gaussians are penalized:
        $\omega'_{p,k,t} = (1-\alpha)\omega_{p,k,t-1} - P_c$ for all $k \in \{1, ..., K_{p,t-1}\}$.
    *   **Gaussian Pruning:** Any Gaussian $k$ whose updated weight $\omega'_{p,k,t}$ falls below $P_c$ is removed from the model.
    *   **Create New Gaussian:** A new Gaussian is created (replacing the weakest if $K_p = K_{max}$) with parameters:
        *   $\vec{\mu}_{new} = \vec{x}_t$
        *   $\sigma^2_{new} = \sigma^2_{init}$
        *   $\omega'_{new} = \alpha$ (or 1 if it's the first Gaussian for that pixel).

#### **3. Renormalization**

After the update, pruning, and creation steps, the set of temporary weights $\{\omega'_{p,k,t}\}$ for the pixel's current Gaussians are normalized to ensure their sum is 1, yielding the final weights for the next frame, $\{\omega_{p,k,t}\}$.

$\omega_{p,k,t} = \frac{\omega'_{p,k,t}}{\sum_{j} \omega'_{p,j,t}}$

### Summary

*   **Family:** Per-pixel Mixture of Gaussians (GMM) with Complexity Penalty.
*   **Idea (from code):** Models each pixel with a mixture of Gaussians, sorted by weight. The background consists of the top-weighted Gaussians that exceed a cumulative weight threshold. When a pixel matches a Gaussian, that Gaussian's weight is strongly increased. In every frame, all Gaussians are slightly penalized by a fixed "complexity prior," causing insignificant ones to fade and be removed. If no match is found, a new Gaussian is created.
*   **Strengths:** Handles multimodal backgrounds effectively. The complexity penalty provides a mechanism to automatically prune unnecessary Gaussians from the model, potentially making it more efficient and stable than a standard GMM.
*   **Weaknesses:** This implementation uses a fixed learning rate, not the adaptive one typically associated with Zivkovic's later work. It is computationally expensive and requires careful tuning of several parameters (`alpha`, `complexity_prior`, `threshold`). It assumes a single shared variance for all color channels.

---

## FrameDifference
---

### Algorithm Description

This algorithm implements the classic frame differencing method for motion detection. The background model is simply the previous frame in the video sequence. The foreground is detected by calculating the absolute difference between the current frame and the previous frame. This difference image is then converted to grayscale and thresholded to produce a binary foreground mask. After processing, the current frame becomes the background model for the next frame.

### Mathematical Formulation

Let $I(p, t)$ be the pixel value (or color vector) at position $p=(x,y)$ at time $t$.
Let $B(p, t)$ be the background model for pixel $p$ at time $t$.

The algorithm's behavior is controlled by a single parameter:
*   $\tau$: `threshold`, the brightness threshold for foreground detection.

#### **1. Initialization (at $t=0$)**

The background model is initialized with the first frame. No foreground detection is performed at this step.
$B(p, 0) = I(p, 0)$
The foreground mask $F(p, 0)$ is empty or all zeros.

#### **2. Foreground Detection (for time $t > 0$)**

**a. Difference Calculation:**
The background model used for subtraction is the model from the previous time step, which is simply the previous frame: $B(p, t-1) = I(p, t-1)$.

The absolute difference $D(p,t)$ between the current frame and the previous frame is calculated:
$D(p,t) = |I(p,t) - B(p,t-1)| = |I(p,t) - I(p,t-1)|$

**b. Grayscale Conversion:**
If the input image (and thus the difference image) is in color, the difference image $D(p,t)$ is converted to a single-channel grayscale image, $D_{gray}(p,t)$. Using the standard formula for BGR to grayscale conversion:
$D_{gray}(p,t) = 0.114 \cdot D_B(p,t) + 0.587 \cdot D_G(p,t) + 0.299 \cdot D_R(p,t)$

If the input is already grayscale, then $D_{gray}(p,t) = D(p,t)$.

**c. Thresholding:**
If the `enableThreshold` parameter is true, a binary foreground mask $F(p,t)$ is generated by applying the threshold $\tau$ to the grayscale difference image:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } D_{gray}(p,t) > \tau \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

If `enableThreshold` is false, the output is the non-thresholded grayscale difference image, $F(p,t) = D_{gray}(p,t)$.

#### **3. Background Model Update**

After the foreground mask is computed, the background model is updated to be the current frame, in preparation for the next iteration:
$B(p,t) = I(p,t)$

### Summary

*   **Family:** Temporal Differencing.
*   **Idea (from code):** Uses the previous frame as the background model. It calculates the absolute difference between the current and previous frames, converts the result to grayscale, and applies a fixed threshold to create the foreground mask. The current frame then becomes the background for the next iteration.
*   **Strengths:** Extremely fast and requires very little memory (storage for only one previous frame). It is very simple to implement.
*   **Weaknesses:** Only detects the edges of moving objects, not their whole bodies (the "aperture problem"). Any object that stops moving is immediately absorbed into the background. It is highly sensitive to camera jitter and global illumination changes.

---

## FuzzyChoquetIntegral
---

### Algorithm Description

This algorithm is a feature-fusion method for background subtraction. It maintains a single, running-average background model. For each frame, it computes multiple "dissimilarity" features between the current frame and the background model. These features include texture dissimilarity (derived from Local Binary Patterns) and color dissimilarity (derived from color components in a chosen space). These multiple sources of evidence are then fused into a single value for each pixel using a **Fuzzy Choquet Integral**. This integral provides a sophisticated, weighted aggregation of the features, controlled by a set of fuzzy measures that define the importance of each feature and their combinations. The resulting "fused similarity" image is then thresholded to produce the foreground mask. The background model is updated selectively for pixels that are deemed to be background.

### Mathematical Formulation

Let $\vec{I}(p,t) \in \mathbb{R}^3$ be the input color vector for a pixel $p=(x,y)$ at time $t$.
Let $\vec{B}(p,t) \in \mathbb{R}^3$ be the floating-point background model color vector.

The algorithm's behavior is controlled by the following parameters:
*   $T_L$: `framesToLearn`, the duration of the initial learning phase.
*   $\alpha_L$: `alphaLearn`, the learning rate during the initial phase.
*   $\alpha_U$: `alphaUpdate`, the learning rate for the selective update phase.
*   $\tau$: `threshold`, the threshold applied to the fused similarity image.
*   $\{g_i\}$: A set of fuzzy measures defining the importance of each feature source.

#### **1. Initial Learning Phase ($t \le T_L$)**

During the initial phase, the background model is learned via a simple running average of the input frames. No foreground detection occurs.
$\vec{B}(p,t) = \alpha_L \cdot \vec{I}(p,t) + (1 - \alpha_L) \cdot \vec{B}(p,t-1)$
(with $\vec{B}(p,0) = \vec{I}(p,0)$).

#### **2. Feature Dissimilarity Calculation ($t > T_L$)**

For each pixel $p$, a set of $n$ feature dissimilarity values, $H_p = \{h_1, h_2, \dots, h_n\}$, is computed between the input frame $\vec{I}(p,t)$ and the background model $\vec{B}(p,t-1)$. The implementation uses $n=3$.

*   **Texture Dissimilarity ($h_1$):** A Local Binary Pattern (LBP) operator is applied to the grayscale versions of the input and background frames. The dissimilarity between these LBP values is then computed.
    *   $L_{in}(p,t) = \text{LBP}(I_{gray}(p,t))$
    *   $L_{bg}(p,t-1) = \text{LBP}(B_{gray}(p,t-1))$
    *   $h_1(p,t) = \text{SimilarityDegree}(L_{in}(p,t), L_{bg}(p,t-1))$

*   **Color Dissimilarities ($h_2, h_3$):** The dissimilarity between the color components of the input and background frames is calculated.
    *   $\{h_2, h_3, \dots\} = \text{SimilarityDegreesImage}(\vec{I}(p,t), \vec{B}(p,t-1))$

These $h_i$ values represent the degree of evidence that the pixel belongs to the background, normalized to the range [0, 1].

#### **3. Fuzzy Fusion via Choquet Integral**

For each pixel $p$, the evidence values in the set $H_p = \{h_1, h_2, h_3\}$ are fused.

**a. Fuzzy Measures:**
A set of fuzzy densities $\{g_1, g_2, g_3\}$ is defined, representing the importance assigned to each feature source (e.g., for `option == 2`, these are $\{0.6, 0.3, 0.1\}$). From these densities, a fuzzy measure $\mu$ is constructed, which assigns a value to every subset of features.

**b. Choquet Integral Calculation:**
For each pixel, the evidence values are sorted in ascending order: $h_{(1)} \le h_{(2)} \le h_{(3)}$. Let the corresponding sets of sources be $A_{(i)} = \{(i), (i+1), \dots, (3)\}$. The discrete Choquet integral $C(p,t)$ is computed as a weighted sum:

$C(p,t) = \sum_{i=1}^{3} (h_{(i)} - h_{(i-1)}) \cdot \mu(A_{(i)})$

where:
*   $h_{(0)}$ is defined as 0.
*   $\mu(A_{(i)})$ is the fuzzy measure of the set of sources corresponding to the $i$-th and all subsequent elements in the sorted list. This value represents the importance of having at least this level of evidence.

The result $C(p,t)$ is a single value for each pixel representing the fused similarity to the background.

#### **4. Foreground Detection**

The fused similarity image $C(p,t)$ is thresholded to produce the final foreground mask $F(p,t)$. The use of `THRESH_BINARY_INV` indicates that low similarity corresponds to foreground.

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } C(p,t) < \tau \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **5. Background Model Update**

The background model is updated selectively. For each pixel $p$, the update is performed only if the pixel was classified as background (i.e., its fused similarity was high enough).

If $C(p,t) \ge \tau$:
$\vec{B}(p,t) = (1-\alpha_U)\vec{B}(p,t-1) + \alpha_U \vec{I}(p,t)$

Otherwise (if pixel is foreground):
$\vec{B}(p,t) = \vec{B}(p,t-1)$

### Summary

*   **Family:** Fuzzy Logic / Information Fusion.
*   **Idea (from code):** Computes multiple dissimilarity scores between the current frame and a running-average background model, based on color and LBP texture features. These scores are then fused into a single similarity value for each pixel using a Fuzzy Choquet Integral, which acts as a sophisticated weighted aggregation. A pixel is classified as foreground if this final similarity score is below a threshold. The background model is updated selectively for background pixels.
*   **Strengths:** Fuses color and texture information, making it potentially more robust than single-feature methods. The Choquet integral provides a powerful way to combine evidence from different sources.
*   **Weaknesses:** Very computationally expensive due to the multi-stage feature extraction and fusion process. The underlying background model is a simple running average, which is slow to adapt and cannot handle multimodal backgrounds. The fuzzy measures that control the fusion are hardcoded and do not adapt to the scene.

---

## FuzzySugenoIntegral
---

### Algorithm Description

This algorithm, like its Choquet counterpart, is a feature-fusion method for background subtraction. It maintains a running-average background model. It extracts multiple features (LBP texture and color) to measure the dissimilarity between the current frame and the background model. These features, treated as sources of evidence, are then fused into a single value for each pixel using a **Fuzzy Sugeno Integral**. The Sugeno integral aggregates the evidence by taking the maximum of the minimums between sorted evidence values and their corresponding fuzzy measures. This fused "similarity" score is then thresholded to create the foreground mask. The background model is updated selectively for pixels classified as background.

### Mathematical Formulation

Let $\vec{I}(p,t) \in \mathbb{R}^3$ be the input color vector for a pixel $p=(x,y)$ at time $t$.
Let $\vec{B}(p,t) \in \mathbb{R}^3$ be the floating-point background model color vector.

The algorithm's behavior is controlled by the following parameters:
*   $T_L$: `framesToLearn`, the duration of the initial learning phase.
*   $\alpha_L$: `alphaLearn`, the learning rate during the initial phase.
*   $\alpha_U$: `alphaUpdate`, the learning rate for the selective update phase.
*   $\tau$: `threshold`, the threshold applied to the fused similarity image.
*   $\{g_i\}$: A set of fuzzy measures defining the importance of each feature source.

#### **1. Initial Learning Phase ($t \le T_L$)**

During this phase, the background model is learned via a simple running average.
$\vec{B}(p,t) = \alpha_L \cdot \vec{I}(p,t) + (1 - \alpha_L) \cdot \vec{B}(p,t-1)$
(with $\vec{B}(p,0) = \vec{I}(p,0)$).

#### **2. Feature Dissimilarity Calculation ($t > T_L$)**

For each pixel $p$, a set of $n=3$ feature evidence values (similarity degrees), $H_p = \{h_1, h_2, h_3\}$, is computed between the input frame $\vec{I}(p,t)$ and the background model $\vec{B}(p,t-1)$. These values are assumed to be in the range [0, 1] and represent the degree of similarity to the background for each feature.

#### **3. Fuzzy Fusion via Sugeno Integral**

For each pixel $p$, the evidence values in the set $H_p = \{h_1, h_2, h_3\}$ are fused.

**a. Fuzzy Measures:**
A set of fuzzy densities $\{g_1, g_2, g_3\}$ is defined (e.g., for `option == 2`, these are $\{0.6, 0.3, 0.1\}$). From these densities, a fuzzy measure $\mu$ is constructed, which assigns a value to every subset of features.

**b. Sugeno Integral Calculation:**
For each pixel, the evidence values are sorted in ascending order: $h_{(1)} \le h_{(2)} \le h_{(3)}$. Let the corresponding sets of sources be $A_{(i)} = \{(i), (i+1), \dots, (3)\}$. The discrete Sugeno integral $S(p,t)$ is computed as the maximum of a series of minimums:

$S(p,t) = \max_{i=1}^{3} \left[ \min(h_{(i)}, \mu(A_{(i)})) \right]$

where:
*   $\mu(A_{(i)})$ is the fuzzy measure of the set of sources corresponding to the $i$-th and all subsequent elements in the sorted list.

The result $S(p,t)$ is a single value for each pixel representing the fused similarity to the background.

#### **4. Foreground Detection**

The fused similarity image $S(p,t)$ is thresholded. The use of `THRESH_BINARY_INV` in the code indicates that a low similarity value corresponds to foreground.

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } S(p,t) < \tau \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **5. Background Model Update**

The background model is updated selectively. For each pixel $p$, the update is performed only if the pixel was classified as background.

If $S(p,t) \ge \tau$:
$\vec{B}(p,t) = (1-\alpha_U)\vec{B}(p,t-1) + \alpha_U \vec{I}(p,t)$

Otherwise (if pixel is foreground):
$\vec{B}(p,t) = \vec{B}(p,t-1)$

### Summary

*   **Family:** Fuzzy Logic / Information Fusion.
*   **Idea (from code):** Computes multiple dissimilarity scores between the current frame and a running-average background model, based on color and LBP texture features. These scores are then fused into a single similarity value using a Fuzzy Sugeno Integral, which aggregates the evidence using `min` and `max` operations. A pixel is classified as foreground if this final similarity score is below a threshold. The background model is updated selectively for background pixels.
*   **Strengths:** Fuses color and texture information, making it potentially more robust than single-feature methods. The Sugeno integral is computationally less demanding than the Choquet integral as it relies on simpler `min`/`max` operations.
*   **Weaknesses:** Still a very complex and computationally expensive algorithm. The advanced fusion logic is built upon a simple running-average background model that is slow to adapt and cannot handle multimodal scenes. The fuzzy measures that control the fusion are hardcoded and static. The output of the integral is less nuanced than a Cho-quet integral, acting more like a complex median filter.

---

## GMG
---

***Disclaimer:*** *The core implementation of this algorithm resides within the OpenCV library, and its source code is not directly analyzed here. This model is based on the parameters exposed in the wrapper code and the established understanding of the GMG algorithm as described in the paper "Statistical background modelling for foreground detection: A survey" by Godbehere, Matsukawa, and Goldberg (2012).*

### Algorithm Description

The GMG (Godbehere, Matsukawa, Goldberg) algorithm models the background of each pixel using a small, fixed number of Gaussian distributions (typically 3 to 5). Unlike a traditional GMM, it does not use a mixture model but rather a Bayesian framework to determine the most likely background model. A key feature of GMG is its use of a priori probabilities for the foreground and background, which allows it to adapt quickly. The algorithm first observes a number of frames to build an initial model. For subsequent frames, it determines the probability that each pixel belongs to the background model and thresholds this probability to segment the foreground.

### Mathematical Formulation

Let $\vec{x}_t$ be the RGB color vector of a pixel at time $t$. The background model for this pixel consists of a set of $K$ Gaussian distributions, $\{ \mathcal{N}(\vec{\mu}_k, \Sigma_k) \}_{k=1}^K$.

The algorithm's behavior is controlled by the following parameters exposed in the wrapper:
*   $T_{init}$: `initializationFrames`, the number of frames for initial model building.
*   $T_{fg}$: `decisionThreshold`, the probability threshold to classify a pixel as foreground.

#### **1. Initial Learning Phase ($t \le T_{init}$)**

During the first $T_{init}$ frames, the algorithm gathers statistics to build the initial Gaussian models for each pixel. This typically involves clustering the observed pixel values for each pixel location into $K$ groups and fitting a Gaussian distribution to each group. During this phase, no foreground output is generated.

#### **2. Foreground/Background Probability Calculation ($t > T_{init}$)**

For each new frame, the algorithm calculates the probability that the pixel $\vec{x}_t$ belongs to the background, $P(BG | \vec{x}_t)$. This is done using Bayes' theorem:

$P(BG | \vec{x}_t) = \frac{p(\vec{x}_t | BG) P(BG)}{p(\vec{x}_t | BG) P(BG) + p(\vec{x}_t | FG) P(FG)}$

where:
*   $p(\vec{x}_t | BG)$ is the likelihood of observing $\vec{x}_t$ given it is background. This is calculated from the pixel's set of $K$ background Gaussians. It is typically the maximum likelihood over the set:
    $p(\vec{x}_t | BG) = \max_{k=1..K} \left[ \frac{1}{(2\pi)^{3/2}|\Sigma_k|^{1/2}} \exp\left(-\frac{1}{2}(\vec{x}_t - \vec{\mu}_k)^T \Sigma_k^{-1} (\vec{x}_t - \vec{\mu}_k)\right) \right]$

*   $p(\vec{x}_t | FG)$ is the likelihood of observing $\vec{x}_t$ given it is foreground. This is often modeled as a uniform distribution, implying that any color is equally likely for a foreground object.

*   $P(BG)$ and $P(FG)$ are the prior probabilities of a pixel being background or foreground, respectively. These priors are updated over time based on previous classifications, allowing the model to adapt. $P(BG) + P(FG) = 1$.

#### **3. Foreground Detection**

A pixel is classified as foreground if its probability of being background is below the decision threshold. The foreground mask $F(p,t)$ is defined as:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } P(BG | \vec{x}_t) < (1 - T_{fg}) \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

*(Note: The OpenCV implementation uses a `decisionThreshold` where higher values mean it is easier to be classified as foreground. This implies the comparison is likely $P(FG | \vec{x}_t) > T_{fg}$, which is equivalent to the formulation above).*

#### **4. Model Update**

The parameters of the Gaussian models ($\vec{\mu}_k, \Sigma_k$) and the prior probabilities ($P(BG), P(FG)$) are updated based on the classification. If a pixel is classified as background, the parameters of the best-matching Gaussian are updated to include the new pixel value. The prior $P(BG)$ is increased. If the pixel is classified as foreground, $P(FG)$ is increased. This update mechanism allows the model to adapt to changes in the background over time.

### Summary

*   **Family:** Bayesian Estimation / Statistical Modeling.
*   **Idea (from code):** This is a wrapper for the OpenCV `BackgroundSubtractorGMG` algorithm. It initializes the model over a set number of frames. For each subsequent frame, it calculates the probability of the pixel belonging to the background. It then classifies the pixel as foreground if this probability is below a `decisionThreshold`. The core logic for model creation and updates is handled internally by the OpenCV library.
*   **Strengths:** Based on a well-regarded algorithm that performs well on many benchmarks. It is generally robust to noise and gradual lighting changes. The use of priors allows for faster adaptation than some GMM methods.
*   **Weaknesses:** The core implementation is a black box within the OpenCV library, offering limited customization beyond a few high-level parameters. It requires an initial training period where the scene must be static. Performance can degrade with sudden, large-scale illumination changes.

---

## IndependentMultimodal (IMBS)
---

### Algorithm Description

The Independent Multimodal Background Subtractor (IMBS) is a non-parametric, sample-based algorithm. It operates in distinct cycles of sampling and model creation. For each pixel, it collects a buffer of $N$ color samples over a period of time. Once the buffer is full, it builds a background model by clustering these samples. Each cluster, or "bin," is represented by its mean color and its size (height). Bins with a height above a certain minimum are considered valid background components. Foreground detection is then performed by comparing the current pixel's color to the set of valid background bin colors. If the distance to the nearest background bin is above a threshold, the pixel is marked as foreground. The algorithm includes sophisticated post-processing, including HSV-based shadow/highlight suppression and a persistence mechanism to incorporate stationary foreground objects into the background over time.

### Mathematical Formulation

For each pixel $p$, the algorithm maintains a temporary sample buffer $S_p = \{\vec{s}_1, \vec{s}_2, \dots, \vec{s}_N\}$, where $N$ is `numSamples`. The background model for pixel $p$, denoted $M_p$, is a set of up to $K$ (`maxBgBins`) valid background "bins," $M_p = \{b_1, b_2, \dots, b_K\}$. Each bin $b_k$ is a tuple $b_k = \langle \vec{v}_k, c_k, f_k \rangle$, where:
*   $\vec{v}_k$: The mean RGB color of the bin.
*   $c_k$: The height (count) of the bin.
*   $f_k$: A boolean flag indicating if the bin was learned from foreground pixels.

**Parameters:**
*   $N$: `numSamples`, the number of samples collected before model creation.
*   $P_S$: `samplingPeriod`, the time in milliseconds between collecting samples.
*   $H_{min}$: `minBinHeight`, the minimum number of samples a bin must contain to be considered part of the background model.
*   $\tau_{assoc}$: `associationThreshold`, the L-infinity distance for associating a new sample with an existing bin during model creation.
*   $\tau_{fg}$: `fgThreshold`, the L-infinity distance for classifying a pixel as foreground.
*   $P_p$: `persistencePeriod`, the time a foreground object must be static before being absorbed into the background.

#### **Phase 1: Sample Collection**

The algorithm collects $N$ frames at intervals of $P_S$ milliseconds. Let these frames be $\vec{I}(p, t_1), \vec{I}(p, t_2), \dots, \vec{I}(p, t_N)$. These $N$ samples form the buffer $S_p$ for each pixel.

#### **Phase 2: Background Model Creation (at the end of a cycle)**

Once $N$ samples are collected, the background model $M_p$ is created from the sample buffer $S_p$.

1.  **Clustering/Binning:** The samples in $S_p$ are clustered to form a temporary set of bins.
    *   The first sample $\vec{s}_1$ creates the first bin with mean $\vec{v}_1 = \vec{s}_1$ and height $c_1=1$.
    *   For each subsequent sample $\vec{s}_i \in S_p$, find the first existing bin $b_j$ such that the L-infinity distance is below the association threshold:
        $d_\infty(\vec{s}_i, \vec{v}_j) = \max_{k \in \{R,G,B\}} |s_{i,k} - v_{j,k}| \le \tau_{assoc}$
    *   **If a match is found:** The matched bin's parameters are updated with a running average:
        *   $\vec{v}_j \leftarrow \frac{\vec{v}_j \cdot c_j + \vec{s}_i}{c_j + 1}$
        *   $c_j \leftarrow c_j + 1$
    *   **If no match is found:** A new bin is created with $\vec{v}_{new} = \vec{s}_i$ and $c_{new}=1$.

2.  **Model Filtering and Sorting:**
    From the temporary bins created above, only those with a height $c_k \ge H_{min}$ are considered valid. These valid bins form the background model $M_p$ for the next cycle. They are then sorted in descending order of their height $c_k$.

#### **Phase 3: Foreground Detection (during the next cycle)**

For each incoming frame $\vec{I}(p,t)$ in the next cycle:

1.  **Matching:** Find the minimum L-infinity distance between the input pixel $\vec{I}(p,t)$ and the mean color $\vec{v}_k$ of every valid, non-foreground bin in the background model $M_p$.
    $d_{min}(p,t) = \min_{k | f_k=\text{false}} d_\infty(\vec{I}(p,t), \vec{v}_k)$

2.  **Classification:** The pixel is classified as foreground if this minimum distance is greater than the foreground threshold.
    $F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } d_{min}(p,t) > \tau_{fg} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **4. Special Cases and Post-Processing**

*   **Persistence:** If a pixel matches a bin that was marked as foreground ($f_k=\text{true}$), it is given a special label (`PERSISTENCE_LABEL`). If a pixel remains in this state for a cumulative duration exceeding $P_p$, the `isFg` flags ($f_k$) for all of its background model bins are reset to `false`, effectively absorbing the object into the background.
*   **Shadow/Highlight Suppression:** Pixels initially classified as foreground are converted to the HSV color space. A pixel is re-classified as a shadow (`SHADOW_LABEL`) if its color satisfies a specific set of conditions on hue, saturation, and value distortion relative to the background model bins.
*   **Morphological Filtering:** Optional morphological opening and closing operations are applied to the final foreground mask to remove noise.

### Summary

*   **Family:** Non-parametric Sample-based Clustering.
*   **Idea (from code):** Operates in cycles. It collects a buffer of N pixel color samples over time. Once full, it builds a background model for each pixel by clustering these samples based on color proximity (L-infinity distance). Clusters with enough samples become background "bins." In the next cycle, a pixel is foreground if its color is not close to any of its background bins. It includes HSV-based shadow detection and a persistence mechanism to absorb static foreground objects into the background.
*   **Strengths:** Non-parametric, so it can model complex, arbitrary background distributions without assuming a Gaussian shape. Can handle multimodal backgrounds effectively. The persistence mechanism provides a clear rule for adapting to "sleeping" foreground objects.
*   **Weaknesses:** High memory usage due to storing a large sample buffer for every pixel. The model is static between long sampling cycles, so it cannot adapt to rapid background changes. Performance is highly sensitive to many parameters (sampling period, number of samples, thresholds).

---

## KDE
---

### Algorithm Description

This algorithm is a non-parametric background subtraction method that uses Kernel Density Estimation (KDE) to model the probability distribution of each pixel's color. For each pixel, it maintains a buffer of its last $N$ observed color values. The probability of a new pixel color belonging to the background is estimated by summing the contributions of a kernel function (pre-calculated in a Look-Up Table) centered at each of the $N$ historical samples. A pixel is classified as foreground if this estimated probability is below a certain threshold. The historical sample buffer is updated over time using a sophisticated temporal buffering and selective update mechanism to adapt to changes in the background. The algorithm can operate in either RGB or a custom color ratio space, and it can dynamically estimate the kernel bandwidth (standard deviation) for each pixel channel.

### Mathematical Formulation

For each pixel $p$, the background is represented by a sample buffer of its last $N$ (`SampleSize`) observed color vectors: $S_p(t) = \{\vec{s}_{p,1}, \vec{s}_{p,2}, \dots, \vec{s}_{p,N}\}$.

**Parameters:**
*   $N$: `SampleSize`, the number of historical samples per pixel.
*   $\tau$: `Threshold`, the probability threshold for foreground classification.
*   $\vec{\sigma}_p$: A vector of standard deviations (kernel bandwidths) for each color channel of pixel $p$. This can be estimated or fixed.

#### **1. Optional Color Space Conversion**

If `UseColorRatiosFlag` is true, the input RGB color vector $\vec{I}(p,t) = (B, G, R)$ is converted to a custom 3-channel space $(C_1, C_2, C_3)$:
*   $C_1 = (B+G+R)/3$ (Intensity)
*   $C_2 = \text{clip}(255 \cdot \frac{G+10}{B+G+R+30}, 0, 255)$ (Green ratio)
*   $C_3 = \text{clip}(255 \cdot \frac{R+10}{B+G+R+30}, 0, 255)$ (Red ratio)
All subsequent calculations are performed in this space. Otherwise, standard RGB is used.

#### **2. Probability Density Estimation**

The probability density of a new observed pixel color $\vec{x}_t$ at pixel $p$ is estimated using the Parzen-window method. Assuming independence between color channels, the joint probability is the product of the individual channel probabilities:

$P(\vec{x}_t | S_p(t-1)) = \prod_{k=1}^{3} P(x_{t,k} | S_{p,k}(t-1))$

where $S_{p,k}$ is the set of historical samples for channel $k$. The probability for a single channel $k$ is the average of the kernel function evaluated at each sample:

$P(x_{t,k} | S_{p,k}(t-1)) = \frac{1}{N} \sum_{i=1}^{N} K(x_{t,k} - s_{p,k,i}; \sigma_{p,k})$

The implementation uses a pre-computed Gaussian kernel Look-Up Table (LUT), so the kernel function is:
$K(u; \sigma) = \frac{1}{\sqrt{2\pi}\sigma} e^{-\frac{u^2}{2\sigma^2}}$
The code approximates this by directly looking up the value in the `kerneltable` based on the difference $u = x_{t,k} - s_{p,k,i}$ and the bandwidth bin $\sigma_{p,k}$.

The joint probability is therefore:
$P(\vec{x}_t | S_p(t-1)) = \prod_{k=1}^{3} \left( \frac{1}{N} \sum_{i=1}^{N} K(x_{t,k} - s_{p,k,i}; \sigma_{p,k}) \right)$

The implementation simplifies this by summing the kernel responses directly:
$\text{Sum}(p,t) = \sum_{i=1}^{N} \left( \prod_{k=1}^{3} K(x_{t,k} - s_{p,k,i}; \sigma_{p,k}) \right)$

The final probability score used for thresholding is this sum normalized by the number of samples considered, $j \le N$:
$\text{Score}(p,t) = \frac{\text{Sum}(p,t)}{j}$

#### **3. Foreground Detection**

A pixel is classified as foreground if its probability score is below the threshold $\tau$. The code uses an early exit optimization where the summation stops if the running sum exceeds $\tau \cdot N$.

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } \text{Score}(p,t) \le \tau \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **4. Background Model Update**

The update mechanism is complex, involving a main sample buffer (`Sequence`) and a temporary circular buffer (`TemporalBuffer`).

1.  The current frame and its foreground mask are added to the `TemporalBuffer`.
2.  Periodically, a pair of frames is selected from the `TemporalBuffer`.
3.  For pixels that were marked as background in both corresponding masks, this pair of frames is used to replace the oldest pair of frames in that pixel's main sample buffer (`Sequence`).
4.  This ensures that only verified background pixels are used to update the long-term background model.

#### **5. Bandwidth (SD) Estimation**

If `SdEstimateFlag` is true, the kernel bandwidth $\sigma_{p,k}$ for each channel of each pixel is dynamically estimated. This is done by:
1.  For each pixel, creating a histogram of the absolute differences between consecutive frames in its sample buffer.
2.  Calculating the median of this difference histogram.
3.  The standard deviation is estimated as a scaled version of this median ($\sigma \approx 1.04 \cdot \text{median}$). This value is then mapped to a discrete bin to be used with the kernel LUT.

### Summary

*   **Family:** Non-parametric Kernel Density Estimation (KDE).
*   **Idea (from code):** Models the background of each pixel by storing a history of its last N color samples. The probability of a new pixel's color is estimated by summing the values of a Gaussian kernel centered on each of the N samples. If this probability is below a threshold, the pixel is foreground. The kernel's bandwidth (standard deviation) can be dynamically estimated from the median of inter-frame differences. The sample history is updated selectively using a temporal buffer to ensure only background pixels are added.
*   **Strengths:** Non-parametric, allowing it to model complex, multimodal background distributions without assuming a specific shape (like Gaussian). Can dynamically adapt the kernel bandwidth for each pixel channel. The selective update mechanism is robust.
*   **Weaknesses:** Very high memory consumption, as it stores a large sample buffer for every pixel. Extremely high computational cost due to the density estimation requiring a loop over all N samples for every pixel. Performance is sensitive to the number of samples and the probability threshold.

---

## KNN
---

***Disclaimer:*** *The core implementation of this algorithm resides within the OpenCV library, and its source code is not directly analyzed here. This model is based on the parameters exposed in the wrapper code and the established understanding of the KNN-based background subtraction algorithm as described in the paper by Zivkovic and van der Heijden (2006).*

### Algorithm Description

This algorithm is a non-parametric background subtraction method that models the probability density of each pixel's color using a history of recently observed color values. Unlike Kernel Density Estimation (KDE) which sums kernel responses over all samples, this method uses a K-Nearest Neighbors (K-NN) approach. To classify a new pixel, it calculates the distance to all samples in its history and counts how many of these samples are "close" (within a given radius). If this count is below a threshold (K), the pixel is considered foreground. This is an efficient approximation of a full K-NN search. The model is adaptive, with new observations continuously replacing the oldest ones in the sample history.

### Mathematical Formulation

For each pixel $p$, the background is represented by a sample buffer of its last $N$ (`history`) observed color vectors: $S_p(t) = \{\vec{s}_{p,1}, \vec{s}_{p,2}, \dots, \vec{s}_{p,N}\}$.

The algorithm's behavior is controlled by the following parameters exposed in the wrapper:
*   $N$: `history`, the number of historical samples per pixel.
*   $\tau^2_D$: `dist2Threshold`, the squared distance threshold to determine if a sample is a "neighbor."
*   $K$: The number of neighbors required to classify a pixel as background. This is not directly exposed as a parameter in the wrapper but is a core concept of the underlying algorithm, implicitly related to the density of the background distribution.

#### **1. Foreground Detection**

For a new observed pixel color $\vec{x}_t$ at pixel $p$, the algorithm determines how many of the historical samples in the buffer $S_p(t-1)$ are "close" to it.

**a. Distance Calculation:**
The squared Euclidean distance is calculated between the new pixel $\vec{x}_t$ and each sample $\vec{s}_{p,i}$ in the history buffer:
$d^2_i = ||\vec{x}_t - \vec{s}_{p,i}||^2$

**b. Neighbor Counting:**
A sample $\vec{s}_{p,i}$ is considered a "neighbor" if its squared distance to the new pixel is within the threshold $\tau^2_D$. The algorithm counts the number of such neighbors, $C_p(t)$:

$C_p(t) = \sum_{i=1}^{N} \mathbb{I}(d^2_i \le \tau^2_D)$
where $\mathbb{I}(\cdot)$ is the indicator function (1 if the condition is true, 0 otherwise).

**c. Classification:**
The pixel is classified as foreground if the number of close neighbors $C_p(t)$ is less than a required minimum, $K$. The exact value of $K$ is an internal parameter of the OpenCV implementation, but conceptually, the rule is:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } C_p(t) < K \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **2. Background Model Update**

The background model is updated using a simple First-In, First-Out (FIFO) circular buffer. The new observation $\vec{x}_t$ is added to the sample buffer $S_p$, and the oldest sample in the buffer is discarded. This update happens for every frame, regardless of whether the pixel was classified as foreground or background, ensuring the model continuously adapts.

$S_p(t) = (S_p(t-1) \setminus \{\vec{s}_{oldest}\}) \cup \{\vec{x}_t\}$

#### **3. Shadow Detection**

The algorithm also includes a shadow detection mechanism, controlled by `shadowThreshold`. If a pixel is classified as foreground but its color is "similar" to the background model (i.e., a significant change in brightness but not in chrominance), it is labeled as a shadow (typically with a gray value like 127) instead of foreground. The exact color distortion model is internal to the OpenCV implementation.

### Summary

*   **Family:** Non-parametric K-Nearest Neighbors (K-NN).
*   **Idea (from code):** This is a wrapper for the OpenCV `BackgroundSubtractorKNN` algorithm. For each pixel, it maintains a history of its last `history` color samples. To classify a new pixel, it counts how many historical samples are within a `dist2Threshold` (squared Euclidean distance). If this count is below an internal threshold (K), the pixel is foreground. The model continuously updates by replacing the oldest sample with the current one. It also includes shadow detection based on `shadowThreshold`.
*   **Strengths:** Non-parametric, so it can model complex, multimodal background distributions. It is adaptive and can handle gradual changes in the background. Includes built-in shadow detection.
*   **Weaknesses:** The core implementation is a black box within the OpenCV library, limiting direct control over the K-NN parameters (like K itself). High memory consumption due to storing a history of samples for every pixel. Computationally more expensive than simpler methods.

---

## LBAdaptiveSOM
---

### Algorithm Description

This algorithm models the background of an entire image using a single, large Self-Organizing Map (SOM). However, it is conceptually structured as if each pixel has its own local $M \times N$ (hardcoded to $3 \times 3$) SOM. For each incoming pixel, the algorithm finds the Best Matching Unit (BMU) within its corresponding local SOM. If the distance to this BMU is below a threshold, the pixel is classified as background. In this case, the BMU and its neighboring nodes in the SOM are updated to move closer to the input pixel's color, with the update strength determined by a Gaussian-like kernel. If the distance is above the threshold, the pixel is foreground, and no update occurs. The algorithm has two phases: an initial training phase with a higher learning rate that decays over time, and a stable online phase with a low, constant learning rate.

### Mathematical Formulation

For each pixel location $p=(i,j)$ in the input image, there is a corresponding $3 \times 3$ grid of SOM nodes. Let the weight vector (RGB color) of a node at position $(k,l)$ within the local SOM for pixel $p$ be denoted by $\vec{w}_{p,k,l}(t)$.

**Parameters:**
*   $T_{steps}$: `trainingSteps`, the duration of the initial training phase.
*   $\epsilon_1$: `epsilon1`, the squared distance threshold during training.
*   $\epsilon_2$: `epsilon2`, the squared distance threshold during the online phase.
*   $\alpha_1$: `alpha1`, the initial learning rate factor for training.
*   $\alpha_2$: `alpha2`, the constant learning rate factor for the online phase.
*   $W$: A $3 \times 3$ neighborhood kernel, pre-computed from Pascal's triangle to approximate a Gaussian.

#### **1. Initialization**

At time $t=0$, the weight vectors $\vec{w}_{p,k,l}(0)$ for all nodes in all local SOMs are initialized with the color of their corresponding pixel $p$ from the first frame, $\vec{I}(p,0)$.

#### **2. Phase-Dependent Parameter Selection**

At each frame $t$, the matching threshold $\epsilon(t)$ and the base learning rate $\alpha_{base}(t)$ are selected based on whether the algorithm is in the training or online phase. Let the frame counter be $K$.

*   **If $K \le T_{steps}$ (Training Phase):**
    *   $\epsilon(t) = \epsilon_1$
    *   The learning rate decays linearly: $\alpha_{base}(t) = \alpha_1 - K \cdot \frac{\alpha_1 - \alpha_2}{T_{steps}}$

*   **If $K > T_{steps}$ (Online Phase):**
    *   $\epsilon(t) = \epsilon_2$
    *   The learning rate is constant: $\alpha_{base}(t) = \alpha_2$

#### **3. BMU Search and Classification**

For each input pixel $\vec{I}(p,t)$:

**a. Find Best Matching Unit (BMU):**
The algorithm finds the node $(k^*, l^*)$ in the local SOM for pixel $p$ whose weight vector is closest to the input pixel's color. The distance is the squared Euclidean distance.

$(k^*, l^*) = \underset{k,l \in \{1,2,3\}}{\arg\min} ||\vec{I}(p,t) - \vec{w}_{p,k,l}(t)||^2$

Let the minimum distance be $d^2_{min} = ||\vec{I}(p,t) - \vec{w}_{p,k^*,l^*}(t)||^2$.

**b. Classification:**
The pixel is classified based on this minimum distance:

$F(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if } d^2_{min} \le \epsilon(t) \\ 255 \text{ (Foreground)} & \text{if } d^2_{min} > \epsilon(t) \end{cases}$

#### **4. SOM Weight Update**

The SOM is updated **only if** the pixel was classified as background.

If $F(p,t) = 0$, the weight vector of the BMU $(k^*, l^*)$ and its neighbors are updated. The update rule for a node $(k,l)$ in the neighborhood of the BMU is:

$\vec{w}_{p,k,l}(t+1) = \vec{w}_{p,k,l}(t) + \alpha_{eff}(k,l) \cdot (\vec{I}(p,t) - \vec{w}_{p,k,l}(t))$

where the effective learning rate, $\alpha_{eff}(k,l)$, is modulated by the neighborhood kernel $W$:

$\alpha_{eff}(k,l) = \alpha_{base}(t) \cdot W_{k-k^*, l-l^*}$

The neighborhood is a $3 \times 3$ region centered on the BMU. Nodes outside this neighborhood are not updated.

If the pixel was classified as foreground, no weights are updated:
$\vec{w}_{p,k,l}(t+1) = \vec{w}_{p,k,l}(t)$ for all $k,l$.

### Summary

*   **Family:** Self-Organizing Map (SOM) / Neural Network.
*   **Idea (from code):** Models the background of each pixel with a dedicated 3x3 Self-Organizing Map (SOM). For each input pixel, it finds the closest node (BMU) in its SOM. If the distance is below a threshold, the pixel is background; otherwise, it's foreground. If background, the BMU and its neighbors are updated to move closer to the input pixel's color, with the learning rate decaying during an initial training phase.
*   **Strengths:** Can model multimodal backgrounds by having multiple nodes per pixel. The neighborhood update ensures a smooth and topologically ordered representation of the background color space, making it robust to small camera jitter and noise.
*   **Weaknesses:** High memory usage, storing a 3x3 color grid for every pixel. Computationally expensive due to the BMU search and neighborhood update for every pixel. The model is slow to adapt to sudden, large-scale changes after the initial training phase due to the low learning rate.

---

## LBFuzzyAdaptiveSOM
---

### Algorithm Description

This algorithm enhances the standard Self-Organizing Map (SOM) approach by incorporating fuzzy logic into its classification and update rules. Like the `LBAdaptiveSOM`, it maintains a local $3 \times 3$ SOM for each pixel. For an incoming pixel, it finds the Best Matching Unit (BMU) in its local SOM. Instead of a crisp binary classification, it calculates a "fuzzy background membership" value based on how closely the pixel matches the BMU. This fuzzy value is then used for two purposes: 1) it is thresholded to make the final foreground/background decision, and 2) it modulates the learning rate for the SOM update. A pixel that is a very good match (low fuzzy value) will trigger a strong update, while a pixel that is a poor match (high fuzzy value) will trigger a very weak or no update, creating a smoother, more adaptive learning process.

### Mathematical Formulation

For each pixel location $p=(i,j)$, there is a corresponding $3 \times 3$ grid of SOM nodes. The weight vector (RGB color) of a node at position $(k,l)$ within the local SOM for pixel $p$ is denoted by $\vec{w}_{p,k,l}(t)$.

**Parameters:**
*   $T_{steps}$: `trainingSteps`, the duration of the initial training phase.
*   $\epsilon_1, \epsilon_2$: Squared distance thresholds for training and online phases.
*   $\alpha_1, \alpha_2$: Base learning rate factors for training and online phases.
*   $W$: A $3 \times 3$ neighborhood kernel.
*   $E_{fuzzy}$: `FUZZYEXP`, the exponent for the fuzzy learning rate calculation (hardcoded to -5.0).
*   $\tau_{fuzzy}$: `FUZZYTHRESH`, the threshold for the fuzzy background membership (hardcoded to 0.8).

#### **1. Phase-Dependent Parameter Selection**

This step is identical to the non-fuzzy SOM. At each frame $t$, the matching threshold $\epsilon(t)$ and the base learning rate $\alpha_{base}(t)$ are selected based on whether the frame counter $K$ is within the training phase ($K \le T_{steps}$).

#### **2. BMU Search and Fuzzy Membership Calculation**

For each input pixel $\vec{I}(p,t)$:

**a. Find Best Matching Unit (BMU):**
The algorithm finds the node $(k^*, l^*)$ in the local SOM for pixel $p$ that is closest to the input pixel's color.
$(k^*, l^*) = \underset{k,l \in \{1,2,3\}}{\arg\min} ||\vec{I}(p,t) - \vec{w}_{p,k,l}(t)||^2$
Let the minimum squared distance be $d^2_{min}$.

**b. Fuzzy Background Membership:**
A fuzzy membership value, $\mu_{BG}(p,t)$, is calculated, representing the degree to which the pixel belongs to the background. This value is the ratio of the minimum distance to the current phase's distance threshold, clamped to [0, 1].

$\mu_{BG}(p,t) = \min\left( \frac{d^2_{min}}{\epsilon(t)}, 1.0 \right)$

#### **3. Foreground/Background Classification**

The pixel is classified based on its fuzzy membership value:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } \mu_{BG}(p,t) \ge \tau_{fuzzy} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **4. Fuzzy SOM Weight Update**

Unlike the standard SOM, the update is performed for **every pixel**, but its strength is modulated by the fuzzy membership.

**a. Fuzzy Learning Rate:**
A fuzzy-modulated maximum learning rate, $\alpha_{max}(t)$, is calculated:

$\alpha_{max}(t) = \alpha_{base}(t) \cdot \exp(E_{fuzzy} \cdot \mu_{BG}(p,t))$

**b. Neighborhood Update:**
The weight vector of the BMU $(k^*, l^*)$ and its neighbors are updated. The update rule for a node $(k,l)$ in the neighborhood of the BMU is:

$\vec{w}_{p,k,l}(t+1) = \vec{w}_{p,k,l}(t) + \alpha_{eff}(k,l) \cdot (\vec{I}(p,t) - \vec{w}_{p,k,l}(t))$

where the effective learning rate, $\alpha_{eff}(k,l)$, is now modulated by both the neighborhood kernel $W$ and the fuzzy learning rate $\alpha_{max}(t)$:

$\alpha_{eff}(k,l) = \alpha_{max}(t) \cdot W_{k-k^*, l-l^*}$

This means that pixels with a high fuzzy membership (poor match) will have a very small $\alpha_{max}$, leading to a negligible update, while pixels with a low fuzzy membership (good match) will have a larger $\alpha_{max}$ and cause a more significant update.

### Summary

*   **Family:** Fuzzy Self-Organizing Map (SOM) / Neuro-Fuzzy System.
*   **Idea (from code):** Models each pixel's background with a 3x3 SOM. For each input pixel, it finds the closest node (BMU) and calculates a "fuzzy background membership" based on the distance. A pixel is foreground if this membership exceeds a threshold. The SOM's learning rate is modulated by this fuzzy membership, meaning good matches cause strong updates and poor matches cause weak updates.
*   **Strengths:** Can model multimodal backgrounds. The fuzzy update mechanism provides a more graceful and adaptive learning process than a hard-switched update, making it potentially more robust to noise and small variations.
*   **Weaknesses:** Very high memory usage and computational cost. The core logic is complex and depends on several interacting parameters. The underlying SOM is still slow to adapt to large, sudden changes after its initial training phase.

---

## LBFuzzyGaussian
---

### Algorithm Description

This algorithm models the background of each pixel using a single, independent Gaussian distribution for each color channel (R, G, B). It introduces fuzzy logic to both the classification and update steps. For each pixel, it calculates the squared Mahalanobis distance to the background model. This distance is then normalized to create a "fuzzy background membership" value, which represents the degree to which the pixel belongs to the background. This fuzzy value is thresholded to make the final foreground/background decision. Crucially, this fuzzy value also modulates the learning rate for the update of the Gaussian's mean and variance. A pixel that is a good match for the background model (low fuzzy value) will cause a strong update, while a poor match will cause a very weak update, leading to a smooth and adaptive learning process.

### Mathematical Formulation

For each pixel $p$, the background is modeled by a single multi-variate Gaussian with a diagonal covariance matrix. This is equivalent to three independent Gaussians, one for each channel $k \in \{R,G,B\}$. The model for pixel $p$ is defined by a mean vector $\vec{\mu}_p(t) = (\mu_{p,R}, \mu_{p,G}, \mu_{p,B})$ and a variance vector $\vec{\sigma}^2_p(t) = (\sigma^2_{p,R}, \sigma^2_{p,G}, \sigma^2_{p,B})$.

**Parameters:**
*   $\alpha_{max}$: `alphamax`, the maximum learning rate.
*   $\tau^2_M$: `threshold`, the squared Mahalanobis distance threshold factor.
*   $\tau_{BG}$: `threshBG`, the fuzzy membership threshold for background classification.
*   $\sigma^2_{noise}$: `noise`, the minimum variance for any channel.
*   $E_{fuzzy}$: `FUZZYEXP`, the exponent for the fuzzy learning rate calculation (hardcoded to -5.0).

#### **1. Initialization**

At time $t=0$, for each pixel $p$:
*   The mean $\vec{\mu}_p(0)$ is initialized with the color of the input pixel from the first frame, $\vec{I}(p,0)$.
*   The variance for each channel is initialized to the global noise variance: $\vec{\sigma}^2_p(0) = (\sigma^2_{noise}, \sigma^2_{noise}, \sigma^2_{noise})$.

#### **2. Fuzzy Membership Calculation**

For each input pixel $\vec{I}(p,t)$:

**a. Squared Mahalanobis Distance:**
The squared Mahalanobis distance, $d^2_M(p,t)$, between the input pixel and the background model is calculated:

$d^2_M(p,t) = \sum_{k \in \{R,G,B\}} \frac{(I_k(p,t) - \mu_{p,k}(t-1))^2}{\sigma^2_{p,k}(t-1)}$

**b. Fuzzy Background Membership:**
A fuzzy membership value, $\mu_{BG}(p,t)$, is calculated. This value is the ratio of the Mahalanobis distance to the threshold factor, clamped to the range [0, 1].

$\mu_{BG}(p,t) = \min\left( \frac{d^2_M(p,t)}{\tau^2_M}, 1.0 \right)$

#### **3. Foreground/Background Classification**

The pixel is classified based on its fuzzy membership value:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } \mu_{BG}(p,t) \ge \tau_{BG} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **4. Fuzzy Model Update**

The model's mean and variance are updated for every pixel, but the learning rate is modulated by the fuzzy membership.

**a. Fuzzy Learning Rate:**
A fuzzy-modulated learning rate, $\alpha(t)$, is calculated:

$\alpha(t) = \alpha_{max} \cdot \exp(E_{fuzzy} \cdot \mu_{BG}(p,t))$

**b. Mean and Variance Update:**
The mean and variance for each channel $k \in \{R,G,B\}$ are updated using an exponential moving average with the fuzzy learning rate:

*   **Mean:**
    $\mu_{p,k}(t) = (1-\alpha(t))\mu_{p,k}(t-1) + \alpha(t) I_k(p,t)$

*   **Variance:**
    Let the squared difference be $d^2_k = (I_k(p,t) - \mu_{p,k}(t-1))^2$.
    $\sigma^2_{p,k}(t) = (1-\alpha(t))\sigma^2_{p,k}(t-1) + \alpha(t) d^2_k$

    The variance for each channel is then clamped to a minimum value:
    $\sigma^2_{p,k}(t) = \max(\sigma^2_{p,k}(t), \sigma^2_{noise})$

### Summary

*   **Family:** Per-pixel Fuzzy Gaussian Model.
*   **Idea (from code):** Models each pixel's background with a single Gaussian per color channel. It calculates a "fuzzy background membership" value based on the Mahalanobis distance of the current pixel to the model. A pixel is foreground if this membership exceeds a threshold. The learning rate for updating the Gaussian's mean and variance is modulated by this fuzzy membership, so good matches cause strong updates and poor matches cause weak ones.
*   **Strengths:** The fuzzy update mechanism provides a smooth, adaptive learning process that is robust to noise. It is computationally efficient and has a low memory footprint. The per-channel variance allows it to handle color shifts more effectively than a single-variance model.
*   **Weaknesses:** Cannot handle multimodal backgrounds (e.g., waving trees) as it only stores one Gaussian per pixel. It is slow to adapt to sudden, large-scale illumination changes. Performance can be sensitive to the interaction between the Mahalanobis and fuzzy thresholds.

---

## LBMixtureOfGaussians
---

### Algorithm Description

This algorithm models the background of each pixel using a Mixture of Gaussians (GMM). Each pixel is represented by a small, fixed number of Gaussian distributions (hardcoded to 3). Each Gaussian has a mean color, a per-channel variance, and a weight. For each new pixel, the algorithm determines if it matches one of the existing Gaussians based on its Mahalanobis distance. If a match is found, the parameters of the matched Gaussian are updated, and the weights of all Gaussians for that pixel are adjusted. If no match is found, a new Gaussian is created, replacing the one with the lowest significance if the maximum number of Gaussians is already present. The Gaussians are continuously sorted by a significance metric (weight divided by total standard deviation). The background is modeled by the most significant Gaussians that account for a certain portion of the total weight.

### Mathematical Formulation

For each pixel $p$, the color distribution is modeled as a mixture of up to $K_{max}$ (`NUMBERGAUSSIANS`, hardcoded to 3) Gaussians. The model for pixel $p$ at time $t$ is $M_p(t) = \{ G_{p,k}(t) \}_{k=1}^{K_p}$, where $K_p$ is the current number of Gaussians for the pixel. Each Gaussian $G_{p,k}$ is a tuple $G_{p,k} = \langle \vec{\mu}_{p,k}, \vec{\sigma}^2_{p,k}, \omega_{p,k} \rangle$, where:
*   $\vec{\mu}_{p,k}$: The mean RGB color vector.
*   $\vec{\sigma}^2_{p,k}$: A vector of per-channel variances $(\sigma^2_R, \sigma^2_G, \sigma^2_B)$.
*   $\omega_{p,k}$: The weight of the Gaussian.

**Parameters:**
*   $\alpha$: `LEARNINGRATEMOG`, the learning rate.
*   $\tau^2_M$: `THRESHOLDMOG`, the squared Mahalanobis distance threshold factor.
*   $T_B$: `BGTHRESHOLDMOG`, the cumulative weight threshold to define the background model.
*   $\sigma^2_{init}$: `INITIALVARMOG`, the initial variance for new Gaussians.

#### **1. Matching and Classification**

For each input pixel $\vec{I}(p,t)$:

**a. Find Match:**
The algorithm finds the first Gaussian $G_{p,m}$ in its current, sorted list that matches the input pixel. A match is declared if the squared Mahalanobis distance is below the threshold:

$d^2_M(p,m) = \sum_{j \in \{R,G,B\}} \frac{(I_j(p,t) - \mu_{p,m,j}(t-1))^2}{\sigma^2_{p,m,j}(t-1)} < \tau^2_M$

**b. Background Model Identification:**
The Gaussians for pixel $p$ are kept sorted in descending order based on a sort key $S_k = \omega_k / \sqrt{\sigma^2_{k,R} + \sigma^2_{k,G} + \sigma^2_{k,B}}$. The background is represented by the first $B$ Gaussians in this sorted list, where $B$ is the smallest integer satisfying:
$\sum_{k=1}^{B} \omega_{p,k} > T_B$

**c. Classification:**
The pixel is classified as foreground if the matched Gaussian $G_{p,m}$ is not part of the background model (i.e., its index $m$ is greater than $B$).

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } m > B \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **2. Model Parameter Update**

**a. Case 1: A match is found with Gaussian $m$.**
*   **Weight Update:**
    *   For the matched Gaussian $m$: $\omega'_{p,m,t} = \omega_{p,m,t-1} + \alpha(1 - \omega_{p,m,t-1})$
    *   For all other Gaussians $k \ne m$: $\omega'_{p,k,t} = (1-\alpha)\omega_{p,k,t-1}$
*   **Mean and Variance Update (for Gaussian $m$ only):**
    *   $\vec{\mu}_{p,m,t} = (1-\alpha)\vec{\mu}_{p,m,t-1} + \alpha \vec{I}(p,t)$
    *   For each channel $j \in \{R,G,B\}$:
        $\sigma^2_{p,m,j}(t) = (1-\alpha)\sigma^2_{p,m,j}(t-1) + \alpha (I_j(p,t) - \mu_{p,m,j}(t))^2$
    *   The variance for each channel is then clamped: $\sigma^2_{p,m,j}(t) = \max(\sigma^2_{p,m,j}(t), \sigma^2_{init})$.

**b. Case 2: No match is found.**
*   A new Gaussian is created, replacing the one with the lowest sort key if the model is full ($K_p = K_{max}$).
*   The new Gaussian $G_{new}$ is initialized with:
    *   $\vec{\mu}_{new} = \vec{I}(p,t)$
    *   $\vec{\sigma}^2_{new} = (\sigma^2_{init}, \sigma^2_{init}, \sigma^2_{init})$
    *   $\omega'_{new} = \alpha$ (The code uses `LEARNINGRATEMOG`).

**c. Weight Renormalization:**
After the update, the weights $\{\omega'_{p,k,t}\}$ for all of the pixel's Gaussians are normalized to ensure they sum to 1, yielding the final weights $\{\omega_{p,k,t}\}$.

**d. Sorting:**
The list of Gaussians for the pixel is re-sorted based on the updated sort key $S_k$.

### Summary

*   **Family:** Per-pixel Mixture of Gaussians (GMM).
*   **Idea (from code):** Models each pixel with a small mixture of weighted Gaussians (up to 3). For a new pixel, it finds the first Gaussian that matches based on Mahalanobis distance. If the matched Gaussian is part of the background (determined by a cumulative weight threshold), the pixel is background. If no match is found, a new Gaussian is created, replacing the least significant one. All Gaussians are updated and re-sorted based on a weight/variance metric after each pixel.
*   **Strengths:** Can model multimodal backgrounds. The per-channel variance allows it to adapt to color-specific noise and changes. It is adaptive and can learn new background objects.
*   **Weaknesses:** High memory usage, storing multiple Gaussians for every pixel. Computationally more expensive than single-Gaussian methods. The fixed, small number of Gaussians (3) limits its ability to model very complex scenes. Performance is sensitive to several tuning parameters.

---

## LBP_MRF
---

### Algorithm Description

This algorithm is a complex, multi-stage background subtraction method that combines a Mixture of Gaussians-like model at the feature level with a Markov Random Field (MRF) for spatial regularization. For each pixel, it maintains a background model consisting of a small number of weighted LBP histograms. An incoming frame is converted to an LBP image, and for each pixel, a local LBP histogram is computed. This histogram is compared to the pixel's background model to determine a "background rate," which serves as the initial, pixel-wise classification. This initial segmentation is then refined using an MRF solved with a min-cut/max-flow algorithm. The MRF smooths the segmentation by penalizing disagreements between neighboring pixels, resulting in a spatially coherent foreground mask. The background model of LBP histograms is updated over time using a learning rate.

### Mathematical Formulation

For each pixel $p$, the background is modeled as a mixture of $K$ (`HUHistogramsPerPixel`) LBP histograms. The model for pixel $p$ at time $t$ is $M_p(t) = \{ G_{p,k}(t) \}_{k=1}^{K}$, where each component $G_{p,k}$ is a tuple $G_{p,k} = \langle H_{p,k}, \omega_{p,k}, f_{p,k} \rangle$:
*   $H_{p,k}$: A normalized LBP histogram with $B$ (`HUHistogramBins`) bins.
*   $\omega_{p,k}$: The weight of the histogram component.
*   $f_{p,k}$: A boolean flag indicating if this component is part of the background.

**Parameters:**
*   $\alpha_H$: `HUHistLRate`, the learning rate for histograms.
*   $\alpha_\omega$: `HUWeightsLRate`, the learning rate for weights.
*   $\tau_{prox}$: `HUPrThres`, the proximity threshold for matching a histogram.
*   $\tau_{bg}$: `HUBackgrThres`, the cumulative weight threshold to define the background model.
*   $\lambda$: `HUMinCutWeight`, the weight of the data term in the MRF energy function.

#### **1. LBP Histogram Extraction**

For an input image, it is first converted to a specified color space (e.g., CIELuv) and then to an LBP image. For each pixel $p$, a local histogram, $H_{curr}(p,t)$, is computed over a square neighborhood (`HUHistogramArea`) in the LBP image.

#### **2. Pixel-wise Background Matching and Model Update**

For each pixel $p$ with its current histogram $H_{curr}(p,t)$:

**a. Find Best Match:**
The algorithm calculates the similarity to each of the $K$ model histograms using **histogram intersection**:
$\mathcal{I}_k = \sum_{i=1}^{B} \min(H_{curr,i}(p,t), H_{p,k,i}(t-1))$
The best matching histogram is the one with the maximum intersection, $G_{p,m}$, with similarity $\mathcal{I}_{max}$.

**b. Model Update:**
*   **If no good match is found** (i.e., $\mathcal{I}_{max} < \tau_{prox}$): The model component with the lowest weight is replaced. Its histogram is set to $H_{curr}(p,t)$, its weight is set to a small initial value (0.01), and its background flag $f$ is set to `false`.
*   **If a match is found with $G_{p,m}$:**
    *   **Histogram Update:** The matched histogram is updated with a running average:
        $H_{p,m}(t) = (1-\alpha_H)H_{p,m}(t-1) + \alpha_H H_{curr}(p,t)$
    *   **Weight Update:** The weights are updated to favor the matched component:
        $\omega_{p,k}(t) = (1-\alpha_\omega)\omega_{p,k}(t-1) + \alpha_\omega \cdot \delta_{k,m}$
        (where $\delta_{k,m}$ is 1 if $k=m$ and 0 otherwise).

**c. Background Model Re-evaluation:**
After updating, the weights are normalized. The components are sorted by weight in descending order. The background model is the smallest set of top-weighted components whose weights sum to greater than $\tau_{bg}$. The background flags $f_{p,k}$ are updated accordingly.

**d. Initial Classification (Background Rate):**
The "background rate" for the pixel, $\mu_{BG}(p,t)$, is calculated as the maximum intersection value found with any component flagged as background:
$\mu_{BG}(p,t) = \max_{k | f_{p,k}=\text{true}} \mathcal{I}_k$

#### **3. MRF-based Spatial Regularization**

The final segmentation is found by minimizing an energy function over a graph representing the image. This is a classic MRF segmentation problem solved via min-cut/max-flow.

Let $L = \{l_p | p \in \mathcal{P}\}$ be a labeling of all pixels $\mathcal{P}$, where $l_p \in \{0 \text{ (FG)}, 1 \text{ (BG)}\}$. The energy function to be minimized is:

$E(L) = \sum_{p \in \mathcal{P}} V_p(l_p) + \sum_{(p,q) \in \mathcal{N}} V_{p,q}(l_p, l_q)$

*   **Data Term (Unary Potential) $V_p(l_p)$:** This term represents the cost of assigning label $l_p$ to pixel $p$, based on the initial classification.
    *   $V_p(l_p=0) = \lambda \cdot \mu_{BG}(p,t)$ (Cost of being FG is proportional to its similarity to the BG).
    *   $V_p(l_p=1) = \lambda \cdot (1 - \mu_{BG}(p,t))$ (Cost of being BG is proportional to its dissimilarity).
    The code sets the terminal weights (`tweights`) in the graph based on these costs.

*   **Smoothness Term (Pairwise Potential) $V_{p,q}(l_p, l_q)$:** This term penalizes neighboring pixels $(p,q) \in \mathcal{N}$ having different labels. The implementation uses a simple Potts model with a constant penalty of 1.
    $V_{p,q}(l_p, l_q) = \begin{cases} 1 & \text{if } l_p \ne l_q \\ 0 & \text{if } l_p = l_q \end{cases}$
    This is implemented by adding edges between neighboring nodes in the graph with a capacity of 1.

The final foreground mask $F(p,t)$ is the set of pixels assigned the label $l_p=0$ by the min-cut/max-flow algorithm.

### Summary

*   **Family:** Texture-based Modeling with Spatial Regularization (LBP + MRF).
*   **Idea (from code):** For each pixel, it maintains a background model composed of a mixture of weighted LBP histograms. It computes a local LBP histogram for each new pixel and compares it to its model via histogram intersection to get an initial "background rate." This pixel-wise probability map is then fed into a Markov Random Field (MRF) as the data term. The MRF is solved using a min-cut/max-flow graph algorithm, which enforces spatial smoothness, producing a clean, final foreground mask. The LBP histogram model is updated based on the new frame.
*   **Strengths:** Very robust, as it combines texture information (LBP), which is resilient to lighting changes, with spatial context (MRF), which eliminates noise and produces coherent objects. The mixture model can handle multi-modal background textures.
*   **Weaknesses:** Extremely computationally expensive and memory-intensive due to the combination of per-pixel histogram models and a graph-cut optimization on every frame. The algorithm is very complex and has a large number of parameters that are difficult to tune.

---

## LBSimpleGaussian
---

### Algorithm Description

This algorithm models the background of each pixel using a single Gaussian distribution for each color channel (R, G, B), assuming independence between channels. A pixel is classified as foreground if its squared Mahalanobis distance to the background model exceeds a fixed threshold. The key feature of this implementation is its **unconditional update rule**: the mean and variance of the Gaussian model for every pixel are updated at every frame, regardless of whether the pixel is classified as foreground or background. This means stationary foreground objects will be quickly absorbed into the background model.

### Mathematical Formulation

For each pixel $p$, the background is modeled by a multi-variate Gaussian with a diagonal covariance matrix. This is equivalent to three independent Gaussians, one for each channel $k \in \{R,G,B\}$. The model for pixel $p$ is defined by a mean vector $\vec{\mu}_p(t) = (\mu_{p,R}, \mu_{p,G}, \mu_{p,B})$ and a variance vector $\vec{\sigma}^2_p(t) = (\sigma^2_{p,R}, \sigma^2_{p,G}, \sigma^2_{p,B})$.

**Parameters:**
*   $\alpha$: `ALPHAGAUSS`, the learning rate.
*   $\tau^2_M$: `THRESHGAUSS`, the squared Mahalanobis distance threshold factor. The code squares this value internally.
*   $\sigma^2_{noise}$: `NOISEGAUSS`, a value used to initialize and clamp the variance.

#### **1. Initialization**

At time $t=0$, for each pixel $p$:
*   The mean $\vec{\mu}_p(0)$ is initialized with the color of the input pixel from the first frame, $\vec{I}(p,0)$.
*   The variance for each channel is initialized to the global noise variance: $\vec{\sigma}^2_p(0) = (\sigma^2_{noise}, \sigma^2_{noise}, \sigma^2_{noise})$.

#### **2. Foreground Detection**

For each input pixel $\vec{I}(p,t)$:

**a. Squared Mahalanobis Distance:**
The squared Mahalanobis distance, $d^2_M(p,t)$, between the input pixel and the background model from the previous frame is calculated:

$d^2_M(p,t) = \sum_{k \in \{R,G,B\}} \frac{(I_k(p,t) - \mu_{p,k}(t-1))^2}{\sigma^2_{p,k}(t-1)}$

**b. Classification:**
The pixel is classified as foreground if this distance exceeds the threshold:

$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } d^2_M(p,t) \ge \tau^2_M \\ 0 \text{ (Background)} & \text{if } d^2_M(p,t) < \tau^2_M \end{cases}$

#### **3. Unconditional Model Update**

The model's mean and variance are updated for **every pixel** at **every frame**, regardless of its classification.

**a. Mean Update:**
The mean for each channel $k$ is updated using an exponential moving average:
$\mu_{p,k}(t) = (1-\alpha)\mu_{p,k}(t-1) + \alpha I_k(p,t)$

**b. Variance Update:**
The variance for each channel $k$ is also updated using an exponential moving average. Note that the variance update uses the **newly computed mean** $\mu_{p,k}(t)$.
Let the squared difference from the new mean be $d^2_k = (I_k(p,t) - \mu_{p,k}(t))^2$.
$\sigma^2_{p,k}(t) = (1-\alpha)\sigma^2_{p,k}(t-1) + \alpha d^2_k$

The variance for each channel is then clamped to an **upper bound** (note the use of `std::min` in the code, which is unusual and likely a bug; typically a minimum variance is enforced).
$\sigma^2_{p,k}(t) = \min(\sigma^2_{p,k}(t), \sigma^2_{noise})$

### Summary

*   **Family:** Per-pixel Single Gaussian Model.
*   **Idea (from code):** Models each pixel's background with a single Gaussian per color channel. It classifies a pixel as foreground if its Mahalanobis distance to the model exceeds a threshold. The model's mean and variance are then updated for every pixel using a running average, regardless of whether it was classified as foreground or background.
*   **Strengths:** Very fast, simple to implement, and has a low memory footprint. Its continuous update makes it highly adaptive to changes in the scene.
*   **Weaknesses:** The unconditional update rule means that any stationary object is very quickly absorbed into the background model, making it unsuitable for detecting stopped objects. It cannot handle multimodal backgrounds. The variance update clamps to a maximum value (`std::min`), which is unusual and likely a bug that could prevent the model from representing high-variance backgrounds correctly.

---

## LOBSTER
---

### Algorithm Description

LOBSTER (LOcal Binary Similarity segmenTER) is a non-parametric, sample-based background subtraction method. For each pixel, it maintains a background model composed of a fixed number ($N$) of background samples. Each sample consists of both a color value and a corresponding LBSP (Local Binary Similarity Pattern) texture descriptor. A new pixel is classified as background if it simultaneously matches a sufficient number ($K$) of these background samples in both color and texture space. The model is updated stochastically: background pixels have a chance to replace a random sample in their own model or in a neighbor's model, which allows for slow, adaptive changes and spatial propagation of background information.

### Mathematical Formulation

For each pixel $p$, the background model $M_p$ consists of a set of $N$ (`m_nBGSamples`) samples, $M_p = \{s_1, s_2, \dots, s_N\}$. Each sample $s_i$ is a pair $s_i = \langle \vec{c}_i, \vec{d}_i \rangle$, where:
*   $\vec{c}_i$: The RGB color vector of the sample.
*   $\vec{d}_i$: The LBSP texture descriptor vector of the sample.

**Parameters:**
*   $N$: `m_nBGSamples`, the total number of background samples per pixel.
*   $K$: `m_nRequiredBGSamples`, the minimum number of matching samples required to be classified as background.
*   $\tau_c$: `m_nColorDistThreshold`, the matching threshold for color.
*   $\tau_d$: `m_nDescDistThreshold`, the matching threshold for the LBSP descriptor.
*   $R$: `learningRate`, an integer controlling the update probability (update rate is $1/R$).

#### **1. Feature Extraction**

For an input pixel $\vec{I}(p,t)$, two features are extracted:
*   **Color:** The RGB color vector, $\vec{C}_{in}(p,t) = \vec{I}(p,t)$.
*   **Texture:** An LBSP descriptor, $\vec{D}_{in}(p,t)$, is computed. The LBSP is a variant of LBP where the threshold for the binary comparison is not zero, but a fraction of the center pixel's intensity, making it robust to illumination variations.

#### **2. Foreground/Background Classification**

For each input pixel $p$, the algorithm counts the number of background samples in its model $M_p(t-1)$ that it matches.

**a. Matching Condition:**
An input pixel $(\vec{C}_{in}, \vec{D}_{in})$ matches a background sample $s_i = \langle \vec{c}_i, \vec{d}_i \rangle$ if and only if **both** the color and descriptor distances are below their respective thresholds.

*   **Color Distance:** The distance is the sum of per-channel L1 distances.
    $d_{color}(\vec{C}_{in}, \vec{c}_i) = \sum_{k \in \{R,G,B\}} |C_{in,k} - c_{i,k}| \le \tau_c$

*   **Descriptor Distance:** The distance is the sum of per-channel Hamming distances (number of differing bits) of the LBSP descriptors.
    $d_{desc}(\vec{D}_{in}, \vec{d}_i) = \sum_{k \in \{R,G,B\}} \text{hdist}(D_{in,k}, d_{i,k}) \le \tau_d$

**b. Counting Matches:**
The algorithm counts the number of samples, $C_{match}$, in the set $M_p(t-1)$ that satisfy the matching condition.

**c. Classification:**
The pixel is classified as background if the number of matches is sufficient:

$F(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if } C_{match} \ge K \\ 255 \text{ (Foreground)} & \text{otherwise} \end{cases}$

#### **3. Stochastic Model Update**

The background model is updated only for pixels classified as background ($F(p,t)=0$). The update is probabilistic, controlled by the learning rate $R$.

For a background pixel $p$:

1.  **Self-Update:** With a probability of $1/R$, a random sample $s_j \in M_p(t-1)$ is chosen and replaced with the current pixel's features:
    $s_j \leftarrow \langle \vec{C}_{in}(p,t), \vec{D}_{in}(p,t) \rangle$

2.  **Neighbor Update:** With a probability of $1/R$, a random 3x3 neighbor of pixel $p$, let's call it $q$, is chosen. A random sample $s_k \in M_q(t-1)$ in the neighbor's model is then replaced with the current features of pixel $p$:
    $s_k \leftarrow \langle \vec{C}_{in}(p,t), \vec{D}_{in}(p,t) \rangle$

If a pixel is classified as foreground, its model is not updated. The final foreground mask is post-processed with a median blur.

### Summary

*   **Family:** Non-parametric Spatio-Temporal Sample-based Modeling.
*   **Idea (from code):** For each pixel, it maintains a background model of N samples, where each sample is a {color, LBSP texture descriptor} pair. A new pixel is classified as background if it matches at least K of these samples in both color (L1 distance) and texture (Hamming distance). If a pixel is background, it has a random chance to update its own model or the model of a random neighbor by replacing a sample with its own color and texture.
*   **Strengths:** Combines color and texture, making it robust to both illumination changes and textureless objects. The K-out-of-N matching is resilient to noise. The spatial neighbor-update mechanism allows the model to propagate information and potentially repair occluded background areas.
*   **Weaknesses:** High memory footprint and computational cost due to storing and comparing against N samples for every pixel. The stochastic update is slow to adapt to fast or large-scale background changes. Performance is sensitive to the tuning of multiple thresholds and parameters.

---

## MixtureOfGaussianV1
---

***Disclaimer:*** *The core implementation of this algorithm resides within the OpenCV 2 library, and its source code is not directly analyzed here. This model is based on the parameters exposed in the wrapper code and the established understanding of the GMM algorithm as described in the paper by KadewTraKuPong and Bowden (2001), which the OpenCV documentation cites.*

### Algorithm Description

This algorithm models the color distribution of each pixel as a Mixture of Gaussians (GMM). Each pixel is represented by a small, variable number of Gaussian distributions, each with its own mean, variance, and weight. When a new pixel arrives, it is checked against the existing Gaussians. If a match is found (based on Mahalanobis distance), the parameters of the matched Gaussian are updated. If no match is found, a new Gaussian is created, replacing the least probable one if the maximum number of Gaussians is already present. The Gaussians are continuously sorted by their significance (weight/standard deviation), and the most significant ones that comprise a certain portion of the distribution are considered the background model. The wrapper code adds an optional, final thresholding step to the foreground mask produced by the core algorithm.

### Mathematical Formulation

For each pixel $p$, the color distribution is modeled as a mixture of up to $K_{max}$ Gaussians. The model for pixel $p$ at time $t$ is $M_p(t) = \{ G_{p,k}(t) \}_{k=1}^{K_p}$, where $K_p$ is the current number of Gaussians for the pixel. Each Gaussian $G_{p,k}$ is a tuple $G_{p,k} = \langle \vec{\mu}_{p,k}, \sigma^2_{p,k}, \omega_{p,k} \rangle$, where:
*   $\vec{\mu}_{p,k}$: The mean RGB color vector.
*   $\sigma^2_{p,k}$: A single, shared variance for all color channels.
*   $\omega_{p,k}$: The weight of the Gaussian.

**Parameters:**
*   $\alpha$: `alpha`, the learning rate. A value of -1 (or any negative value) passed to the `mog` operator implies an automatic, frame-dependent learning rate. Otherwise, it's a fixed value.
*   $T_D$: An internal threshold factor (typically ~2.5) for the matching threshold.
*   $T_B$: An internal cumulative weight threshold to define the background model.
*   $\tau_{post}$: `threshold`, an optional post-processing threshold applied to the final mask.

#### **1. Matching and Classification**

This process is identical to the standard Stauffer and Grimson GMM.

**a. Find Match:**
For an input pixel $\vec{I}(p,t)$, the algorithm finds the first Gaussian $G_{p,m}$ that matches. A match is declared if the squared Mahalanobis distance (simplified for spherical covariance) is below a threshold:
$\frac{||\vec{I}(p,t) - \vec{\mu}_{p,m}(t-1)||^2}{\sigma^2_{p,m}(t-1)} < T_D^2$

**b. Background Model Identification:**
The Gaussians are sorted in descending order by a significance metric $S_k = \omega_k / \sigma_k$. The background is represented by the first $B$ Gaussians in this sorted list, where $B$ is the smallest integer satisfying:
$\sum_{k=1}^{B} \omega_{p,k} > T_B$

**c. Classification:**
The pixel is classified as foreground if the matched Gaussian $G_{p,m}$ is not part of the background model (i.e., its index $m$ in the sorted list is greater than $B$). The initial foreground mask $F'(p,t)$ is:
$F'(p,t) = \begin{cases} 255 & \text{if } m > B \text{ or no match found} \\ 0 & \text{otherwise} \end{cases}$

#### **2. Model Parameter Update**

**a. Case 1: A match is found with Gaussian $m$.**
*   **Weight Update:**
    $\omega_{p,k}(t) = (1-\alpha)\omega_{p,k}(t-1) + \alpha \cdot \delta_{k,m}$
    (where $\delta_{k,m}$ is 1 if $k=m$ and 0 otherwise).
*   **Mean and Variance Update (for Gaussian $m$ only):**
    Let the update rate be $\rho = \alpha / \omega_{p,m}(t)$.
    *   $\vec{\mu}_{p,m}(t) = (1-\rho)\vec{\mu}_{p,m}(t-1) + \rho \vec{I}(p,t)$
    *   $\sigma^2_{p,m}(t) = (1-\rho)\sigma^2_{p,m}(t-1) + \rho ||\vec{I}(p,t) - \vec{\mu}_{p,m}(t)||^2$

**b. Case 2: No match is found.**
*   A new Gaussian is created, replacing the one with the lowest significance if the model is full.
*   The new Gaussian is initialized with mean $\vec{I}(p,t)$, a high initial variance, and a low initial weight.

**c. Weight Renormalization:**
After the update, the weights for all of the pixel's Gaussians are normalized to sum to 1.

#### **3. Optional Post-Processing**

If `enableThreshold` is true, the foreground mask $F'(p,t)$ generated by the MOG algorithm is subjected to an additional binary thresholding step:

$F(p,t) = \begin{cases} 255 & \text{if } F'(p,t) > \tau_{post} \\ 0 & \text{otherwise} \end{cases}$

Since the MOG output is already binary (0 or 255), this step effectively only removes foreground pixels if $\tau_{post}$ is greater than 0 and less than 255, which is an unusual use of thresholding. Typically, this would be used on a shadow-mask output (e.g., where shadows are 127), but this implementation does not appear to handle shadows.

### Summary

*   **Family:** Per-pixel Mixture of Gaussians (GMM).
*   **Idea (from code):** This is a wrapper for the OpenCV 2 `BackgroundSubtractorMOG` algorithm. It models each pixel with a mixture of weighted Gaussians. The core algorithm, hidden within OpenCV, classifies pixels based on whether they match one of the Gaussians designated as "background." This wrapper passes a learning rate (`alpha`) to the OpenCV function and then optionally applies an additional, fixed binary threshold to the resulting foreground mask.
*   **Strengths:** Implements a well-known and effective algorithm (GMM) for handling multimodal backgrounds. It is adaptive and can learn changes in the scene over time.
*   **Weaknesses:** The core logic is a black box within the OpenCV 2 library, offering very limited control. The added `threshold` parameter is redundant, as the underlying `BackgroundSubtractorMOG` already performs a complete segmentation, making this final step largely ineffective unless the MOG output included non-binary values (like shadows), which are not handled here. This version is outdated, superseded by `MOG2` in later OpenCV versions.

---

## MixtureOfGaussianV2
---

***Disclaimer:*** *The core implementation of this algorithm resides within the OpenCV library, and its source code is not directly analyzed here. This model is based on the parameters exposed in the wrapper code and the established understanding of the GMM algorithm as described in the papers by Zivkovic and van der Heijden (2004, 2006), which the OpenCV documentation cites.*

### Algorithm Description

This algorithm is an advanced version of the Gaussian Mixture Model. Unlike simpler GMMs that use a fixed number of Gaussian components, this method adaptively selects the appropriate number of components for each pixel. For each pixel, it maintains a mixture of Gaussians, each defined by a mean, variance, and weight. When a new pixel color is observed, it is checked against the existing Gaussians. If a match is found, the model is updated. If not, a new component is created. The key innovation is that the number of components is not fixed; components with very low weights are pruned, allowing the model to automatically determine the complexity required to represent the background. The background is modeled by the most probable Gaussians. The algorithm also includes a built-in shadow detection mechanism.

### Mathematical Formulation

For each pixel $p$, the color distribution is modeled as a mixture of a variable number of Gaussians, $K_p(t)$, up to a maximum $K_{max}$. The model for pixel $p$ at time $t$ is $M_p(t) = \{ G_{p,k}(t) \}_{k=1}^{K_p(t)}$, where each Gaussian $G_{p,k}$ is a tuple $G_{p,k} = \langle \vec{\mu}_{p,k}, \sigma^2_{p,k}, \omega_{p,k} \rangle$:
*   $\vec{\mu}_{p,k}$: The mean RGB color vector.
*   $\sigma^2_{p,k}$: A single, shared variance for all color channels.
*   $\omega_{p,k}$: The weight of the Gaussian.

**Parameters:**
*   $\alpha$: `alpha`, the learning rate. If set to a negative value, a frame-dependent learning rate is used, where $\alpha_t = 1/t$ for the first few hundred frames, then becomes constant.
*   $T_D$: An internal threshold factor (typically ~2.5) for the matching threshold.
*   $T_B$: An internal cumulative weight threshold to define the background model.
*   $\tau_{shadow}$: An internal threshold for shadow detection.
*   $\tau_{post}$: `threshold`, an optional post-processing threshold applied to the final mask.

#### **1. Matching and Classification**

This process is identical to the standard Stauffer and Grimson GMM.

**a. Find Match:**
For an input pixel $\vec{I}(p,t)$, the algorithm finds the first Gaussian $G_{p,m}$ that matches, based on the squared Mahalanobis distance:
$\frac{||\vec{I}(p,t) - \vec{\mu}_{p,m}(t-1)||^2}{\sigma^2_{p,m}(t-1)} < T_D^2$

**b. Background Model Identification:**
The Gaussians are sorted in descending order by significance ($S_k = \omega_k / \sigma_k$). The background is the smallest set of the most significant Gaussians, $B$, whose weights sum to more than a threshold $T_B$.

**c. Classification:**
The pixel is classified as foreground if the matched Gaussian is not part of the background model. The algorithm also performs shadow detection.
$F'(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } m > B \text{ or no match found} \\ 127 \text{ (Shadow)} & \text{if pixel is a shadow of a background model} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$
A pixel is considered a shadow if it matches a background Gaussian in chrominance but has a lower brightness.

#### **2. Model Update and Component Selection**

**a. Parameter Update:**
If a match is found with Gaussian $m$, its parameters and the weights of all components are updated using the standard GMM update rules (see `MixtureOfGaussianV1` model).

**b. Component Creation/Pruning:**
*   **If no match is found:** A new Gaussian is created with the current pixel as its mean, a high initial variance, and a low initial weight. This new component replaces the least probable (lowest $\omega_k$) component if the maximum number of Gaussians ($K_{max}$) is already reached.
*   **Adaptive Component Count:** The key feature of MOG2 is that after weight renormalization, any component whose weight $\omega_k$ falls below a very small threshold is removed from the mixture for that pixel. This allows the number of components $K_p(t)$ to vary for each pixel over time.

#### **3. Optional Post-Processing**

If `enableThreshold` is true, the foreground mask $F'(p,t)$ (which may contain shadow values) is subjected to an additional binary thresholding step:

$F(p,t) = \begin{cases} 255 & \text{if } F'(p,t) > \tau_{post} \\ 0 & \text{otherwise} \end{cases}$

This step has the effect of converting both shadows (value 127) and foreground (value 255) to the final foreground value (255), assuming $\tau_{post}$ is set to a low value like the default of 15.

### Summary

*   **Family:** Adaptive Per-pixel Mixture of Gaussians (GMM).
*   **Idea (from code):** This is a wrapper for the OpenCV `BackgroundSubtractorMOG2` algorithm. It initializes the MOG2 object and calls its `apply` method with a specified `learningRate` (`alpha`). The core MOG2 algorithm (internal to OpenCV) adaptively manages the number of Gaussian components per pixel, classifies pixels as foreground, background, or shadow, and updates the model. The wrapper then optionally applies a final binary threshold to the resulting foreground mask.
*   **Strengths:** Leverages a robust and widely used algorithm that adaptively models complex backgrounds, handles multimodal scenes, and includes built-in shadow detection.
*   **Weaknesses:** The core implementation is a black box within the OpenCV library, limiting direct control over MOG2's internal parameters. The optional post-thresholding step can be redundant or counterproductive, as MOG2 already provides a nuanced foreground mask (including shadow labels) that this step may oversimplify.

---

## MultiCue
---

### Algorithm Description

The `MultiCue` algorithm is a sophisticated background subtraction method that fuses two distinct cues: a local texture model and a pixel-wise color model. Both models are based on the codebook paradigm, featuring a main background model and a "cachebook" for handling static foreground objects (ghosts). The primary segmentation is performed by the texture model, which generates a "confidence map." This map is then refined using the color model in a verification step, particularly in low-texture areas. The resulting foreground mask undergoes extensive post-processing, including morphological operations and a bounding box verification stage that uses Hausdorff distance on edge images to eliminate false positives.

### Mathematical Formulation

The algorithm operates on a down-sampled image in a custom XYZ color space derived from HSV.

#### **1. Texture Model**

For each pixel $p$, the texture model consists of $N_{neighbors}$ (`g_nNeighborNum`, default 6) independent codebooks, one for each pre-defined neighboring pixel $q_i$. Each codebook $C_{p,i}$ models the distribution of the intensity difference between the center pixel $p$ and its neighbor $q_i$.

*   **Texture Feature:** For a pixel $p$ and its neighbor $q_i$, the feature is the difference in their Z-channel (intensity) values:
    $f_T(p, q_i) = Z(p) - Z(q_i)$

*   **Texture Codebook:** Each codebook $C_{p,i}$ is a set of codewords $\{cw_{p,i,k}\}$. Each codeword $cw_{p,i,k}$ has a mean $\mu_k$ and an interval $[\mu_k - \tau_T, \mu_k + \tau_T]$, where $\tau_T$ is `g_nTextureTrainVolRange`.

*   **Texture Confidence Map:** For a new frame, a texture confidence map, $Conf_T(p,t)$, is generated. For each pixel $p$, the algorithm counts how many of its neighbor-features $f_T(p, q_i)$ match a codeword in their respective codebooks $C_{p,i}$. A match occurs if $f_T$ falls within a codeword's interval.
    Let $M(p,t)$ be the number of matched neighbors. The confidence is:
    $Conf_T(p,t) = 1 - \frac{M(p,t)}{N_{neighbors}}$

#### **2. Color Model**

For each pixel $p$, the color model is a single codebook $C_p$ that models the distribution of the pixel's XYZ color vector.

*   **Color Feature:** The feature is the 3D color vector $\vec{f}_C(p) = (X(p), Y(p), Z(p))$.

*   **Color Codebook:** The codebook $C_p$ is a set of codewords $\{cw_{p,k}\}$. Each codeword has a mean color vector $\vec{\mu}_k$. A match occurs if an input color vector falls within a bounding box of size $\tau_C$ (`g_nColorTrainVolRange`) around the mean $\vec{\mu}_k$.

#### **3. Landmark Array Generation (Cue Fusion)**

The final initial segmentation, called the "landmark array" $L(p,t)$, is generated by fusing the two cues.

1.  **Primary Segmentation:** A pixel is initially marked as foreground if its texture confidence is high.
    If $Conf_T(p,t) > \frac{\text{g\_iT\_ModelThreshold}}{\text{g\_nNeighborNum}}$, then $L(p,t) = 255$ (Foreground).

2.  **Color-based Verification (for low-texture areas):** If a pixel is not marked as foreground by the texture model, a verification step is performed. This happens only if both the background model and the current input are determined to be "low-texture" (average intensity difference is low).
    *   If the input color $\vec{f}_C(p)$ matches any codeword in the color background model $C_p$, the pixel is marked as ambiguous/potentially background: $L(p,t) = 125$.
    *   If no color match is found, the pixel is marked as foreground: $L(p,t) = 255$.

3.  Otherwise, the pixel is background: $L(p,t) = 0$.

#### **4. Post-Processing**

The landmark array $L(p,t)$ undergoes significant post-processing.
1.  **Morphological Filtering:** A custom morphological filter (majority vote in a window) is applied to $L(p,t)$ to produce a binary mask.
2.  **Labeling and Bounding Box Extraction:** Connected components are found, and bounding boxes are generated for each.
3.  **Bounding Box Verification:** Each bounding box is verified. Invalid boxes are removed. A key verification step involves comparing the Canny edge map of the foreground mask within the box to the Canny edge map of the original image in that box. The **partial Hausdorff distance** between the two edge point sets is calculated. If this distance is too high (greater than `dThreshold=10`), it implies the segmented shape does not align well with image edges, and the box is rejected as a "ghost."

#### **5. Model Update**

Both the Texture and Color models use a codebook update mechanism similar to the `CodeBook` algorithm.
*   For pixels determined to be background, their features are used to update the main background models.
*   For pixels inside valid foreground bounding boxes, their features are used to update separate "cachebook" models.
*   A ghost absorption mechanism is in place, where codewords that persist in a cachebook for a long time (`g_iAbsortionPeriod`) are promoted to the main background model.

### Summary

*   **Family:** Multi-Cue Codebook-based Modeling.
*   **Idea (from code):** The algorithm first builds a background model based on local texture, defined as the intensity differences between a pixel and its neighbors. This produces an initial "texture confidence map." In low-texture areas where this is unreliable, it uses a secondary, pixel-wise color codebook model to verify the result. The fused mask is then heavily post-processed: it is morphologically cleaned, blobs are labeled, and bounding boxes are generated. Finally, a sophisticated verification step rejects false-positive blobs by comparing the Canny edges of the segmented shape against the Canny edges of the original image within the bounding box using Hausdorff distance.
*   **Strengths:** Very robust due to the fusion of texture and color cues. The post-processing step, particularly the Hausdorff distance verification on edges, is highly effective at eliminating ghost objects and segmentation artifacts that do not align with real-world contours.
*   **Weaknesses:** Extremely complex and computationally expensive due to its many stages (dual models, labeling, edge detection, Hausdorff distance). It operates on a down-sampled version of the image for performance, which can lead to a loss of detail and a lower-resolution final mask. The large number of parameters makes it difficult to tune.

---

## MultiLayer
---

### Algorithm Description

The `MultiLayer` algorithm is a sophisticated background subtraction method that fuses color and texture information using a multi-component mixture model for each pixel. Each component in the mixture, referred to as a "mode," represents a distinct background state and models both a color distribution and an LBP (Local Binary Pattern) texture descriptor. A key feature is the concept of "layers," where stable background modes are assigned a layer number, creating a prioritized background representation. A pixel is classified as foreground if its combined color and texture distance to the best-matching background mode exceeds a threshold. The algorithm includes shadow and highlight suppression and updates the mixture model components based on which one best matches the current observation.

### Mathematical Formulation

For each pixel $p$, the background is modeled as a mixture of up to $K$ (`m_nMaxLBPModeNum`) components (modes). The model for pixel $p$ is $M_p(t) = \{ G_{p,k}(t) \}_{k=1}^{K_p}$, where $K_p$ is the current number of modes. Each mode $G_{p,k}$ is a tuple:
$G_{p,k} = \langle \vec{\mu}_{p,k}, \vec{D}_{p,k}, \omega_{p,k}, \vec{C}^{min}_{p,k}, \vec{C}^{max}_{p,k}, L_{p,k} \rangle$, where:
*   $\vec{\mu}_{p,k}$: The mean RGB color vector.
*   $\vec{D}_{p,k}$: The mean LBP texture descriptor vector.
*   $\omega_{p,k}$: The weight of the mode.
*   $\vec{C}^{min}_{p,k}, \vec{C}^{max}_{p,k}$: The observed min and max color values for this mode.
*   $L_{p,k}$: The background layer number (0 if not a stable background mode).

**Parameters:**
*   $\alpha_C$: `m_fModeUpdatingLearnRate`, the learning rate for color and texture.
*   $\alpha_\omega$: `m_fWeightUpdatingLearnRate`, the learning rate for weights.
*   $W_C$: `m_fColorWeight`, the weight of the color distance.
*   $W_T$: `m_fTextureWeight`, the weight of the texture distance.
*   $\tau_{BG}$: `m_fPatternColorDistBgThreshold`, the distance threshold for foreground classification.
*   $\tau_{update}$: `m_fPatternColorDistBgUpdatedThreshold`, the distance threshold for updating a matched mode.
*   $T_B$: `m_fBackgroundModelPercent`, the cumulative weight threshold to define the background model.

#### **1. Feature Extraction**

For an input pixel $\vec{I}(p,t)$, two features are extracted:
*   **Color:** The RGB color vector, $\vec{C}_{in}(p,t) = \vec{I}(p,t)$.
*   **Texture:** An LBP descriptor, $\vec{D}_{in}(p,t)$, is computed from the input image.

#### **2. Foreground/Background Classification**

**a. Distance Calculation:**
For each mode $G_{p,k}$ in the pixel's model, a combined distance $d_{total}(p,k,t)$ is calculated as a weighted sum of the color distance and the texture distance.

*   **Color Distance ($d_C$):** This is a complex metric that combines two components:
    1.  **Range Distance:** A binary value (0 or 1) indicating if the input color $\vec{C}_{in}$ falls outside the learned color range $[\vec{C}^{min}_{p,k} \cdot \tau_{shadow}, \vec{C}^{max}_{p,k} \cdot \tau_{highlight}]$.
    2.  **Noised Angle:** The angular difference between the input color vector and the mean color vector $\vec{\mu}_{p,k}$, adjusted for noise.
    $d_C(p,k,t) = f(\text{range\_dist}, \text{noised\_angle})$

*   **Texture Distance ($d_T$):** This is the Hamming distance (normalized by descriptor length) between the input LBP descriptor and the mode's mean LBP descriptor.
    $d_T(p,k,t) = \frac{1}{N_{LBP}} \sum_{i=1}^{N_{LBP}} |D_{in,i}(p,t) - D_{p,k,i}(t-1)|$

*   **Total Distance:**
    $d_{total}(p,k,t) = W_C \cdot d_C(p,k,t) + W_T \cdot d_T(p,k,t)$

**b. Find Best Match:**
The algorithm finds the mode $G_{p,m}$ with the minimum total distance, $d_{min} = d_{total}(p,m,t)$.

**c. Classification:**
The pixel is classified based on $d_{min}$ and whether the best-matching mode $m$ is part of the background. The background is defined as the first $B$ modes, sorted by weight, whose cumulative weight exceeds $T_B$.
*   If $m$ is not part of the background model (i.e., its index is greater than $B$), the distance is penalized: $d_{min} = \max(d_{min}, \tau_{BG} \cdot 2.5)$.
*   The final foreground mask is generated by thresholding this distance:
    $F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } d_{min} \ge \tau_{BG} \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

#### **3. Model Update**

**a. Case 1: No good match found ($d_{min} \ge \tau_{update}$):**
*   The weights of all existing modes are decreased.
*   If the number of modes is less than $K$, a new mode is created, initialized with the current pixel's features, and given a low initial weight.
*   If the model is full, the mode with the lowest weight is replaced.

**b. Case 2: A match is found with mode $m$ ($d_{min} < \tau_{update}$):**
*   **Weight Update:** The weight of the matched mode is increased, and the weights of all other modes are decreased.
    $\omega_{p,m}(t) = (1-\alpha'_\omega)\omega_{p,m}(t-1) + \alpha'_\omega$
    $\omega_{p,k}(t) = (1-\alpha'_\omega)\omega_{p,k}(t-1)$ for $k \ne m$
    (where $\alpha'_\omega$ is a modified learning rate that increases for more stable modes).
*   **Color and Texture Update (for mode $m$ only):**
    $\vec{\mu}_{p,m}(t) = (1-\alpha_C)\vec{\mu}_{p,m}(t-1) + \alpha_C \vec{C}_{in}(p,t)$
    $\vec{D}_{p,m}(t) = (1-\alpha_C)\vec{D}_{p,m}(t-1) + \alpha_C \vec{D}_{in}(p,t)$
*   The min/max color range is also updated with the current color.
*   **Layer Management:** If a mode becomes very stable (its `max_weight` exceeds a threshold), it is assigned a new, higher layer number $L_{p,m}$. This creates a hierarchy of background layers.

### Summary

*   **Family:** Multi-Feature Per-pixel Mixture Model.
*   **Idea (from code):** Models each pixel with a mixture of components ("modes"). Each mode captures both a color distribution (mean, min/max range) and an LBP texture pattern. For a new pixel, it calculates a combined distance to each mode, which is a weighted sum of a color distance (which accounts for shadows/highlights) and a texture distance (LBP Hamming distance). A pixel is classified as foreground if its distance to the best-matching background mode is above a threshold. The model is updated using GMM-like rules. Very stable background modes are assigned a "layer" number, creating a prioritized, multi-layered background representation.
*   **Strengths:** The fusion of color and texture features makes it robust to both illumination changes and textureless regions. The mixture model handles complex, multi-modal backgrounds. It explicitly models and suppresses shadows and highlights.
*   **Weaknesses:** Extremely complex, with a vast number of interacting parameters that are very difficult to tune. It is computationally expensive due to the per-pixel mixture model and the dual-feature distance calculation on every frame. The layering concept, while powerful, adds significant complexity to the model's state and behavior.

---

## PAWCS (Pixel-based Adaptive Word Consensus Segmenter)
---

### Algorithm Description

PAWCS (Pixel-based Adaptive Word Consensus Segmenter) is a highly adaptive, non-parametric background subtraction algorithm. It uses a dual-dictionary approach, maintaining both a **local** (per-pixel) and a **global** (frame-wide) dictionary of "words." A word is a feature pair consisting of a color vector and an LBSP texture descriptor. A pixel is classified as background if a "consensus" of its local background words matches the current observation. This decision is reinforced by checking against the global dictionary. The algorithm's key feature is its self-adjusting nature; it maintains numerous per-pixel maps to dynamically update its own parameters, such as distance thresholds and learning rates, based on local statistics like the mean minimum distance to the model and segmentation stability.

### Mathematical Formulation

#### **1. Feature and Model Definition**

*   **Word:** A feature vector $W = \langle \vec{C}, \vec{D} \rangle$, where $\vec{C}$ is the color vector and $\vec{D}$ is the LBSP descriptor vector.
*   **Local Dictionary ($L_p$):** For each pixel $p$, a dictionary of $N_{local}$ (`m_nMaxLocalWords`) words, $L_p = \{W_{p,i}\}$, is maintained. Each word also stores its first occurrence time $t_{first}$, last occurrence time $t_{last}$, and total occurrences $n_{occ}$.
*   **Global Dictionary ($G$):** A single dictionary of $N_{global}$ (`m_nMaxGlobalWords`) words, $G = \{W_{g,j}\}$, is shared across all pixels. Each global word also stores a spatial occurrence map.
*   **Word Weight:** The significance of a local word is a function of its age and frequency:
    $w(W_{p,i}, t) = \frac{n_{occ,i}}{(t_{last,i} - t_{first,i}) + 2(t - t_{last,i}) + \omega_{offset}}$
    where $\omega_{offset}$ is `m_nLocalWordWeightOffset`.

#### **2. Per-Pixel Adaptive Parameters**

PAWCS maintains several per-pixel maps that are updated at each frame $t$. For a pixel $p$:
*   $R(p,t)$: Distance threshold factor (`m_oDistThresholdFrame`).
*   $T(p,t)$: Learning rate (`m_oUpdateRateFrame`).
*   $v(p,t)$: Variation modulator for $R$ and $T$ (`m_oDistThresholdVariationFrame`).
*   $D_{min}^{ST}(p,t), D_{min}^{LT}(p,t)$: Short-term and long-term moving averages of the minimum distance to the model (`m_oMeanMinDistFrame_ST/LT`).

These parameters are updated based on feedback loops. For example:
*   If the pixel is consistently classified as background with low distance, $T(p,t)$ decreases and $R(p,t)$ decreases.
*   If the pixel is noisy or frequently misclassified, $T(p,t)$ increases and $R(p,t)$ increases.
    $R(p,t) \approx R(p,t-1) \pm \delta_R \cdot v(p,t)$
    $T(p,t) \approx T(p,t-1) \pm \delta_T / v(p,t)$

#### **3. Foreground/Background Classification**

**a. Dynamic Threshold Calculation:**
For each pixel $p$, dynamic color and descriptor distance thresholds are calculated based on the adaptive parameter $R(p,t)$:
*   $\tau_C(p,t) = \sqrt{R(p,t)} \cdot \tau_{C,min}$
*   $\tau_D(p,t) = 2^{\lfloor R(p,t)+0.5 \rfloor} + \tau_{D,offset}$

**b. Word Consensus:**
For an input word $W_{in}(p,t) = \langle \vec{C}_{in}, \vec{D}_{in} \rangle$:
1.  The algorithm iterates through the pixel's sorted local dictionary $L_p$.
2.  For each local word $W_{p,i}$, it checks for a match:
    $d_C(\vec{C}_{in}, \vec{C}_{p,i}) \le \tau_C(p,t)$ AND $d_D(\vec{D}_{in}, \vec{D}_{p,i}) \le \tau_D(p,t)$
    (where $d_C$ is a mixed L1/chromaticity distance and $d_D$ is a Hamming distance).
3.  It accumulates the weights $w(W_{p,i}, t)$ of all matching local words. Let this sum be $W_{consensus}$.
4.  A consensus is reached if the accumulated weight exceeds a threshold, which is itself dynamic and based on the weight of the most significant word in the local dictionary:
    $W_{consensus} \ge \frac{w(W_{p,1}, t)}{2 \cdot R(p,t)}$

**c. Global Verification:**
If the local consensus is not met, the algorithm performs a check against the global dictionary $G$. If $W_{in}(p,t)$ matches a high-weight global word that is spatially relevant to pixel $p$, the pixel may still be classified as background.

**d. Final Classification:**
$F(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if local consensus is met OR global verification passes} \\ 255 \text{ (Foreground)} & \text{otherwise} \end{cases}$

#### **4. Model Update**

*   **If pixel $p$ is background:**
    1.  The matching local words have their occurrence counts $n_{occ}$ incremented.
    2.  With a probability of $1/T(p,t)$, a random neighbor $q$ is chosen, and its local dictionary $L_q$ is updated with the input word $W_{in}(p,t)$ (similar to ViBe's spatial propagation).
    3.  With a probability based on the learning rate, the global dictionary $G$ is updated.
*   **If pixel $p$ is foreground:**
    1.  The local dictionary $L_p$ is updated by replacing its least significant word with $W_{in}(p,t)$. This allows new objects to be learned.

### Summary

*   **Family:** Adaptive Non-parametric Sample-based Modeling.
*   **Idea (from code):** Maintains both a per-pixel **local** dictionary and a frame-wide **global** dictionary of background "words" ({color, LBSP descriptor} pairs). A pixel is classified as background if its current features match a "consensus" of words in its local dictionary, determined by summing the weights of matching words. This decision is cross-checked with the global dictionary. Critically, the algorithm is highly self-adaptive: it uses several per-pixel feedback maps to continuously adjust its own distance thresholds and learning rates based on local segmentation stability and background dynamics.
*   **Strengths:** Extremely adaptive due to its per-pixel feedback mechanism for thresholds and learning rates. The dual local/global dictionary approach allows it to model both specific pixel history and common background patterns. The use of color and LBSP features provides robustness to various scene conditions.
*   **Weaknesses:** Immensely complex, with numerous interacting parameter maps and feedback loops, making it very difficult to tune or predict its behavior. It has a very high memory footprint and computational cost due to the multiple dictionaries and full-frame adaptive maps that are updated on every frame.

---

## PixelBasedAdaptiveSegmenter (PBAS)
---

### Algorithm Description

PBAS is a non-parametric, sample-based background subtraction method that is highly adaptive. For each pixel, it maintains a background model consisting of a fixed number of recent feature vectors. The key innovation of PBAS is its self-adjusting mechanism for two crucial parameters: the distance threshold ($R$) and the model update rate ($T$), which are maintained on a per-pixel basis. A pixel is classified as foreground if its distance to the background model is consistently high. The values of $R$ and $T$ are then dynamically adjusted based on a moving average of the minimum distance to the background model, allowing the algorithm to automatically tune its sensitivity and learning speed according to local scene dynamics.

### Mathematical Formulation

#### **1. Feature Extraction**

For each pixel $p$, a feature vector $F(p,t)$ is computed that combines color and gradient magnitude.
*   **Color Feature $\vec{C}(p,t)$:** The raw color vector (e.g., RGB or Grayscale).
*   **Gradient Magnitude Feature $\vec{M}(p,t)$:** The Sobel gradient magnitude, computed per color channel.
    $F(p,t) = \langle \vec{M}(p,t), \vec{C}(p,t) \rangle$

#### **2. Background Model**

For each pixel $p$, the background model $B_p$ is a collection of $N$ (`N`) previously observed feature vectors:
$B_p(t-1) = \{ F_{p,1}, F_{p,2}, \dots, F_{p,N} \}$

#### **3. Per-Pixel Adaptive Parameters**

For each pixel $p$, two parameters are maintained and adapted over time:
*   $R(p,t)$: The distance threshold (`actualR`).
*   $T(p,t)$: The model update rate divisor (`actualT`).
*   $D_{min\_avg}(p,t)$: A running average of the minimum distance to the background model (`meanMinDist`).

#### **4. Foreground/Background Classification**

**a. Distance Calculation:**
For an input feature vector $F_{in}(p,t) = \langle \vec{M}_{in}, \vec{C}_{in} \rangle$, the distance to each background sample $F_{p,i} = \langle \vec{M}_{i}, \vec{C}_{i} \rangle$ is a weighted L2 norm:
$d(F_{in}, F_{p,i}) = \alpha \cdot \frac{||\vec{M}_{in} - \vec{M}_{i}||_2}{\bar{M}_{global}} + \beta \cdot ||\vec{C}_{in} - \vec{C}_{i}||_2$
where $\alpha$ and $\beta$ are fixed weights, and $\bar{M}_{global}$ (`formerMeanMag`) is a running average of gradient magnitudes across all foreground pixels in the previous frame, used for normalization.

**b. Matching and Classification:**
The algorithm counts the number of background samples, $C_{match}$, that are "close" to the input feature vector:
$C_{match} = |\{ i \mid d(F_{in}(p,t), F_{p,i}) < R(p,t-1) \}|$

The pixel is classified as foreground if this count is below a minimum threshold:
$Mask(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if } C_{match} \ge \#_{min} \\ 255 \text{ (Foreground)} & \text{otherwise} \end{cases}$
where $\#_{min}$ is the parameter `Raute_min`.

If the pixel is classified as background, the minimum distance found among the matching samples, $d_{min}(p,t)$, is recorded.

#### **5. Parameter and Model Update**

**a. Update of Adaptive Parameters (Feedback Loop):**
*   **Distance Threshold Update:** The threshold $R(p,t)$ is adjusted to move towards a value proportional to the running average of the minimum distance, $D_{min\_avg}$.
    If $R(p,t-1) < D_{min\_avg}(p,t-1) \cdot R_{scale}$, then $R(p,t) = R(p,t-1) \cdot (1 + R_{incdec})$.
    Else, $R(p,t) = R(p,t-1) \cdot (1 - R_{incdec})$.
    $R(p,t)$ is then clamped to a minimum value $R_{lower}$.

*   **Update Rate Divisor Update:** The update rate divisor $T(p,t)$ is adjusted based on the classification result.
    If $Mask(p,t)$ is Background: $T(p,t) = T(p,t-1) - \frac{T_{inc}}{D_{min\_avg}(p,t-1) + 1}$.
    If $Mask(p,t)$ is Foreground: $T(p,t) = T(p,t-1) + \frac{T_{dec}}{D_{min\_avg}(p,t-1) + 1}$.
    $T(p,t)$ is clamped to the range $[T_{lower}, T_{upper}]$.

*   **Mean Minimum Distance Update:** If the pixel was background, the running average is updated:
    $D_{min\_avg}(p,t) = (1 - 1/N) \cdot D_{min\_avg}(p,t-1) + (1/N) \cdot d_{min}(p,t)$.

**b. Background Model Update:**
If a pixel is classified as background, its model is updated with a probability of $1/T(p,t)$.
*   **Self-Update:** With probability $1/T(p,t)$, one of the $N$ background samples in $B_p$ is randomly chosen and replaced with the current feature vector $F_{in}(p,t)$.
*   **Neighbor Update:** With probability $1/T(p,t)$, a random spatial neighbor $q$ of pixel $p$ is chosen. One of the $N$ background samples in the neighbor's model, $B_q$, is randomly chosen and replaced with the neighbor's current feature vector $F_{in}(q,t)$.

### Summary

*   **Family:** Adaptive Non-parametric Sample-based Modeling.
*   **Idea (from code):** Maintains a per-pixel model of N recent background samples (color + gradient magnitude). A pixel is classified as background if its feature vector matches at least `#min` samples. The core of the algorithm is a feedback loop: the distance threshold `R` and the update rate `T` are not fixed but are dynamically adjusted for each pixel based on a running average of its minimum distance to the background model. This allows the algorithm to automatically tighten/loosen its parameters and speed up/slow down learning in response to local scene changes.
*   **Strengths:** Highly adaptive to dynamic scenes (e.g., waving trees, illumination changes) due to the per-pixel, self-adjusting threshold and update rate. The use of both color and gradient magnitude provides robustness.
*   **Weaknesses:** Computationally more expensive than methods with fixed parameters due to the per-pixel feedback calculations on every frame. The feedback mechanism itself is controlled by several meta-parameters (`R_incdec`, `T_inc`, etc.) which can be complex to tune.

---

## SigmaDelta
---

### Algorithm Description

The Sigma-Delta algorithm is a simple and efficient method for background subtraction. For each pixel (and each color channel independently), it maintains two key values: a running approximation of the background value ($M_t$) and an approximation of the per-pixel variance ($V_t$). The core idea is a two-step feedback loop. First, the background model $M_t$ is updated to slowly track the input image. Second, the variance model $V_t$ is updated to track an amplified version of the difference between the input image and the background model. A pixel is then classified as foreground if the instantaneous difference is greater than its estimated variance.

### Mathematical Formulation

The algorithm operates independently on each color channel of each pixel. Let $I_t$ be the value of a single channel of a single pixel at time $t$.

**State Variables:**
For each channel of each pixel, the model maintains:
*   $M_t$: The background model (approximated mean).
*   $V_t$: The variance model.

**Parameters:**
*   $N$: `ampFactor`, the amplification factor for the difference image.
*   $V_{min}$: `minVar`, the lower bound for the variance model.
*   $V_{max}$: `maxVar`, the upper bound for the variance model.

#### **1. Initialization (at t=0)**

*   The background model is initialized with the first frame:
    $M_0 = I_0$
*   The variance model is initialized to the minimum variance:
    $V_0 = V_{min}$

#### **2. Per-Frame Update (for t > 0)**

At each time step $t$, the following sequence of operations is performed for each pixel channel:

**a. Update Background Model ($M_t$):**
The background model is updated by incrementing or decrementing it by 1 in the direction of the current input pixel value. This is a simple 1-bit delta update.

$M_t = \begin{cases} M_{t-1} + 1 & \text{if } M_{t-1} < I_t \\ M_{t-1} - 1 & \text{if } M_{t-1} > I_t \\ M_{t-1} & \text{if } M_{t-1} = I_t \end{cases}$

**b. Calculate Difference Image ($O_t$):**
The absolute difference between the *newly updated* background model and the input pixel is calculated.

$O_t = |M_t - I_t|$

**c. Update Variance Model ($V_t$):**
The variance model is updated by incrementing or decrementing it by 1 in the direction of the *amplified* difference image.

Let the amplified difference be $O'_t = N \cdot O_t$.

$V_t' = \begin{cases} V_{t-1} + 1 & \text{if } V_{t-1} < O'_t \\ V_{t-1} - 1 & \text{if } V_{t-1} > O'_t \\ V_{t-1} & \text{if } V_{t-1} = O'_t \end{cases}$

The result is then clamped to the allowed variance range:
$V_t = \max(\min(V_t', V_{max}), V_{min})$

**d. Classification:**
The final classification is performed by comparing the non-amplified difference image with the *newly updated* variance model.

$F(p,t) = \begin{cases} \text{Foreground} & \text{if } O_t \ge V_t \\ \text{Background} & \text{if } O_t < V_t \end{cases}$

For 3-channel images, a pixel is marked as foreground if **any** of its color channels are classified as foreground.

### Summary

*   **Family:** Per-pixel Adaptive Filtering.
*   **Idea (from code):** For each pixel, it maintains a background model (M) and a variance model (V). In each frame, it first updates the background model by incrementing or decrementing it by 1 towards the current pixel's value. Then, it calculates the absolute difference (O) between this new background and the current pixel. The variance model is then updated by moving it by 1 towards an amplified version of this difference. A pixel is classified as foreground if its difference (O) is greater than or equal to its updated variance (V).
*   **Strengths:** Extremely fast and has a very low memory footprint, as it relies only on simple integer operations (increment/decrement) and comparisons. It is adaptive and can handle slow lighting changes.
*   **Weaknesses:** The fixed, single-step update mechanism adapts very slowly to rapid or large-scale changes in the background. It only models a single background mode per pixel, making it unsuitable for complex scenes with multi-modal distributions (e.g., waving trees). Performance is sensitive to the `ampFactor` parameter.

---

## StaticFrameDifference
---

### Algorithm Description

This algorithm implements a basic static frame differencing method. It captures the very first frame of the video sequence and stores it as a permanent, unchanging background model. For all subsequent frames, it calculates the absolute difference between the current frame and this static background model. The resulting difference image is converted to grayscale, and a binary foreground mask is produced by applying a fixed threshold.

### Mathematical Formulation

Let the input video sequence be $I(\vec{p}, t)$, where $\vec{p}$ is the pixel coordinate and $t$ is the frame number.

**Parameters:**
*   $\tau$: `threshold`, the brightness threshold for segmentation.

#### **1. Initialization (at t=0)**

The background model, $B(\vec{p})$, is initialized with the first frame of the sequence and never changes thereafter.

$B(\vec{p}) = I(\vec{p}, 0)$

#### **2. Foreground Detection (for t > 0)**

For each subsequent frame $I(\vec{p}, t)$:

**a. Absolute Difference:**
The absolute difference, $D(\vec{p}, t)$, between the current input frame and the static background model is computed for each color channel.

$D(\vec{p}, t) = |I(\vec{p}, t) - B(\vec{p})|$

**b. Grayscale Conversion:**
The resulting multi-channel difference image $D(\vec{p}, t)$ is converted to a single-channel grayscale image, $D_{gray}(\vec{p}, t)$. The implementation uses OpenCV's `cv::cvtColor` with the `CV_BGR2GRAY` flag, which applies the following standard conversion formula:

$D_{gray}(\vec{p}, t) = 0.299 \cdot D_R(\vec{p}, t) + 0.587 \cdot D_G(\vec{p}, t) + 0.114 \cdot D_B(\vec{p}, t)$

**c. Thresholding:**
A binary foreground mask, $F(\vec{p}, t)$, is obtained by applying a fixed threshold $\tau$ to the grayscale difference image.

$F(\vec{p}, t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } D_{gray}(\vec{p}, t) > \tau \\ 0 \text{ (Background)} & \text{if } D_{gray}(\vec{p}, t) \le \tau \end{cases}$

### Summary

*   **Family:** Static Frame Differencing.
*   **Idea (from code):** Captures the very first frame as a permanent, static background model. For every subsequent frame, it computes the absolute difference against this initial frame, converts the result to grayscale, and applies a fixed threshold to produce the foreground mask. The background model is never updated.
*   **Strengths:** Extremely fast and requires very little memory (stores only one frame). It is the simplest form of background subtraction.
*   **Weaknesses:** Completely non-adaptive. Any change in the background after the first frame (e.g., lighting changes, moved objects) will result in permanent false detections. It is highly sensitive to camera noise and jitter.

---

## SuBSENSE
---

### Algorithm Description

SuBSENSE (Self-Balanced Sensitivity Segmenter) is a highly adaptive, non-parametric background subtraction algorithm that builds upon the principles of ViBe and PBAS. For each pixel, it maintains a background model consisting of a fixed number of recent feature samples, where each sample is a pair containing a color vector and an LBSP texture descriptor. A pixel is classified as foreground if it fails to match a minimum number of these background samples. The core of SuBSENSE is its sophisticated, multi-layered feedback mechanism that dynamically adjusts per-pixel distance thresholds ($R$) and model update rates ($T$) based on several short-term and long-term local scene statistics, including segmentation stability and the minimum distance to the background model.

### Mathematical Formulation

#### **1. Feature Extraction and Background Model**

*   **Feature Vector:** For each pixel $p$, a feature vector $F(p,t)$ is computed, consisting of a color vector and an LBSP descriptor.
    $F(p,t) = \langle \vec{C}(p,t), \vec{D}(p,t) \rangle$
*   **Background Model:** For each pixel $p$, the background model $B_p$ is a collection of $N$ (`m_nBGSamples`) previously observed feature vectors:
    $B_p(t-1) = \{ F_{p,1}, F_{p,2}, \dots, F_{p,N} \}$, where $F_{p,i} = \langle \vec{C}_i, \vec{D}_i \rangle$.

#### **2. Per-Pixel Adaptive Parameters**

For each pixel $p$, the algorithm maintains several adaptive parameters that evolve over time:
*   $R_f(p,t)$: Distance threshold factor (`m_oDistThresholdFrame`).
*   $T(p,t)$: Model update rate divisor (`m_oUpdateRateFrame`).
*   $v(p,t)$: Variation modulator for the adaptation speed of $R_f$ and $T$ (`m_oVariationModulatorFrame`).
*   $D_{min}^{ST}(p,t), D_{min}^{LT}(p,t)$: Short-term and long-term moving averages of the minimum distance to the model (`m_oMeanMinDistFrame_ST/LT`).
*   $S_{raw}^{ST}(p,t), S_{raw}^{LT}(p,t)$: Short-term and long-term moving averages of the raw segmentation result (`m_oMeanRawSegmResFrame_ST/LT`).

#### **3. Foreground/Background Classification**

**a. Dynamic Threshold Calculation:**
For each pixel $p$, dynamic color and descriptor distance thresholds are calculated based on the adaptive factor $R_f(p,t)$:
*   $\tau_C(p,t) = R_f(p,t) \cdot \tau_{C,min}$
*   $\tau_D(p,t) = 2^{\lfloor R_f(p,t)+0.5 \rfloor} + \tau_{D,offset}$

**b. Distance Calculation and Matching:**
For an input feature vector $F_{in}(p,t)$, the algorithm computes two distances to each background sample $F_{p,i}$:
*   **Color Distance:** $d_C(p,i,t) = ||\vec{C}_{in}(p,t) - \vec{C}_i||_1$
*   **Descriptor Distance:** $d_D(p,i,t) = \frac{1}{2} (\text{hdist}(\vec{D}_{in}, \vec{D}_i) + \text{hdist}(\vec{D}'_{in}, \vec{D}_i))$, where $\vec{D}'_{in}$ is an LBSP descriptor computed with the background sample's color as the center.

A sample $i$ is considered a match if **both** distances are below their respective thresholds:
$d_C(p,i,t) \le \tau_C(p,t)$ AND $d_D(p,i,t) \le \tau_D(p,t)$

**c. Classification:**
The algorithm counts the number of matching samples, $C_{match}$.
$Mask(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if } C_{match} \ge \#_{min} \\ 255 \text{ (Foreground)} & \text{otherwise} \end{cases}$
where $\#_{min}$ is `m_nRequiredBGSamples`. The minimum combined distance $d_{min}(p,t)$ among the matches is also recorded.

#### **4. Parameter and Model Update**

**a. Update of Adaptive Parameters (Feedback Loop):**
This is the core of the algorithm. The parameters are updated based on feedback from the current and past frames.
*   **Update Rate Divisor ($T$):** $T(p,t)$ is increased if the pixel is foreground (slowing down learning) and decreased if it is background (speeding up learning). The magnitude of the change is inversely proportional to the mean minimum distance $D_{min}$ and scaled by the variation modulator $v(p,t)$.
    If Background: $T(p,t) = T(p,t-1) - \frac{T_{decr} \cdot v(p,t-1)}{D_{min}(p,t-1)}$
    If Foreground: $T(p,t) = T(p,t-1) + \frac{T_{incr}}{D_{min}(p,t-1) \cdot v(p,t-1)}$
*   **Variation Modulator ($v$):** $v(p,t)$ increases if the region is unstable (detected via "blinking" pixels) and decreases otherwise.
*   **Distance Threshold Factor ($R_f$):** $R_f(p,t)$ is adjusted to move towards a value related to the mean minimum distance. It increases if it's currently lower than a target based on $D_{min}$ and decreases otherwise. The magnitude of change is scaled by $v(p,t)$.
    $R_f(p,t) \approx R_f(p,t-1) \pm R_{var} \cdot v(p,t-1)$
*   **Moving Averages:** The short-term and long-term averages ($D_{min}^{ST/LT}$, $S_{raw}^{ST/LT}$) are updated using exponential moving averages with the new $d_{min}(p,t)$ and $Mask(p,t)$ values.

**b. Background Model Update:**
If a pixel is classified as background, its model is updated with a probability of $1/T(p,t)$.
*   **Self-Update:** With probability $1/T(p,t)$, one of the $N$ background samples in $B_p$ is randomly chosen and replaced with the current feature vector $F_{in}(p,t)$.
*   **Neighbor Update:** With probability $1/T(p,t)$, a random spatial neighbor $q$ is chosen, and one of the $N$ samples in its model, $B_q$, is replaced with the feature vector of pixel $p$, $F_{in}(p,t)$.

### Summary

*   **Family:** Adaptive Non-parametric Sample-based Modeling.
*   **Idea (from code):** Maintains a per-pixel model of N {color, LBSP descriptor} samples. A pixel is classified as background if it matches at least `#min` samples based on dynamic, per-pixel distance thresholds. The core of the algorithm is a feedback loop where these distance thresholds (`R`) and the model update rates (`T`) are continuously adjusted for each pixel based on local scene dynamics (like segmentation stability and the historical minimum distance to the model). Background pixels have a `1/T` chance to update their own model or a neighbor's model.
*   **Strengths:** Extremely adaptive to local conditions like illumination changes and camera jitter due to the per-pixel feedback mechanism for thresholds and learning rates. The fusion of color and LBSP texture features provides high robustness across various scene types.
*   **Weaknesses:** Very high computational cost and memory usage due to storing N samples per pixel plus multiple full-frame floating-point maps for the adaptive parameters. The feedback system has many meta-parameters, making it complex and difficult to tune from scratch.

---

## T2FGMM_UM / T2FGMM_UV
---

### Algorithm Description

The T2FGMM algorithm is a background subtraction method based on a Gaussian Mixture Model (GMM) for each pixel. It is an adaptation of the Grimson GMM that incorporates concepts from Type-2 Fuzzy logic to modify the matching process. Instead of a standard Mahalanobis distance, it uses a fuzzy distance metric, $H$, which is calculated differently for the two variants of the algorithm: Unimodal Variance (UM) and Univariate Variance (UV).

For each pixel, the algorithm maintains a mixture of $K$ Gaussian components, sorted by a significance measure ($\omega/\sigma$). The first $B$ components that cumulatively exceed a background weight threshold form the background model. A pixel is classified as background if its fuzzy distance to one of these background components is within a certain threshold. The algorithm produces two outputs based on a low and high threshold.

### Mathematical Formulation

#### **1. Gaussian Mixture Model**

For each pixel $p$, the model is a mixture of up to $K_{max}$ (`m_params.MaxModes()`) Gaussian components. Each component $k$ is defined by:
*   $\omega_{k,t}$: Weight.
*   $\vec{\mu}_{k,t}$: Mean color vector $(\mu_R, \mu_G, \mu_B)$.
*   $\sigma^2_{k,t}$: Variance.
*   $S_{k,t}$: Significance, defined as $S_{k,t} = \omega_{k,t} / \sigma_{k,t}$.

The components for each pixel are always sorted in descending order of significance $S$.

#### **2. Background Model Identification**

The background model consists of the first $B$ components in the sorted mixture for which the cumulative weight exceeds a threshold $T_B$ (`m_bg_threshold`).
$B = \min_{b \in \{1, \dots, K_{max}\}} \left( \sum_{k=1}^{b} \omega_{k,t} > T_B \right)$

#### **3. Fuzzy Distance Calculation and Matching**

Let the input pixel color vector be $\vec{I}_t = (I_R, I_G, I_B)$. For each Gaussian component $k$, the absolute color difference is $\vec{d}_k = |\vec{\mu}_{k,t} - \vec{I}_t| = (d_R, d_G, d_B)$.

A fuzzy distance vector, $\vec{H}_k = (H_R, H_G, H_B)$, is calculated for each channel. The calculation depends on the algorithm variant:

**a. T2FGMM-UM (Unimodal Variance):**
This variant uses a fuzzy membership function that is parabolic within a central region and linear outside of it, controlled by a factor $k_m$ (`km`). For each channel $c \in \{R, G, B\}$:
$H_c = \begin{cases} \frac{d_c^2}{2\sigma_{k,t}^4} + \frac{k_m d_c}{\sigma_{k,t}^2} + \frac{k_m^2}{2} & \text{if } d_c \le k_m \sigma_{k,t}^2 \\ \frac{2 k_m d_c}{\sigma_{k,t}^2} & \text{if } d_c > k_m \sigma_{k,t}^2 \end{cases}$
*(Note: The implementation in the code has a slight deviation from this standard formulation, using $\sigma$ instead of $\sigma^2$ in the denominators, which is reflected here.)*
$H_c = \begin{cases} \frac{d_c^2}{2\sigma_{k,t}^2} + \frac{k_m d_c}{\sigma_{k,t}} + \frac{k_m^2}{2} & \text{if } d_c \le k_m \sigma_{k,t} \\ \frac{2 k_m d_c}{\sigma_{k,t}} & \text{if } d_c > k_m \sigma_{k,t} \end{cases}$

**b. T2FGMM-UV (Univariate Variance):**
This variant uses a fuzzy membership function that scales the Mahalanobis distance based on a factor $k_v$ (`kv`). For each channel $c \in \{R, G, B\}$:
$H_c = \left( \frac{1}{k_v^2} - k_v^2 \right) \frac{d_c^2}{2\sigma_{k,t}^2}$

**c. Total Distance and Matching:**
The total squared fuzzy distance is calculated as:
$D_{fuzzy}^2 = H_R^2 + H_G^2 + H_B^2$

A pixel $\vec{I}_t$ is considered a match for component $k$ if its distance is within a threshold scaled by the component's variance. The algorithm uses two thresholds, $T_{low}$ and $T_{high}$:
*   Match (Low): $D_{fuzzy}^2 < T_{low} \cdot \sigma_{k,t}^2$
*   Match (High): $D_{fuzzy}^2 < T_{high} \cdot \sigma_{k,t}^2$

#### **4. Classification**

A pixel is classified as background if it matches a component $k$ that is part of the background model (i.e., $k \le B$).
*   $Mask_{low}(p,t) = 0$ if a match (Low) is found for some $k \le B$.
*   $Mask_{high}(p,t) = 0$ if a match (High) is found for some $k \le B$.
Otherwise, the pixel is classified as foreground (255).

#### **5. Model Update**

**a. No Match Found:**
If the pixel does not match any existing component, the component with the lowest significance ($k=K_{max}$) is replaced with a new one:
*   $\vec{\mu}_{K_{max},t} = \vec{I}_t$
*   $\sigma^2_{K_{max},t} = \sigma^2_{init}$ (a high initial variance)
*   $\omega_{K_{max},t} = \alpha$ (a low initial weight)
The weights of all components are then renormalized.

**b. Match Found (for component $k_{match}$):**
*   **Update Weights:**
    $\omega_{k,t} = (1-\alpha)\omega_{k,t-1} + \alpha \cdot \delta_k$
    where $\delta_k=1$ for $k=k_{match}$ and $\delta_k=0$ otherwise.
*   **Update Mean and Variance:**
    Let $\rho = \alpha / \omega_{k_{match},t}$.
    $\vec{\mu}_{k_{match},t} = (1-\rho)\vec{\mu}_{k_{match},t-1} + \rho \vec{I}_t$
    $\sigma^2_{k_{match},t} = (1-\rho)\sigma^2_{k_{match},t-1} + \rho (D_{fuzzy}^2)$
The variance is clamped to a predefined range.

After updating, all component weights are renormalized, and the components are re-sorted based on their new significance values.

### Summary

*   **Family:** Fuzzy Gaussian Mixture Model.
*   **Idea (from code):** Models each pixel with a mixture of Gaussians, sorted by significance (`weight/variance`). The top-ranked Gaussians form the background model. Instead of a standard Mahalanobis distance, it uses a "fuzzy distance" metric to match a pixel to a Gaussian. The metric is calculated differently for the two variants: `T2FGMM-UM` uses a hybrid parabolic/linear function controlled by a factor `km`, while `T2FGMM-UV` uses a scaled squared distance controlled by `kv`. A pixel is background if its fuzzy distance to a background Gaussian is below a threshold.
*   **Strengths:** As a GMM, it can model multi-modal backgrounds (e.g., waving leaves). The fuzzy distance metric offers a more flexible matching criterion than a hard threshold, potentially providing better robustness to noise. It produces both a high and low confidence foreground mask.
*   **Weaknesses:** Computationally expensive due to the per-pixel mixture model and the sorting of Gaussians on every frame. It has a high memory footprint. The parameters controlling the fuzzy logic (`km`, `kv`) are not intuitive and add complexity to the tuning process. The model uses a single shared variance for all three color channels, which may not be optimal.

---

## T2FMRF_UM / T2FMRF_UV
---

### Algorithm Description

The T2FMRF algorithm enhances the T2FGMM segmentation by applying a Markov Random Field (MRF) to enforce spatial and temporal consistency. The process is two-staged:

1.  **Pixel-wise Classification:** An initial foreground/background classification is performed for each pixel using the standard T2FGMM algorithm (either the UM or UV variant). This stage also updates the parameters of the underlying Gaussian Mixture Model.
2.  **MRF Optimization:** The initial classification is treated as an initial labeling for an MRF. The goal is to find a final labeling that minimizes a global energy function. This energy function is composed of three parts: a data term (how well a pixel's label fits the GMM), a spatial smoothness term (encouraging neighboring pixels to have the same label), and a temporal smoothness term (encouraging the current label to be consistent with the previous frame's label). The algorithm uses an iterative optimization method called Iterated Conditional Modes (ICM) to find the final, spatially and temporally regularized segmentation.

### Mathematical Formulation

#### **1. Initial Segmentation (T2FGMM)**

This stage is identical to the T2FGMM algorithm described previously. It takes an input frame $I_t$ and produces an initial binary labeling $L_{init}(p,t)$ for each pixel $p$, where $l_p \in \{0 \text{ (Background)}, 1 \text{ (Foreground)}\}$. It also maintains the updated GMM for each pixel.

#### **2. MRF Energy Minimization**

The algorithm seeks to find a final labeling $L^*$ that minimizes a global energy function $E(L)$.
$L^* = \arg\min_{L} E(L)$

The global energy function is the sum of local potentials over all pixels:
$E(L) = \sum_{p} \left( V_{data}(l_p) + V_{spatial}(l_p, \mathcal{N}_p) + V_{temporal}(l_p, \mathcal{N}_p) \right)$
where $l_p$ is the label of pixel $p$, and $\mathcal{N}_p$ is the 8-connected neighborhood of $p$.

**a. Data Term ($V_{data}$):**
This term measures the cost of assigning a label $l_p$ to pixel $p$, given the underlying GMM. It is based on the squared difference between the pixel's grayscale intensity, $I_{gray}(p)$, and the mean of the primary Gaussian component, $\mu_{gray}(p)$, scaled by its variance $\sigma^2(p)$.

*   Cost for Background label ($l_p=0$):
    $V_{data}(l_p=0) = \frac{(I_{gray}(p) - \mu_{gray}(p))^2}{2\sigma^2(p)}$
*   Cost for Foreground label ($l_p=1$): The foreground is modeled as a Gaussian shifted by $2.5\sigma$ from the background mean.
    $V_{data}(l_p=1) = \frac{(I_{gray}(p) - (\mu_{gray}(p) \pm 2.5\sigma(p)))^2}{2\sigma^2(p)}$
    The sign is chosen to move the mean further away from the current pixel value.

**b. Spatial Smoothness Term ($V_{spatial}$):**
This term penalizes differences in labels between a pixel $p$ and its neighbors $q \in \mathcal{N}_p$. It is a Potts model potential.
$V_{spatial}(l_p, \mathcal{N}_p) = \sum_{q \in \mathcal{N}_p} \beta \cdot (1 - \delta(l_p, l_q))$
where $\delta(a,b)$ is the Kronecker delta (1 if $a=b$, 0 otherwise), and $\beta$ (`beta`) is a constant weighting factor. The energy increases by $\beta$ for each neighbor with a different label and decreases by $\beta$ for each neighbor with the same label.

**c. Temporal Smoothness Term ($V_{temporal}$):**
This term penalizes differences between the current label of pixel $p$ and the final labels of its neighbors (including itself) from the previous frame, $L_{t-1}$.
$V_{temporal}(l_p, \mathcal{N}_p) = \sum_{q \in \mathcal{N}_p \cup \{p\}} \beta_{time} \cdot (1 - \delta(l_p, l_q^{t-1}))$
where $\beta_{time}$ (`beta_time`) is a constant weighting factor. The energy increases by $\beta_{time}$ for each neighbor in the previous frame's final segmentation that had a different label, and decreases otherwise.

#### **3. Optimization (Iterated Conditional Modes - ICM)**

The algorithm uses ICM to minimize the global energy function. ICM is an iterative, greedy algorithm.

1.  **Initialization:** The labeling $L$ is initialized with the output of the T2FGMM stage, $L_{init}$.
2.  **Iteration:** For a fixed number of iterations (in this implementation, 2 iterations):
    *   For each pixel $p$:
        *   Calculate the total local energy for assigning the Background label (0): $E_{local}(l_p=0) = V_{data}(0) + V_{spatial}(0, \mathcal{N}_p) + V_{temporal}(0, \mathcal{N}_p)$.
        *   Calculate the total local energy for assigning the Foreground label (1): $E_{local}(l_p=1) = V_{data}(1) + V_{spatial}(1, \mathcal{N}_p) + V_{temporal}(1, \mathcal{N}_p)$.
        *   Update the pixel's label to the one that yields the minimum local energy:
            $l_p^{new} = \arg\min_{l \in \{0,1\}} E_{local}(l_p=l)$
3.  **Finalization:** The labeling $L$ after the final iteration is the output segmentation mask.

### Summary

*   **Family:** Hybrid GMM with Markov Random Field (MRF) regularization.
*   **Idea (from code):** It's a two-stage process. First, it uses the T2FGMM (UM or UV) algorithm to produce an initial, pixel-wise foreground probability map. Second, this map is used as the data term in a Markov Random Field energy function. This energy function is then minimized using an iterative optimization (ICM), which enforces spatial smoothness (neighboring pixels should have similar labels) and temporal smoothness (the current label should be similar to the previous frame's final label). The result is a spatially and temporally regularized version of the initial T2FGMM output.
*   **Strengths:** The MRF post-processing provides significant noise reduction, removing small, isolated foreground pixels and filling small holes within detected objects. The temporal component adds stability to the segmentation, reducing flickering.
*   **Weaknesses:** Extremely high computational cost, as it runs a full T2FGMM and then performs an iterative MRF optimization on top. It introduces additional complex parameters (`beta` for space, `beta_time` for time) that must be tuned. The temporal constraint can cause ghost objects to persist longer if the underlying GMM is slow to adapt.

---

## TwoPoints
---

### Algorithm Description

The `TwoPoints` algorithm is a non-parametric background subtraction method that models the background of each pixel using two intensity values, which we will call $H_1$ and $H_2$. These two points do not represent a strict min/max range but rather two representative samples of the background. A pixel is considered background if its intensity is close to *either* of these two points. The matching threshold is adaptive; it is the maximum of a fixed global threshold and the distance between the two model points for that pixel. The model is updated stochastically: for pixels classified as background, one of the two model points is randomly chosen and replaced with the current pixel's intensity. This update also propagates to a random spatial neighbor.

### Mathematical Formulation

The algorithm operates independently on each pixel $p$.

**State Variables:**
For each pixel $p$, the model maintains two 8-bit integer values:
*   $H_1(p,t)$: The first background history point.
*   $H_2(p,t)$: The second background history point.

**Parameters:**
*   $\tau_{base}$: `matchingThreshold`, the base matching threshold.
*   $U_f$: `updateFactor`, a parameter controlling the stochastic update rate (though its use in the code is indirect, influencing random jumps).

#### **1. Initialization (at t=0)**

For each pixel $p$ with intensity $I(p,0)$, the two history points are initialized with a small amount of noise around the initial value.
*   $H_1(p,0) = \text{clamp}_{0,255}(I(p,0) - 10)$
*   $H_2(p,0) = \text{clamp}_{0,255}(I(p,0) + 10)$
The values are then swapped if necessary to ensure $H_1(p,0) \le H_2(p,0)$.

#### **2. Foreground/Background Classification (at t > 0)**

**a. Adaptive Threshold Calculation:**
For each pixel $p$, a dynamic matching threshold $\tau(p,t)$ is calculated:
$\tau(p,t) = \max(\tau_{base}, |H_2(p,t-1) - H_1(p,t-1)|)$

**b. Matching:**
An intermediate match count, $C_{match}(p,t)$, is computed. A pixel's intensity $I(p,t)$ matches a history point $H_i$ if the absolute difference is within the dynamic threshold.
$C_{match}(p,t) = \delta_1 + \delta_2$
where:
$\delta_1 = \begin{cases} 1 & \text{if } |I(p,t) - H_1(p,t-1)| \le \tau(p,t) \\ 0 & \text{otherwise} \end{cases}$
$\delta_2 = \begin{cases} 1 & \text{if } |I(p,t) - H_2(p,t-1)| \le \tau(p,t) \\ 0 & \text{otherwise} \end{cases}$

**c. Classification:**
The final foreground mask $F(p,t)$ is determined by the match count. The implementation uses the match count directly, but for a binary mask, the logic is:
$F(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if } C_{match}(p,t) > 0 \\ 255 \text{ (Foreground)} & \text{if } C_{match}(p,t) = 0 \end{cases}$
*(Note: The implementation actually outputs the match count (0, 1, or 2) to the segmentation map, which is then used as the update mask. A value > 0 implies background.)*

#### **3. Model Update (at t > 0)**

The update is performed stochastically only for pixels classified as background ($F(p,t)=0$). The update mechanism iterates through the image using random horizontal and vertical jumps.

For a background pixel $p$ selected for an update:

**a. Self-Update:**
One of the two history points is randomly chosen and replaced with the current pixel's intensity.
Let $k = \text{rand}(1,2)$.
$H_{k}(p,t) = I(p,t)$
$H_{3-k}(p,t) = H_{3-k}(p,t-1)$

**b. Neighbor Update:**
A random spatial neighbor, $q$, of pixel $p$ is selected. One of its history points is updated based on a comparison with the current pixel's intensity $I(p,t)$.
If $2 \cdot I(p,t) < (H_1(q,t-1) + H_2(q,t-1))$:
$H_1(q,t) = I(p,t)$
Else:
$H_2(q,t) = I(p,t)$

### Summary

*   **Family:** Non-parametric Sample-based Modeling.
*   **Idea (from code):** Models each pixel's background with two intensity values (H1, H2). A pixel is classified as background if its current value is close to either H1 or H2. The matching threshold is adaptive for each pixel, defined as the maximum of a base threshold and the distance between H1 and H2. For background pixels, the model is updated stochastically: one of the two points is randomly replaced with the current pixel's value, and this update is also propagated to a random neighbor.
*   **Strengths:** Very fast with a low memory footprint (two bytes per pixel). The adaptive threshold provides some tolerance for minor background variations and noise.
*   **Weaknesses:** The two-point model is too simplistic for complex scenes with dynamic textures or gradual illumination changes. The random, sparse update mechanism can lead to slow adaptation and persistent ghosting. The neighbor propagation logic can be unstable. It is designed for grayscale only.

---

## ViBe
---

### Algorithm Description

ViBe (Visual Background Extractor) is a non-parametric, sample-based background subtraction method. For each pixel, it maintains a background model consisting of a fixed number of intensity/color samples taken from previous frames. A pixel is classified as foreground if it is not "close" to a minimum number of samples in its background model. The model is updated stochastically: pixels classified as background have a chance to replace one of the samples in their own model and also to update the model of a random spatial neighbor. This spatial propagation allows the model to adapt to gradual changes and fill in occlusions.

### Mathematical Formulation

The algorithm operates on each pixel $p$.

**State Variables:**
For each pixel $p$, the background model $B_p$ is a collection of $N$ (`numberOfSamples`) previously observed intensity/color values:
$B_p(t-1) = \{ S_{p,1}, S_{p,2}, \dots, S_{p,N} \}$

**Parameters:**
*   $N$: `numberOfSamples`, the number of samples per pixel (default 20).
*   $\#_{min}$: `matchingNumber`, the minimum number of matches required to be classified as background (default 2).
*   $\tau$: `matchingThreshold`, the radius for matching (default 20).
*   $\phi$: `updateFactor`, the subsampling factor for updates (default 16). A 1-in-$\phi$ chance.

#### **1. Initialization (at t=0)**

The background model for each pixel $p$ is initialized by taking $N$ samples from the 8-connected neighborhood of $p$ in the first frame, $I(\cdot, 0)$.
$B_p(0) = \{ I(q_1, 0), I(q_2, 0), \dots, I(q_N, 0) \}$, where $q_i \in \mathcal{N}_8(p) \cup \{p\}$.
*(Note: The implementation shows a slightly different initialization where some samples are taken from the pixel itself with added noise, but the core idea is to populate the model from the local neighborhood of the first frame.)*

#### **2. Foreground/Background Classification (at t > 0)**

**a. Matching:**
For an input pixel with value $I(p,t)$, the algorithm counts the number of samples in its background model, $C_{match}$, that are within a certain distance.

*   **For Grayscale Images:** The distance is the absolute difference.
    $d(I(p,t), S_{p,i}) = |I(p,t) - S_{p,i}|$
    The matching condition is $d(I(p,t), S_{p,i}) \le \tau$.
    *(Note: The implementation contains a call to an unused adaptive threshold `distance_Han2014Improved`, but the C3R version uses the fixed threshold, which is more standard for ViBe.)*

*   **For Color Images:** The distance is the L1 norm (Manhattan distance).
    $d(I(p,t), S_{p,i}) = ||\vec{I}(p,t) - \vec{S}_{p,i}||_1 = |I_R - S_R| + |I_G - S_G| + |I_B - S_B|$
    The matching condition is $d(I(p,t), S_{p,i}) \le \tau_{color}$, where the implementation uses a scaled threshold $\tau_{color} = 4.5 \cdot \tau$.

The match count is:
$C_{match}(p,t) = |\{ i \mid d(I(p,t), S_{p,i}) \le \tau \}|$

**b. Classification:**
The pixel is classified based on the match count.
$F(p,t) = \begin{cases} 0 \text{ (Background)} & \text{if } C_{match}(p,t) \ge \#_{min} \\ 255 \text{ (Foreground)} & \text{otherwise} \end{cases}$

#### **3. Model Update (at t > 0)**

The update is performed stochastically only for pixels classified as background ($F(p,t)=0$).

For a background pixel $p$:

**a. Self-Update:**
The pixel has a probability of $1/\phi$ of updating its own model. If selected, a randomly chosen sample from its model, $S_{p,k}$, is replaced with the current pixel's value.
Let $k = \text{rand}(1,N)$.
$S_{p,k}(t) = I(p,t)$

**b. Neighbor Update:**
The pixel also has a probability of $1/\phi$ of updating the model of a random spatial neighbor, $q \in \mathcal{N}_8(p)$. If selected, a randomly chosen sample from the neighbor's model, $S_{q,k}$, is replaced with the current pixel's value (from pixel $p$).
Let $q = p + \text{rand_neighbor_offset}()$.
Let $k = \text{rand}(1,N)$.
$S_{q,k}(t) = I(p,t)$

*(Note: The implementation uses a precomputed table of random numbers to decide if an update occurs and which sample/neighbor to update, but the mathematical principle is a $1/\phi$ probability for both events.)*

### Summary

*   **Family:** Non-parametric Sample-based Modeling.
*   **Idea (from code):** Maintains a per-pixel model of N background color samples. A pixel is classified as background if its color is close to at least `#min` of these samples within a fixed radius. For pixels classified as background, a stochastic update occurs: there is a 1-in-`updateFactor` chance that the current pixel's value will replace a randomly chosen sample in its own model. Additionally, there is a 1-in-`updateFactor` chance that the pixel's value will be used to replace a random sample in a random neighbor's model.
*   **Strengths:** Fast, using only simple distance calculations and integer operations. The sample-based model can handle some background motion (e.g., waving trees). The random spatial propagation of samples helps the model adapt to gradual changes and fill in ghosts left by removed objects.
*   **Weaknesses:** Uses a fixed global distance threshold, making it highly sensitive to illumination changes. The random update mechanism can be slow to converge or adapt to rapid, large-scale background changes. It has a significant memory footprint, storing N samples for every pixel.

---

## VuMeter
---

### Algorithm Description

The `VuMeter` algorithm is a histogram-based method for background subtraction. For each pixel, it maintains a histogram of grayscale intensity values observed over time. This histogram represents the probability distribution of the background for that pixel. The intensity range (0-255) is divided into a fixed number of bins.

In each frame, the histogram for every pixel is updated using an exponential moving average. The bin corresponding to the current pixel's intensity is increased, while all bins are decayed by a learning factor. A pixel is classified as foreground if the probability (i.e., the value) of its current intensity bin in the histogram is below a fixed threshold. The background image is also updated, with each pixel's value changing to the current input pixel's value only if the current value has a higher probability in the histogram than the existing background value.

### Mathematical Formulation

The algorithm operates independently on each pixel $p$.

**State Variables:**
For each pixel $p$, the model maintains a histogram, $H_p(t)$, which is a vector of probabilities for each intensity bin.
*   $H_p(t) = [h_{p,0}(t), h_{p,1}(t), \dots, h_{p,B-1}(t)]$
    where $B$ is the number of bins (`m_nBinCount`).

**Parameters:**
*   $S_{bin}$: `m_nBinSize`, the size of each intensity bin. The number of bins is $B = 256 / S_{bin}$.
*   $\alpha$: `m_fAlpha`, the learning rate (decay factor).
*   $\tau$: `m_fThreshold`, the probability threshold for foreground detection.

#### **1. Initialization (at t=0)**

The histogram for each pixel is initialized to a zero distribution.
$h_{p,b}(0) = 0$ for all bins $b \in \{0, \dots, B-1\}$.
The background image, $BG(p,t)$, is initialized with the first frame.

#### **2. Per-Frame Update and Classification (for t > 0)**

For each pixel $p$ with grayscale intensity $I(p,t)$:

**a. Histogram Update (Exponential Moving Average):**
First, all bins in the histogram are decayed by the learning factor $\alpha$.
$h'_{p,b}(t) = \alpha \cdot h_{p,b}(t-1)$ for all $b \in \{0, \dots, B-1\}$.

Next, the bin corresponding to the current pixel's intensity is incremented. Let $b_{in}$ be the index of the bin for the current intensity:
$b_{in} = \lfloor I(p,t) / S_{bin} \rfloor$

The final updated histogram is:
$h_{p,b}(t) = \begin{cases} h'_{p,b}(t) + (1-\alpha) & \text{if } b = b_{in} \\ h'_{p,b}(t) & \text{if } b \ne b_{in} \end{cases}$

**b. Foreground/Background Classification:**
The probability of the current pixel's intensity, $P(I(p,t))$, is the value of its corresponding bin in the *newly updated* histogram.
$P(I(p,t)) = h_{p,b_{in}}(t)$

The pixel is classified as foreground if this probability is below the threshold $\tau$.
$F(p,t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } P(I(p,t)) < \tau \\ 0 \text{ (Background)} & \text{otherwise} \end{cases}$

**c. Background Model Update:**
The background image is updated by comparing the probability of the current pixel's intensity with the probability of the existing background pixel's intensity.
Let $b_{bg}$ be the bin index for the current background pixel's intensity, $BG(p,t-1)$.
$b_{bg} = \lfloor BG(p,t-1) / S_{bin} \rfloor$

The probability of the current background is $P(BG(p,t-1)) = h_{p,b_{bg}}(t)$.

The background is updated as follows:
$BG(p,t) = \begin{cases} I(p,t) & \text{if } P(I(p,t)) > P(BG(p,t-1)) \\ BG(p,t-1) & \text{otherwise} \end{cases}$

### Summary

*   **Family:** Per-pixel Histogram Modeling.
*   **Idea (from code):** Maintains a per-pixel histogram of grayscale intensity probabilities. In each frame, all histogram bins are decayed by a learning rate (`alpha`), and the bin for the current pixel's intensity is increased (an exponential moving average). A pixel is classified as foreground if the probability in its current intensity bin falls below a fixed threshold. The visible background image is updated to the current pixel's value only if its probability is higher than the existing background pixel's probability.
*   **Strengths:** Can model multi-modal backgrounds (e.g., waving trees) due to its histogram-based nature. The exponential moving average update allows it to adapt to gradual changes in the background.
*   **Weaknesses:** Very high memory footprint, as it stores a full histogram (an array of floats) for every pixel. Performance is highly dependent on the `binSize` parameter, creating a trade-off between intensity precision and memory usage. The implementation is designed for grayscale images only.

---

## WeightedMovingMean
---

### Algorithm Description

The `WeightedMovingMean` algorithm computes a background model by calculating a temporal average of the three most recent frames. It does not maintain a persistent, recursively updated background model like a standard moving average. Instead, at each time step, the background is freshly computed from the current frame and the two preceding frames. The algorithm has two modes: a weighted average, where the most recent frames are given higher importance, and a simple unweighted average. The foreground is then detected by thresholding the absolute difference between the current frame and this newly computed background.

### Mathematical Formulation

Let the input video sequence be $I(\vec{p}, t)$, where $\vec{p}$ is the pixel coordinate and $t$ is the frame number. The algorithm requires a history of the two previous frames, $I(\vec{p}, t-1)$ and $I(\vec{p}, t-2)$.

**Parameters:**
*   `enableWeight`: A boolean that switches between weighted and unweighted averaging.
*   $\tau$: `threshold`, the brightness threshold for segmentation.

#### **1. Background Model Calculation (at t > 1)**

At each time step $t$, the background model $B(\vec{p}, t)$ is calculated. The calculation depends on the `enableWeight` parameter.

**a. Weighted Mode (`enableWeight` = true):**
The background is a weighted average of the current frame and the two previous frames.
$B(\vec{p}, t) = 0.5 \cdot I(\vec{p}, t) + 0.3 \cdot I(\vec{p}, t-1) + 0.2 \cdot I(\vec{p}, t-2)$

**b. Unweighted Mode (`enableWeight` = false):**
The background is a simple average of the current frame and the two previous frames.
$B(\vec{p}, t) = \frac{I(\vec{p}, t) + I(\vec{p}, t-1) + I(\vec{p}, t-2)}{3}$

*(Note: The implementation performs these calculations using floating-point arithmetic with pixel values scaled to the range [0, 1], then converts the result back to an 8-bit integer image.)*

#### **2. Foreground Detection (at t > 1)**

**a. Absolute Difference:**
The absolute difference, $D(\vec{p}, t)$, between the current input frame and the *just-computed* background model is calculated.
$D(\vec{p}, t) = |I(\vec{p}, t) - B(\vec{p}, t)|$

**b. Grayscale Conversion:**
If the input is a 3-channel color image, the resulting difference image $D(\vec{p}, t)$ is converted to a single-channel grayscale image, $D_{gray}(\vec{p}, t)$, using the standard formula:
$D_{gray}(\vec{p}, t) = 0.299 \cdot D_R(\vec{p}, t) + 0.587 \cdot D_G(\vec{p}, t) + 0.114 \cdot D_B(\vec{p}, t)$

**c. Thresholding:**
A binary foreground mask, $F(\vec{p}, t)$, is obtained by applying a fixed threshold $\tau$ to the grayscale difference image.
$F(\vec{p}, t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } D_{gray}(\vec{p}, t) > \tau \\ 0 \text{ (Background)} & \text{if } D_{gray}(\vec{p}, t) \le \tau \end{cases}$

#### **3. State Update**

After processing frame $t$, the frame history is updated for the next iteration:
*   $I(\cdot, t-1)$ becomes the new $I(\cdot, t-2)$.
*   $I(\cdot, t)$ becomes the new $I(\cdot, t-1)$.

### Summary

*   **Family:** Temporal Averaging / Moving Average.
*   **Idea (from code):** At each time step, the background model is completely recalculated as a weighted average of the current frame (50%), the previous frame (30%), and the frame before that (20%). It then computes the absolute difference between the current frame and this newly generated background, converts it to grayscale, and applies a fixed threshold to create the foreground mask. It does not maintain a persistent, recursively updated background model.
*   **Strengths:** Simple, fast, and more stable against noise than simple frame differencing due to the 3-frame averaging window.
*   **Weaknesses:** Has a very short memory; any object that stops for more than two frames is immediately incorporated into the background and vanishes. The background model exhibits significant motion blur ("ghosting") around moving objects. It is not adaptive to any long-term changes.

---

## WeightedMovingVariance
---

### Algorithm Description

The `WeightedMovingVariance` algorithm is a statistical method that, like `WeightedMovingMean`, uses a sliding window of the three most recent frames. It does not maintain a persistent background model. Instead, at each time step, it first computes a mean image from the three frames. It then calculates the weighted sample variance of these three frames relative to their just-computed mean. The final output is generated by taking the square root of this variance (the standard deviation), converting it to an 8-bit image, and applying a fixed threshold. The algorithm does not use the difference between the input and the mean for segmentation, but rather the magnitude of the standard deviation itself.

### Mathematical Formulation

Let the input video sequence be $I(\vec{p}, t)$, where $\vec{p}$ is the pixel coordinate and $t$ is the frame number. The algorithm requires a history of the two previous frames, $I(\vec{p}, t-1)$ and $I(\vec{p}, t-2)$.

**Parameters:**
*   `enableWeight`: A boolean that switches between weighted and unweighted calculations.
*   $\tau$: `threshold`, the brightness threshold for segmentation.

#### **1. Mean Calculation (at t > 1)**

At each time step $t$, a mean image, $\mu(\vec{p}, t)$, is calculated from the three most recent frames.

**a. Weighted Mode (`enableWeight` = true):**
$\mu(\vec{p}, t) = 0.5 \cdot I(\vec{p}, t) + 0.3 \cdot I(\vec{p}, t-1) + 0.2 \cdot I(\vec{p}, t-2)$

**b. Unweighted Mode (`enableWeight` = false):**
*(Note: The implementation for the unweighted mean has a minor error, using a weight of 0.3 for all frames, which does not sum to 1. The formula reflects the code.)*
$\mu(\vec{p}, t) = 0.3 \cdot I(\vec{p}, t) + 0.3 \cdot I(\vec{p}, t-1) + 0.3 \cdot I(\vec{p}, t-2)$

#### **2. Weighted Variance Calculation (at t > 1)**

A weighted sample variance, $\sigma^2(\vec{p}, t)$, is calculated using the mean from the previous step.

**a. Weighted Mode (`enableWeight` = true):**
The weights are $w_0=0.5, w_1=0.3, w_2=0.2$.
$\sigma^2(\vec{p}, t) = w_0(I(\vec{p}, t) - \mu(\vec{p}, t))^2 + w_1(I(\vec{p}, t-1) - \mu(\vec{p}, t))^2 + w_2(I(\vec{p}, t-2) - \mu(\vec{p}, t))^2$

**b. Unweighted Mode (`enableWeight` = false):**
The weights are $w_0=w_1=w_2=0.3$.
$\sigma^2(\vec{p}, t) = 0.3(I(\vec{p}, t) - \mu(\vec{p}, t))^2 + 0.3(I(\vec{p}, t-1) - \mu(\vec{p}, t))^2 + 0.3(I(\vec{p}, t-2) - \mu(\vec{p}, t))^2$

*(Note: All calculations are performed using floating-point arithmetic with pixel values scaled to [0, 1].)*

#### **3. Foreground Detection (at t > 1)**

**a. Standard Deviation:**
The standard deviation, $\sigma(\vec{p}, t)$, is computed.
$\sigma(\vec{p}, t) = \sqrt{\sigma^2(\vec{p}, t)}$

**b. Grayscale Conversion and Scaling:**
The resulting standard deviation image, which is multi-channel if the input was color, is converted to a single-channel 8-bit grayscale image. This involves two steps:
1.  The floating-point image $\sigma(\vec{p}, t)$ is scaled from the range [0, 1] to [0, 255].
2.  If it's a color image, it is converted to grayscale using the standard formula:
    $\sigma_{gray}(\vec{p}, t) = 0.299 \cdot \sigma_R(\vec{p}, t) + 0.587 \cdot \sigma_G(\vec{p}, t) + 0.114 \cdot \sigma_B(\vec{p}, t)$

**c. Thresholding:**
A binary foreground mask, $F(\vec{p}, t)$, is obtained by applying a fixed threshold $\tau$ directly to the grayscale standard deviation image.
$F(\vec{p}, t) = \begin{cases} 255 \text{ (Foreground)} & \text{if } \sigma_{gray}(\vec{p}, t) > \tau \\ 0 \text{ (Background)} & \text{if } \sigma_{gray}(\vec{p}, t) \le \tau \end{cases}$

#### **4. State Update**

After processing frame $t$, the frame history is updated for the next iteration:
*   $I(\cdot, t-1)$ becomes the new $I(\cdot, t-2)$.
*   $I(\cdot, t)$ becomes the new $I(\cdot, t-1)$.

### Summary

*   **Family:** Temporal Statistical / Moving Variance.
*   **Idea (from code):** At each time step, it computes a mean image from the current and two previous frames. It then calculates the weighted sample variance of these three frames relative to their just-computed mean. The final output is the square root of this variance (the standard deviation), which is then converted to grayscale and thresholded. Pixels are classified as foreground if their local temporal standard deviation is high, not by comparing them to a background model.
*   **Strengths:** Fast and requires minimal memory. It is effective at detecting any kind of motion or change, as any temporal variation within the 3-frame window will increase the variance.
*   **Weaknesses:** It has an extremely short memory; any object that stops moving for more than two frames will have zero variance and will instantly vanish. The output highlights areas of change, not necessarily whole objects, and is sensitive to camera noise which can create high-variance pixels. It does not produce a stable background model.

---
