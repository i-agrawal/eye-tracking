#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <cmath>
#include <stdio.h>

using namespace std;
using namespace cv;

bool leftDown;
int prevX, prevY, newX, newY, px, py;

void mouseHandler(int event, int x, int y, int flags, void* userdata){
  if(event == EVENT_LBUTTONDOWN){
    leftDown = true;
    prevX = x;
    prevY = y;
  }
  else if(event == EVENT_LBUTTONUP)
    leftDown = false;
  px = x;
  py = y;
}

int close(Rect &rect, int x, int y){
  if(abs(x - rect.x) < 10 && abs(y - rect.y) < 10)
    return 1;
  else if(abs(x - (rect.x + rect.width)) < 10 && abs(y - rect.y) < 10)
    return 2;
  else if(abs(x - rect.x) < 10 && abs(y - (rect.y + rect.height)) < 10)
    return 3;
  else if(abs(x - (rect.x + rect.width)) < 10 && abs(y - (rect.y + rect.height)) < 10)
    return 4;
  else if(abs(y - rect.y) < 10)
    return 5;
  else if(abs(x - (rect.x + rect.width)) < 10)
    return 6;
  else if(abs(y - (rect.y + rect.height)) < 10)
    return 7;
  else if(abs(x - rect.x) < 10)
    return 8;
  else if(x > rect.x && x < rect.x + rect.width && y > rect.y && y < rect.y + rect.height)
    return 9;
  return -1;
}

void changeRect(Rect &rect, int cx, int cy, int cw, int ch){
  rect.x += cx;
  rect.y += cy;
  if(rect.width + cw < 0){
    rect.x += rect.width + cw;
    rect.width = abs(rect.width + cw);
  }
  else{
    rect.width += cw;
  }
  if(rect.height + ch < 0){
    rect.y += rect.height + cw;
    rect.height = abs(rect.height + cw);
  }
  else{
    rect.height += ch;
  }
}

int main(int argc, const char ** argv){
  leftDown = false;
  prevX = prevY = newX = newY = 0;
  String face_cascade_name = "haarcascade_frontalface_alt.xml";
  CascadeClassifier face_cascade;
  string window_name = "WINDOW";

  CvCapture* capture;
  Mat frame, gray_frame;

  string path = argv[1];
  path += "/";
  face_cascade_name = path + face_cascade_name;

  //-- 1. Load the cascades
  if(!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };

  double offset = 0.0;
  std::vector<Rect> faces;
  namedWindow(window_name, 1);
  capture = cvCaptureFromCAM( -1 );

  if(capture){
    int k = -1;
    while(k == -1){
      frame = cvarrToMat(cvQueryFrame(capture));
      flip(frame, frame, 1);
      imshow(window_name, frame);
      k = waitKey(10);
    }
  }

  Mat frameClone = frame.clone();
  cvtColor(frame, gray_frame, CV_BGR2GRAY);
  equalizeHist(gray_frame, gray_frame);
  face_cascade.detectMultiScale(gray_frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50));

  if(faces.empty()){
    printf("--(!) Error No Face Detected In Image. Quitting\n");
    return -1;
  }

  Rect rect = faces[0];
  rectangle(frameClone, rect, Scalar(255, 0, 0));

  int place = 0;
  double changeX = 0, changeY = 0;
  setMouseCallback(window_name, mouseHandler, NULL);
  int k = -1;
  while(k == -1){
    imshow(window_name, frameClone);

    if(leftDown){
      place = close(rect, prevX, prevY);
      if(place != -1){
        newX = px;
        newY = py;
        changeX = newX - prevX;
        changeY = newY - prevY;

        if(place == 1)
          changeRect(rect, changeX, changeY, -changeX, -changeY);
        else if(place == 2)
          changeRect(rect, 0, changeY, changeX, -changeY);
        else if(place == 3)
          changeRect(rect, changeX, 0, -changeX, changeY);
        else if(place == 4)
          changeRect(rect, 0, 0, changeX, changeY);
        else if(place == 5)
          changeRect(rect, 0, changeY, 0, -changeY);
        else if(place == 6)
          changeRect(rect, 0, 0, changeX, 0);
        else if(place == 7)
          changeRect(rect, 0, 0, 0, changeY);
        else if(place == 8)
          changeRect(rect, changeX, 0, -changeX, 0);
        else if(place == 9)
          changeRect(rect, changeX, changeY, 0, 0);

        frameClone = frame.clone();
        rectangle(frameClone, rect, Scalar(255, 0, 0));
        prevX = px;
        prevY = py;
      }
    }

    k = waitKey(10);
  }
  return 0;
}
