#include "MainMenu.h"
#include <QPushButton>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>

QMutex video_cache_mutex;
unsigned char  *video_data[CAMERA_NUM_LIMIT];               //存储原数图像数据指针
unsigned int   video_data_size[CAMERA_NUM_LIMIT] = { 0 };   //记录原始数据大小，动态分配空间，防止内存非法访问
unsigned int   video_chnId[CAMERA_NUM_LIMIT] = { 0 };       //记录相机通道号
QQueue<QImage> video_frame_cache_[CAMERA_NUM_LIMIT];        //视频缓存

static bool YUV420ToBGR24(unsigned char* pY, unsigned char* pU, unsigned char* pV, unsigned char* pRGB24, int width, int height)
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
			int G = (pY[yIdx] - 16) - 0.69825  * (pV[uIdx] - 128) - 0.33557 * (pU[vIdx] - 128);                        // g分量
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

//清除相机缓存
void ClearVideoFrameCache()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_frame_cache_[i].clear();
		video_data_size[i] = 0;
	}
}

//主界面构造函数
MainMenu::MainMenu(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

/**************************************************系统主界面全局初始化********************************************************/
	{
		//初始化显示图标
		SetIconInit();
		ChangeSystemMode(0);

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
			RefreshUserGroupList();
			RefreshUserInfoList();
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


/**************************************************视频监控界面********************************************************/
	{
		videoTimer = NULL;
		vzbox_online_status = false;
		display_video_windows_num_ = FOUR_WINDOWS;
		video_display_label = NULL;

		ui.toolButton_oneWindow->setEnabled(true);
		ui.toolButton_fourWindows->setEnabled(false);
		ui.toolButton_nineWindows->setEnabled(true);

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
		connect(ui.pushButton_allVideoPlay, &QPushButton::clicked, this, &MainMenu::DealAutoPlayAllVideo);

		//停止播放的全部视频
		connect(ui.pushButton_allVideoStop, &QPushButton::clicked, this, &MainMenu::DealStopPlayAllVideo);

		//播放视频（定时器）
		connect(&video_show_timer_, &QTimer::timeout, this, &MainMenu::DealPlayVideoTimer);
	}
    
/**************************************************相机配置界面********************************************************/
	{
		
		
	}

/***************************************************人脸库界面********************************************************/
	{
		ui.listWidget_userInfoList->setViewMode(QListView::IconMode);
		ui.listWidget_userInfoList->setResizeMode(QListWidget::Adjust);
		ui.listWidget_userInfoList->setMovement(QListWidget::Static);

		p_user_info_item = NULL;
		p_user_list_item = NULL;
		p_user_info_item = new DisplayUserInfoItem[PAGE_USERINFO_NUM];
		p_user_list_item = new QListWidgetItem[PAGE_USERINFO_NUM];

		group_cur_id_ = 1;
		user_list_cur_page_num_ = 1;
		connect(ui.pushButton_userListNext, &QPushButton::clicked, [=]() {
			if (user_list_cur_page_total_ > user_list_cur_page_num_)
			{
				user_list_cur_page_num_++;
				DisplaynPageUserInfoList(group_cur_id_, user_list_cur_page_num_);
			}
		});

		connect(ui.pushButton_userListLast, &QPushButton::clicked, [=]() {
			if (1 < user_list_cur_page_num_)
			{
				user_list_cur_page_num_--;
				DisplaynPageUserInfoList(group_cur_id_, user_list_cur_page_num_);
			}
		});
	}

/**************************************************抓拍查询界面********************************************************/
	{

	}

/**************************************************轨迹查看界面********************************************************/
	{

	}

/**************************************************智能测试界面********************************************************/
	{

	}
}

MainMenu::~MainMenu()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		if (video_data[i] != NULL)
			free(video_data[i]);
	}
	
	VzLPRClient_Close(vzbox_handle_);
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

	video_show_timer_.stop();
	ui.listWidget_CameraList->clear();	
	camera_list_buff.clear();
	CloseAllVideoDisplay();
	ClearVideoFrameCache();
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

//播放已挂载的全部相机的视频
void MainMenu::AutoPlayAllVideo()
{
	int video_num = 0;
	QVector<QString>::iterator it = camera_list_buff.begin();
	while (it != camera_list_buff.end() && video_num < CAMERA_NUM_LIMIT)
	{
		camera_handle_[video_num] = VzLPRClient_Open((*it).toUtf8(), 80, "admin", "admin");
		if (camera_handle_[video_num] == 0)
		{
			qDebug() << "open camera " << *it << "failed,video ID:" << video_num;
			it++;
			video_num++;
			continue;
		}
		qDebug() << "open camera " << *it << "success, video ID:" << video_num;

		it++;
		video_num++;
	}

	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		if (camera_handle_[i] != 0)
		{
			video_chnId[i] = i;
			VzLPRClient_SetVideoFrameCallBack(camera_handle_[i], MainMenu::VideoFrameCallBack, (void *)&video_chnId[i]);
			//VzLPRClient_SetVideoFrameCallBack(camera_handle_[i], MainMenu::CameraFrameCallBack, (void *)&video_display_label[i]);
		}
	}
}

//关闭所有显示的视频
void MainMenu::CloseAllVideoDisplay()
{
	CloseAllCameraHandle();
	RefreshVideoDisplayWindow();
}

//关闭所有已打开的相机
void MainMenu::CloseAllCameraHandle()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		if (camera_handle_[i] != 0)
		{			
			qDebug() << "close camera handle:" << i;
			VzLPRClient_SetVideoFrameCallBack(camera_handle_[i], NULL, NULL);
			VzLPRClient_Close(camera_handle_[i]);
			camera_handle_[i] = 0;
		}
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

//相机视频回调缓存
void MainMenu::VideoFrameCallBack(VzLPRClientHandle handle, void * pUserData, const VzYUV420P * pFrame)
{
	int* chnnal_id = (int *)pUserData;
	//qDebug() << "static VideoFrameCallBack chnnal:" << *chnnal_id;

	//为frame分配空间
	if (video_data[*chnnal_id] == NULL)
	{
		qDebug() << "malloc chnnalId:" << *chnnal_id;
		video_data_size[*chnnal_id] = pFrame->height * pFrame->width * 3;
		video_data[*chnnal_id] = (unsigned char*)malloc(video_data_size[*chnnal_id]);
		memset(video_data[*chnnal_id], 0, video_data_size[*chnnal_id]);
	}
	else
	{
		if (video_data_size[*chnnal_id] != pFrame->height * pFrame->width * 3)  //重新验证分配的空间，防止frame_size变化导致的程序崩溃
		{
			//qDebug() << "----->" << "image data size have changed" << *chnnal_id;
			free(video_data[*chnnal_id]);
			video_data[*chnnal_id] = NULL;
			video_data_size[*chnnal_id] = pFrame->height * pFrame->width * 3;
			video_data[*chnnal_id] = (unsigned char*)malloc(video_data_size[*chnnal_id]);
			memset(video_data[*chnnal_id], 0, video_data_size[*chnnal_id]);
		}
	}

	if (YUV420ToBGR24(pFrame->pY, pFrame->pU, pFrame->pV, video_data[*chnnal_id], pFrame->width, pFrame->height))
	{
		QMutexLocker locker(&video_cache_mutex);

		//qDebug() << "+++++video_get image chnnal_id:" << *chnnal_id;
		//qDebug() << "Ysize:" << pFrame->widthStepY << "Usize:" << pFrame->widthStepU << "Vsize:" << pFrame->widthStepV << "height:" << pFrame->height << "width:" << pFrame->width;
		QImage video_image = QImage(video_data[*chnnal_id], pFrame->width, pFrame->height, pFrame->width * 3, QImage::Format_RGB888);
		if (video_frame_cache_[*chnnal_id].size() >= FRAME_NUM_SIZE_LIMIT)
		{
			video_frame_cache_[*chnnal_id].pop_front();
			video_frame_cache_[*chnnal_id].push_back(video_image);
		}
		else
		{
			video_frame_cache_[*chnnal_id].push_back(video_image);
		}
	}
}

void MainMenu::CameraFrameCallBack(VzLPRClientHandle handle, void * pUserData, const VzYUV420P * pFrame)
{
	DisplayVideoLabel* video_label = (DisplayVideoLabel *)pUserData;
	//qDebug() << "static VideoFrameCallBack chnnal:" << *chnnal_id;

	//为frame分配空间
	int frame_size = pFrame->height * pFrame->width * 3;
	unsigned char *frame_data = (unsigned char *)malloc(frame_size);
	memset(frame_data, 0, frame_size);

	//qDebug() << "camera chnnal:"<< &video_label <<" callback address:" << &pFrame;
	if (YUV420ToBGR24(pFrame->pY, pFrame->pU, pFrame->pV, frame_data, pFrame->width, pFrame->height))
	{
		QMutexLocker locker(&video_cache_mutex);
		QImage frame_image = QImage(frame_data, pFrame->width, pFrame->height, pFrame->width * 3, QImage::Format_RGB888);
		video_label->setDisplayImage(frame_image);
		//video_label->setPixmap(QPixmap::fromImage(frame_image.scaled(video_label->width(), video_label->height())));
	}
	if (frame_data)
		free(frame_data);
}

//切换系统功能模式（通过索引切换）
void MainMenu::ChangeSystemMode(int index)
{
    ui.stackedWidget_systemMode->setCurrentIndex(index);
}

//刷新人脸库列表
void MainMenu::RefreshUserGroupList()
{
	if (!vzbox_online_status)
		return;

	VZ_FACE_LIB_RESULT face_lib_list;
	int ret = VzClient_SearchFaceRecgLib(vzbox_handle_, &face_lib_list);
	if (ret == VZSDK_SUCCESS)
	{
		qDebug() << "get face library sucess,count:" << face_lib_list.lib_count;
		for (int i = 0; i < MAX_FACE_LIB_COUNT; i++)
		{
			qDebug() << "lib " << i << ":" << QString::fromLocal8Bit(face_lib_list.lib_items[i].name);
		}
	}
}

//刷新用户信息列表
void MainMenu::RefreshUserInfoList()
{
	if (!vzbox_online_status)
		return;

	//ui.listWidget_userInfoList->clear();
	DisplaynPageUserInfoList(group_cur_id_, user_list_cur_page_num_);
	/*VZ_FACE_USER_RESULT user_face_info;
	VZ_FACE_LIB_SEARCH_CONDITION condition = { 0 };
	condition.page_num = 1;
	condition.page_count = 25;

	int ret = VzClient_SearchFaceRecgUser(vzbox_handle_, &condition, 1, &user_face_info);
	if (ret == VZSDK_SUCCESS)
	{
		QImage  img;
		qDebug() << "get face info sucess, count:" << user_face_info.face_count << "  total count:" << user_face_info.total_count;
		for (int i = 0; i < user_face_info.face_count; i++)
		{
			char face_path[100] = { 0 };
			sprintf(face_path, "faceImageCache/%d.jpg", user_face_info.face_items[i].pic_index);
			FILE *face_data = fopen(face_path, "wb+");
			if (face_data)
			{
				int pFaceSize = 1024 * 1024;
				char *pFacedata = (char*)malloc(pFaceSize);
				memset(pFacedata, 0, pFaceSize);
				int ret = VzClient_LoadFaceImageByPath(vzbox_handle_, user_face_info.face_items[i].img_url, pFacedata, &pFaceSize);
				if (ret == VZSDK_SUCCESS)
				{
					fwrite(pFacedata, 1, pFaceSize, face_data);
				}
				free(pFacedata);
				fclose(face_data);
				img.load(face_path);
				if (!img.isNull())
					qDebug() << "image is null";
			}
			p_user_info_item[i].SetDisplayItemInfo(QString::fromLocal8Bit(user_face_info.face_items[i].user_name), img);
			p_user_list_item[i].setSizeHint(USERINFO_ITEM_SIZE);

			ui.listWidget_userInfoList->addItem(&p_user_list_item[i]);
			ui.listWidget_userInfoList->setItemWidget(&p_user_list_item[i], &p_user_info_item[i]);
		}
		ui.label_totalUserInfo->clear();
		ui.label_totalUserInfo->setText(QString::fromLocal8Bit("共 %1 条记录").arg(user_face_info.total_count));
	}
	else
	{
		qDebug() << "get face info failed"<<ret;
	}*/
}

void MainMenu::DisplaynPageUserInfoList(int group_id, int page_num)
{
	VZ_FACE_LIB_SEARCH_CONDITION condition = { 0 };
	condition.page_num = page_num;
	condition.page_count = PAGE_USERINFO_NUM;

	cur_group_toal_user_info_ = { 0 };
	int ret = VzClient_SearchFaceRecgUser(vzbox_handle_, &condition, group_id, &cur_group_toal_user_info_);
	if (ret != VZSDK_SUCCESS)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取用户信息失败！"));
		return;
	}

	qDebug() << "All user:" << cur_group_toal_user_info_.total_count;
	//ui.listWidget_userInfoList->clear();
	for (int i = 0; i < cur_group_toal_user_info_.face_count; i++)
	{
		QImage img;
		char face_path[100] = { 0 };
		sprintf(face_path, "faceImageCache/%d.jpg", cur_group_toal_user_info_.face_items[i].pic_index);
		FILE *face_data = fopen(face_path, "wb+");
		if (face_data)
		{
			int pFaceSize = 1024 * 1024;
			char *pFacedata = (char*)malloc(pFaceSize);
			memset(pFacedata, 0, pFaceSize);
			int ret = VzClient_LoadFaceImageByPath(vzbox_handle_, cur_group_toal_user_info_.face_items[i].img_url, pFacedata, &pFaceSize);
			if (ret != VZSDK_SUCCESS)
			{
				fclose(face_data);
				free(pFacedata);
			}
			else
			{
				fwrite(pFacedata, sizeof(char), pFaceSize, face_data);				
				fclose(face_data);
				free(pFacedata);
				img.load(face_path);
			}		
		}
		p_user_info_item[i].SetDisplayItemInfo(QString::fromLocal8Bit(cur_group_toal_user_info_.face_items[i].user_name), img);
		p_user_list_item[i].setSizeHint(USERINFO_ITEM_SIZE);

		ui.listWidget_userInfoList->addItem(&p_user_list_item[i]);
		ui.listWidget_userInfoList->setItemWidget(&p_user_list_item[i], &p_user_info_item[i]);
	}

	if (cur_group_toal_user_info_.total_count % PAGE_USERINFO_NUM)
	{
		user_list_cur_page_total_ = cur_group_toal_user_info_.total_count / PAGE_USERINFO_NUM + 1;
	}
	else
	{
		user_list_cur_page_total_ = cur_group_toal_user_info_.total_count / PAGE_USERINFO_NUM;
	}
	
	ui.label_totalUserInfo->clear();
	ui.label_totalUserInfo->setText(QString::fromLocal8Bit("共%2 页     %1 条记录    当前 %3 页").arg(cur_group_toal_user_info_.total_count)
									.arg(user_list_cur_page_total_).arg(user_list_cur_page_num_));

}

//绘制系统背景
void MainMenu::paintEvent(QPaintEvent * event)
{
    //QPainter painter(this);
    //QPixmap pix;
    //pix.load("./icon/MainBackGround.jpg");
    ////指定长宽
    //painter.drawPixmap(0, 0, this->width(), this->height(), pix);
}

//改变大小时刷新视频窗口
void MainMenu::resizeEvent(QResizeEvent * event)
{
	RefreshVideoDisplayWindow();
}

//关闭系统主窗口
void MainMenu::closeEvent(QCloseEvent * event)
{

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
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("没有打开的设备！"));
		return;
	}

	try
	{
		AutoPlayAllVideo();
		video_show_timer_.start((int)(1000.0/VIDEO_FRAME_RATE));
	}
	catch (...)
	{
		qDebug() << "auto play all_video failed ";
	}
}

//停止播放的视频，但不关闭已打开的相机
void MainMenu::DealStopPlayAllVideo()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		if (camera_handle_[i] != 0)
		{
			qDebug() << "stop camera handle:" << i;
			VzLPRClient_SetVideoFrameCallBack(camera_handle_[i], NULL, NULL);
		}
	}
}

//通过定时器软中断，显示相机视频
void MainMenu::DealPlayVideoTimer()
{	
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		if (video_frame_cache_[i].size() > 0)
		{
			QImage img;
			{
				QMutexLocker locker(&video_cache_mutex);
				img = video_frame_cache_[i].front();
				video_frame_cache_[i].pop_front();
			}

			try
			{
				if (!img.isNull() && img.width() > video_display_label[i].width() && img.height() > video_display_label[i].height())
				{
					QImage img_show = img.scaled(video_display_label[i].width(), video_display_label[i].height());
					if(!img_show.isNull())
						video_display_label[i].setPixmap(QPixmap::fromImage(img_show));
				}
			}
			catch (...)
			{
				qDebug() << "chnnal image get failed" << i;
				continue;
			}
		}
	}
}






