#include "flow.h"
#include "Liveness.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

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
// 计算光流
// 算法出自论文: <<A Liveness Detection Method for Face Recognition Based on Optical Flow Field>>
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

float moveIdentificate(Mat image, Rect faceRect, Mat &gray, Mat &preGray, Mat &flow, int &optFlowPicNumber, Mat &moveIdenImg)
{
	//光流部分流程
	//计算光流部分的差值
	Point faceCenter;
	Mat cflow;
	//Mat show_flow,show_image;
	float optDifference = 0;
	faceCenter.x = faceRect.x + faceRect.width / 2;
	faceCenter.y = faceRect.y + faceRect.height / 2;
	//if ((faceCenter.y - WIN_HEIGHT / 4) > 0 && (faceCenter.y + WIN_HEIGHT / 4) < WIN_HEIGHT &&
	//	(faceCenter.x - WIN_WIDTH / 4) > 0 && (faceCenter.x + WIN_WIDTH / 4) < WIN_WIDTH){
	//	Mat faceArea = image(Range(faceCenter.y - WIN_HEIGHT / 6, faceCenter.y + WIN_HEIGHT / 6),
	//		Range(faceCenter.x - WIN_WIDTH / 6, faceCenter.x + WIN_WIDTH / 6));

	if ((faceCenter.y - faceRect.height / 2) > 0 && (faceCenter.y + faceRect.height / 2) < WIN_HEIGHT &&
		(faceCenter.x - faceRect.width / 2) > 0 && (faceCenter.x + faceRect.width / 2) < WIN_WIDTH){
		Mat faceArea = image(Range(faceCenter.y - faceRect.height / 2, faceCenter.y + faceRect.height / 2),
			Range(faceCenter.x - faceRect.width / 2, faceCenter.x + faceRect.width / 2));
		Mat resize_face_area;
		cv::resize(faceArea, resize_face_area, cv::Size(112,96));
		imshow("resize_face", resize_face_area);


		
		Mat imgHSV;
		vector<Mat> hsvSplit;
		cvtColor(cv::Mat(resize_face_area), imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
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


		//Mat gary_resize_face_area;
		// Grayscale
		//cvtColor(resize_face_area, gary_resize_face_area, CV_BGR2GRAY);
		// Histogram Equlization - Ineffective
		//equalizeHist(img, img);
		// Image Resize
		//resize(gary_resize_face_area, gary_resize_face_area, Size(64, 64));
		// DoG Features
		//Mat DoG_Feature = DoG(gary_resize_face_area);
		//imshow("DoG", gary_resize_face_area);


		/*string Mapping_r8_path = "mapping_n8.txt";
		string Mapping_r16_path = "mapping_n16.txt";

		// LBP mapping
		vector<int> mapping_r8;
		vector<int> mapping_r16;
		int mapping;
		// LBP n=8 mapping
		vector<int> SVM_mapping_r8;
		ifstream Mapping_r8_file;
		Mapping_r8_file.open(Mapping_r8_path);
		if (!Mapping_r8_file.is_open())
		{
			std::cout << "Can not open file: " << Mapping_r8_path << endl;
			return false;
		}
		for (int i = 0; i < 256; i++)
		{
			Mapping_r8_file >> mapping;
			SVM_mapping_r8.push_back(mapping);
		}
		mapping_r8 = SVM_mapping_r8;

		// LBP n=16 mapping
		vector<int> SVM_mapping_r16;
		ifstream Mapping_r16_file;
		Mapping_r16_file.open(Mapping_r16_path);
		if (!Mapping_r16_file.is_open())
		{
			std::cout << "Can not open file: " << Mapping_r16_path << endl;
			return false;
		}
		for (int i = 0; i < 65536; i++)
		{
			Mapping_r16_file >> mapping;
			SVM_mapping_r16.push_back(mapping);
		}
		mapping_r16 = SVM_mapping_r16;

		// LBP Features
		Mat LBP_Feature = LBP(gary_resize_face_area, mapping_r8, mapping_r16);*/



		cvtColor(resize_face_area, gray, CV_BGR2GRAY);
		if (preGray.data){
			calcOpticalFlowFarneback(preGray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
			//flow.copyTo(show_flow);
			//for (int y = 0; y<faceArea.rows; y += 5){
			//	for (int x = 0; x<faceArea.cols; x += 5){
			//		const Point2f flowatxy = show_flow.at<Point2f>(y, x) * 10;
			//		line(faceArea, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255, 0, 0));
			//		circle(faceArea, Point(x, y), 1, Scalar(0, 0, 0), -1);
			//	}
			//}
			//imshow("flow", faceArea);

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
}
