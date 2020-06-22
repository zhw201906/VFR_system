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
	unsigned char sex;							// 0:�У�1:Ů��2:δ֪
	unsigned char age;							// 1:���� 2:���� 3:���� 4:���꣬5:δ֪
	unsigned char have_hat;                     // 0:û��1:�У�2:δ֪
	unsigned char have_glasses;                 // 0:û��1:�У�2:δ֪
	//unsigned char have_mask;                    // 0:û��1:�У�2:δ֪
	//char age_real;								// ��ʵ����
	short confidence;			                // ���Ŷ�
	unsigned int msec;							// ʱ���������
	char datetime[20];							// ʱ�䣬��ʽ��yyyy-MM-dd HH:mm:ss
}FaceSnapInfo;


typedef struct {

}FaceRecognizeInfo;
#endif
