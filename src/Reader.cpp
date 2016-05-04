#include "Reader.h"

Reader::Reader(const string &cascade_path): window_name("Display Window"){
  face_cascade.load(cascade_path);
  namedWindow(window_name, WINDOW_AUTOSIZE);
}

void Reader::Detect(Mat &image, Mat &ROI, Point &lpupil, Point &rpupil){
  vector<Rect> faces;               //vector to hold the rectangles in which there are faces
  vector<Mat>  RGBchannels;         //vector to hold the black and white image
  split(image, RGBchannels);        //get the gray image
  Mat gray_image = RGBchannels[2];
  //find the faces in the image
  face_cascade.detectMultiScale(gray_image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, Size(150, 150));
  for(unsigned i = 0; i < faces.size(); ++i)
    rectangle(image, faces[i], 1234);
  //if no faces were found do nothing
  if(faces.empty())
    return;
  Rect face = faces[0];
  Mat faceROI = gray_image(face);
  ROI = faceROI;

  if(kSmoothFaceImage){
    double sigma = kSmoothFaceFactor * face.width;
    GaussianBlur(faceROI, faceROI, Size(255, 255), sigma);
  }

  int er_width  = face.width  * (kEyePercentWidth  / 100.0);
  int er_height = face.height * (kEyePercentHeight / 100.0);
  int er_top    = face.height * (kEyePercentTop    / 100.0);

  Rect lefter (face.width * (kEyePercentSide / 100.0),                         er_top, er_width, er_height);
  Rect righter(face.width - er_width - face.width * (kEyePercentSide / 100.0), er_top, er_width, er_height);

  Point left_pupil  = FindPupil(faceROI, lefter);
  Point right_pupil = FindPupil(faceROI, righter);

  left_pupil.x  += lefter.x;
  left_pupil.y  += lefter.y;
  right_pupil.x += righter.x;
  right_pupil.y += righter.y;

  lpupil = left_pupil;
  rpupil = right_pupil;
}

void Reader::Display(Mat &image){
  Mat faceROI = image;
  Point left_pupil, right_pupil;
  Detect(image, faceROI, left_pupil, right_pupil);

  circle(faceROI, left_pupil,  3, 1234);
  circle(faceROI, right_pupil, 3, 1234);

  imshow(window_name, faceROI);
}
