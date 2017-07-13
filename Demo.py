import numpy as np
import cv2
import libbgs

## BGS Library algorithms
bgs = libbgs.FrameDifference()
#bgs = libbgs.StaticFrameDifference()
#bgs = libbgs.AdaptiveBackgroundLearning()
#bgs = libbgs.AdaptiveSelectiveBackgroundLearning()
#bgs = libbgs.DPAdaptiveMedian()
#bgs = libbgs.DPEigenbackground()
#bgs = libbgs.DPGrimsonGMM()
#bgs = libbgs.DPMean()
#bgs = libbgs.DPPratiMediod()
#bgs = libbgs.DPTexture()
#bgs = libbgs.DPWrenGA()
#bgs = libbgs.DPZivkovicAGMM()
#bgs = libbgs.FuzzyChoquetIntegral()
#bgs = libbgs.FuzzySugenoIntegral()
#bgs = libbgs.GMG() # if opencv 2.x
#bgs = libbgs.IndependentMultimodal()
#bgs = libbgs.KDE()
#bgs = libbgs.KNN() # if opencv 3.x
#bgs = libbgs.LBAdaptiveSOM()
#bgs = libbgs.LBFuzzyAdaptiveSOM()
#bgs = libbgs.LBFuzzyGaussian()
#bgs = libbgs.LBMixtureOfGaussians()
#bgs = libbgs.LBSimpleGaussian()
#bgs = libbgs.LBP_MRF()
#bgs = libbgs.LOBSTER()
#bgs = libbgs.MixtureOfGaussianV1() # if opencv 2.x
#bgs = libbgs.MixtureOfGaussianV2()
#bgs = libbgs.MultiCue()
#bgs = libbgs.MultiLayer()
#bgs = libbgs.PAWCS()
#bgs = libbgs.PixelBasedAdaptiveSegmenter()
#bgs = libbgs.SigmaDelta()
#bgs = libbgs.SuBSENSE()
#bgs = libbgs.T2FGMM_UM()
#bgs = libbgs.T2FGMM_UV()
#bgs = libbgs.T2FMRF_UM()
#bgs = libbgs.T2FMRF_UV()
#bgs = libbgs.VuMeter()
#bgs = libbgs.WeightedMovingMean()
#bgs = libbgs.WeightedMovingVariance()
#bgs = libbgs.TwoPoints()
#bgs = libbgs.ViBe()
#bgs = libbgs.CodeBook()

video_file = "dataset/video.avi"

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
		img_bgmodel = bgs.getBackgroundModel();
		
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
