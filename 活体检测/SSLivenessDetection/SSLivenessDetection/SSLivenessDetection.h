/*******   ��˼������SDK�ӿ� ***************

Author��Yingpeng Chen
Date��2018-01-2
Version��V1.0.0.2

************/

#ifndef SSLIVENESSDETECTION_H
#define SSLIVENESSDETECTION_H


#ifdef SSLIVENESSDETECTION_EXPORTS
#define SSLIVENESSDETECTION_API __declspec(dllexport)
#else
#define SSLIVENESSDETECTION_API __declspec(dllimport)
#endif

typedef void* SSLDHandle; //�����

//��������Ϣ
typedef struct SSLDFaceRect
{
	int x;	//x����ֵ
	int y;	//y����ֵ
	int w;	//��������
	int h;	//������߶�
};

/*typedef struct SSLDFrameInfo
{
	int frame_width;		//����ͼƬ�Ŀ��
	int frame_height;		//����ͼƬ�ĸ߶�
	SSLDFaceRect rect;		//����ͼƬ�е���������Ϣ
};*/

/***
*�������ܣ�01 �������ʼ��
*param: @IN width ��������ͷ���
*param: @IN height ��������ͷ�߶�
*param: @OUT ssHandle �����
*
*����ֵ��0��ʾ�ɹ���-1��ʾʧ�ܣ�
***/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Init(int width, int height, SSLDHandle* ssHandle);

/***
*�������ܣ�02 ������ж��
*param: @IN ssHandle �����
*
*����ֵ��0 ��ʾ�ɹ��� -1��ʾʧ�ܣ�
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_UnInit(SSLDHandle ssHandle);


/***
*�������ܣ�03 ������
*param: @IN ssHandle �����
*param: @IN pFrameData ͼƬ֡����
*param��@IN frameDataLength ͼƬ֡���ݴ�С
*param��@IN rect ��������Ϣ
*param��@OUT threshold ��ֵ
*
*����ֵ��0 ��ʾ�ɹ���-1��ʾʧ��
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Detect(SSLDHandle ssHandle, char* pFrameData, int frameDataLength, SSLDFaceRect rect, double * threshold);



#endif