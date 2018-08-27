/*
*Company:		Synthesis
*Author:		Yingpeng Chen
*Date:			2017/12/22
*/

#ifndef __LINENESS_H__
#define __LIVENESS_H__

#include <opencv.hpp>

using namespace cv;
class Codebook
{
public:
	const static int m_COLOR_SCALES = 16;
	double m_max;
	double m_stat[m_COLOR_SCALES];

	Codebook()
	{
		m_max = 0;
		for (int i = 0; i < m_COLOR_SCALES; ++i)
		{
			m_stat[i] = 0.0;
		}
	}

	Codebook(const Codebook& codebook)
	{
		m_max = codebook.m_max;
		memcpy(m_stat, codebook.m_stat, sizeof(double)* m_COLOR_SCALES);
	}
};

class Liveness
{
public:
	Liveness(int width, int height, int nchannls);
	~Liveness();
	double borderMoveIdentificate(IplImage * img, int x, int y, int radius);
	//float faceMoveIdentificate(Mat image, Rect faceRect, Mat &gray, Mat &preGray, Mat &flow, int &optFlowPicNumber, Mat &moveIdenImg);

	//std::vector<float> optBorderDiffArray;
	//std::vector<float> optFaceDiffArray;

	IplImage* img;

	//Mat liveFrame;
	//Mat prev_gray_frame;
	//int faceFrameNumber;


private:
	Codebook *m_codebooks;
	int m_width;//原始大小
	int m_height;
	int m_size;
	int m_widthstep;
	int m_nChannels;
	int m_difwidth;//放缩大小
	int m_difheight;
	int m_difsize;
	int m_difv_step;
	int m_difh_step;
	
	int *m_dif;
	double m_weight;
private:
	const static int m_GRID_WIDTH = 1;
	const static int m_GRID_HEIGHT = 1;
};

#endif 