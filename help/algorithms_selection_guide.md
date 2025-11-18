# Practical Guide: Algorithm Recommendations by Challenge

Here is a guide recommending specific algorithms for common real-world computer vision challenges.

### Recommended Algorithms by Challenge

| Challenge | Recommended Algorithms | Why? |
| :--- | :--- | :--- |
| **Dynamic Backgrounds**<br>(Waving trees, water ripples, rain, fountains) | **MixtureOfGaussianV2 (Zivkovic)**<br>**IndependentMultimodal (IMBS)**<br>**ViBe** | **MOG2** and **IMBS** explicitly model multiple colors per pixel, allowing a pixel to be "sky" one moment and "leaf" the next. **ViBe** uses a history buffer that naturally captures these variations without assuming a Gaussian shape. |
| **Sudden Illumination Changes**<br>(Cloud movements, light switch toggles) | **SuBSENSE**<br>**LOBSTER**<br>**DPTexture** | These algorithms rely on **LBSP** or **LBP (Local Binary Patterns)** features. LBP compares a pixel's intensity to its neighbors rather than using raw color values, making it invariant to global brightness shifts. |
| **Camouflage**<br>(Foreground color is similar to background) | **SuBSENSE**<br>**PAWCS**<br>**MultiLayer** | These "fusion" methods combine **Color + Texture** (LBSP/LBP) into a single descriptor. Even if the color matches the background, the texture pattern (edges/gradients) will likely differ, allowing detection. |
| **Shadows**<br>(Moving cast shadows from objects) | **MixtureOfGaussianV2**<br>**KNN**<br>**IndependentMultimodal (IMBS)** | **MOG2** and **KNN** have built-in shadow detection logic (often checking for brightness drops without color shifts). **IMBS** converts pixels to HSV space explicitly to suppress shadows and highlights. |
| **High-Speed / Embedded**<br>(Raspberry Pi, mobile, high FPS requirements) | **ViBe**<br>**SigmaDelta**<br>**TwoPoints** | **ViBe** uses simple integer comparisons and random sampling, making it extremely fast. **SigmaDelta** and **TwoPoints** rely on basic increment/decrement operations, requiring minimal CPU cycles and memory. |
| **Sleeping Foreground**<br>(Objects that stop moving and become background) | **AdaptiveSelectiveBackgroundLearning**<br>**CodeBook** | **Selective Learning** stops updating the model when an object is detected, preventing it from fading into the background. **CodeBook** uses a "cache" system to hold potential background pixels before promoting them, handling stopped objects intelligently. |
| **Camera Jitter**<br>(Wind shaking the camera, vibrations) | **LBP\_MRF**<br>**T2FMRF**<br>**SuBSENSE** | **MRF (Markov Random Field)** methods enforce spatial coherence, meaning isolated noise pixels from jitter are smoothed out. **SuBSENSE** uses spatial propagation (checking neighbor's models) which helps stabilize the mask against small movements. |
| **Ghosting**<br>(False detection where an object *used* to be) | **IndependentMultimodal (IMBS)**<br>**MultiCue** | **IMBS** has specific "persistence" logic to distinguish between a new static object and a revealed background. **MultiCue** uses edge-based verification (Hausdorff distance) to reject foreground blobs that don't match real image edges. |

### Decision Heuristic

  * **For general purpose use:** Start with **MixtureOfGaussianV2 (MOG2)** or **KNN** (available in OpenCV). They offer the best balance of performance and ease of use.
  * **For complex outdoor scenes:** Use **SuBSENSE** or **PBAS**. They are the "heavy lifters" that auto-tune themselves to handle wind, rain, and light simultaneously.
  * **For strictly static cameras (security):** **ViBe** is often the best choice for its speed and rapid bootstrapping.

