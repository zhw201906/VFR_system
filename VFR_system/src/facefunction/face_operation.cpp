#include "face_operation.h"


//裁剪图片
static void CutIplImage(IplImage* src, IplImage* dst, int x, int y)
{
	CvSize size = cvSize(dst->width, dst->height);//区域大小
	cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
	cvCopy(src, dst); //复制图像
	cvResetImageROI(src);//源图像用完后，清空ROI
}

FACE_OPERATION_ENGINE* FACE_OPERATION_ENGINE::face_recg_Handle = NULL;

FACE_OPERATION_ENGINE::FACE_OPERATION_ENGINE(FACE_ENGINE_INIT_PARAMS initParam)
{
	engineHandle = this->CreatAndInitEngine(initParam);
	//this->FaceLibraryInit();
}

MHandle FACE_OPERATION_ENGINE::CreatAndInitEngine(FACE_ENGINE_INIT_PARAMS initParam)
{
	MRESULT res = MOK;
	ASF_ActiveFileInfo  activeFileInfo = { 0 };
	res = ASFGetActiveFileInfo(&activeFileInfo);
	if (res != MOK)
	{
		printf("ASFGetActiveFileInfo fail: %d\n", res);
	}

	const ASF_VERSION version = ASFGetVersion();
	//printf("\nVersion:%s\n", version.Version);
	//printf("BuildDate:%s\n", version.BuildDate);
	//printf("CopyRight:%s\n", version.CopyRight);

	//printf("\n************* Face Recognition *****************\n");

	//激活接口,首次激活需联网
	char appid[] = "dqKEuBzR3rHNBqaQGKzTNJHtA2QyCvS582ForZj1zLS";
	char key[] = "sxrnBPvm3Wp56bHoJLp7JDjZph1JvJeniJDkUaeuu7X";
	res = ASFOnlineActivation(appid, key);
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		printf("ASFActivation fail: %d\n", res);
	else
		printf("ASFActivation sucess: %d\n", res);

	//初始化接口
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE |
		ASF_LIVENESS | ASF_IR_LIVENESS;
	res = ASFInitEngine(initParam.detectMode, initParam.faceOrient, initParam.faceScaleVal, initParam.faceMaxNum, initParam.functionMask, &engineHandle);
	if (res != MOK)
		printf("ASFInitEngine fail: %d\n", res);
	else
		printf("ASFInitEngine sucess: %d\n", res);
	return engineHandle;
}

//MRESULT FACE_OPERATION_ENGINE::FaceLibraryInit(void)
//{
//	if (this->recg_db_index_ == NULL)
//	{
//		recg_db_index_ = new(SQLITE_OPERATION);
//	}
//	recg_db_index_->SqliteInit();
//	recg_db_index_->MemeryInit();
//	return FACE_OPERATION_OK;
//}

MRESULT FACE_OPERATION_ENGINE::FacesDetectTask(IplImage* src, ASF_MultiFaceInfo *detectResult)
{
	if (src == NULL)
		return MERR_FSDK_FR_INVALID_IMAGE_INFO;

	IplImage* cutImg1 = cvCreateImage(cvSize(src->width - src->width % 4, src->height), IPL_DEPTH_8U, src->nChannels);
	CutIplImage(src, cutImg1, 0, 0);

	MRESULT res = ASFDetectFaces(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		                        (MUInt8*)cutImg1->imageData, detectResult);
	return res;
}

//UI 界面中人脸检测接口
MRESULT FACE_OPERATION_ENGINE::FacesDetectTask(IplImage* src, FaceDetectResult *detectResult, cv::Mat &dst)
{
	if (src == NULL)
		return MERR_FSDK_FR_INVALID_IMAGE_INFO;

	cv::Mat img(src, true);

	IplImage* cutImg1 = cvCreateImage(cvSize(src->width - src->width % 4, src->height), IPL_DEPTH_8U, src->nChannels);
	CutIplImage(src, cutImg1, 0, 0);

	MRESULT res = ASFDetectFaces(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		(MUInt8*)cutImg1->imageData, &detectResult->detectInfo);

	for (int i = 0; i < detectResult->detectInfo.faceNum; ++i)
	{
		int width  = detectResult->detectInfo.faceRect[i].right -  detectResult->detectInfo.faceRect[i].left;
		int height = detectResult->detectInfo.faceRect[i].bottom - detectResult->detectInfo.faceRect[i].top;
		cv::Rect rect(detectResult->detectInfo.faceRect[i].left, detectResult->detectInfo.faceRect[i].top, width, height);

		cv::rectangle(img, rect, cv::Scalar(0, 255, 0), 1, 8, 0);
		char label_text[30] = { 0 };
		sprintf(label_text, "%d", i + 1);
		std::string label(label_text);
		cv::Point label_point(detectResult->detectInfo.faceRect[i].left + width / 2 - 6, detectResult->detectInfo.faceRect[i].bottom - 6);
		CvScalar textColor = cvScalar(0, 0, 255);
		cv::putText(img, label, label_point, 1, 1.5, textColor);

	}

	dst = img;

	MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
	res = ASFProcess(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		(MUInt8*)cutImg1->imageData, &detectResult->detectInfo, processMask);

	res = ASFGetAge(engineHandle, &detectResult->ageInfo);

	res = ASFGetGender(engineHandle, &detectResult->genderInfo);

	res = ASFGetFace3DAngle(engineHandle, &detectResult->angleInfo);

	return res;
}




MRESULT FACE_OPERATION_ENGINE::FacesDetectRecg(IplImage* src, ASF_MultiFaceInfo *detectResult, PloyRecg ploy, DisplayRecg dis)
{
	if (src == NULL)
		return MERR_FSDK_FR_INVALID_IMAGE_INFO;
	cv::Mat img(src, true);

	IplImage* cutImg1 = cvCreateImage(cvSize(src->width - src->width % 4, src->height), IPL_DEPTH_8U, src->nChannels);
	CutIplImage(src, cutImg1, 0, 0);

	MRESULT res = ASFDetectFaces(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		(MUInt8*)cutImg1->imageData, detectResult);
	if (res != MOK)
	{
		printf("ASFDetectFaces failed:%d\n", res);
		return res;
	}
	if (ploy == PLOY_RECG)
	{
		for (int i = 0; i < detectResult->faceNum; ++i)
		{
			int width = detectResult->faceRect[i].right - detectResult->faceRect[i].left;
			int height = detectResult->faceRect[i].bottom - detectResult->faceRect[i].top;
			cv::Rect rect(detectResult->faceRect[i].left, detectResult->faceRect[i].top, width, height);

			cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
		}
	}
	if (dis == DISPLAY)
	{
		cv::Mat dst;
		cv::resize(img, dst, cv::Size(960, img.rows*960.0 / img.cols));
		cv::imshow("detect result", dst);
	}
	cvReleaseImage(&cutImg1);
	return res;
}

MRESULT FACE_OPERATION_ENGINE::FaceFeatureExtract(IplImage *src, cv::Mat &dst, AllFacesFeature &all_faces_feature)
{
	if (src == NULL)
		return MERR_FSDK_FR_INVALID_IMAGE_INFO;

	cv::Mat img(src, true);

	IplImage* cutImg1 = cvCreateImage(cvSize(src->width - src->width % 4, src->height), IPL_DEPTH_8U, src->nChannels);
	CutIplImage(src, cutImg1, 0, 0);

	ASF_MultiFaceInfo  detectInfo;
	MRESULT res = ASFDetectFaces(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		(MUInt8*)cutImg1->imageData, &detectInfo);

	all_faces_feature.user_num = detectInfo.faceNum;
	all_faces_feature.face_feature = (ASF_FaceFeature *)malloc(detectInfo.faceNum);
	for (int i = 0; i < detectInfo.faceNum; ++i)
	{
		int width =  detectInfo.faceRect[i].right -  detectInfo.faceRect[i].left;
		int height = detectInfo.faceRect[i].bottom - detectInfo.faceRect[i].top;
		cv::Rect rect(detectInfo.faceRect[i].left, detectInfo.faceRect[i].top, width, height);

		cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
		char label_text[30] = { 0 };
		sprintf(label_text, "%d", i + 1);
		std::string label(label_text);
		cv::Point label_point(detectInfo.faceRect[i].left + width / 2 - 6, detectInfo.faceRect[i].bottom - 6);
		CvScalar textColor = cvScalar(0, 0, 255);
		cv::putText(img, label, label_point, 1, 1.5, textColor);


		ASF_SingleFaceInfo singleFace;
		singleFace.faceRect.left   = detectInfo.faceRect[i].left;
		singleFace.faceRect.right  = detectInfo.faceRect[i].right;
		singleFace.faceRect.top    = detectInfo.faceRect[i].top;
		singleFace.faceRect.bottom = detectInfo.faceRect[i].bottom;
		singleFace.faceOrient = detectInfo.faceOrient[i];

		ASF_FaceFeature  feature;

		MRESULT res = ASFFaceFeatureExtract(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
			(MUInt8*)cutImg1->imageData, &singleFace, &feature);


		all_faces_feature.face_feature[i].feature = (MByte*)malloc(feature.featureSize);
		memcpy(all_faces_feature.face_feature[i].feature, feature.feature, feature.featureSize);
		all_faces_feature.face_feature[i].featureSize = feature.featureSize;
		//free(feature.feature);
	}

	cvReleaseImage(&cutImg1);
	dst = img;

	return res;
}

MRESULT FACE_OPERATION_ENGINE::FaceFeatureExtract(IplImage* src, ASF_MultiFaceInfo detectResult, ASF_FaceFeature *feature)
{
	ASF_SingleFaceInfo singleFace;
	singleFace.faceRect.left = detectResult.faceRect[0].left;
	singleFace.faceRect.right = detectResult.faceRect[0].right;
	singleFace.faceRect.top = detectResult.faceRect[0].top;
	singleFace.faceRect.bottom = detectResult.faceRect[0].bottom;
	singleFace.faceOrient = detectResult.faceOrient[0];

	IplImage* cutImg1 = cvCreateImage(cvSize(src->width - src->width % 4, src->height), IPL_DEPTH_8U, src->nChannels);
	CutIplImage(src, cutImg1, 0, 0);
	MRESULT res = ASFFaceFeatureExtract(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		(MUInt8*)cutImg1->imageData, &singleFace, feature);
	cvReleaseImage(&cutImg1);
	return res;
}

//MRESULT FACE_OPERATION_ENGINE::FaceCompareTask(AllFacesFeature *faces_feature, std::vector<FacesFeatureUrl>* memery_user_feature_)
//{
//	int res = 0;
//	for (int i = 0; i < faces_feature->user_num; i++)
//	{
//		std::vector<FacesFeatureUrl>::iterator it = memery_user_feature_->begin();
//		float max_confidence = 0;
//		int   user_id = 0;
//		while (memery_user_feature_->end() != it)
//		{
//			float confidence = 0;
//			ASF_FaceFeature asf_feature;
//			asf_feature.feature = it->feature;
//			asf_feature.featureSize = it->feature_size;
//			res = ASFFaceFeatureCompare(engineHandle, &faces_feature->face_feature[i], &asf_feature, &confidence);
//			if (max_confidence < confidence)
//			{
//				max_confidence = confidence;
//				user_id = it->user_id;
//			}
//			it++;
//		}
//		faceRecognitionResult[max_confidence] = user_id;
//	}
//	//int res = ASFFaceFeatureCompare(engineHandle, &feature1, &feature2, confidenceValue);
//	return res;
//
//}

MRESULT FACE_OPERATION_ENGINE::FaceCompareTask(ASF_FaceFeature feature1, ASF_FaceFeature feature2, MFloat* confidenceValue)
{
	int res = ASFFaceFeatureCompare(engineHandle, &feature1, &feature2, confidenceValue);
	return res;
}

MRESULT FACE_OPERATION_ENGINE::FaceFeatureCompare(ASF_FaceFeature feature, MFloat * confidenceValue)
{
	return MRESULT();
}

//MRESULT FACE_OPERATION_ENGINE::FaceRecognition(ASF_FaceFeature &feature, int *personId)
//{
//	///
//	std::map<int, UserFaceFeature>::iterator it = recg_db_index_->memery_feature_.begin();
//	for (; it != recg_db_index_->memery_feature_.end(); ++it)
//	{
//		MFloat confidenceValue;
//		ASF_FaceFeature feature2;
//		feature2.featureSize = it->second.featureLen;
//		feature2.feature = (MByte*)malloc(feature2.featureSize);
//		memcpy(feature2.feature, it->second.feature, it->second.featureLen);
//		FaceCompareTask(feature, feature2, &confidenceValue);
//		faceRecognitionResult[confidenceValue] = it->first;
//		free(feature2.feature);
//	}
//	std::map<MFloat, int>::reverse_iterator iter = faceRecognitionResult.rbegin();
//	*personId = iter->second;
//	//printf("Person id:%d,Recognition confidence:%.3f\n", iter->second, iter->first);
//
//	for(;iter!= faceRecognitionResult.rend();++iter)
//		printf("Person id:%d,Recognition confidence:%.3f\n", iter->second, iter->first);
//
//	return FACE_OPERATION_OK;
//}

MRESULT FACE_OPERATION_ENGINE::FaceRecognition(IplImage* src, MULTI_RECOGNITION_RESULT *userInfor)
{
	if (src == NULL)
		return MERR_FSDK_FR_INVALID_IMAGE_INFO;
	cv::Mat img(src, true);

	IplImage* cutImg1 = cvCreateImage(cvSize(src->width - src->width % 4, src->height), IPL_DEPTH_8U, src->nChannels);
	CutIplImage(src, cutImg1, 0, 0);

	ASF_MultiFaceInfo detectResult;
	MRESULT res = ASFDetectFaces(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
		(MUInt8*)cutImg1->imageData, &detectResult);
	if (res != MOK)
	{
		printf("ASFDetectFaces failed:%d\n", res);
		return res;
	}

	for (int i = 0; i < detectResult.faceNum; ++i)
	{
		ASF_SingleFaceInfo singleFace;
		singleFace.faceRect.left = detectResult.faceRect[i].left;
		singleFace.faceRect.right = detectResult.faceRect[i].right;
		singleFace.faceRect.top = detectResult.faceRect[i].top;
		singleFace.faceRect.bottom = detectResult.faceRect[i].bottom;
		singleFace.faceOrient = detectResult.faceOrient[i];

		ASF_FaceFeature feature;
		MRESULT res = ASFFaceFeatureExtract(engineHandle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8,
			(MUInt8*)cutImg1->imageData, &singleFace, &feature);
		if (res != MOK)
		{
			printf("extract feature failed!\n");
		}
		else
		{
			int personId;
//			res = FaceRecognition(feature, &personId);
			if (res != MOK)
			{
				printf("the person is not in library!\n");
			}
			else
			{
				userInfor->userId = (int *)malloc(detectResult.faceNum);
				userInfor->userId[i] = personId;
				userInfor->userNum = detectResult.faceNum;
			}
		}
	}
	cvReleaseImage(&cutImg1);

	return FACE_OPERATION_OK;
}

FACE_OPERATION_ENGINE * FACE_OPERATION_ENGINE::GetInstance()
{
    if (face_recg_Handle == NULL)
    {
        FaceEngineInitParams face_engine_init_params;
        face_engine_init_params.detectMode = ASF_DETECT_MODE_IMAGE;
        face_engine_init_params.faceMaxNum = 40;
        face_engine_init_params.faceOrient = ASF_OP_0_ONLY;
        face_engine_init_params.faceScaleVal = 32;
        face_engine_init_params.functionMask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE |
            ASF_LIVENESS | ASF_IR_LIVENESS;

        face_recg_Handle = new FACE_OPERATION_ENGINE(face_engine_init_params);
    }

    return face_recg_Handle;
}

FACE_OPERATION_ENGINE::~FACE_OPERATION_ENGINE()
{
	ASFUninitEngine(engineHandle);
	//delete recg_db_index_;
}

