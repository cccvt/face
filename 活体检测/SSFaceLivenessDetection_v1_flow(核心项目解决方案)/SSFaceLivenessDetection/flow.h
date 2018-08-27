/*
 *Company:		Synthesis
 *Author:		Yingpeng Chen
 *Date:			2017/12/22
 */

#ifndef __FLOW_H__
#define __FLOW_H__

#include <opencv2/opencv.hpp>
using namespace cv;

#define THRESHOLD_MOVE 17           //[0, 无穷]， 20帧算法的输出总和，越小越严
#define WIN_WIDTH 640
#define WIN_HEIGHT 480


/*Calculate horizontal or vertical component of the optical flow field
* dim = 0 for horizontal, dim = 1 for vertical
* m_left and m_right is the left boundary and right boundary of the cols and rows respectively
*/
float calculateUorV(Mat flow, int m_left, int m_right, int n_left, int n_right, int dim);

// 计算光流
// 算法出自论文: <<A Liveness Detection Method for Face Recognition Based on Optical Flow Field>>
float opticalDifference(Mat flow);

float moveIdentificate(Mat image, Rect face_rect, Mat &gray, Mat &preGray, Mat &flow, int &optFlowPicNumber, Mat &moveIdenImg);



#endif 