#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "facedetect-dll.h"
#include "CameraDS.h"
#include "flow.h"
#include "Liveness.h"
#include <fstream>
#include <sstream>
//#include <fiostream.h>
#include "SSLivenessDetection.h"
#include <time.h>

#pragma comment(lib,"libfacedetect.lib")
#pragma comment(lib,"SSLivenessDetection.lib")

using namespace cv;

//define the buffer size. Do not change the size!
#define DETECT_BUFFER_SIZE 0x20000

const int nor_width = 640;
const int nor_height = 480;
const int inf_width = 640;
const int inf_height = 480;

void liveness_detect(Liveness *live, IplImage* img, Rect faceRect, Mat &preGray, int *frameNumber, std::vector<float> &optFlowDiffArray, std::vector<float> &optLiveDiffArray, Mat &moveIdenImg)
{
	if (NULL == img)
		printf("Input frame data error!");
	
	if (faceRect.x < 0 || faceRect.y < 0 || faceRect.width <= 0 || faceRect.height <= 0)
		printf("Input face rect error!");

	//computer flow and live threshold value
	Mat frame, gray_frame, flow;
	frame = cv::Mat(img);
	cvtColor(frame, gray_frame, CV_BGR2GRAY);

	//int count = *frameNumber;

	optFlowDiffArray[(*frameNumber) % 20] = moveIdentificate(frame, faceRect, gray_frame, preGray, flow, (*frameNumber), moveIdenImg);
	optLiveDiffArray[(*frameNumber) % 20] = live->isLiveness(&IplImage(frame), faceRect.x, faceRect.y, faceRect.width);

	//std::cout << "flow threshold is " << optFlowDiffArray[(*frameNumber) % 20] << std::endl;
	//std::cout << "codebook threshold is " << optLiveDiffArray[(*frameNumber) % 20] << std::endl;
	std::cout << "Frame number is " << (*frameNumber) << std::endl;
}

DWORD WINAPI thread_liveness_detect(LPVOID p)
{
	CCameraDS * camera = (CCameraDS*)p;
	IplImage *img = cvCreateImage(cvSize(inf_width, inf_height), 8, 3);

	int * pResults = NULL;
	//pBuffer is used in the detection functions.
	//If you call functions in multiple threads, please create one buffer for each thread!
	unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
	if (!pBuffer)
	{
		fprintf(stderr, "Can not alloc inf camera buffer.\n");
		return -1;
	}
	int doLandmark = 1;

	//query frame
	Mat frame,gray_frame;
	Mat result_multiview_reinforce;

	//liveness detect
	Liveness liveness(inf_width, inf_height, 3);
	std::vector<float> liveDiffArray(20, 0.0);
	//detect flow
	Mat prev_gray_frame;
	int optFlowPicNumber = 0;
	std::vector<float> optFlowDiffArray(20, 0.0);
	Mat moveIdenInfImg;

	//SSLivenessHandle handle;
	//SS_LivenessSDK_Init(inf_width, inf_height, &handle);
	//FrameInfo info;


	//print log file
	int liveNumber = 0;
	int nonLiveNumber = 0;
    std::ofstream out("out.txt");

	clock_t start, finish;
	double totaltime;

	std::vector<cv::Point2f> landmarks;
	

	while (true)
	{
		if (camera->QueryFrame((unsigned char*)img->imageData))
		{
			start = clock();
			//face detection
			cvFlip(img);
			frame = cv::Mat(img);
			cvtColor(frame, gray_frame, CV_BGR2GRAY);
	
			pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray_frame.ptr(0)), gray_frame.cols, gray_frame.rows, (int)gray_frame.step,
				1.2f, 3, 48, 0, doLandmark);

			printf("%d faces detected.\n", (pResults ? *pResults : 0));

			result_multiview_reinforce = frame.clone();
			//print the detection results
			int x, y, w, h, neighbors, angle;
			for (int i = 0; i < (pResults ? *pResults : 0); i++)
			{
				short * p = ((short*)(pResults + 1)) + 142 * i;
				x = p[0];
				y = p[1];
				w = p[2];
				h = p[3];
				neighbors = p[4];
				angle = p[5];

				//printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
				
				if (doLandmark)
				{
					for (int j = 0; j < 68; j++)
					{
						landmarks.push_back(Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]));
						circle(result_multiview_reinforce, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
					}		
				}

				Rect faceRect;
				faceRect.x = x;
				faceRect.y = y;
				faceRect.width = w;
				faceRect.height = h;


				/*info.frame_width = inf_width;
				info.frame_height = inf_height;
				info.rect.x = x;
				info.rect.y = y;
				info.rect.w = w;
				info.rect.h = h;


				int rtn = SS_LivenessSDK_Detect(handle, (char*)img->imageData, info);
				finish = clock();
				totaltime = (double)(finish - start);
				std::cout << "\n�˳��������ʱ��Ϊ" << totaltime << "���룡" << std::endl;

				std::cout << rtn << std::endl;
				switch (rtn)
				{
				case 1001:
					printf("Face liveness detecting ....../n");
					break;
				case 1002:
					printf("This is liveness!/n");
					break;
				case 1003:
					printf("This is not liveness/n");
					break;
				deflaut:
					printf("Occur other error!");
					break;
				}

				if (out.is_open())
				{
					if (1001 == rtn )
						out << "Face is detecting liveness ......!" << std::endl;
					else if (1002 == rtn)
						out << "This is Liveness!" << std::endl;
					else if (1003 == rtn)
						out << "This is not Liveness!" << std::endl;
					else
						out << "Occur other error!" << std::endl;
				}*/

				//liveness_detect(&liveness, img, faceRect, prev_gray_frame, &optFlowPicNumber, optFlowDiffArray, liveDiffArray, moveIdenInfImg);
				//STEP1:����
				Mat flow;
				//computer flow value
				optFlowDiffArray[optFlowPicNumber % 20] = moveIdentificate(frame, faceRect, gray_frame, prev_gray_frame, flow, optFlowPicNumber, moveIdenInfImg);
				liveDiffArray[optFlowPicNumber % 20] = liveness.isLiveness(img, x, y, w);

				//out << "flow diff value: " << optFlowDiffArray[optFlowPicNumber % 20] << std::endl;
				//out << "border diff value: " << liveDiffArray[optFlowPicNumber % 20] << std::endl;
				std::cout << "flow threshold = " << optFlowDiffArray[optFlowPicNumber % 20] << std::endl;
				//std::cout << "codebook threshold = " << liveDiffArray[optFlowPicNumber % 20] << std::endl;
				std::cout << "Frame number is " << optFlowPicNumber << std::endl;

				//ÿ�λ��֮ǰ��
				if (optFlowPicNumber >= 30 && optFlowPicNumber % 20 == 0)
				{
					//compute sum flow diff
					float sumFlowDif = 0;
					for (auto iter = optFlowDiffArray.begin(); iter != optFlowDiffArray.end(); iter++)
					{
						sumFlowDif += *iter;
					}
					std::cout << "Value of Move Identification: " << sumFlowDif << std::endl;
					
					//compute sum live diff
					float sumLiveDiff = 0;
					for (auto iter = liveDiffArray.begin(); iter != liveDiffArray.end(); iter++)
					{
						sumLiveDiff += *iter;
					}
					//std::cout << "Value of Liveness Detection: " << sumLiveDiff << std::endl;

					if (sumFlowDif >0 && sumFlowDif <= 15 && liveDiffArray[optFlowPicNumber % 20] <= 0.1)
					{
						liveNumber++;
						std::cout << "This is Liveness!" << std::endl;

						rectangle(result_multiview_reinforce, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
					}
					else
					{
						nonLiveNumber++;
						std::cout << "This is not Liveness!" << std::endl;
						rectangle(result_multiview_reinforce, Rect(x, y, w, h), Scalar(0, 0, 255), 2);
					}

					//print liveness detection result
					if (out.is_open())
					{
						out << "Value of Move Identification: " << sumFlowDif << std::endl;
						//out << "Value of Liveness Detection: " << sumLiveDiff << std::endl;

						if (sumFlowDif > 0 && sumFlowDif <= 15 && liveDiffArray[optFlowPicNumber % 20] <= 0.1)
							out << "This is Liveness!" << std::endl;
						else
							out << "This is not Liveness!" << std::endl;
					}
					optFlowPicNumber = 0;
				}
			}
			cv::imshow("InfCameraWindow", result_multiview_reinforce);

			if (cvWaitKey(20) == 'q')
				break;
		}
	}
	out << "Live number is " << liveNumber << std::endl;
	out << "Non live number is " << nonLiveNumber << std::endl;
	out.close();
	//release the buffer
	free(pBuffer);
	cvReleaseImage(&img);
	//SS_LivenessSDK_UnInit(handle);

	return 0;
}


int main(int argc, char* argv[])
{
	//open dual camera
	//CCameraDS *norCamera = new CCameraDS();		//��������ͷ
	CCameraDS *infCamera = new CCameraDS();		//��������ͷ

	//IplImage* infImage = cvCreateImage(cvSize(inf_width, inf_height), 8, 3);

	/*int norNum = norCamera->FindVidOfCamera("vid_261a&pid_0016");
	if (norNum >= 0)
	{
		// ����������ͷ
		if (norCamera->OpenCamera(norNum, false, nor_width, nor_height))
		{
		}
	}*/
	

	int infNum = infCamera->FindVidOfCamera("vid_261a&pid_0016");
	if (infNum >= 0)
	{
		// �򿪺�������ͷ
		if (infCamera->OpenCamera(infNum, false, inf_width, inf_height))
		{
		
			HANDLE hThread = CreateThread(NULL, 0, thread_liveness_detect, (void*)infCamera, 0, NULL);
			Sleep(1000000);
			CloseHandle(hThread);
		}
	}

	//release the buffer
	//cvReleaseImage(&infImage);
	if (NULL != infCamera)
	{
		delete infCamera;
		infCamera = NULL;
	}
	

	return 0;
}


/*#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp" 
#include "opencv2\highgui\highgui.hpp" 
//#include "opencv2/core/utility.hpp" 
#include "opencv2/video/tracking.hpp"
#include <iostream>
using namespace cv;
using namespace std;


template <typename Vec, typename Operator>
void foreach(Vec &v, Operator &op, size_t off, size_t len) {
	for (size_t i = off; i<len; ++i) {
		op(i, v[i]);
	}
}

template <typename Vec, typename Operator>
void foreach(Vec &v, Operator &op) {
	return foreach(v, op, 0, v.size());
}


void Abs(int i, float &it)
{
	it = abs(it);
}

//struct Copy
//{
//    vector<float> &v;
//    Copy(vector<float> & v) : v(v) {}
//    void operator () (float it)
//    {
//        v.push_back(it);
//    }
//};

struct MinMaxId
{
	float m, M;
	int mi, Mi;
	MinMaxId() : m(99999), M(-99999), mi(0), Mi(0) {}
	void operator () (int i, float & it)
	{
		if (it > M) { Mi = i;  M = it; }
		if (it < m) { mi = i;  m = it; }
	}
};


inline double hamming(double n, int N)
{
	return 0.54 - 0.46*cos((2.0*CV_PI*n) / (N - 1));
}
struct Hamming
{
	int N;
	Hamming(int n) : N(n) {}
	void operator () (int i, float & it)
	{
		it = it * float(hamming(i, N));
	}
};
struct HammingInv
{
	int N;
	HammingInv(int n) : N(n) {}
	void operator () (int i, float & it)
	{
		it = it / float(hamming(i, N));
	}
};


struct Ring
{
	vector<float> elm;
	vector<int64> tim;
	int p;

	Ring(int n = 1) : elm(n, 0), tim(n, 0), p(0)  {}

	void push(int64 t, float v)
	{
		p += 1;
		p %= elm.size();

		elm[p] = v;
		tim[p] = t;
	}

	// our timebuffer got sampled, whenever there was a frame available.
	// upsample to len samples evenly spaced in time 
	int wrap(vector<float> & din, size_t len = 512, float ts = 0.02f)
	{
		int   e = (p + 1) % elm.size();
		float t = float(tim[e] / getTickFrequency());
		float tz = float(tim[p] / getTickFrequency());
		while (din.size()<len && t<tz)
		{
			int  nxt = (e + 1) % elm.size();
			float t0 = float(tim[e] / getTickFrequency());
			float t1 = float(tim[nxt] / getTickFrequency());
			float v0 = elm[e];
			float v1 = elm[nxt];
			float v = float(v0 + (t - t0)*(v1 - v0) / (t1 - t0)); // lerp
			din.push_back(v);

			t += ts;
			if (t >= t1)
			{
				e = nxt;
			}
		}
		return p - e;
	}
};

void paint(Mat & img, const vector<float> & elm, int x, int y, Scalar col, float sy = 1.0f, float sx = 1.0f)
{
	size_t len = std::min(elm.size(), size_t(img.cols));
	for (size_t i = 1; i<len; i++)
	{
		line(img, Point(x + int(sx*(i - 1)), y + int(sy*elm[i - 1])), Point(x + int(sx*(i)), y + int(sy*elm[i])), col, 1);
	}
}

float bin2bpm(int b, float ts = 0.02f)
{
	return (60.0f * b) / (2.0f * 512.0f * ts);
}

void ontrack(int t, void *p)
{
	KalmanFilter * kf = (KalmanFilter*)p;
	double v = 1.0f / t;
	setIdentity(kf->processNoiseCov, Scalar::all(v));
	setIdentity(kf->measurementNoiseCov, Scalar::all(v));
	//kf->init(2,1);
}

int main(int argc, char** argv)
{
	bool doDct = true;
	bool doHam = true;
	bool doAbs = true;
	bool doKal = true;

	int hind = 2;
	int tpos = 15;
	int twid = 14;
	int kali = 1000;
	int spike_thresh = 30;

	Ring ring(256);
	Ring orig(256);
	Ring peak(128);
	Rect region = Rect(130, 100, 60, 60);
	Rect region_2 = Rect(440, 100, 60, 60);

	KalmanFilter KF(2, 1, 0);
	Mat measurement = Mat::zeros(1, 1, CV_32F);
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(.0001));
	setIdentity(KF.measurementNoiseCov, Scalar::all(0.0002));
	setIdentity(KF.errorCovPost, Scalar::all(1));

	namedWindow("cam", 0);
	namedWindow("control", 0);
	createTrackbar("pos", "control", &tpos, 512 - 128);
	createTrackbar("width", "control", &twid, 128);
	createTrackbar("kali", "control", &kali, 90000, ontrack, &KF);

	VideoCapture cap(0);
	int f = 0;
	int64 t = 0;
	while (cap.isOpened())
	{
		int64 t0 = getTickCount();
		Mat frame;
		cap >> frame;
		if (frame.empty())
			break;
		Mat draw = frame.clone();

		// take the diff of a skin to a non-skin rect as measure for change
		Mat roi1 = frame(region);
		Mat roi2 = frame(region_2);

		imshow("roi1", roi1);
		imshow("roi2", roi2);
		//Mat roi1; cvtColor(frame(region),roi1,COLOR_BGR2YCrCb);
		//Mat roi2; cvtColor(frame(region_2),roi2,COLOR_BGR2YCrCb);
		//roi1.copyTo(draw(region));
		Scalar m = mean(roi1);
		Scalar m2 = mean(roi2);
		float z = float(m[hind] - m2[hind]);
		//orig.push(t0,z);
		//if ( doKal )
		//{
		//    Mat predicted = KF.predict();
		//    measurement.at<float>(0) = z;
		//    KF.correct(measurement);
		//    z = predicted.at<float>(0);
		//}
		ring.push(t0, z);

		rectangle(draw, region, Scalar(200, 0, 0));
		line(draw, region.tl(), Point(region.tl().x, region.tl().y + int(m[0] / 4)), Scalar(150, 170, 0), 5);
		rectangle(draw, region_2, Scalar(20, 80, 10));
		line(draw, region_2.tl(), Point(region_2.tl().x, region_2.tl().y + int(m2[0] / 4)), Scalar(150, 170, 0), 5);

		int disiz = 0, dosiz = 0, Mi = 0;
		float pf = 1.0f;
		while (doDct && ring.elm.back() != 0) // once
		{
			pf = 3.0f;
			// skip dft if input contains spikes
			MinMaxId mm;
			foreach(ring.elm, mm);
			if (mm.M - mm.m > spike_thresh)
				break;

			vector<float> din, dout;
			int left = ring.wrap(din);
			disiz = ring.elm.size() - left;
			dosiz = din.size();
			if (doHam)
				foreach(din, Hamming(din.size()));

			dft(din, dout);

			if (doAbs)
				foreach(dout, Abs);
			paint(draw, dout, 50, 250, Scalar(0, 0, 200), 1.f, 4.f);
			rectangle(draw, Point(50 + tpos * 4, 250 - 30), Point(50 + (tpos + twid) * 4, 250 + 30), Scalar(200, 0, 0));

			// process the selected fft window:
			vector<float> clipped;
			clipped.insert(clipped.begin(), dout.begin() + tpos, dout.begin() + tpos + twid);

			// peak of fft is the actual cardiac
			MinMaxId mm3;
			foreach(clipped, mm3);

			vector<float> idft;
			dft(clipped, idft, DFT_INVERSE);

			if (doHam)
				foreach(idft, Hamming(idft.size()));
			paint(draw, idft, 50 + tpos, 250 - 30, Scalar(0, 220, 220), 0.8f, 5.0f);

			//// peak of ifft is the actual cardiac ?
			//MinMaxId mm3;
			//foreach(idft,mm3);

			float z = mm3.M;
			orig.push(t0, z);
			if (doKal)
			{
				Mat predicted = KF.predict();
				measurement.at<float>(0) = z;
				KF.correct(measurement);
				z = predicted.at<float>(0);
				paint(draw, orig.elm, 50, 350, Scalar(40, 40, 40), 3, 4);
			}

			peak.push(t0, z);
			//peak.push(mm3.Mi,mm3.M);
			paint(draw, peak.elm, 50, 350, Scalar(20, 140, 140), 3, 4);
			//vector<float> tim;
			//std::for_each(peak.tim.begin(),peak.tim.end(),Copy(tim));
			//paint(draw,tim,50,420,Scalar(20,20,60),1,4);
			circle(draw, Point(50 + peak.p * 4, 350 + int(peak.elm[peak.p]) * 3), 3, Scalar(60, 0, 230), 2);
			Mi = mm3.Mi + tpos;

			break;
		}
		paint(draw, ring.elm, 50, 50 - int(z), Scalar((hind == 0 ? 200 : 0), (hind == 1 ? 200 : 0), (hind == 2 ? 200 : 0)), pf);
		circle(draw, Point(50 + ring.p, 50 - int(z) + int(pf*ring.elm[ring.p])), 3, Scalar(60, 230, 0), 2);
		imshow("cam", draw);
		int k = waitKey(1);
		if (k == 27) break;
		if (k == '1') doDct = !doDct;
		if (k == '2') doHam = !doHam;
		if (k == '3') doAbs = !doAbs;
		if (k == '4') { hind++; hind %= 3; cerr << "hind " << hind << endl; }
		if (k == '5') doKal = !doKal;
		int64 t1 = getTickCount();
		t += (t1 - t0);
		if (f % 10 == 9)
		{
			double fps = 1.0 / double((t / 10) / getTickFrequency());
			cerr << format("%4d %3.3f %3.3f %6d %6d %3.3f", f, fps, z, disiz, dosiz, bin2bpm(Mi)) << endl;
			t = 0;
		}
		f++;
	}
	return 0;
}*/
