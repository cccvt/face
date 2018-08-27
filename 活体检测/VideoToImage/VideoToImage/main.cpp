#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>
#include <time.h>
#include <io.h>

#ifdef _DEBUG
#pragma comment(lib,"opencv_core249d.lib")
#pragma comment(lib,"opencv_highgui249d.lib")
#else
#pragma comment(lib,"opencv_core249.lib")
#pragma comment(lib,"opencv_highgui249.lib")
#endif

using namespace std;

int GetROI(cv::Mat image, cv::Rect rect,cv::Mat &img_roi)
{
	cv::Mat img = image.clone();
	int cols = img.cols, rows = img.rows;
	//ROI越界，返回
	if (cols - rect.x<rect.width || rows - rect.y<rect.height)
		return -1;
	img_roi = img(cv::Rect(rect.x, rect.y, rect.width, rect.height));
	rectangle(img, rect, cv::Scalar(0, 0, 255), 2);
	image.copyTo(img);    //ROI和它的父图像指向同一块缓冲区，经次操作消除 标记ROI的矩形框
	//imshow("ROI", roi);
}

void getFiles(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


int main(int argc, char **argv)
{
	//* 方法一
	time_t tt = time(NULL);//这句返回的只是一个时间cuo
	tm* t = localtime(&tt);
	/*printf("%d-%02d-%02d %02d:%02d:%02d\n",
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);*/
	//const char *file = argv[1];
//	const char *file = "F:\\待分解视频\\111.mp4";
	const char *Dir1 = "F:\\待分解图片";
	std::cout << Dir1 << std::endl;
//	char videosFileNames[_MAX_PATH];
	int frame_count = 0;

	/*for (int index = 14; index < 19; ++index)
	{*/
	//sprintf(videosFileNames, "%s%d%s", file, index + 1, ".mp4");
	//std::cout << "正在处理视频文件： " << videosFileNames << std::endl;
	//std::cout << "正在处理视频文件： " << file << std::endl;
	const char* imageSaveFolder = "F:\\desImages12\\";
	std::cout << imageSaveFolder << std::endl;
	//cv::VideoCapture cap("D:\VideoToImage\Release\222.mp4");
	
	
//	cv::VideoCapture cap(Dir1);
//	if (!cap.isOpened()) {
//		std::cout << "Failed to open video file  " << Dir1 << std::endl;
//		return -1;
//	}

	
	cv::Mat img;

	vector <string> filelist;//用容器方式来存储文件名

	getFiles(Dir1, filelist);


	for (int countall = 0; countall < filelist.size(); countall++)
	{
		img = cv::imread(filelist[countall], 1);
		string name1 = filelist[countall];
//		string name2 = name1.substr[name1.find("白"),6];

//			bool success = cap.read(img);
//			if (!success) {
//				std::cout << "Process " << frame_count << " frames from " << std::endl;
//				break;
//			}


			if (img.empty())
				std::cout << "Error when read frame" << std::endl;
			else
			{
				//get roi
				//				char imageSavePath[_MAX_PATH];
				//				sprintf(imageSavePath, "%s%d-%02d-%02d-%02d-%02d-%02d_%06d%s", imageSaveFolder,
				//					t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, count, ".jpg");
				//				imwrite(imageSavePath, img);
				//				count++;
				int count = 0;

				int rectnum_x = 4;
				int rectnum_y = 3;

				for (int j = 0; j < rectnum_y; j++)
				{
					for (int i = 0; i < rectnum_x; i++)
					{
						count++;
						cv::Mat img_roi;
						cv::Rect rect;
						rect.x = 1920 / rectnum_x * (i);
						rect.y = 1080 / rectnum_y * (j);
						rect.width = 1920 / rectnum_x;
						rect.height = 1080 / rectnum_y;
						GetROI(img, rect, img_roi);
						/*char nameWindows[_MAX_PATH];
						sprintf(nameWindows, "%s%d", "ROI_0", i + 1);
						cv::imshow(nameWindows, img_roi);*/

						char imageSavePath[_MAX_PATH];
						sprintf(imageSavePath, "%s%d-%d%s", imageSaveFolder,
							countall, count, ".jpg");

						imwrite(imageSavePath, img_roi);
						std::cout << "图片" << countall<<"_" <<count<< "已存入文件夹内！" << std::endl;
					}
				}
			}

			frame_count++;
			//cv::imshow("video", img);
			cv::waitKey(1);
			/*	}*/
		
	}
	return 0;
}

