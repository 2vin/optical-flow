#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

#define OFFSET 120
#define NUMFEATURES 500

VideoCapture cap(0);

// Change the parameters to suitable values
VideoWriter writer("video.avi", CV_FOURCC('M','J','P','G'), 15, Size(640,480), true);

Mat frame,smallFrame, stableFrame;
Mat gray, prevGray;

vector<Point2f> corners;
vector<uchar> status;
vector<float> errors;
vector<Point2f> trackedFeatures;

Mat_<float> rigidTransform;


Mat rotate(Mat src, double angle)
{
  Mat dst;
  Point2f pt(src.cols/2.0, src.rows/2.0);
  Mat r = getRotationMatrix2D(pt, angle, 1.0);
  warpAffine(src, dst, r, Size(src.cols, src.rows));
  return dst;
}

int getImage()
{
  if(cap.isOpened())
  {
    cap>>frame;
    if(frame.cols>frame.rows*3/2)
      frame = frame(Rect(frame.cols/2, 0, frame.cols/2, frame.rows-2*0));
      resize(frame,smallFrame,Size(frame.cols,frame.rows));
      cvtColor(smallFrame,gray,CV_BGR2GRAY);
  }
  return 0;
}

int videoStable(Mat gray, Mat prevGray)
{
  status.clear();
  errors.clear();
  corners.clear();

  if(trackedFeatures.size() < NUMFEATURES)
  {
    trackedFeatures.clear();
    goodFeaturesToTrack(prevGray,trackedFeatures,NUMFEATURES,0.05,10);
  }
  calcOpticalFlowPyrLK(prevGray,gray,trackedFeatures,corners,status,errors,Size(10,10));

  Mat invTrans = Mat_<float>::eye(2,3);
  warpAffine(frame,stableFrame,invTrans.rowRange(0,2),Size());

  trackedFeatures.clear();

  Rect r;
  r.x = 0;
  r.y = 0*frame.rows/frame.cols;
  r.width = stableFrame.cols;
  r.height = stableFrame.rows;
  Mat x = stableFrame(r);

  resize(x,x,Size(frame.cols,frame.rows));


  for (int i = 0; i < corners.size(); ++i) {
    if(corners[i].y<380&&corners[i].y>100)
    line(smallFrame, corners[i], trackedFeatures[i], Scalar(255,0,255),2 );
  }

  for (int i = 0; i < trackedFeatures.size(); ++i) {
    if(corners[i].y<380&&corners[i].y>100)
    circle(smallFrame,trackedFeatures[i],2,Scalar(0,0,255),CV_FILLED);
  }
  for (int i = 0; i < corners.size(); ++i) {
    if(corners[i].y<380&&corners[i].y>100)
    circle(smallFrame,corners[i],1,Scalar(0,255,0),CV_FILLED);
  }
  writer.write(smallFrame);

  return 0;
}

int main()
{

  rigidTransform = Mat::eye(3,3,CV_32FC1);
  while(true)
  {
    getImage();

    if(!prevGray.empty())
    {
      videoStable(gray,prevGray);
      imshow("smallFrame",smallFrame);
      
      if(waitKey(10)>0)
        exit(0);
      }

      prevGray = gray.clone();
    }
  return 0;
}
