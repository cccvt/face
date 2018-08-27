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

typedef void* SSLivenessHandle; //�����

//��������Ϣ
typedef struct FaceRect
{
	int x;	//x����ֵ
	int y;	//y����ֵ
	int w;	//��������
	int h;	//������߶�
};

typedef struct FrameInfo
{
	int frame_width;	//����ͼƬ�Ŀ��
	int frame_height;	//����ͼƬ�ĸ߶�
	FaceRect rect;		//����ͼƬ�е���������Ϣ
};

/***
*�������ܣ�01 �������ʼ��
*param: @IN width ��������ͷ���
*param: @IN height ��������ͷ�߶�
*param: @OUT ssHandle �����
*
*����ֵ��0��ʾ�ɹ���-1��ʾʧ�ܣ�
***/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Init(int width, int height, SSLivenessHandle* ssHandle);

/***
*�������ܣ�02 ������ж��
*param: @IN ssHandle �����
*
*����ֵ������0 ��ʾ�ɹ���1001 ��ʾ���ڼ������У�1002 ��ʾ�������ǻ��壬1003��ʾ�������ǻ��壻
*        С��0 ��ʾʧ�ܣ�-1001 ��ʾ����ľ����ͼƬָ�����Ϊ�գ�-1002 ��ʾͼƬ��Ϣ�������
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_UnInit(SSLivenessHandle ssHandle);


/***
*�������ܣ�03 ������
*param: @IN ssHandle �����
*param: @IN pFrameData ͼƬ֡����
*param��@IN info ��ͼƬ��Ӧ����Ϣ
*
*����ֵ��0 ��ʾ�ɹ��� -1��ʾʧ�ܣ�
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Detect(SSLivenessHandle ssHandle, char* pFrameData, FrameInfo info);



#endif