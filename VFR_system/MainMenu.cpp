#include "MainMenu.h"
#include <QPushButton>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>

static QImage video_image[CAMERA_NUM_LIMIT];
unsigned char *video_data[CAMERA_NUM_LIMIT];
int video_data_size[CAMERA_NUM_LIMIT] = { 0 };
int video_chnId[CAMERA_NUM_LIMIT] = { 0 };

bool YUV420ToBGR24(unsigned char* pY, unsigned char* pU, unsigned char* pV, unsigned char* pRGB24, int width, int height)
{
	int yIdx, uIdx, vIdx, idx;
	int offset = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			yIdx = i * width + j;
			vIdx = (i / 4)* width + j / 2;
			uIdx = (i / 4)* width + j / 2;

			int R = (pY[yIdx] - 16) + 1.370805 * (pV[uIdx] - 128);                                                     // r分量	
			int G = (pY[yIdx] - 16) - 0.69825 * (pV[uIdx] - 128) - 0.33557 * (pU[vIdx] - 128);                         // g分量
			int B = (pY[yIdx] - 16) + 1.733221 * (pU[vIdx] - 128);                                                     // b分量

			R = R < 255 ? R : 255;
			G = G < 255 ? G : 255;
			B = B < 255 ? B : 255;

			R = R < 0 ? 0 : R;
			G = G < 0 ? 0 : G;
			B = B < 0 ? 0 : B;

			pRGB24[offset++] = (unsigned char)R;
			pRGB24[offset++] = (unsigned char)G;
			pRGB24[offset++] = (unsigned char)B;

		}
	}
	return true;
}

void callBack(VzLPRClientHandle handle, void *pUserData, const VZ_LPRC_IMAGE_INFO *pFrame)
{
	qDebug() << "height:" << pFrame->uHeight << "width:" << pFrame->uWidth << "format:" << pFrame->uPixFmt << "size:" << sizeof(pFrame->pBuffer) << "line size:" << pFrame->uPitch;
	//unsigned char
}

void VideoFrameCallBack(VzLPRClientHandle handle, void *pUserData, const VzYUV420P *pFrame)
{
	try
	{
		int* chnnal_id = (int *)pUserData;

		if (video_data[*chnnal_id] == NULL)
		{
			qDebug() << "malloc ";
			video_data_size[*chnnal_id] = pFrame->height*pFrame->width * 3;
			video_data[*chnnal_id] = (unsigned char*)malloc(video_data_size[*chnnal_id]);
			memset(video_data[*chnnal_id], 0, video_data_size[*chnnal_id]);
		}
		else
		{
			if (video_data_size[*chnnal_id] != pFrame->height*pFrame->width * 3)
			{
				qDebug() << "----->" << "image data size have changed. chnnal ID:" << *chnnal_id;
				free(video_data[*chnnal_id]);
				video_data_size[*chnnal_id] = pFrame->height*pFrame->width * 3;
				video_data[*chnnal_id] = (unsigned char*)malloc(video_data_size[*chnnal_id]);
				memset(video_data[*chnnal_id], 0, video_data_size[*chnnal_id]);
			}
		}

		//qDebug() << "chnnid:" << *chnnal_id;
		if (YUV420ToBGR24(pFrame->pY, pFrame->pU, pFrame->pV, video_data[*chnnal_id], pFrame->width, pFrame->height))
		{
			//qDebug() << "Ysize:" << pFrame->widthStepY << "Usize:" << pFrame->widthStepU << "Vsize:" << pFrame->widthStepV << "height:" << pFrame->height << "width:" << pFrame->width;
			video_image[*chnnal_id] = QImage(video_data[*chnnal_id], pFrame->width, pFrame->height, pFrame->width * 3, QImage::Format_RGB888);
		}
	}
	catch (...)
	{
		qDebug() << "display video error " ;
	}
}

//void VideoFrameCallBack(VzLPRClientHandle handle, void * pUserData, const VZ_LPRC_IMAGE_INFO * pFrame)
//{
//	if (video_data == NULL)
//	{
//		video_data_size = pFrame->uPitch*pFrame->uHeight;
//		video_data = (unsigned char*)malloc(video_data_size);
//		memset(video_data, 0, video_data_size);
//	}
//	else
//	{
//		if (video_data_size != pFrame->uPitch*pFrame->uHeight)
//		{
//			qDebug() << "----->" << "image data size have changed";
//			free(video_data);
//			video_data_size = pFrame->uPitch*pFrame->uHeight;
//			video_data = (unsigned char*)malloc(video_data_size);
//			memset(video_data, 0, video_data_size);
//		}
//	}
//
//	memcpy(video_data, pFrame->pBuffer, video_data_size);
//	video_image = QImage(video_data, pFrame->uWidth, pFrame->uHeight, pFrame->uPitch, QImage::Format_BGR888);
//
//
//	if (!video_image.isNull())
//		qDebug() << "image data:" << video_data << "   image data size:" << sizeof(video_data);
//		//qDebug() << "height:" << pFrame->uHeight << "width:" << pFrame->uWidth << "format:" << pFrame->uPixFmt << "size:" << sizeof(pFrame->pBuffer) << "line size:" << pFrame->uPitch;
//
//}

MainMenu::MainMenu(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	videoTimer = NULL;
	vzbox_online_status = false;
	display_video_windows_num_ = FOUR_WINDOWS;
	video_display_label = NULL;

	ui.toolButton_oneWindow->setEnabled(true);
	ui.toolButton_fourWindows->setEnabled(false);
	ui.toolButton_nineWindows->setEnabled(true);

    //初始化显示图标
    SetIconInit();

	//初始化显示视频窗口label
	video_display_label = new DisplayVideoLabel[CAMERA_NUM_LIMIT];
	if (video_display_label == NULL)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("视频显示控件初始化失败！"));
		return;
	}
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		video_display_label[i].setParent(ui.widget_video_window);
		video_display_label[i].setChannelId(i);
		connect(&video_display_label[i], &DisplayVideoLabel::singleClickedMouse, this, &MainMenu::DealSingleClickedVideoLabel);
		connect(&video_display_label[i], &DisplayVideoLabel::doubleClickedMouse, this, &MainMenu::DealDoubleClickedVideoLabel);
	}

	//打开设备按钮
	connect(ui.pushButton_openDev, &QPushButton::clicked, this, &MainMenu::DealOpenVzbox);

	//关闭设备按钮
	connect(ui.pushButton_closeDev, &QPushButton::clicked, this, &MainMenu::DealCloseVzbox);

	//显示视频窗口
	RefreshVideoDisplayWindow();
	//修改窗口显示视频的个数   
	connect(ui.toolButton_oneWindow, &QPushButton::clicked, [=]() {
		display_video_windows_num_ = ONE_WINDOWS;
		RefreshVideoDisplayWindow();
		ui.toolButton_oneWindow->setEnabled(false);
		ui.toolButton_fourWindows->setEnabled(true);
		ui.toolButton_nineWindows->setEnabled(true);
	});
	connect(ui.toolButton_fourWindows, &QPushButton::clicked, [=]() {
		display_video_windows_num_ = FOUR_WINDOWS;
		RefreshVideoDisplayWindow();
		ui.toolButton_oneWindow->setEnabled(true);
		ui.toolButton_fourWindows->setEnabled(false);
		ui.toolButton_nineWindows->setEnabled(true);
	});
	connect(ui.toolButton_nineWindows, &QPushButton::clicked, [=]() {
		display_video_windows_num_ = NINE_WINDOWS;
		RefreshVideoDisplayWindow();
		ui.toolButton_oneWindow->setEnabled(true);
		ui.toolButton_fourWindows->setEnabled(true);
		ui.toolButton_nineWindows->setEnabled(false);
	});

	//一键播放全部视频
	connect(ui.pushButton_allVideoPlay, &QPushButton::clicked, [=]() {
		try 
		{
			DealAutoPlayAllVideo();
			video_show_timer_.start(50);
		}
		catch (...)
		{
			qDebug() << "play failed ";
		}
	});

	//播放视频（定时器）
	connect(&video_show_timer_, &QTimer::timeout, [=]() {
		for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
		{
			if (!video_image[i].isNull())
			{
				QImage img = video_image[i].scaled(video_display_label[i].width(), video_display_label[i].height());
				video_display_label[i].setPixmap(QPixmap::fromImage(img));
			}
		}
	});

    //通过按钮切换系统功能
    connect(ui.pushButton_onlineMonitoring, &QPushButton::clicked, [=]() {
        ChangeSystemMode(0);
        RefreshVideoDisplayWindow();
    });

    connect(ui.pushButton_cameraManage, &QPushButton::clicked, [=]() {
        ChangeSystemMode(1);
    });

    connect(ui.pushButton_libraryManage, &QPushButton::clicked, [=]() {
        ChangeSystemMode(2);
    });

    connect(ui.pushButton_snapResult, &QPushButton::clicked, [=]() {
        ChangeSystemMode(3);
    });

    connect(ui.pushButton_trackPath, &QPushButton::clicked, [=]() {
        ChangeSystemMode(4);
    });

    connect(ui.pushButton_smartTest, &QPushButton::clicked, [=]() {
        ChangeSystemMode(5);
    });

}

MainMenu::~MainMenu()
{
	free(video_data);
	VzLPRClient_Close(vzbox_handle_);
	//VzLPRClient_Close(camera_handle_);
	VzLPRClient_Cleanup();
	if (video_display_label != NULL)
	{
		delete[]video_display_label;
		video_display_label = NULL;
	}
}

//设置按钮图标
void MainMenu::SetIconInit()
{
    ui.toolButton_addCamera->setIcon(QIcon("./icon/add.jpg"));
    ui.toolButton_delCamera->setIcon(QIcon("./icon/del.jpg"));
    ui.toolButton_editCamera->setIcon(QIcon("./icon/edit.jpg"));
    ui.toolButton_refreshCamera->setIcon(QIcon("./icon/refresh.jpg"));

    ui.toolButton_addLibrary->setIcon(QIcon("./icon/add.jpg"));
    ui.toolButton_delLibrary->setIcon(QIcon("./icon/del.jpg"));
    ui.toolButton_editLibrary->setIcon(QIcon("./icon/edit.jpg"));
    ui.toolButton_refreshLibrary->setIcon(QIcon("./icon/refresh.jpg"));
}

//打开设备
void MainMenu::DealOpenVzbox()
{
	if (vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("设备已打开！"));
		return;
	}
	vzbox_ip_ = ui.lineEdit_devIp->text();
	vzbox_port_ = ui.lineEdit_devPort->text();
	vzbox_user_name_ = ui.lineEdit_userName->text();
	vzbox_password_ = ui.lineEdit_password->text();
	if (vzbox_ip_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("IP不允许为空！"));
		return;
	}

	if (vzbox_port_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("端口号不允许为空！"));
		return;
	}

	if (vzbox_user_name_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("用户名不允许为空！"));
		return;
	}

	if (vzbox_password_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("密码不允许为空！"));
		return;
	}

	int res = VzLPRClient_Setup();
	if (res != 0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("设备初始化失败！"));
		return;
	}

	vzbox_handle_ = VzLPRClient_Open(vzbox_ip_.toUtf8(),
		vzbox_port_.toInt(),
		vzbox_user_name_.toUtf8(),
		vzbox_password_.toUtf8());
	if (vzbox_handle_ == 0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("打开设备失败！"));
		return;
	}

	vzbox_online_status = true;
	ui.lineEdit_devIp->setEnabled(false);
	ui.lineEdit_devPort->setEnabled(false);
	ui.lineEdit_password->setEnabled(false);
	ui.lineEdit_userName->setEnabled(false);

	RefreshCameraList();
	RefreshVideoDisplayWindow();
}

//关闭设备
void MainMenu::DealCloseVzbox()
{
	if (vzbox_handle_ == 0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("没有打开的设备！"));
		return;
	}
	VzLPRClient_Close(vzbox_handle_);
	vzbox_handle_ = 0;

	vzbox_online_status = false;
	ui.lineEdit_devIp->setEnabled(true);
	ui.lineEdit_devPort->setEnabled(true);
	ui.lineEdit_password->setEnabled(true);
	ui.lineEdit_userName->setEnabled(true);

	ui.listWidget_CameraList->clear();
	camera_list_buff.clear();
	RefreshVideoDisplayWindow();
}

//刷新相机列表
void MainMenu::RefreshCameraList()
{
	VZ_BOX_CAM_GROUP camera_list;
	int ret = VzClient_GetCamGroupParam(vzbox_handle_, &camera_list);
	if (ret != VZSDK_SUCCESS)
	{
		qDebug() << QString::fromLocal8Bit("获取相机列表失败");
		return;
	}

	qDebug() << camera_list.cam_count;
	camera_list_buff.clear();
	ui.listWidget_CameraList->clear();

	for (int i = 0; i < camera_list.cam_count; ++i)
	{
		qDebug() << "list:" << camera_list.cam_items[i].ip;
		camera_list_buff.push_back(QString(camera_list.cam_items[i].ip));
		ui.listWidget_CameraList->addItem(camera_list_buff[i]);
	}	
}

//刷新视频显示窗口（显示样式）
void MainMenu::RefreshVideoDisplayWindow()
{
	CleanAllDisplayWindows();
	switch (display_video_windows_num_)
	{
		case ONE_WINDOWS:  OneWindowsDisplay();  break;
		case FOUR_WINDOWS: FourWindowsDisplay(); break;
		case NINE_WINDOWS: NineWindowsDisplay(); break;
	}
}

//刷新视频显示窗口样式（单击触发）
void MainMenu::RefreshVideoDisplayStyle()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].setStyleSheet(DISPLAY_LABEL_STYLE);
	}
}

//清除所有视频显示
void MainMenu::CleanAllDisplayWindows()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].hide();
	}
}

//单窗口显示视频模式（仅显示第1路）
void MainMenu::OneWindowsDisplay()
{
	int row_size = (ui.widget_video_window->height() - ONE_WINDOWS - 1) / ONE_WINDOWS;
	int col_size = (ui.widget_video_window->width()  - ONE_WINDOWS - 1) / ONE_WINDOWS;

	QRect rect(1, 1, col_size, row_size);
	video_display_label[0].setText(QString("video %1").arg(1));
	video_display_label[0].setStyleSheet(DISPLAY_LABEL_STYLE);
	video_display_label[0].setGeometry(rect);
	video_display_label[0].show();
}

//四窗口显示视频模式（显示前4路）
void MainMenu::FourWindowsDisplay()
{
	qDebug() << "window x:" << ui.widget_video_window->width() << "y:" << ui.widget_video_window->height();
	int row_size = (ui.widget_video_window->height() - FOUR_WINDOWS - 1) / FOUR_WINDOWS;
	int col_size = (ui.widget_video_window->width()  - FOUR_WINDOWS - 1) / FOUR_WINDOWS;

	int  video_num = 0;
	for (int i = 0; i < FOUR_WINDOWS; i++)
	{
		for (int j = 0; j < FOUR_WINDOWS; j++)
		{
			QRect rect(col_size * j + j, row_size * i + i, col_size, row_size);
			video_display_label[video_num].setText(QString("video %1").arg(video_num + 1));
			video_display_label[video_num].setStyleSheet(DISPLAY_LABEL_STYLE);
			video_display_label[video_num].setGeometry(rect);
			video_display_label[video_num].show();
			video_num++;
		}
	}
	qDebug() << "windows num:" << display_video_windows_num_ << "every video windows width:"
		<< col_size << "height:"
		<< row_size;
}

//九窗口显示视频模式（显示前9路）
void MainMenu::NineWindowsDisplay()
{
	int row_size = (ui.widget_video_window->height() - NINE_WINDOWS - 1) / NINE_WINDOWS;
	int col_size = (ui.widget_video_window->width()  - NINE_WINDOWS - 1) / NINE_WINDOWS;

	int  video_num = 0;
	for (int i = 0; i < NINE_WINDOWS; i++)
	{
		for (int j = 0; j < NINE_WINDOWS; j++)
		{
			QRect rect(col_size * j + j, row_size * i + i, col_size, row_size);
			
			video_display_label[video_num].setText(QString("video %1").arg(video_num + 1));
			video_display_label[video_num].setStyleSheet(DISPLAY_LABEL_STYLE);
			video_display_label[video_num].setGeometry(rect);
			video_display_label[video_num].show();
			video_num++;
		}
	}
	qDebug() << "windows num:" << display_video_windows_num_ << "every video windows width:"
		<< col_size << "height:"
		<< row_size;
}

//显示一路视频（由chnId指定）
void MainMenu::ShowOneChnVideo(int chnId)
{
	static int change = 0;
	if (change % 2 == 0)
	{
		int row_size = (ui.widget_video_window->height() - 2);
		int col_size = (ui.widget_video_window->width() - 2);

		CleanAllDisplayWindows();
		QRect rect(1, 1, col_size, row_size);
		video_display_label[chnId].setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		video_display_label[chnId].setText(QString("video %1").arg(chnId + 1));
		video_display_label[chnId].setStyleSheet(DISPLAY_LABEL_STYLE);
		video_display_label[chnId].setGeometry(rect);
		video_display_label[chnId].show();
	}
	else
	{
		RefreshVideoDisplayWindow();
	}
	change++;
}

//改变一路视频窗口样式（由chnId指定）
void MainMenu::ChangeOneVideoStyle(int chnId)
{
	RefreshVideoDisplayStyle();
	video_display_label[chnId].setStyleSheet(ClICKED_LABEL_STYLE);
}

//相机视频回调显示
//void MainMenu::VideoFrameCallBack(VzLPRClientHandle handle, void * pUserData, const VzYUV420P * pFrame)
//{
//	if (video_data == NULL)
//	{
//		qDebug() << "malloc ";
//		video_data_size = pFrame->height*pFrame->width * 3;
//		video_data = (unsigned char*)malloc(video_data_size);
//		memset(video_data, 0, video_data_size);
//	}
//	else
//	{
//		if (video_data_size != pFrame->height*pFrame->width * 3)
//		{
//			qDebug() << "----->" << "image data size have changed";
//			free(video_data);
//			video_data_size = pFrame->height*pFrame->width * 3;
//			video_data = (unsigned char*)malloc(video_data_size);
//			memset(video_data, 0, video_data_size);
//		}
//	}
//
//	int* chnnal_id = (int *)pUserData;
//	qDebug() << "chnnid:" << *chnnal_id;
//	//if (YUV420ToBGR24(pFrame->pY, pFrame->pU, pFrame->pV, video_data, pFrame->width, pFrame->height))
//	//{
//	//	//qDebug() << "Ysize:" << pFrame->widthStepY << "Usize:" << pFrame->widthStepU << "Vsize:" << pFrame->widthStepV << "height:" << pFrame->height << "width:" << pFrame->width;
//	//	video_image = QImage(video_data, pFrame->width, pFrame->height, pFrame->width * 3, QImage::Format_RGB888);
//	//}
//}

//切换系统功能模式（通过索引切换）
void MainMenu::ChangeSystemMode(int index)
{
    ui.stackedWidget_systemMode->setCurrentIndex(index);
}

void MainMenu::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load("./icon/MainBackGround.jpg");
    //指定长宽
    painter.drawPixmap(0, 0, this->width(), this->height(), pix);
}

//改变大小时刷新视频窗口
void MainMenu::resizeEvent(QResizeEvent * event)
{
	RefreshVideoDisplayWindow();
}

//处理鼠标单击视频窗口
void MainMenu::DealSingleClickedVideoLabel(int chn)
{
	qDebug() << "mouse single clicked video" << chn;
	ChangeOneVideoStyle(chn);
}

//处理鼠标双击视频窗口
void MainMenu::DealDoubleClickedVideoLabel(int chn)
{
	qDebug() << "mouse double clicked video" << chn;
	ShowOneChnVideo(chn);
}

//一键播放全部视频
void MainMenu::DealAutoPlayAllVideo()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_chnId[i] = i;
	}

	int video_num = 0;
	QVector<QString>::iterator it = camera_list_buff.begin();
	while (it != camera_list_buff.end() && video_num < CAMERA_NUM_LIMIT)
	{
		camera_handle_[video_num] = VzLPRClient_Open((*it).toUtf8(),80,"admin","admin");
		if (camera_handle_[video_num] == 0)
		{
			qDebug() << "open camera " << *it << "failed,video ID:" << video_num;
			it++;
			video_num++;
			continue;
		}

		qDebug() << "open camera " << *it << "success, video ID:" << video_num;
		//VzLPRClient_StartRealPlayByChannel_V2(camera_handle_[video_num],NULL,0,0,)
		//VZLPRC_VIDEO_FRAME_CALLBACK pFun_cb = (VZLPRC_VIDEO_FRAME_CALLBACK)&MainMenu::VideoFrameCallBack;
		VzLPRClient_SetVideoFrameCallBack(camera_handle_[video_num], ::VideoFrameCallBack, (void *)&video_chnId[video_num]);
		it++;
		video_num++;
	}
}






