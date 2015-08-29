#include "FaceTracker.h"
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;
using namespace robot;

#define CAMERA
#define TIME_THRESHOLD 2
#define POINT_THRESHOLD 2

static string WIN_NAME = "FaceTracker";
void displayFaceRegion(Mat& frame, FaceTracker& ft);
//TODO:跟踪状态函数 bool isTracked() 实现
//bool isTracked(){ return true; }

int main()
{
	//跟踪器声明
	FaceTracker ft;
	ft.consensus.estimate_rotation = true;
	ft.consensus.estimate_scale = true;

	//视频流声明
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened()) {
		cerr << "Unable to open video capture." << endl;
		return -1;
	}

	Mat srcImage, grayImage;
	vector<Rect> faces;

	// call facelib to get the initial face bounding box: init_bbox
	//... 代码请自行补充

	//使用Haar分类器识别人脸
	CascadeClassifier face_cascade;
	if (!face_cascade.load("haarcascade_frontalface_alt.xml"))
	{
		printf("--(!)Error loading HaarXML!\n");
		return -1;
	};


	//记录跟踪时间
	TickMeter   time_Tracking;
	time_Tracking.reset();

	while (1)
	{
		capture >> srcImage;
		cvtColor(srcImage, grayImage, CV_BGR2GRAY);
		face_cascade.detectMultiScale(grayImage, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		imwrite("c:/temp/grey.bmp", grayImage);

		//检测到人脸，初始化追踪器，开始计时
		if (faces.size() != 0)
		{
			ft.initialize(grayImage, faces[0]);
			time_Tracking.start();

			//跟踪器跟随状态下，绘制窗体，循环读取摄像头
			while (true)
			{
				//停表计时，大于阈值则重置跟踪器
				time_Tracking.stop();
				if (time_Tracking.getTimeSec()>TIME_THRESHOLD)
				{
					time_Tracking.reset();
					break;
				}

				time_Tracking.start();

				//获取跟踪窗并输出图像
				ft.processFrame(grayImage);
				displayFaceRegion(srcImage, ft);

				capture >> srcImage;

			}
		}

		imshow(WIN_NAME, srcImage);

		int ch = waitKey(20);
		if (ch == 'q' || ch == 'Q') break;
	}

	return 0;
}

//绘制旋转矩形框，显示图像
void displayFaceRegion(Mat& frame, FaceTracker& ft)
{
	Point2f vertices[4];
	ft.bb_rot.points(vertices);

	//由旋转矩形定点绘制各边
	for (int i = 0; i < 4; i++)
		line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));

	imshow(WIN_NAME, frame);
}