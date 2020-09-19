#include "FaceAlgServer.h"

//对齐图片
static void CutIplImage(IplImage* src, IplImage* dst, int x, int y)
{
	CvSize size = cvSize(dst->width, dst->height);//区域大小
	cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
	cvCopy(src, dst); //复制图像
	cvResetImageROI(src);//源图像用完后，清空ROI
}


FaceAlgServer * FaceAlgServer::p_face_alg_server = NULL;

FaceAlgServer::~FaceAlgServer()
{
	if (p_face_alg_server == NULL)
	{
		delete p_face_alg_server;
	}
}

FaceAlgServer::FaceAlgServer()
{
	if (!CreatEngineInit())
	{
		qDebug() << "engine creat failed";
	}
}

FaceAlgServer *FaceAlgServer::Instance()
{
	if (p_face_alg_server == NULL)
	{
		p_face_alg_server = new FaceAlgServer;
	}

	return p_face_alg_server;
}

bool FaceAlgServer::CreatEngineInit()
{
	MRESULT res = MOK;
	ASF_ActiveFileInfo  activeFileInfo = { 0 };
	res = ASFGetActiveFileInfo(&activeFileInfo);
	if (res != MOK)
	{
		printf("ASFGetActiveFileInfo fail: %d\n", res);
	}
	const ASF_VERSION version = ASFGetVersion();
	//激活接口,首次激活需联网
	char appid[] = "dqKEuBzR3rHNBqaQGKzTNJHtA2QyCvS582ForZj1zLS";
	char key[] = "sxrnBPvm3Wp56bHoJLp7JDjZph1JvJeniJDkUaeuu7X";
	res = ASFOnlineActivation(appid, key);
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		qDebug() << "ASFOnlineActivation fail...";
	else
		qDebug() << "ASFOnlineActivation success...";


	//初始化接口
	/* | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS | ASF_IR_LIVENESS;*/
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION;
	res = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_ALL_OUT, 16, 5, mask, &face_engine_handle);
	if (res != MOK)
	{
		qDebug() << "ASFInitEngine fail...";
		return false;
	}
	else
		return true;
}

bool FaceAlgServer::isAlgReady() const
{
	if (face_engine_handle)
	{
		return true;
	}
	return false;
}

void  FaceAlgServer::DetectFaceAndRect(cv::Mat &src, cv::Mat &dst)
{
	if (!isAlgReady())
	{
		return;
	}

	//qDebug() << "width:" << src.cols << "  height:" << src.rows;

	cv::Mat mat_src = src(cv::Rect(0, 0, src.cols - src.cols % 4, src.rows)).clone();
	IplImage imgTmp = mat_src;
	IplImage *img = cvCloneImage(&imgTmp);

	ASVLOFFSCREEN img_data = { 0 };
	ASF_MultiFaceInfo detect_result = { 0 };
	img_data.i32Height = img->height;
	img_data.i32Width  = img->width;
	img_data.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	img_data.ppu8Plane[0] = (MUInt8*)img->imageData;
	img_data.pi32Pitch[0] = img->widthStep;
	
	MRESULT res = ASFDetectFacesEx(face_engine_handle, &img_data, &detect_result);
	if (res != MOK)
	{
		qDebug() << "ASFGetActiveFileInfo fail...";
	}

	src.copyTo(dst);
	for (int i = 0; i < detect_result.faceNum; i++)
	{
		cv::rectangle(dst, 
			cv::Rect(detect_result.faceRect[i].left, detect_result.faceRect[i].top, 
					 detect_result.faceRect[i].right- detect_result.faceRect[i].left, 
					 detect_result.faceRect[i].bottom- detect_result.faceRect[i].top), 
			cv::Scalar(0, 255, 0), 2, 8, 0);
	}
}



