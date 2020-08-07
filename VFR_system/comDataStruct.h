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

typedef struct CameraPositionInfo_ {
	int     camera_id;
	QString camera_ip;
	QString position_name;
	QPoint  position;
}CameraPositionInfo;

typedef struct {
	int threshold_value;
	int id;
	int enable;
	int lib_type;
	char name[32];
	char remark[64];
}FaceLibInfo;

typedef  struct {
	char card_number[32];
	char db_name[32];
	int  face_id;
	int  pic_index;
	char group_id[32];
	char phone[16];
	char province[32];
	char city[16];
	int  sex;
	int  user_type;
	char birthday[16];
	char address[64];
	char user_name[64];
	char img_url[128];
}UserInfo;

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
	unsigned char channel_id;					// 所属视频通道号
	char          datetime[20];					// 时间，格式：yyyy-MM-dd HH:mm:ss
	unsigned int  msec;							// 时间戳，毫秒
	unsigned char num;							// 人脸个数
	unsigned char snap_num;					    // 全图个数
	int recg_face_id;			        // 识别结果所在人脸库中的id
	unsigned short recg_face_lib_id;    // 人脸库id
	unsigned char  recg_face_score;     // 识别结果分数[0-100]
	char recg_people_name[16];	        // 识别出的人名
	char recg_img_url[64];				// 图片路径
	char recg_birthday[12];				// 生日
	char recg_prov[18];					// 所在省份
}FaceRecognizeInfo;



#endif
