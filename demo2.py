import numpy as np
import cv2
import pybgs as bgs

print("OpenCV Version: {}".format(cv2.__version__))

def is_cv2():
  return check_opencv_version("2.")

def is_cv3():
  return check_opencv_version("3.")

def is_cv4():
  return check_opencv_version("4.")

def check_opencv_version(major):
  return cv2.__version__.startswith(major)

## bgslibrary algorithms
algorithms=[]
algorithms.append(bgs.FrameDifference())
algorithms.append(bgs.StaticFrameDifference())
algorithms.append(bgs.WeightedMovingMean())
algorithms.append(bgs.WeightedMovingVariance())
algorithms.append(bgs.AdaptiveBackgroundLearning())
algorithms.append(bgs.AdaptiveSelectiveBackgroundLearning())
algorithms.append(bgs.MixtureOfGaussianV2())
if is_cv2():
  algorithms.append(bgs.MixtureOfGaussianV1()) # if opencv 2.x
  algorithms.append(bgs.GMG()) # if opencv 2.x
if is_cv3():
  algorithms.append(bgs.KNN()) # if opencv 3.x
if is_cv2() or is_cv3():
  algorithms.append(bgs.DPAdaptiveMedian())
  algorithms.append(bgs.DPGrimsonGMM())
  algorithms.append(bgs.DPZivkovicAGMM())
  algorithms.append(bgs.DPMean())
  algorithms.append(bgs.DPWrenGA())
  algorithms.append(bgs.DPPratiMediod())
  algorithms.append(bgs.DPEigenbackground())
  algorithms.append(bgs.DPTexture())
  algorithms.append(bgs.T2FGMM_UM())
  algorithms.append(bgs.T2FGMM_UV())
  algorithms.append(bgs.T2FMRF_UM())
  algorithms.append(bgs.T2FMRF_UV())
  algorithms.append(bgs.FuzzySugenoIntegral())
  algorithms.append(bgs.FuzzyChoquetIntegral())
  algorithms.append(bgs.LBSimpleGaussian())
  algorithms.append(bgs.LBFuzzyGaussian())
  algorithms.append(bgs.LBMixtureOfGaussians())
  algorithms.append(bgs.LBAdaptiveSOM())
  algorithms.append(bgs.LBFuzzyAdaptiveSOM())
  algorithms.append(bgs.LBP_MRF())
  algorithms.append(bgs.MultiLayer())
  algorithms.append(bgs.PixelBasedAdaptiveSegmenter())
  algorithms.append(bgs.VuMeter())
  algorithms.append(bgs.KDE())
  algorithms.append(bgs.IndependentMultimodal())
  algorithms.append(bgs.MultiCue())
algorithms.append(bgs.SigmaDelta())
algorithms.append(bgs.SuBSENSE())
algorithms.append(bgs.LOBSTER())
algorithms.append(bgs.PAWCS())
algorithms.append(bgs.TwoPoints())
algorithms.append(bgs.ViBe())
algorithms.append(bgs.CodeBook())

video_file = "dataset/video.avi"

for algorithm in algorithms:
  print("Running ", algorithm.__class__)
  
  capture = cv2.VideoCapture(video_file)
  while not capture.isOpened():
    capture = cv2.VideoCapture(video_file)
    cv2.waitKey(1000)
    print("Wait for the header")

  #pos_frame = capture.get(cv2.cv.CV_CAP_PROP_POS_FRAMES)
  #pos_frame = capture.get(cv2.CV_CAP_PROP_POS_FRAMES)
  pos_frame = capture.get(1)
  while True:
    flag, frame = capture.read()
    
    if flag:
      cv2.imshow('video', frame)
      #pos_frame = capture.get(cv2.cv.CV_CAP_PROP_POS_FRAMES)
      #pos_frame = capture.get(cv2.CV_CAP_PROP_POS_FRAMES)
      pos_frame = capture.get(1)
      #print str(pos_frame)+" frames"
      
      img_output = algorithm.apply(frame)
      img_bgmodel = algorithm.getBackgroundModel()
      
      cv2.imshow('img_output', img_output)
      cv2.imshow('img_bgmodel', img_bgmodel)

    else:
      #capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, pos_frame-1)
      #capture.set(cv2.CV_CAP_PROP_POS_FRAMES, pos_frame-1)
      #capture.set(1, pos_frame-1)
      #print "Frame is not ready"
      cv2.waitKey(1000)
      break
    
    if 0xFF & cv2.waitKey(10) == 27:
      break
    
    #if capture.get(cv2.cv.CV_CAP_PROP_POS_FRAMES) == capture.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT):
    #if capture.get(cv2.CV_CAP_PROP_POS_FRAMES) == capture.get(cv2.CV_CAP_PROP_FRAME_COUNT):
    #if capture.get(1) == capture.get(cv2.CV_CAP_PROP_FRAME_COUNT):
      #break

  cv2.destroyAllWindows()

print("Finished")
