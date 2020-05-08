#include "MainMenu.h"
#include <QPushButton>
#include <QDebug>
#include <QImage>
#include <QPixmap>

static QImage video_image;
unsigned char *video_data;
int video_data_size = 0;

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
	if (video_data == NULL)
	{
		qDebug() << "malloc ";
		video_data_size = pFrame->height*pFrame->width * 3;
		video_data = (unsigned char*)malloc(video_data_size);
		memset(video_data, 0, video_data_size);
	}
	else
	{
		if (video_data_size != pFrame->height*pFrame->width * 3)
		{
			qDebug() << "----->" << "image data size have changed";
			free(video_data);
			video_data_size = pFrame->height*pFrame->width * 3;
			video_data = (unsigned char*)malloc(video_data_size);
			memset(video_data, 0, video_data_size);
		}
	}

	if (YUV420ToBGR24(pFrame->pY, pFrame->pU, pFrame->pV, video_data, pFrame->width, pFrame->height))
	{
		//qDebug() << "Ysize:" << pFrame->widthStepY << "Usize:" << pFrame->widthStepU << "Vsize:" << pFrame->widthStepV << "height:" << pFrame->height << "width:" << pFrame->width;
		video_image = QImage(video_data, pFrame->width, pFrame->height, pFrame->width * 3, QImage::Format_RGB888);
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

	//初始化显示视频窗口label
	video_display_label = new QLabel[CAMERA_NUM_LIMIT];
	if (video_display_label == NULL)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("视频显示控件初始化失败！"));
		return;
	}
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].setParent(ui.widget_video_window);
	}

	//打开设备按钮
	connect(ui.pushButton_openDev, &QPushButton::clicked, this, &MainMenu::DealOpenVzbox);

	//关闭设备按钮
	connect(ui.pushButton_closeDev, &QPushButton::clicked, this, &MainMenu::DealCloseVzbox);

	//显示视频窗口
	//RefreshVideoDisplayWindow();
	//修改窗口显示视频的个数   
	connect(ui.toolButton_oneWindow, &QPushButton::clicked, [=]() {
		display_video_windows_num_ = ONE_WINDOWS;
		//ui.toolButton_oneWindow
		RefreshVideoDisplayWindow();
	});
	connect(ui.toolButton_fourWindows, &QPushButton::clicked, [=]() {
		display_video_windows_num_ = FOUR_WINDOWS;
		//ui.toolButton_oneWindow
		RefreshVideoDisplayWindow();
	});
	connect(ui.toolButton_nineWindows, &QPushButton::clicked, [=]() {
		display_video_windows_num_ = NINE_WINDOWS;
		//ui.toolButton_oneWindow
		RefreshVideoDisplayWindow();
	});

	qDebug() << "class finished";
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

	//char str[200];
	//for (int i = 0; i < camera_list.cam_count; i++)
	//{
	//	sprintf(str, "Cam[%d]-%s : online=%d\n", camera_list.cam_items[i].chn_id,
	//		camera_list.cam_items[i].ip, camera_list.cam_items[i].conn_status);
	//	
	//}

	qDebug() << camera_list.cam_count;
	camera_list_buff.clear();
	ui.listWidget_CameraList->clear();
	for (int i = 0; i < camera_list.cam_count; ++i)
	{
		char str[200];
		sprintf(str, "Cam[%d]-%s : online=%d\n", camera_list.cam_items[i].chn_id,
			camera_list.cam_items[i].ip, camera_list.cam_items[i].conn_status);
		ui.listWidget_CameraList->addItem(str);
		qDebug() << "list:" << camera_list.cam_items[i].ip;
		camera_list_buff.push_back(QString(camera_list.cam_items[i].ip));
		camera_list_buff.push_back(QString(camera_list.cam_items[i].ip));
		qDebug() << "buff:" << camera_list_buff[i];
		ui.listWidget_CameraList->addItem(camera_list_buff[i]);
	}
	ui.listWidget_CameraList->addItem("ip    status");
}

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

void MainMenu::CleanAllDisplayWindows()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].hide();
	}
	qDebug() << "frame width:"
		<< ui.widget_video_window->width() << "height:"
		<< ui.widget_video_window->height();

	qDebug() << "frame x:"
		<< ui.widget_video_window->x() << "y:"
		<< ui.widget_video_window->y();


}

void MainMenu::OneWindowsDisplay()
{
	int row_size = (ui.widget_video_window->height() - 2) / ONE_WINDOWS;
	int col_size = (ui.widget_video_window->width() - 2) / ONE_WINDOWS;

	QRect rect(1, 1, col_size, row_size);
	video_display_label[0].setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	video_display_label[0].setText(QString("video %1").arg(1));
	video_display_label[0].setStyleSheet(DISPLAY_LABEL_STYLE);
	video_display_label[0].setGeometry(rect);
	video_display_label[0].show();
}

void MainMenu::FourWindowsDisplay()
{
	int row_size = (ui.widget_video_window->height() - 3) / FOUR_WINDOWS;
	int col_size = (ui.widget_video_window->width() - 3) / FOUR_WINDOWS;

	int  video_num = 0;
	for (int i = 0; i < FOUR_WINDOWS; i++)
	{
		for (int j = 0; j < FOUR_WINDOWS; j++)
		{
			QRect rect(col_size * j + j, row_size * i + i, col_size, row_size);
			video_display_label[video_num].setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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

void MainMenu::NineWindowsDisplay()
{
	int row_size = (ui.widget_video_window->height() - 4) / NINE_WINDOWS;
	int col_size = (ui.widget_video_window->width() - 4) / NINE_WINDOWS;


	int  video_num = 0;
	for (int i = 0; i < NINE_WINDOWS; i++)
	{
		for (int j = 0; j < NINE_WINDOWS; j++)
		{
			QRect rect(col_size * j + j, row_size * i + i, col_size, row_size);
			video_display_label[video_num].setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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

void MainMenu::resizeEvent(QResizeEvent * event)
{
	RefreshVideoDisplayWindow();
}






