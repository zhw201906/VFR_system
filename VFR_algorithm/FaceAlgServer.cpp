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
	if (p_face_alg_server != NULL)
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
	oper_status = OPER_NONE;
	p_SnapFaceCallBack = NULL;
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
	res = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_ALL_OUT, 16, 30, mask, &face_engine_handle);
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

	cv::Mat mat_src = src(cv::Rect(0, 0, src.cols - src.cols % 4, src.rows));
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
    static int person_count = 0;
    static int person_id    = -1;


    if (oper_status != OPER_NONE) 
    {    
        for (int i = 0; i < detect_result.faceNum; i++)
    	{
    	    int rect_x = detect_result.faceRect[i].left;
    	    int rect_y = detect_result.faceRect[i].top;
    	    int rect_w = detect_result.faceRect[i].right  - detect_result.faceRect[i].left;
    	    int rect_h = detect_result.faceRect[i].bottom - detect_result.faceRect[i].top;

    	    if ((oper_status & OPER_RECT) == OPER_RECT)
    	    {
                cv::rectangle(dst, 
    			    cv::Rect(rect_x, rect_y, rect_w, rect_h), 
    			    cv::Scalar(0, 255, 0), 2, 8, 0);
    	    }

            rect_x = (rect_x - SNAP_EXPAND_SIZE / 2) > 0 ? (rect_x - SNAP_EXPAND_SIZE / 2) : 0;
            rect_y = (rect_y - SNAP_EXPAND_SIZE / 2) > 0 ? (rect_y - SNAP_EXPAND_SIZE / 2) : 0;
            rect_w = (rect_x + rect_w + SNAP_EXPAND_SIZE) > mat_src.cols ? mat_src.cols - rect_x : rect_w + SNAP_EXPAND_SIZE;
            rect_h = (rect_y + rect_h + SNAP_EXPAND_SIZE) > mat_src.rows ? mat_src.rows - rect_y : rect_h + SNAP_EXPAND_SIZE;

            if (person_id < detect_result.faceID[i])
            {
                if ((oper_status & OPER_SAVE) == OPER_SAVE)
                {
                    AlgSaveSnapTask(mat_src, cv::Rect(rect_x, rect_y, rect_w, rect_h));
                    person_id = detect_result.faceID[i];
                }

        		if ((oper_status & OPER_SNAP) == OPER_SNAP)
                {
                    AlgSnapTask(mat_src, cv::Rect(rect_x, rect_y, rect_w, rect_h));
                    person_id = detect_result.faceID[i];
                }
            }
    	}
    }

	if ((oper_status & OPER_COUNT) == OPER_COUNT)
	{
        if (detect_result.faceNum > 0)
        {
            person_count = max(detect_result.faceID[detect_result.faceNum - 1] + 1, person_count);
        }

        char label_text[50] = { 0 };
		sprintf(label_text, "[%d/%d]", detect_result.faceNum, person_count);
		std::string label(label_text);
		cv::Point label_point(15, 30);
		CvScalar textColor = cvScalar(0, 0, 255);
		cv::putText(dst, label, label_point, 1, 1.5, textColor, 2);
	}

    cvReleaseImage(&img);
}

void  FaceAlgServer::SetFaceAlgMode(char state)
{
    oper_status = state;
}

void  FaceAlgServer::SetSnapFaceCallBack(pSnapFaceFun pfun, void *puser)
{
    p_SnapFaceCallBack = pfun;
    p_context = puser;
}

void FaceAlgServer::AlgSnapTask(const cv::Mat &img, cv::Rect &rect)
{
    if (p_SnapFaceCallBack != NULL)
    {
        p_SnapFaceCallBack(img(rect), p_context);
    }
}

void FaceAlgServer::AlgSaveSnapTask(const cv::Mat &img, cv::Rect &rect, std::string path)
{
    QDateTime cur_time = QDateTime::currentDateTime();
    qint64 epoch_time  = cur_time.toMSecsSinceEpoch();
    char img_name[100] = {0};
    sprintf(img_name, "/%lld.jpg", (long long)epoch_time);
    path.append(img_name);
    cv::imwrite(path, img(rect));
}


