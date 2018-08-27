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

typedef void* SSLDHandle; //活检句柄

//人脸框信息
typedef struct SSLDFaceRect
{
	int x;	//x坐标值
	int y;	//y坐标值
	int w;	//人脸框宽度
	int h;	//人脸框高度
};

/*typedef struct SSLDFrameInfo
{
	int frame_width;		//输入图片的宽度
	int frame_height;		//输入图片的高度
	SSLDFaceRect rect;		//输入图片中的人脸框信息
};*/

/***
*函数功能：01 活体检测初始化
*param: @IN width 红外摄像头宽度
*param: @IN height 红外摄像头高度
*param: @OUT ssHandle 活检句柄
*
*返回值：0表示成功，-1表示失败；
***/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Init(int width, int height, SSLDHandle* ssHandle);

/***
*函数功能：02 活体检测卸载
*param: @IN ssHandle 活检句柄
*
*返回值：0 表示成功， -1表示失败；
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_UnInit(SSLDHandle ssHandle);


/***
*函数功能：03 活体检测
*param: @IN ssHandle 活检句柄
*param: @IN pFrameData 图片帧数据
*param：@IN frameDataLength 图片帧数据大小
*param：@IN rect 人脸框信息
*param：@OUT threshold 阈值
*
*返回值：0 表示成功，-1表示失败
*/
SSLIVENESSDETECTION_API int __stdcall SS_LivenessSDK_Detect(SSLDHandle ssHandle, char* pFrameData, int frameDataLength, SSLDFaceRect rect, double * threshold);



#endif