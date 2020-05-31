#pragma once
#ifndef _FACE_OPERATION_H
#define _FACE_OPERATION_H

#include <stdio.h>
#include <stdlib.h>
#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <direct.h>
#include <iostream>  
#include <stdarg.h>
#include <string>
#include "opencv2/opencv.hpp"
#include <windows.h>
#include <time.h>

//#include "sqlite_operation.h"

#pragma comment(lib, "libarcsoft_face_engine.lib")

#define FACE_OPERATION_OK     0
#define FACE_OPERATION_ERR   -1

typedef struct
{
	ASF_DetectMode     detectMode;    //检测模式：图片流、视频流
	ASF_OrientPriority faceOrient;    //人脸角度
	MInt32             faceScaleVal;  //人脸尺寸
	MInt32             faceMaxNum;    //人脸个数
	MInt32             functionMask;  //引擎功能
}FACE_ENGINE_INIT_PARAMS;
typedef  FACE_ENGINE_INIT_PARAMS  FaceEngineInitParams;

typedef struct
{
	int *userId;
	int userNum;
}MULTI_RECOGNITION_RESULT;

typedef struct
{
	ASF_MultiFaceInfo  detectInfo;
	ASF_AgeInfo        ageInfo;
	ASF_GenderInfo     genderInfo;
	ASF_Face3DAngle    angleInfo;
}FaceDetectResult;

typedef struct
{
	int               user_num;
	ASF_FaceFeature  *face_feature;
}AllFacesFeature;

enum PloyRecg{PLOY_RECG,NOT_PLOY_RECG};
enum DisplayRecg{DISPLAY,NOT_DISPLAY};

class FACE_OPERATION_ENGINE
{
public:
	~FACE_OPERATION_ENGINE();

	MHandle GetFaceOperationHandle(void) { return this->engineHandle; };
//	SQLITE_OPERATION *GetRecgDbIndex(void) { return this->recg_db_index_; };

//	MRESULT FaceLibraryInit(void);

	MRESULT FacesDetectTask(IplImage* src, ASF_MultiFaceInfo *detectResult );   //人脸检测任务
	MRESULT FacesDetectTask(IplImage* src, FaceDetectResult *detectResult, cv::Mat &dst);
	MRESULT FacesDetectRecg(IplImage* src, ASF_MultiFaceInfo *detectResult, PloyRecg ploy = PLOY_RECG, DisplayRecg dis = DISPLAY);

	MRESULT FaceFeatureExtract(IplImage* src, cv::Mat &dst, AllFacesFeature &faces_feature);   //人脸特征提取
	MRESULT FaceFeatureExtract(IplImage* src, ASF_MultiFaceInfo detectResult, ASF_FaceFeature *feature);   //人脸特征提取
//	MRESULT FaceCompareTask(AllFacesFeature *faces_feature, std::vector<FacesFeatureUrl> *memery_user_feature_);
	MRESULT FaceCompareTask(ASF_FaceFeature feature1, ASF_FaceFeature feature2, MFloat* confidenceValue);  //人脸比较任务
	MRESULT FaceFeatureCompare(ASF_FaceFeature feature, MFloat* confidenceValue);
//	MRESULT FaceRecognition(ASF_FaceFeature &feature, int *personId);   //人脸识别
	MRESULT FaceRecognition(IplImage* src, MULTI_RECOGNITION_RESULT *userInfor);

	//void  PrintfMSG(const char* msg);
    static FACE_OPERATION_ENGINE* GetInstance();

private:
    FACE_OPERATION_ENGINE(FACE_ENGINE_INIT_PARAMS initParam);
    MHandle CreatAndInitEngine(FACE_ENGINE_INIT_PARAMS initParam);   //创建与初始化人脸识别引擎

private:
	MHandle engineHandle;
    static FACE_OPERATION_ENGINE *face_recg_Handle;
//	SQLITE_OPERATION *recg_db_index_;

public:
	std::map<MFloat, int> faceRecognitionResult;
};

typedef  FACE_OPERATION_ENGINE  FaceEngineClass;









#endif

