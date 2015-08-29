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
//TODO:����״̬���� bool isTracked() ʵ��
//bool isTracked(){ return true; }

int main()
{
	//����������
	FaceTracker ft;
	ft.consensus.estimate_rotation = true;
	ft.consensus.estimate_scale = true;

	//��Ƶ������
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened()) {
		cerr << "Unable to open video capture." << endl;
		return -1;
	}

	Mat srcImage, grayImage;
	vector<Rect> faces;

	// call facelib to get the initial face bounding box: init_bbox
	//... ���������в���

	//ʹ��Haar������ʶ������
	CascadeClassifier face_cascade;
	if (!face_cascade.load("haarcascade_frontalface_alt.xml"))
	{
		printf("--(!)Error loading HaarXML!\n");
		return -1;
	};


	//��¼����ʱ��
	TickMeter   time_Tracking;
	time_Tracking.reset();

	while (1)
	{
		capture >> srcImage;
		cvtColor(srcImage, grayImage, CV_BGR2GRAY);
		face_cascade.detectMultiScale(grayImage, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		imwrite("c:/temp/grey.bmp", grayImage);

		//��⵽��������ʼ��׷��������ʼ��ʱ
		if (faces.size() != 0)
		{
			ft.initialize(grayImage, faces[0]);
			time_Tracking.start();

			//����������״̬�£����ƴ��壬ѭ����ȡ����ͷ
			while (true)
			{
				//ͣ���ʱ��������ֵ�����ø�����
				time_Tracking.stop();
				if (time_Tracking.getTimeSec()>TIME_THRESHOLD)
				{
					time_Tracking.reset();
					break;
				}

				time_Tracking.start();

				//��ȡ���ٴ������ͼ��
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

//������ת���ο���ʾͼ��
void displayFaceRegion(Mat& frame, FaceTracker& ft)
{
	Point2f vertices[4];
	ft.bb_rot.points(vertices);

	//����ת���ζ�����Ƹ���
	for (int i = 0; i < 4; i++)
		line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));

	imshow(WIN_NAME, frame);
}