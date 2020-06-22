#pragma once
#ifndef  COM_DATA_STRUCT_H_
#define  COM_DATA_STRUCT_H_

#include "VzBoxClientSDK.h"
#include "VzClientSDK.h"
#include "VzClientSDKDefine.h"
#include "VzFaceClientSDK.h"
#include "VzLPRClientSDK.h"


#define  VZSDK_SUCCESS  0
#define  VZSDK_FAILED   -1


typedef struct {
    int camera_id;
    int channel_id;
    VZ_BOX_CAM_INFO camera_item;
}CameraAttribute;

typedef  VZ_FACE_USER_ITEM  UserInfo;

typedef struct {
	unsigned char sex;							// 0:男，1:女，2:未知
	unsigned char age;							// 1:少年 2:青年 3:中年 4:老年，5:未知
	unsigned char have_hat;                     // 0:没，1:有，2:未知
	unsigned char have_glasses;                 // 0:没，1:有，2:未知
	//unsigned char have_mask;                    // 0:没，1:有，2:未知
	//char age_real;								// 真实年龄
	short confidence;			                // 置信度
	unsigned int msec;							// 时间戳，毫秒
	char datetime[20];							// 时间，格式：yyyy-MM-dd HH:mm:ss
}FaceSnapInfo;


typedef struct {

}FaceRecognizeInfo;
#endif
