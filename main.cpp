#include "neural-network/NeuralNetwork.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int main(){
  CvCapture * capture;
  Mat frame;
  capture = cvCaptureFromCAM(-1);
  namedWindow("Display Window", WINDOW_AUTOSIZE);
  if(capture){
    while(true){
      frame = cvarrToMat(cvQueryFrame(capture));
      flip(frame, frame, 1);
      imshow("Display Window", frame);

      int c = waitKey(10);
      if((char)c == 'c') {
        break;
      }
    }
  }

  return 0;
}
