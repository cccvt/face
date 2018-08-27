#include "Liveness.h"
#include  <opencv.hpp>



// Sigmoid
double sigmoid(double input)
{
	return 1 / (1 + exp(-input));
}

// DoG Features
Mat DoG(Mat input, double sigma1, double sigma2)
{
	Mat XF1, XF2, DXF, output;
	int size1, size2;
	// Filter Sizes
	size1 = 2 * (int)(3 * sigma1) + 3;
	size2 = 2 * (int)(3 * sigma2) + 3;
	// Gaussian Filter
	GaussianBlur(input, XF1, Size(size1, size1), sigma1, sigma1, BORDER_REPLICATE);
	GaussianBlur(input, XF2, Size(size2, size2), sigma2, sigma2, BORDER_REPLICATE);
	// Difference
	DXF = XF1 - XF2;
	// Discrete Fourier Transform
	DXF.convertTo(DXF, CV_64FC1);
	dft(DXF, output);
	return abs(output);
}

// LBP Features
Mat LBP(Mat input, vector<int> mapping_r8, vector<int> mapping_r16)
{

	// LBP(r2, n16)
	Mat LBP_16_2 = lbp::ELBP(input, 2, 16);
	for (int i = 0; i < LBP_16_2.rows; i++)
	for (int j = 0; j < LBP_16_2.cols; j++)
		LBP_16_2.at<int>(i, j) = mapping_r16[LBP_16_2.at<int>(i, j)];
	Mat hist_16_2 = lbp::histogram(LBP_16_2, 243);

	std::cout << "hist_16_2: " <<  hist_16_2 << std::endl;

	imshow("hist_16_2", hist_16_2);

	// LBP(r2, n8)
	Mat LBP_8_2 = lbp::ELBP(input, 2, 8);
	for (int i = 0; i < LBP_8_2.rows; i++)
	for (int j = 0; j < LBP_8_2.cols; j++)
		LBP_8_2.at<int>(i, j) = mapping_r8[LBP_8_2.at<int>(i, j)];
	Mat hist_8_2 = lbp::histogram(LBP_8_2, 59);

	// LBP(r1, n8) * 9
	Mat LBP_8_1 = lbp::ELBP(input, 1, 8);
	for (int i = 0; i < LBP_8_1.rows; i++)
	for (int j = 0; j < LBP_8_1.cols; j++)
		LBP_8_1.at<int>(i, j) = mapping_r8[LBP_8_1.at<int>(i, j)];
	Mat hist_8_1 = lbp::spatial_histogram(LBP_8_1, 59, Size_<int>(30, 30), 14);

	// Histogram Concatenate
	Mat hist_8, hist;
	hconcat(hist_8_2, hist_8_1, hist_8);
	hconcat(hist_16_2, hist_8, hist);
	return hist;
}


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

double Liveness::isLiveness(IplImage * pFrame, int x, int y, int radius)
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

	//cvShowImage("InfFrame", pFrame);
	//cvWaitKey(1);
	if (count == 0)
		return 0;
	else
	{
		double roi = count / float((x_max2 - x_min2) * (y_max2 - y_min2) - (x_max1 - x_min1) * (y_max1 - y_min1));
		return roi;
	}
}
