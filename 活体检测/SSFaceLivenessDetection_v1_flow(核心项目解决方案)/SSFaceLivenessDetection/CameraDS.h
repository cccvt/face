//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to:
//		HardyAI@OpenCV China
//		flymanbox@OpenCV China (for his contribution to function CameraName, and frame width/height setting)
// Last modification: April 9, 2009
//
// 使用说明：
//   1. 将CameraDS.h CameraDS.cpp以及目录DirectShow复制到你的项目中
//   2. 菜单 Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      设置为 DirectShow/Include
//   3. 菜单 Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      设置为 DirectShow/Lib
//////////////////////////////////////////////////////////////////////

#ifndef CCAMERA_H
#define CCAMERA_H

#define WIN32_LEAN_AND_MEAN

#include <atlbase.h>

#include "qedit.h"
#include "dshow.h"

#include <windows.h>
#include <opencv.hpp>

#define MYFREEMEDIATYPE(mt)	{if ((mt).cbFormat != 0)		\
					{CoTaskMemFree((PVOID)(mt).pbFormat);	\
					(mt).cbFormat = 0;						\
					(mt).pbFormat = NULL;					\
				}											\
				if ((mt).pUnk != NULL)						\
				{											\
					(mt).pUnk->Release();					\
					(mt).pUnk = NULL;						\
				}}									

#define SUCCEED_OK 0
#define FAILURE_NO -1
class CCameraDS  
{
private:

	bool m_bConnected, m_bLock, m_bChanged;
	int m_nWidth, m_nHeight;
	long m_nBufferSize;

	IplImage *m_pFrame;
	byte* m_pFrameData;
	CComPtr<IGraphBuilder> m_pGraph;
	CComPtr<ISampleGrabber> m_pSampleGrabber;
	CComPtr<IMediaControl> m_pMediaControl;
	CComPtr<IMediaEvent> m_pMediaEvent;

	CComPtr<IBaseFilter> m_pSampleGrabberFilter;
	CComPtr<IBaseFilter> m_pDeviceFilter;
	CComPtr<IBaseFilter> m_pNullFilter;

	CComPtr<IPin> m_pGrabberInput;
	CComPtr<IPin> m_pGrabberOutput;
	CComPtr<IPin> m_pCameraOutput;
	CComPtr<IPin> m_pNullInputPin;

	bool BindFilter(int nCamIDX, IBaseFilter **pFilter);
	void SetCrossBar();

public:
	CCameraDS();
	virtual ~CCameraDS();

	/**********************
	*函数功能：打开摄像头设备
	*参数：
	*@param IN nCamID 摄像头编号，取值可以为0,1,2,...
	*@param IN bDisplayProperties 指示是否自动弹出摄像头属性页
	*@param IN nWidth 设置的摄像头的宽
	*@param IN nHeight 设置的摄像头的高
	*返回值：
	*@return true表示获取成功，false表示获取失败；
	*注：如果摄像头不支持所设定的宽度和高度，则返回false。
	***************************/
	bool OpenCamera(int nCamID, bool bDisplayProperties = true, int nWidth = 320, int nHeight = 240);

	//关闭摄像头，析构函数会自动调用这个函数
	void CloseCamera();

	/**********************
	*函数功能：获取摄像头的数目
	*参数：
	*@param None
	*返回值：
	*@return 表示找到摄像头个数
	*注：可以不用创建CCameraDS实例，采用int c=CCameraDS::CameraCount();得到结果。
	***************************/
	static int GetCameraCount(); 
	
	/**********************
	*函数功能：根据摄像头的编号返回摄像头的名字
	*参数：
	*@param IN nCamID 摄像头编号
	*@param OUT sName 用于存放摄像头名字的数组
	*@param IN nBufferSize sName的大小
	*返回值：
	*@return 0表示获取成功，-1表示获取失败；
	*注：可以不用创建CCameraDS实例，采用CCameraDS::CameraName();得到结果。
	***************************/
	static int GetCameraName(int nCamID, char* sName, int nBufferSize);

	
	/**********************
	*函数功能：根据摄像头pid_vid获取摄像头编号
	*参数：
	*@param IN vidName 摄像头的pid_vid号
	*返回值：
	*@return 表示找到该vid号的设备编号
	***************************/
	int FindVidOfCamera(const char* vidName);

	/**********************
	*函数功能：根据摄像头的编号获取摄像头的分辨率
	*参数：
	*@param IN nCamID 摄像头ID
	*@param OUT res 该摄像头可打开的分辨率
	*返回值：
	*@return 0表示获取成功，-1表示获取失败；
	***************************/
	int GetCameraResolution(int nCamID, char *res);

	//返回图像宽度
	int GetWidth(){return m_nWidth;} 
	//返回图像高度
	int GetHeight(){return m_nHeight;}

	//抓取一帧，返回的IplImage不可手动释放！
	//返回图像数据的为RGB模式的Top-down(第一个字节为左上角像素)，即IplImage::origin=0(IPL_ORIGIN_TL)
	bool QueryFrame(unsigned char* data);

	//获取摄像头一帧数据
	/**********************
	*函数功能：获取摄像头一帧数据
	*参数：
	*@param IN nCamID 摄像头ID
	*@param OUT res 该摄像头可打开的分辨率
	*返回值：
	*@return 0表示获取成功，-1表示获取失败；
	***************************/
	//byte* QueryFrameData();
};

#endif 
