// SSLivenessDetection.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "stdio.h"
#include <stdexcept>
#include "SSLivenessDetection.h"
#include "Liveness.h"

//const int COUNT = 20;			//每20帧活检一次
//const int FRAMENUMBER = 30;		//每30帧一次循环
//const float FThreshold = 15.0;  //光流阈值
//const float BThreshold = 0.1;	//边界阈值

double sumElem(Mat img)
{
	double sum = 0;

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			sum += img.ptr<uchar>(i)[j];
		}
	}

	return sum;
}


SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Init(int width, int height, SSLDHandle* ssHandle)
{
	int rtn = -1;
	try
	{
		if (width <= 0 || height <= 0)
		{
			printf("Input camera resolution error");
			return rtn;
		}
		//liveness init
		Liveness* live = new Liveness(width, height, 3);
		live->img = cvCreateImage(cvSize(width, height), 8, 3);
		(*ssHandle) = (SSLDHandle)live;
		rtn = 0;
	}
	catch (std::exception ex)
	{
		return rtn;
	}
	return rtn;
		
}

SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_UnInit(SSLDHandle ssHandle)
{
	int rtn = -1;
	try
	{
		if (NULL == ssHandle)
		{
			printf("Input handle error");
			return rtn;
		}
		//liveness init
		Liveness* live = (Liveness*)ssHandle;
		cvReleaseImage(&live->img);
		if (NULL != live)
		{
			delete live;
			live = NULL;
		}	
		rtn = 0;
	}
	catch (std::exception ex)
	{
		return rtn;
	}
	return rtn;

}

SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Detect(SSLDHandle ssHandle, char* p_nor_frame_data/* BGR */, int nor_frame_data_length, SSLDFaceRect rect, double* threshold)
{
	int rtn = -1;
	try
	{
		if (NULL == ssHandle || NULL == p_nor_frame_data)
		{
			//(*status) = -1001;
			printf("Input handle error");
			return rtn;
		}

		if (rect.x <= 0 || rect.y <= 0 || rect.w <= 0 || rect.h <= 0 )
		{
			//(*status) = -1002;
			printf("Input face rect error");
			return rtn;
		}

		//liveness detect
		Liveness* live = (Liveness*)ssHandle;
		memcpy(live->img->imageData, p_nor_frame_data, nor_frame_data_length);

		Mat frame, gray_frame, flow;
		frame = cv::Mat(live->img);
		//imshow("frame", frame);
		cvtColor(frame, gray_frame, CV_BGR2GRAY);

		Rect faceRect;
		faceRect.x = rect.x;
		faceRect.y = rect.y;
		faceRect.width = rect.w;
		faceRect.height = rect.h;
		
		//get roi
		Mat roi = frame(Rect(faceRect.x, faceRect.y, faceRect.width, faceRect.height));

		//skin detection
		Mat imgHSV;
		vector<Mat> hsvSplit;
		cvtColor(roi, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		split(imgHSV, hsvSplit);
		double sum[3], average[3];

		for (int i = 0; i < 3; ++i)
		{
			sum[i] = sumElem(hsvSplit[i]);
			average[i] = sum[i] / hsvSplit[i].total();
		}

		std::cout << "H 色彩均值为： " << average[0] << std::endl;
		std::cout << "S 色彩均值为： " << average[1] << std::endl;
		std::cout << "V 色彩均值为： " << average[2] << std::endl;

		//background move detection
		(*threshold) = live->borderMoveIdentificate(live->img, faceRect.x, faceRect.y, faceRect.width);

		if (average[0] > 7 && average[0] < 20) rtn = 0;
	}
	catch (std::exception ex)
	{
		return rtn;
	}
	return rtn;

}