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
	unsigned char channel_id;					// ������Ƶͨ����
	char          datetime[20];					// ʱ�䣬��ʽ��yyyy-MM-dd HH:mm:ss
	unsigned int  msec;							// ʱ���������
	unsigned char num;							// ��������
	unsigned char snap_num;					    // ȫͼ����
	int recg_face_id;			        // ʶ���������������е�id
	unsigned short recg_face_lib_id;    // ������id
	unsigned char  recg_face_score;     // ʶ��������[0-100]
	char recg_people_name[16];	        // ʶ���������
	char recg_img_url[64];				// ͼƬ·��
	char recg_birthday[12];				// ����
	char recg_prov[18];					// ����ʡ��
}FaceRecognizeInfo;



#endif
