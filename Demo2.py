import numpy as np
import cv2
import libbgs

print("OpenCV Version: {}".format(cv2.__version__))

def is_cv2():
  return check_opencv_version("2.")

def is_cv3():
  return check_opencv_version("3.")

def check_opencv_version(major):
  return cv2.__version__.startswith(major)

## BGS Library algorithms
algorithms=[]
algorithms.append(libbgs.FrameDifference())
algorithms.append(libbgs.StaticFrameDifference())
algorithms.append(libbgs.AdaptiveBackgroundLearning())
algorithms.append(libbgs.AdaptiveSelectiveBackgroundLearning())
algorithms.append(libbgs.DPAdaptiveMedian())
algorithms.append(libbgs.DPEigenbackground())
algorithms.append(libbgs.DPGrimsonGMM())
algorithms.append(libbgs.DPMean())
algorithms.append(libbgs.DPPratiMediod())
algorithms.append(libbgs.DPTexture())
algorithms.append(libbgs.DPWrenGA())
algorithms.append(libbgs.DPZivkovicAGMM())
algorithms.append(libbgs.FuzzyChoquetIntegral())
algorithms.append(libbgs.FuzzySugenoIntegral())
algorithms.append(libbgs.IndependentMultimodal())
algorithms.append(libbgs.KDE())
if is_cv3():
  algorithms.append(libbgs.KNN()) # if opencv 3.x
algorithms.append(libbgs.LBAdaptiveSOM())
algorithms.append(libbgs.LBFuzzyAdaptiveSOM())
algorithms.append(libbgs.LBFuzzyGaussian())
algorithms.append(libbgs.LBMixtureOfGaussians())
algorithms.append(libbgs.LBSimpleGaussian())
algorithms.append(libbgs.LBP_MRF())
algorithms.append(libbgs.LOBSTER())
if is_cv2():
  algorithms.append(libbgs.GMG()) # if opencv 2.x
  algorithms.append(libbgs.MixtureOfGaussianV1()) # if opencv 2.x
algorithms.append(libbgs.MixtureOfGaussianV2())
algorithms.append(libbgs.MultiCue())
algorithms.append(libbgs.MultiLayer())
algorithms.append(libbgs.PAWCS())
algorithms.append(libbgs.PixelBasedAdaptiveSegmenter())
algorithms.append(libbgs.SigmaDelta())
algorithms.append(libbgs.SuBSENSE())
algorithms.append(libbgs.T2FGMM_UM())
algorithms.append(libbgs.T2FGMM_UV())
algorithms.append(libbgs.T2FMRF_UM())
algorithms.append(libbgs.T2FMRF_UV())
algorithms.append(libbgs.VuMeter())
algorithms.append(libbgs.WeightedMovingMean())
algorithms.append(libbgs.WeightedMovingVariance())
algorithms.append(libbgs.TwoPoints())
algorithms.append(libbgs.ViBe())
algorithms.append(libbgs.CodeBook())

video_file = "dataset/video.avi"

for bgs in algorithms:
  print("Running ", bgs.__class__)
  
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
      
      img_output = bgs.apply(frame)
      img_bgmodel = bgs.getBackgroundModel()
      
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