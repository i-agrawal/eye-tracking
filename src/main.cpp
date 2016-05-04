//the classes we created
#include "Reader.h"
#include "NeuralNetwork.h"
//basic includes
#include <iostream>

using namespace std;
using namespace cv;

void on_mouse(int a, int x, int y, int b, void * ptr){
    Point*p = (Point*)ptr;
    p->x = x;
    p->y = y;
}

int main(int argc, char ** argv){
  string path = argv[1];
  path += "/haarcascade_frontalface_alt.xml";
  cout << path << endl;
  Reader iReader(path);
  //Point to store cursor position
  Point pt;
  //the fps
  const int fps = 100;
  //the ms per frame
  const int ms = 1000 / fps;
  //cpture to get user camera input
  CvCapture * capture;
  //image of user input
  Mat frame, ROI;
  //capture now set to user camera
  capture = cvCaptureFromCAM(-1);

  //Mat screen = Mat(cvSize(screen_width, screen_height), CV_64F, Scalar(255, 255, 255));

  if(capture){
    cout << "Press a button to quit" << endl;
    while(true){

      frame = cvarrToMat(cvQueryFrame(capture));
      flip(frame, frame, 1);
      iReader.Display(frame);

      //break out when user presses a key
      int k = waitKey(ms);
      if(k != -1)
        break;
    }
  }

  return 0;
}
