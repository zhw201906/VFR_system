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



#endif
