//the neural-network class we created
#include "NeuralNetwork.h"
//the opencv files we need
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//basic includes
#include <iostream>

using namespace std;
using namespace cv;

void on_mouse(int a, int x, int y, int b, void * ptr){
    Point*p = (Point*)ptr;
    p->x = x;
    p->y = y;
}

int main(){
  //Point to store cursor position
  Point pt;
  //the fps
  const int fps = 60;
  //the ms per frame
  const int ms = 1000 / fps;

  setup network
  vector<unsigned> setup;
  setup.push_back(900);
  setup.push_back(400);
  setup.push_back(100);
  setup.push_back(2);
  NeuralNetwork net(setup);
  //net.save_net("netsave.txt");

  //cpture to get user camera input
  CvCapture * capture;
  //image of user input
  Mat frame;
  //user screen size
  double screen_height, screen_width;
  //the resize for the image
  Size rsz(30, 30);
  //capture now set to user camera
  capture = cvCaptureFromCAM(-1);

  //ask for swidth and sheight
  // cout << "Enter Screen Width and Screen Height" << endl;
  // cin >> screen-width;
  // cin >> screen-height;
  screen_width = 1280;
  screen_height = 800;

  //display window
  namedWindow("Display", 0);
  //used to greyscale image
  vector<Mat> channels(3);

  //used to input into neural network
  vector<double> input, target;

  //wait for user to be ready;
  cout << "Stare at your mouse cursor to calibrate the neural network" << endl;
  cout << "Press a button when ready" << endl;
  resizeWindow("Display", screen_width, screen_height);
  waitKey(0);

  Mat screen = Mat(cvSize(screen_width, screen_height), CV_64F, Scalar(255, 255, 255));

  if(capture){
    cout << "Press a button to quit" << endl;
    while(true){

      //clear the files
      input.clear();
      target.clear();

      frame = cvarrToMat(cvQueryFrame(capture));
      flip(frame, frame, 1);
      imshow("Display", screen);
      resize(frame, frame, rsz);
      split(frame, channels);
      frame = channels[2];

      //turn image into neural network input
      for(unsigned i = 0; i < 30; ++i){
        for(unsigned j = 0; j < 30; ++j){
          //to output the value
          //cout << static_cast<double>(frame.at<uchar>(i, j)) << endl;
          input.push_back(static_cast<double>(frame.at<uchar>(i, j)) / 255.0);
        }
      }

      //get mouse position to find out where looking
      setMouseCallback("Display", on_mouse, (void*)(&pt));
      cout << "x=" << pt.x << "\t y=" << pt.y << endl;
      target.push_back(static_cast<double>(pt.x) / screen_width);
      target.push_back(static_cast<double>(pt.y) / screen_height);

      // put input and backprop
      net.feed_forward(input);
      net.back_prop(target);
      net.save_net("netsave.txt");

      //break out when user presses a key
      int k = waitKey(ms);
      if(k != -1)
        break;
    }
  }

  //save neural-network afterwards
  //net.save_net("netsave.txt");

  return 0;
}
