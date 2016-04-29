#include "neural-network/NeuralNetwork.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;

Point pt;

void onMouse(int event, int x, int y, int, void*){
  pt = Point(x, y);
  //uncomment if you wish to see points
  //cout << "x=" << pt.x << "\t y=" << pt.y << endl;
}

int main(){
  CvCapture * capture;
  Mat frame;
  Mat screen;
  int xres, yres;
  Size rsz(40, 40);
  capture = cvCaptureFromCAM(-1);
  vector<unsigned> setup;
  setup.push_back(1600);
  setup.push_back(900);
  setup.push_back(400);
  setup.push_back(100);
  setup.push_back(2);
  NeuralNetwork net(setup);
  net.save_net("netsave.txt");
  namedWindow("Display", WINDOW_AUTOSIZE);
  vector<Mat> channels(3);

  vector<double> input, target;

  cout << "Enter c when ready" << endl;
  while(true){
    int c = waitKey(10);
    if((char)c == 'c') {
      break;
    }
  }

  if(capture){
    frame = cvarrToMat(cvQueryFrame(capture));
    xres = frame.cols;
    yres = frame.rows;
    int x = 1;
    while(true){
      input.clear();
      target.clear();
      frame = cvarrToMat(cvQueryFrame(capture));
      flip(frame, frame, 1);
      //uncomment if you want to see the image capture
      //imshow("Display Window", frame);
      resize(frame, frame, rsz);
      split(frame, channels);
      frame = channels[2];
      //imshow("Display", screen);

      setMouseCallback("Display", onMouse, 0);

      for(unsigned i = 0; i < 40; ++i){
        for(unsigned j = 0; j < 40; ++j){
          //cout << static_cast<double>(frame.at<uchar>(i, j)) << endl;
          input.push_back(static_cast<double>(frame.at<uchar>(i, j)) / 255.0);
        }
      }

      net.feed_forward(input);
      target.push_back(0.5);
      target.push_back(0.5);
      net.back_prop(target);

      int c = waitKey(10);
      if((char)c == 'c') {
        break;
      }
    }
  }

  //net.save_net("netsave.txt");

  return 0;
}
