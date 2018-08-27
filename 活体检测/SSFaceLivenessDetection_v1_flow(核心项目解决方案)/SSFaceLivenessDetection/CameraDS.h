//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to:
//		HardyAI@OpenCV China
//		flymanbox@OpenCV China (for his contribution to function CameraName, and frame width/height setting)
// Last modification: April 9, 2009
//
// ʹ��˵����
//   1. ��CameraDS.h CameraDS.cpp�Լ�Ŀ¼DirectShow���Ƶ������Ŀ��
//   2. �˵� Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      ����Ϊ DirectShow/Include
//   3. �˵� Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      ����Ϊ DirectShow/Lib
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
	*�������ܣ�������ͷ�豸
	*������
	*@param IN nCamID ����ͷ��ţ�ȡֵ����Ϊ0,1,2,...
	*@param IN bDisplayProperties ָʾ�Ƿ��Զ���������ͷ����ҳ
	*@param IN nWidth ���õ�����ͷ�Ŀ�
	*@param IN nHeight ���õ�����ͷ�ĸ�
	*����ֵ��
	*@return true��ʾ��ȡ�ɹ���false��ʾ��ȡʧ�ܣ�
	*ע���������ͷ��֧�����趨�Ŀ�Ⱥ͸߶ȣ��򷵻�false��
	***************************/
	bool OpenCamera(int nCamID, bool bDisplayProperties = true, int nWidth = 320, int nHeight = 240);

	//�ر�����ͷ�������������Զ������������
	void CloseCamera();

	/**********************
	*�������ܣ���ȡ����ͷ����Ŀ
	*������
	*@param None
	*����ֵ��
	*@return ��ʾ�ҵ�����ͷ����
	*ע�����Բ��ô���CCameraDSʵ��������int c=CCameraDS::CameraCount();�õ������
	***************************/
	static int GetCameraCount(); 
	
	/**********************
	*�������ܣ���������ͷ�ı�ŷ�������ͷ������
	*������
	*@param IN nCamID ����ͷ���
	*@param OUT sName ���ڴ������ͷ���ֵ�����
	*@param IN nBufferSize sName�Ĵ�С
	*����ֵ��
	*@return 0��ʾ��ȡ�ɹ���-1��ʾ��ȡʧ�ܣ�
	*ע�����Բ��ô���CCameraDSʵ��������CCameraDS::CameraName();�õ������
	***************************/
	static int GetCameraName(int nCamID, char* sName, int nBufferSize);

	
	/**********************
	*�������ܣ���������ͷpid_vid��ȡ����ͷ���
	*������
	*@param IN vidName ����ͷ��pid_vid��
	*����ֵ��
	*@return ��ʾ�ҵ���vid�ŵ��豸���
	***************************/
	int FindVidOfCamera(const char* vidName);

	/**********************
	*�������ܣ���������ͷ�ı�Ż�ȡ����ͷ�ķֱ���
	*������
	*@param IN nCamID ����ͷID
	*@param OUT res ������ͷ�ɴ򿪵ķֱ���
	*����ֵ��
	*@return 0��ʾ��ȡ�ɹ���-1��ʾ��ȡʧ�ܣ�
	***************************/
	int GetCameraResolution(int nCamID, char *res);

	//����ͼ����
	int GetWidth(){return m_nWidth;} 
	//����ͼ��߶�
	int GetHeight(){return m_nHeight;}

	//ץȡһ֡�����ص�IplImage�����ֶ��ͷţ�
	//����ͼ�����ݵ�ΪRGBģʽ��Top-down(��һ���ֽ�Ϊ���Ͻ�����)����IplImage::origin=0(IPL_ORIGIN_TL)
	bool QueryFrame(unsigned char* data);

	//��ȡ����ͷһ֡����
	/**********************
	*�������ܣ���ȡ����ͷһ֡����
	*������
	*@param IN nCamID ����ͷID
	*@param OUT res ������ͷ�ɴ򿪵ķֱ���
	*����ֵ��
	*@return 0��ʾ��ȡ�ɹ���-1��ʾ��ȡʧ�ܣ�
	***************************/
	//byte* QueryFrameData();
};

#endif 
