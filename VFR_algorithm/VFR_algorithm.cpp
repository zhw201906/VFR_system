#include "VFR_algorithm.h"

static QQueue<cv::Mat>  before_mat_buff;
static QQueue<QImage>   after_img_buff;
static std::mutex g_alg_lock;

static void MatToQImage(const cv::Mat &frame, QImage &img)
{
    int chana = frame.channels();
    //依据通道数不同，改变不同的装换方式
    if (3 == chana) 
    {
        //调整通道次序       
        cv::cvtColor(frame, frame, CV_BGR2RGB);
        img = QImage(static_cast<uchar *>(frame.data), frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    }
    else if (4 == chana) 
    {
        //argb
        img = QImage(static_cast<uchar *>(frame.data), frame.cols, frame.rows, QImage::Format_ARGB32);
    }
    else 
    {
        //单通道，灰度图
        img = QImage(frame.cols, frame.rows, QImage::Format_Indexed8);
        uchar * matdata = frame.data;
        for (int row = 0; row < frame.rows; ++row) {
            uchar* rowdata = img.scanLine(row);
            memcpy(rowdata, matdata, frame.cols);
            matdata += frame.cols;
        }
    }
}

VFR_algorithm::VFR_algorithm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	is_recg = false;
	is_snap = false;
	is_save = false;
	alg_mode = 0;

    ui.listWidget_snap->setViewMode(QListView::IconMode);
    ui.listWidget_snap->setResizeMode(QListWidget::Adjust);
    ui.listWidget_snap->setMovement(QListWidget::Static);
    ui.listWidget_snap->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui.listWidget_snap->setIconSize(SNAP_FACE_SHOW_SIZE);
    ui.listWidget_snap->setSpacing(10);

	connect(ui.pushButton_selectVideo, &QPushButton::clicked, this, &VFR_algorithm::SlotSelectVideo);
	connect(ui.pushButton_openCamera, &QPushButton::clicked, this, &VFR_algorithm::SlotOpenCamera);
	connect(ui.checkBox_snap,  &QCheckBox::stateChanged, this, &VFR_algorithm::SlotOpenSnap);
	connect(ui.checkBox_rect,  &QCheckBox::stateChanged, this, &VFR_algorithm::SlotShowRecg);
	connect(ui.checkBox_count, &QCheckBox::stateChanged, this, &VFR_algorithm::SlotAreaCount);
	connect(ui.checkBox_saveSnap, &QCheckBox::stateChanged, this, &VFR_algorithm::SlotSaveSnap);
	connect(&video_play_timer, &QTimer::timeout, this, &VFR_algorithm::SlotPlayVideo);
    connect(ui.pushButton_stopVideo, &QPushButton::clicked, [=]() {
        if (video_cap.isOpened())
        {
            video_cap.release();
            video_play_timer.stop();
            ui.label_video->clear();
            ui.textBrowser_log->append(QString::fromLocal8Bit("已停止播放..."));
        }
    });
    connect(ui.pushButton_cleanLog,&QPushButton::clicked,[=](){
        ui.textBrowser_log->clear();
        ui.listWidget_snap->clear();
    });
}

void VFR_algorithm::SlotSelectVideo()
{
	QString video_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频"), VIDEO_DIR,
		tr("video (*.avi *.mp4);; All files (*.*)"));

	if (video_path.isEmpty())
	{
		return;
	}

	ui.textBrowser_log->append(QString::fromLocal8Bit("视频路径:%1").arg(video_path));
	ui.textBrowser_log->append(QString::fromLocal8Bit("开始播放..."));

	video_cap.open(video_path.toStdString());
	video_play_timer.start((int)(1000.0 / VIDEO_FRAME_RATE));
}

void VFR_algorithm::SlotOpenCamera()
{
	video_play_timer.stop();
	video_cap.open(0);
	if (!video_cap.isOpened())
	{
		ui.textBrowser_log->append(QString::fromLocal8Bit("打开摄像头失败"));
		return;
	}
	ui.textBrowser_log->append(QString::fromLocal8Bit("摄像头开启成功..."));
	video_play_timer.start((int)(1000.0 / VIDEO_FRAME_RATE));
}

void VFR_algorithm::SlotPlayVideo()
{	
	cv::Mat in_mat;	//用于储存每一帧的图像
	video_cap >> in_mat;//读取当前帧

	if (in_mat.empty())
	{
		video_play_timer.stop();
		ui.label_video->clear();
		ui.textBrowser_log->append(QString::fromLocal8Bit("播放完毕..."));
		return;
	}

	cv::Mat frame;
    QImage qimg;
    FaceAlgServer::Instance()->DetectFaceAndRect(in_mat, frame);
    MatToQImage(frame, qimg);

    //cv::Mat test_mat = in_mat(cv::Rect(0, 0, 480, 240));
    //QImage test_img;
    //MatToQImage(test_mat, test_img);
    //test_img.save(QString("../SnapFace/small_img.jpg"));
    /*
	if (is_recg)
	{
        FaceAlgServer::Instance()->DetectFaceAndRect(in_mat, frame);
        MatToQImage(frame, qimg);
        //{
        //    g_alg_lock.lock();
        //    before_mat_buff.push_back(in_mat);
        //    g_alg_lock.unlock();
        //}

        //if(!after_img_buff.empty())
        //{
        //    g_alg_lock.lock();
        //    qimg = after_img_buff.front();
        //    after_img_buff.pop_front();
        //    g_alg_lock.unlock();
        //}
	}
	else
	{
		in_mat.copyTo(frame);
        MatToQImage(frame, qimg);
	}
    */
    if(!qimg.isNull())
        ui.label_video->setPixmap(QPixmap::fromImage(qimg.scaled(ui.label_video->width(), ui.label_video->height())));
}

void VFR_algorithm::SlotShowRecg(int state)
{
	if (state == Qt::Checked)
	{
        ui.textBrowser_log->append(QString::fromLocal8Bit("开启绘制人脸框..."));
		is_recg = true;
		alg_mode |= OPER_RECT;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
        //std::thread  alg_thread(VFR_algorithm::DealFaceAlgThread);
        //alg_thread.detach();
	}
	else
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("关闭绘制人脸框..."));
		is_recg = false;
		alg_mode &= ~OPER_RECT;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
	}
}

void VFR_algorithm::SlotOpenSnap(int state)
{
	if (state == Qt::Checked)
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("开启抓拍..."));
		is_snap = true;
		alg_mode |= OPER_SNAP;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
	    FaceAlgServer::Instance()->SetSnapFaceCallBack(VFR_algorithm::DealSnapCallBack, (void *)this);
	}
	else
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("关闭抓拍..."));
		is_snap = false;
		alg_mode &= ~OPER_SNAP;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
		FaceAlgServer::Instance()->SetSnapFaceCallBack(NULL, NULL);
	}
}

void VFR_algorithm::SlotSaveSnap(int state)
{
	if (state == Qt::Checked)
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("开启保存抓拍图..."));
		is_save = true;
		alg_mode |= OPER_SAVE;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
	}
	else
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("关闭保存抓拍图..."));
		is_save = false;
		alg_mode &= ~OPER_SAVE;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
	}
}

void VFR_algorithm::SlotAreaCount(int state)
{
	if (state == Qt::Checked)
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("开启区域计数..."));
		is_count = true;
		alg_mode |= OPER_COUNT;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
	}
	else
	{
	    ui.textBrowser_log->append(QString::fromLocal8Bit("关闭区域计数..."));
		is_count = false;
		alg_mode &= ~OPER_COUNT;
		FaceAlgServer::Instance()->SetFaceAlgMode(alg_mode);
	}
}


void VFR_algorithm::DealFaceAlgThread()
{
    while (true)
    {
        while (!before_mat_buff.isEmpty())
        {
            //qDebug() << "alg task is running...";
            cv::Mat img;
            {
                g_alg_lock.lock();
                before_mat_buff.front().copyTo(img);
                before_mat_buff.pop_front();
                g_alg_lock.unlock();
            }

            cv::Mat frame;
            FaceAlgServer::Instance()->DetectFaceAndRect(img, frame);
          
            {
                QImage qimg;
                MatToQImage(frame, qimg);
                qDebug() << "------------dst img width:" << qimg.width() << "   height:" << qimg.height();
                g_alg_lock.lock();
                after_img_buff.push_back(qimg);
                g_alg_lock.unlock();
            }
        }
        Sleep(2000);
        //qDebug() << "alg thread is running...";
    }
}

void VFR_algorithm::DealSnapCallBack(const cv::Mat &mtx, void *puser)
{
    VFR_algorithm *p_alg = (VFR_algorithm *)puser;
   
    p_alg->ui.textBrowser_log->append(QString::fromLocal8Bit("抓拍成功..."));

    //const char *img_name = "../SnapFace/call_back_img.jpg";
    //cv::imwrite(img_name, img);

    //QImage tmp_img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.step, QImage::Format_RGB888);
    //QImage qimg = tmp_img.rgbSwapped();

    QImage qimg;
    MatToQImage(mtx, qimg);

    //qimg.save(QString("../SnapFace/small_img.jpg"));


    //QString img_path = QString::fromStdString(img_name);
    //QImage qimg;
    //qimg.load(img_path);
    //p_alg->ui.label_video->setPixmap(QPixmap::fromImage(qimg));

    //QLabel *snap_label = new QLabel;
    //snap_label->setFixedSize(QSize(75, 100));
    //snap_label->setPixmap(QPixmap::fromImage(qimg.scaled(snap_label->width(), snap_label->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    QListWidgetItem *snap_item = new QListWidgetItem;
    snap_item->setIcon(QIcon(QPixmap::fromImage(qimg.scaled(SNAP_FACE_SHOW_SIZE))));
    snap_item->setSizeHint(SNAP_FACE_SHOW_SIZE);
    p_alg->ui.listWidget_snap->addItem(snap_item);
    //p_alg->ui.listWidget_snap->show();
    //p_alg->ui.listWidget_snap->setItemWidget(snap_item, snap_label);
}



