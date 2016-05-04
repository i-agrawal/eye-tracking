#ifndef __READER_H__
#define __READER_H__

#include "helpers.h"

using namespace std;
using namespace cv;

class Reader{
private:
  CascadeClassifier face_cascade;
  string window_name;

public:
  Reader(const string &);
  void Detect(Mat &, Mat &, Point &, Point &);
  void Display(Mat &);
};

#endif
