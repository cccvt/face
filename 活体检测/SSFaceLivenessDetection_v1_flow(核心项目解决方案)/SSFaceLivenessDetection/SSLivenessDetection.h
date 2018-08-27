/*******   神思活体检测SDK接口 ***************

Author：Yingpeng Chen
Date：2018-01-2
Version：V1.0.0.2

************/

#ifndef SSLIVENESSDETECTION_H
#define SSLIVENESSDETECTION_H


#ifdef SSLIVENESSDETECTION_EXPORTS
#define SSLIVENESSDETECTION_API __declspec(dllexport)
#else
#define SSLIVENESSDETECTION_API __declspec(dllimport)
#endif

typedef void* SSLivenessHandle; //活检句柄

//人脸框信息
typedef struct FaceRect
{
	int x;	//x坐标值
	int y;	//y坐标值
	int w;	//人脸框宽度
	int h;	//人脸框高度
};

typedef struct FrameInfo
{
	int frame_width;	//输入图片的宽度
	int frame_height;	//输入图片的高度
	FaceRect rect;		//输入图片中的人脸框信息
};

/***
*函数功能：01 活体检测初始化
*param: @IN width 红外摄像头宽度
*param: @IN height 红外摄像头高度
*param: @OUT ssHandle 活检句柄
*
*返回值：0表示成功，-1表示失败；
***/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Init(int width, int height, SSLivenessHandle* ssHandle);

/***
*函数功能：02 活体检测卸载
*param: @IN ssHandle 活检句柄
*
*返回值：大于0 表示成功，1001 表示正在检测过程中，1002 表示检测出来是活体，1003表示检测出来非活体；
*        小于0 表示失败；-1001 表示输入的句柄与图片指针出现为空，-1002 表示图片信息输入错误；
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_UnInit(SSLivenessHandle ssHandle);


/***
*函数功能：03 活体检测
*param: @IN ssHandle 活检句柄
*param: @IN pFrameData 图片帧数据
*param：@IN info 该图片对应的信息
*
*返回值：0 表示成功， -1表示失败；
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Detect(SSLivenessHandle ssHandle, char* pFrameData, FrameInfo info);



#endif