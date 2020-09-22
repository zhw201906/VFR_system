#ifndef _FACE_ALG_SERVER_H
#define _FACE_ALG_SERVER_H

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
#include <QDebug>
#include <QDateTime>

#define  SNAP_EXPAND_SIZE  60

#define  SNAP_SAVE_PATH  "../SnapFace"

enum {
    OPER_NONE  = 0x00,  //无模式
    OPER_RECT  = 0x01,  //人脸检测
    OPER_COUNT = 0x02,  //区域计数
    OPER_SNAP  = 0x04,  //抓拍
    OPER_SAVE  = 0x08   //保存抓拍
};

typedef void (*pSnapFaceFun)(const cv::Mat &img, void *puser);

class FaceAlgServer
{
public:
	~FaceAlgServer();
	static FaceAlgServer *Instance();

	void  DetectFaceAndRect(cv::Mat &src, cv::Mat &dst);
	void  SetFaceAlgMode(char state = OPER_NONE);
	void  SetSnapFaceCallBack(pSnapFaceFun pfun, void *puser);

private:
	FaceAlgServer();
	bool CreatEngineInit();
	bool isAlgReady() const;
	
	void AlgSnapTask(const cv::Mat &img, cv::Rect &rect);
	void AlgSaveSnapTask(const cv::Mat &img, cv::Rect &rect, std::string path = SNAP_SAVE_PATH);

private:
	MHandle face_engine_handle;
	static FaceAlgServer *p_face_alg_server;
	char  oper_status;

	pSnapFaceFun p_SnapFaceCallBack;
	void *p_context;
    
};


#endif
