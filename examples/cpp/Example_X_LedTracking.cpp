#include "Common.h"
#include <opencv2/opencv.hpp>
using namespace cv;

static char * AVC1 = "AVC1";
static const char * WINDOW_NAME = "window";
static const char * INPUT_FILE = "f:\\leds.mp4";
static int MAX_VALUE = 255;
static uint64_t LOW_VALUE = 0x00000000003FFFFF;
static uint64_t MED_VALUE = 0x000007FFFFC00000;
static uint64_t HIG_VALUE = 0xFFFFF80000000000;
static uint64_t DUMPED_VALS[] = {
  0x000107FFEBFFFF36,
  0x0000AFFFE73FFF1A,
  0x0000A7FFE4BFFEFE,
  0x0002CFFFEC7FFF56,
  0x000157FFE8BFFF28,
  0x00035FFFECFFFF26,
  0x0001C7FFE7BFFF40,
  0x00011FFFE8BFFF48,
  0x0005FFFFF1FFFF48,
  0x000507FFEF3FFF3C,
  0x00058FFFEEFFFF6A,
  0x000477FFEDFFFF64,
  0x0003BFFFEEFFFF70,
  0x0004D7FFEEFFFF4A,
  0x00037FFFEA7FFF88,
  0x0005C7FFEB7FFF58,
  0x000517FFEF3FFF4A,
  0x0004FFFFEF3FFF46,
  0x0002EFFFF17FFF94,
  0x000337FFEC3FFF5C,
  0
};
using namespace std;

class LedTracking {


public:
  VideoCapture cap; // open the default camera
  int threshold_value{ 128 };
  RNG rng{ 12345 };

  LedTracking() {
    cap.set(CV_CAP_PROP_FOURCC, *(double*)(&AVC1));
  }

  void reopen() {
    cap.open(INPUT_FILE);
  }

  static void foo(int value, void * data) {
    LedTracking * pThis = (LedTracking *)data;
    pThis->threshold_value = value;
  }

  int run2() {
    int i = 0;
    while (DUMPED_VALS[i]) {

      struct { int32_t x : 22; } xx;
      struct { int32_t x : 21; } yy;
      struct { int32_t x : 21; } zz;

      uint64_t v = DUMPED_VALS[i];
      int z = xx.x = v & LOW_VALUE;
      int y = yy.x = (v & MED_VALUE) >> 22;
      int x = zz.x = (v & HIG_VALUE) >> 43;
      SAY("    %4d %4d %4d", x, y, z);
      ++i;
    }
    return 0;
  }

  int run() {
    reopen();
    Size IMAGE_SIZE(752, 480);

    VideoWriter vw;
    vw.open("f:/leds2.avi", CV_FOURCC_PROMPT, 30, IMAGE_SIZE);
    waitKey(0);
    if (!vw.isOpened()) {
      return -1;
    }


    if (!cap.isOpened())  // check if we succeeded
      return -1;

    namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    moveWindow(WINDOW_NAME, 100, -900);
    createTrackbar("Value",
      WINDOW_NAME, &threshold_value,
      MAX_VALUE, foo, this);
    Mat emptyFrame;
    for (;;)
    {
      Mat frame;
      if (!cap.grab()) {
        break;
      }
      
      if (!cap.read(frame)) {
        break;
      }

      Mat work;
      resize(frame, frame, Size(), 2.0, 1.0);
      cvtColor(frame, work, CV_BGR2GRAY);


      if (emptyFrame.rows == frame.rows) {
        work = work - emptyFrame;
      }

      /// Detect edges using canny
      GaussianBlur(work, work, Size(7, 7), 1.5, 1.5);
      Mat edges;
      Canny(work, edges, threshold_value, 255, 3);

      vector<vector<Point> > contours;
      vector<Vec4i> hierarchy;

      //CV_WRAP explicit MSER(int _delta = 5, int _min_area = 60, int _max_area = 14400,
      //  double _max_variation = 0.25, double _min_diversity = .2,
      //  int _max_evolution = 200, double _area_threshold = 1.01,
      //  double _min_margin = 0.003, int _edge_blur_size = 5);
      //MSER fd(5, 60, 150, 0.25, 0.2, 200, 1.01, 10, 3);
      //vector<KeyPoint> features;
      //fd.detect(work, features);
      //for (int i = 0; i < features.size(); ++i) {
      //  KeyPoint kp = features[i];
      //  circle(work, kp.pt, 3, Scalar(0, 0, 0), -1, 8, 0);
      //  //  // draw the circle outline
      //  circle(work, kp.pt, kp.size, Scalar(255, 255, 255), 3, 8, 0);
      //}

      //vector<Vec3f> circles;
      //HoughCircles(edges, circles, CV_HOUGH_GRADIENT, 3, 10, 32, 200, 1, 1000);
      //for (size_t i = 0; i < circles.size(); i++)
      //{
      //  Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      //  int radius = cvRound(circles[i][2]);
      //  // draw the circle center
      //  circle(work, center, 3, Scalar(0, 255, 0), -1, 8, 0);
      //  // draw the circle outline
      //  circle(work, center, radius, Scalar(0, 0, 255), 3, 8, 0);
      //}

      findContours(edges, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

      vector<vector<Point> > new_contours;
      for (int i = 0; i < contours.size(); ++i) {
          Mat hull; convexHull(contours[i], hull);
          double area = contourArea(hull);
          if (area < 80.0 || area > 240.0) {
            continue;
          }
          new_contours.push_back(contours[i]);
      }
      contours = new_contours;


      vector<vector<Point> > contours_poly(contours.size());
      vector<Rect> boundRect(contours.size());
      vector<Point2f>center(contours.size());
      vector<float>radius(contours.size());

      for (int i = 0; i < contours.size(); i++)
      {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
        minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
      }

      /// Draw polygonal contour + bonding rects + circles
      Mat drawing = Mat::zeros(frame.size(), CV_8UC3);
      for (int i = 0; i < contours.size(); i++)
      {
        if (radius[i] > 10) {
          continue;
        }
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        //drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
        //rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
        circle(drawing, center[i], (int)radius[i] + 1, Scalar(0, 0, 255), 3, 8, 0);
        circle(drawing, center[i], (int)2, Scalar(255, 255, 255), 2, 8, 0);
      }

      //Mat output = Mat::zeros(frame.size(), CV_8UC3);
      //for (int i = 0; i< contours.size(); i++) {
      //  Mat hull; convexHull(contours[i], hull);
      //  double area = contourArea(hull);
      //  if (area < 80.0 || area > 320.0) {
      //    continue;
      //  }
      //  Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
      //  drawContours(output, contours, i, Scalar(255, 255, 255), 2, 8, hierarchy, 0, Point());
      //}

      vw << drawing;
      imshow(WINDOW_NAME, drawing);
      if (waitKey(20) == 'q') break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
  }
};

RUN_APP(LedTracking);
