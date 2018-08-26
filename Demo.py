import numpy as np
import cv2
import libbgs

bgs = libbgs.FrameDifference()
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
