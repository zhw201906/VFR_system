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

class FaceAlgServer
{
public:
	~FaceAlgServer();
	static FaceAlgServer *Instance();

	void  DetectFaceAndRect(cv::Mat &src, cv::Mat &dst);

private:
	FaceAlgServer();
	bool CreatEngineInit();
	bool isAlgReady() const;

private:
	MHandle face_engine_handle;
	static FaceAlgServer *p_face_alg_server;

};


#endif
