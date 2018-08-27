/*
*Company:		Synthesis
*Author:		Yingpeng Chen
*Date:			2017/12/22
*/
#include "stdafx.h"
#include "Liveness.h"
#include "flow.h"
#include  <opencv.hpp>

Liveness::Liveness(int width, int height, int nchannels)
{

	IplImage * temp = cvCreateImage(cvSize(width, height), 8, nchannels);
	m_width = width;
	m_height = height;
	m_widthstep = temp->widthStep;
	m_size = temp->imageSize;
	m_nChannels = nchannels;

	m_difwidth = width / m_GRID_WIDTH;
	m_difheight = height / m_GRID_HEIGHT;
	m_difsize = m_difwidth * m_difheight;

	m_difv_step = m_width * nchannels * m_GRID_WIDTH;
	m_difh_step = nchannels * m_GRID_HEIGHT;

	m_weight = 1.0;
	m_codebooks = new Codebook[m_difsize];
	m_dif = new int[m_difsize];
	memset(m_dif, 0, sizeof(int)* m_difsize);

	cvReleaseImage(&temp);

}

Liveness::~Liveness()
{
	delete[] m_codebooks;
	delete[] m_dif;
}

double Liveness::borderMoveIdentificate(IplImage * pFrame, int x, int y, int radius)
{
	int count = 0;
	Codebook *c = m_codebooks + m_difwidth + 1;

	memset(m_dif, 0, sizeof(int)* m_difsize);
	int *d = m_dif + m_difwidth + 1;

	for (unsigned char *yy = (unsigned char*)pFrame->imageData + 1 + m_difv_step; //仅扫描绿色通道 
		yy < (unsigned char *)pFrame->imageData + m_widthstep * m_height - m_difv_step;
		yy += m_difv_step)
	{
		int s = 0;
		for (unsigned char *xx = yy + m_difh_step; xx < yy + m_widthstep - m_difh_step; xx += m_difh_step)
		{
			//int u = *x >> 4;
			//*x / m_COLOR_SCALES = 16 , 此处是为了效率写成硬编码. // t = 25s

			double &v = c->m_max;												 // t = 26s
			double &w = c->m_stat[*xx >> 4];									 // t = 25s

			if (w < v * 0.15)
			{
				//	++dotCount;
				++*d;
				++*(d - 1);
				++*(d + 1);
				++*(d - m_difwidth);
				++*(d + m_difwidth);

				++*(d - m_difwidth - 1);
				++*(d - m_difwidth + 1);
				++*(d + m_difwidth - 1);
				++*(d + m_difwidth + 1);
			}
			w += m_weight;
			if (w > v) v = w;
			++c;
			++d;

			s++;
		}
		++c; ++c;
		++d; ++d;
	}

	m_weight *= 1.001;

	d = m_dif;

	int x_min1 = x - radius * 0.25;
	int x_max1 = x + radius * 1.25;
	int y_min1 = y - radius * 0.4;
	int y_max1 = y + radius;

	int x_min2 = x - radius * 0.4 ;
	int x_max2 = x + radius * 1.4;
	int y_min2 = y - radius * 0.7;
	int y_max2 = y + radius ;


	if (x_min1 < 0)x_min1 = 0;
	if (x_max1 > m_width) x_max1 = m_width;
	if (y_max1 > m_height) y_max1 = m_height;
	if (y_min1 < 0) y_min1 = 0;


	if (x_min2 < 0)x_min2 = 0;
	if (x_max2 > m_width) x_max2 = m_width;
	if (y_max2 > m_height) y_max2 = m_height;
	if (y_min2 < 0) y_min2 = 0;

	//for (int y = 0; y < m_difheight; ++y)
	//{
	//	for (int x = 0; x < m_difwidth; ++x)
	//	{
	//		if (*d > 8)
	//		{
	//			cvCircle(pFrame, cvPoint(x * m_GRID_WIDTH, y * m_GRID_HEIGHT), 1, cvScalar(255, 0, 255, 0), -1);
	//		}
	//		++d;
	//	}
	//}

	for (int y = 0; y < m_difheight; ++y)
	{
		for (int x = 0; x < m_difwidth; ++x)
		{
			if (*d > 8)
			{
				if (x > x_min1 && x < x_max1 && y > y_min1 && y < y_max1)
				{
					cvCircle(pFrame, cvPoint(x * m_GRID_WIDTH, y * m_GRID_HEIGHT), 1, cvScalar(255, 0, 0, 0), -1);
				}
				else if (x < x_min2 || x > x_max2 || y < y_min2 || y > y_max2)
				{
					cvCircle(pFrame, cvPoint(x * m_GRID_WIDTH, y * m_GRID_HEIGHT), 1, cvScalar(0, 255, 0, 0), -1);
				}
				else
				{
					cvCircle(pFrame, cvPoint(x * m_GRID_WIDTH, y * m_GRID_HEIGHT), 1, cvScalar(0, 0, 255, 0), -1);
					count++;
				}		
			}
			++d;
		}
	}

	//for (int y = y_min2; y < y_max2; ++y)
	//{
	//	for (int x = x_min2; x < x_max2; ++x)
	//	{
	//		if (*d > 8)
	//		{
	//			/*if (x > x_min1 && x < x_max1 && y > y_min1 && y < y_max1)
	//			{
	//			}
	//			else
	//			{
	//			count++;
	//			}
	//			*/
	//			cvCircle(pFrame, cvPoint(x, y), 1, cvScalar(255, 0, 255, 0), -1);
	//		}
	//		++d;
	//	}
	//}

	if (count == 0)
		return 0;
	else
	{
		double roi = count / float((x_max2 - x_min2) * (y_max2 - y_min2) - (x_max1 - x_min1) * (y_max1 - y_min1));
		return roi;
	}
}

/*float Liveness::faceMoveIdentificate(Mat image, Rect faceRect, Mat &gray, Mat &preGray, Mat &flow, int &optFlowPicNumber, Mat &moveIdenImg)
{
	//光流部分流程
	//计算光流部分的差值
	Point faceCenter;
	Mat cflow;
	float optDifference = 0;
	faceCenter.x = faceRect.x + faceRect.width / 2;
	faceCenter.y = faceRect.y + faceRect.height / 2;
	if ((faceCenter.y - WIN_HEIGHT / 4) > 0 && (faceCenter.y + WIN_HEIGHT / 4) < WIN_HEIGHT &&
		(faceCenter.x - WIN_WIDTH / 4) > 0 && (faceCenter.x + WIN_WIDTH / 4) < WIN_WIDTH){
		Mat faceArea = image(Range(faceCenter.y - WIN_HEIGHT / 6, faceCenter.y + WIN_HEIGHT / 6),
			Range(faceCenter.x - WIN_WIDTH / 6, faceCenter.x + WIN_WIDTH / 6));
		cvtColor(faceArea, gray, CV_BGR2GRAY);
		if (preGray.data){
			calcOpticalFlowFarneback(preGray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
			cvtColor(preGray, cflow, CV_GRAY2RGB);
			optDifference = opticalDifference(flow);
			//optFlowDiffArray[optFlowPicNumber % 20] = optDifference;
			//保存图像用于人脸识别
			if (optFlowPicNumber % 10 == 0){
				moveIdenImg = image;
			}
			optFlowPicNumber++;
		}
		std::swap(preGray, gray);
	}
	return optDifference;
}*/