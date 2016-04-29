#ifdef _WIN32
  #define __WINDOWS__
#elif __APPLE__ && __MACH__
  #define __MACINTOSH__
#else
  #define __LINUX__
#endif

//the neural-network class we created
#include "neural-network/NeuralNetwork.h"
//the opencv files we need
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//basic includes
#include <iostream>

using namespace std;
using namespace cv;

int main(){

  //the fps
  const int fps = 60;
  //the ms per frame
  const int ms = 1000 / fps;

  //setup network
  vector<unsigned> setup;
  setup.push_back(1600);
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
  int xres, yres;
  //the resize for the image
  Size rsz(40, 40);
  //capture now set to user camera
  capture = cvCaptureFromCAM(-1);

  //display window
  namedWindow("Display", 0);
  //used to greyscale image
  vector<Mat> channels(3);

  //used to input into neural network
  vector<double> input, target;

  //wait for user to be ready;
  cout << "Press a button when ready" << endl;
  resizeWindow("Display", 1200, 700);
  waitKey(0);

  if(capture){
    //get one frame
    frame = cvarrToMat(cvQueryFrame(capture));
    //find the xresolution
    xres = frame.cols / 3.0;
    //find the yresolution
    yres = frame.rows;
    resizeWindow("Display", xres, yres);
    cout << "Press a button to quit" << endl;
    while(true){

      // input.clear();
      // target.clear();
      frame = cvarrToMat(cvQueryFrame(capture));
      flip(frame, frame, 1);
      imshow("Display", frame);
      // resize(frame, frame, rsz);
      // split(frame, channels);
      // frame = channels[2];

      //turn image into neural network input
      // for(unsigned i = 0; i < 40; ++i){
      //   for(unsigned j = 0; j < 40; ++j){
      //     //cout << static_cast<double>(frame.at<uchar>(i, j)) << endl;
      //     input.push_back(static_cast<double>(frame.at<uchar>(i, j)) / 255.0);
      //   }
      // }

      // put input and backprop
      // net.feed_forward(input);
      // target.push_back(0.5);
      // target.push_back(0.5);
      // net.back_prop(target);

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
