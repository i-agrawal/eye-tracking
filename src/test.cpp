#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <stdio.h>
#include <math.h>

using namespace cv;

struct BoundingRect{
	//data
	Rect rect;
	int distance;
	enum type{
		LEFT, RIGHT, TOP, BOTTOM, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, INSIDE
	};

	//functions
	void change(int, int, int, int);
	int  click (int, int);

	//constructor
	BoundingRect(Rect, int);
};

BoundingRect::BoundingRect(Rect r, int d){
	rect = r;
	distance = d;
}

void BoundingRect::change(int x, int y, int w, int h){
	rect.x += x;
	rect.y += y;
	if(rect.width + w < 0){
		rect.x += rect.width + w;
		rect.width = abs(rect.width + w);
	}
	else{
		rect.width += w;
	}
	if(rect.height + h < 0){
		rect.y += rect.height + w;
		rect.height = abs(rect.height + w);
	}
	else{
		rect.height += h;
	}
}

int BoundingRect::click(int x, int y){
	if(abs(rect.x - x) < distance){
		if(abs(rect.y - y) < distance)
			return TOPLEFT;
		else if(abs(rect.y + rect.height - y) < distance)
			return BOTTOMLEFT;
		else if(y > rect.y - distance && y < rect.y + rect.height + distance)
			return LEFT;
	}
	else if(abs(rect.x + rect.width - x) < distance){
		if(abs(rect.y - y) < distance)
			return TOPRIGHT;
		else if(abs(rect.y + rect.height - y) < distance)
			return BOTTOMRIGHT;
		else if(y > rect.y - distance && y < rect.y + rect.height + distance)
			return RIGHT;
	}
	else if(x > rect.x - distance && x < rect.x + rect.width + distance){
		if(abs(rect.y - y) < distance)
			return TOP;
		else if(abs(rect.y + rect.height - y) < distance)
			return BOTTOM;
		else if(y > rect.y && y < rect.y + rect.height)
			return INSIDE;
	}
	return -1;
}

struct MouseHandler{
	//structure to handle mouse events
	//data
	bool ldown, recieved;
	int  x, y;

	//functions
	static void handle(int, int, int, int, void*);
	void mHandle(int, int, int, int);
	bool change ();

	//constructor
	MouseHandler(const std::string &);
};

MouseHandler::MouseHandler(const std::string &win_name){
	//have the current mouse handler be this upon creation
	setMouseCallback(win_name, handle, this);
	ldown = false;
	x = 0; y = 0;
}

void MouseHandler::handle(int event, int x, int y, int flags, void * data){
	//have the current mouse handler call its mHandle function
	((MouseHandler*)data)->mHandle(event, x, y, flags);
}

void MouseHandler::mHandle(int event, int x, int y, int flags){
	//current mouse handler handles the mouse events
	recieved = true;
	if(event == EVENT_LBUTTONDOWN)
		ldown = true;
	else if(event == EVENT_LBUTTONUP)
		ldown = false;
	this->x = x;
	this->y = y;
	//printf("x = %i, y = %i\n", x, y);
}

bool MouseHandler::change(){
	if(recieved){
		recieved = false;
		return true;
	}
	return false;
}

int main(int argc, char ** argv){
	//set up cascade path
	std::string cascade_path = argv[1];
	cascade_path += "/haarcascade_frontalface_alt.xml";
	CascadeClassifier face_classifier;
	if(!face_classifier.load( cascade_path )){
		printf("--(!) Error: Unable To Load Face Cascade Classifier. Exiting.\n");
		return -1;
	}

	//set up webcam viewer
	CvCapture * capture;
	capture = cvCaptureFromCAM(-1);
	if(!capture){
		printf("--(!) Error: No Webcam Detected. Exiting.\n");
		return -1;
	}

	//set up first window to get image for calibration
	std::string win_name = "Calibrate Window";
	namedWindow(win_name, 0);
	moveWindow (win_name, 0, 0);

	//variables for image capturing
	Mat  frame, gray_frame, clone, eye_display;
	Size framesz(960, 540);

	//continually stream webcam images until user presses a button on
	//image they desire
	while(true){
		frame = cvarrToMat(cvQueryFrame(capture));
		flip  (frame, frame, 1);
		resize(frame, frame, framesz);
		imshow(win_name, frame);
		if(waitKey(10) != -1)
			break;
	}

	//using cascade classifier to find the face rect
	std::vector<Rect> faces;
	cvtColor(frame, gray_frame, CV_BGR2GRAY);
	equalizeHist(gray_frame, gray_frame);
	face_classifier.detectMultiScale(gray_frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(150, 150));
	if(faces.empty()){
		printf("--(!) Error: No Face Detected. Exiting.\n");
		return -1;
	}

	//prepare for mouseHandler
	MouseHandler handler(win_name);
	int x, y, place = -2;

	//get the person's face for viewing
	clone = frame.clone();
	BoundingRect brect(faces[0], 15);
	int width = 300;
	float correction = 0.0;
	rectangle(clone, brect.rect, Scalar(0, 0, 255));

	//set up window for what the box is around
	std::string eye_win_name = "Image Within Box";
	namedWindow(eye_win_name, 1);
	moveWindow (eye_win_name, 960, 0);

	//stream what the box currently looks like
	imshow(win_name, clone);
	imshow(eye_win_name, frame(brect.rect));
	printf("Please Place The Box Around: Your Right Eye / The Eye Towards the Left of the Screen.\n");
	while(true){
		if(handler.change()){
			//if the user has done something then we need to redraw
			//printf("Recieved Mouse Event.\n");
			if(handler.ldown && place == -2){
				place = brect.click(handler.x, handler.y);
				x = handler.x;
				y = handler.y;
			}
			else if(!handler.ldown && place != -2){
				place = -2;
			}
			else if(place != -1 && handler.ldown){
				if(place == brect.LEFT){
					brect.change(handler.x - x, 0, x - handler.x, 0);
				}
				else if(place == brect.RIGHT){
					brect.change(0, 0, handler.x - x, 0);
				}
				else if(place == brect.TOP){
					brect.change(0, handler.y - y, 0, y - handler.y);
				}
				else if(place == brect.BOTTOM){
					brect.change(0, 0, 0, handler.y - y);
				}
				else if(place == brect.TOPLEFT){
					brect.change(handler.x - x, handler.y - y, x - handler.x, y - handler.y);
				}
				else if(place == brect.BOTTOMLEFT){
					brect.change(handler.x - x, 0, x - handler.x, handler.y - y);
				}
				else if(place == brect.TOPRIGHT){
					brect.change(0, handler.y - y, handler.x - x, y - handler.y);
				}
				else if(place == brect.BOTTOMRIGHT){
					brect.change(0, 0, handler.x - x, handler.y - y);
				}
				else{
					brect.change(handler.x - x, handler.y - y, 0, 0);
				}
				//display current frame with rectangle
				clone = frame.clone();
				rectangle(clone, brect.rect, Scalar(255, 0, 0));
				imshow(win_name, clone);

				//display what is currently in rectangle
				eye_display = frame(brect.rect);
				correction = static_cast<float>(width) / eye_display.cols;
				resize(eye_display, eye_display, Size(eye_display.cols * correction, eye_display.rows * correction));
				imshow(eye_win_name, eye_display);

				//change current x and y vals
				x = handler.x;
				y = handler.y;
			}
		}
		if(waitKey(10) != -1)
			break;
	}
	destroyWindow(eye_win_name);

	Rect leye, reye;
	float kEyeTop, kEyeLeft, kEyeRight, kEyeWidth, kEyeHeight;
	kEyeLeft   = (brect.rect.x - faces[0].x) / static_cast<float>(faces[0].width);
	kEyeWidth  = brect.rect.width / static_cast<float>(faces[0].width);
	kEyeRight  = 1 - kEyeWidth - kEyeLeft;
	kEyeTop    = (brect.rect.y - faces[0].y) / static_cast<float>(faces[0].height);
	kEyeHeight = brect.rect.height / static_cast<float>(faces[0].height);

	printf("Percentage From the Left of the face is %f%%.\n", kEyeLeft);
	printf("Percentage From the Top of the face is %f%%.\n", kEyeTop);
	printf("Percentage of the face width is %f%%.\n", kEyeWidth);
	printf("Percentage of the face height is %f%%.\n", kEyeHeight);

	Mat leyef, reyef;
	Size eyesz(320, 180);
	std::string lwin_name = "Left Eye";
	std::string rwin_name = "Right Eye";
	namedWindow(lwin_name, 1);
	namedWindow(rwin_name, 1);
	moveWindow(lwin_name, framesz.width, 0);
	moveWindow(rwin_name, framesz.width, 240);

	while(true){
		//get the frame from camera
		frame = cvarrToMat(cvQueryFrame(capture));
		flip  (frame, frame, 1);
		resize(frame, frame, framesz);

		//get the face
		cvtColor(frame, gray_frame, CV_BGR2GRAY);
		equalizeHist(gray_frame, gray_frame);
		face_classifier.detectMultiScale(gray_frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(150, 150));

		//if there was a face detected perform calculations
		if(!faces.empty()){
			leye.x      = faces[0].x + faces[0].width  * kEyeLeft;
			leye.y      = faces[0].y + faces[0].height * kEyeTop;
			leye.width  = faces[0].width * kEyeWidth;
			leye.height = faces[0].height * kEyeHeight;

			reye.x      = faces[0].x + faces[0].width  * kEyeRight;
			reye.y      = leye.y;
			reye.width  = leye.width;
			reye.height = leye.height;

			leyef = frame(leye);
			resize(leyef, leyef, eyesz);
			reyef = frame(reye);
			resize(reyef, reyef, eyesz);

			imshow(lwin_name, leyef);
			imshow(rwin_name, reyef);
			rectangle(frame, leye, Scalar(255, 255, 0));
			rectangle(frame, reye, Scalar(255, 255, 0));
		}

		imshow(win_name, frame);

		if(waitKey(10) != -1)
			break;
	}

}