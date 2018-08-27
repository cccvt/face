/*
*Company:		Synthesis
*Author:		Yingpeng Chen
*Date:			2017/12/22
*/
#include "stdafx.h"
#include "flow.h"

/*Calculate horizontal or vertical component of the optical flow field
* dim = 0 for horizontal, dim = 1 for vertical
* m_left and m_right is the left boundary and right boundary of the cols and rows respectively
*/
float calculateUorV(Mat flow, int m_left, int m_right, int n_left, int n_right, int dim)
{
	float UorV = 0;
	for (int i = m_left; i<m_right; ++i) {
		for (int j = n_left; j<n_right; ++j) {
			Vec2f flow_at_point = flow.at<Vec2f>(i, j);
			float f = flow_at_point[dim];
			UorV += f;
		}
	}
	UorV /= ((m_right - m_left)*(n_right - n_left));
	return UorV;
}
// compute flow
// come from <<A Liveness Detection Method for Face Recognition Based on Optical Flow Field>>
float opticalDifference(Mat flow)
{
	float optDifference = 0.0;
	int m = flow.rows;
	int n = flow.cols;

	//计算组件的值
	float U_left = calculateUorV(flow, 0, m / 2, 0, n, 0);
	float V_left = calculateUorV(flow, 0, m / 2, 0, n, 1);
	float U_right = calculateUorV(flow, m / 2 + 1, m, 0, n, 0);
	float V_right = calculateUorV(flow, m / 2 + 1, m, 0, n, 1);

	float U_upper = calculateUorV(flow, 0, m, 0, n / 2, 0);
	float V_upper = calculateUorV(flow, 0, m, 0, n / 2, 1);
	float U_lower = calculateUorV(flow, 0, m, n / 2 + 1, n, 0);
	float V_lower = calculateUorV(flow, 0, m, n / 2 + 1, n, 1);

	float U_center = calculateUorV(flow, 0, m, 0, n, 0);
	float V_center = calculateUorV(flow, 0, m, 0, n, 1);

	//计算系数的值
	float a1 = (U_right - U_left) / (m / 2);
	float a2 = (V_right - V_left) / (m / 2);

	float b1 = (U_upper - U_lower) / (n / 2);
	float b2 = (V_upper - V_lower) / (n / 2);

	float c1 = U_center - a1*n / 2 - b1*m / 2;
	float c2 = V_center - a2*n / 2 - b2*m / 2;

	//计算差值
	float numeratorofD = 0.0;
	float denominatorofD = 0.0;
	for (int i = 0; i < m; ++i){
		for (int j = 0; j < n; ++j){
			Vec2f flow_at_point = flow.at<Vec2f>(i, j);
			float U_ij = flow_at_point[0];
			float V_ij = flow_at_point[1];
			float temp1 = (a1*i + b1*j + c1 - U_ij);
			float temp2 = (a2*i + b2*j + c2 - V_ij);
			numeratorofD += sqrt(temp1 * temp1 + temp2 * temp2);
			denominatorofD += sqrt(U_ij* U_ij + V_ij*V_ij);
		}
	}
	optDifference = numeratorofD / denominatorofD;
	return optDifference;
}
