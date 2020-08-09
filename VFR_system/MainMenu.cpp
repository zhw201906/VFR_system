#include "MainMenu.h"
#include <QPushButton>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QMetaType>

QMutex video_cache_mutex;
unsigned char  *video_data[CAMERA_NUM_LIMIT];               //存储原数图像数据指针
unsigned int   video_data_size[CAMERA_NUM_LIMIT] = { 0 };   //记录原始数据大小，动态分配空间，防止内存非法访问
unsigned int   video_chnId[CAMERA_NUM_LIMIT] = { 0 };       //记录相机通道号
QQueue<QImage> video_frame_cache_[CAMERA_NUM_LIMIT];        //视频缓存

//等比缩放图像
//参数1：图像路径（IN）
//参数2：空间宽度（IN）
//参数3：控件高度（IN）
//参数4：缩放后结果图（OUT）
static void Geometric_Scaling_Image(const QString path, const int control_width, const int control_height, QPixmap &dst_pix)
{
    QPixmap pix;
    pix.load(path);

    double ratio_w = pix.width() * 1.0 / control_width;
    double ratio_h = pix.height() * 1.0 / control_height;

    if (ratio_w >= ratio_h)
        pix = pix.scaled(pix.width() / ratio_w, pix.height() / ratio_w);
    else
        pix = pix.scaled(pix.width() / ratio_h, pix.height() / ratio_h);

    dst_pix = pix;
}

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
		qRegisterMetaType<FaceSnapInfo>("FaceSnapInfo");
		//初始化显示图标
		SetIconInit();
		ChangeSystemMode(0);
        CleanAllSetSystemModeButton();
        ui.pushButton_onlineMonitoring->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
        ui.pushButton_onlineMonitoring->setEnabled(false);

		//通过按钮切换系统功能
		connect(ui.pushButton_onlineMonitoring, &QPushButton::clicked, [=]() {
			ChangeSystemMode(0);
			RefreshVideoDisplayWindow();
			//CleanSnapAndRecgResultList();

            CleanAllSetSystemModeButton();
            ui.pushButton_onlineMonitoring->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
            ui.pushButton_onlineMonitoring->setEnabled(false);
		});

		connect(ui.pushButton_cameraManage, &QPushButton::clicked, [=]() {
			ChangeSystemMode(1);

            CleanAllSetSystemModeButton();
            ui.pushButton_cameraManage->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
            ui.pushButton_cameraManage->setEnabled(false);
		});

		connect(ui.pushButton_libraryManage, &QPushButton::clicked, [=]() {
			ChangeSystemMode(2);

            CleanAllSetSystemModeButton();
            ui.pushButton_libraryManage->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
            ui.pushButton_libraryManage->setEnabled(false);
		});

		connect(ui.pushButton_snapResult, &QPushButton::clicked, [=]() {
			ChangeSystemMode(3);

            CleanAllSetSystemModeButton();
            ui.pushButton_snapResult->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
            ui.pushButton_snapResult->setEnabled(false);
		});

		connect(ui.pushButton_trackPath, &QPushButton::clicked, [=]() {
			ChangeSystemMode(4);
			RefreshBuildMapDisplay();

            CleanAllSetSystemModeButton();
            ui.pushButton_trackPath->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
            ui.pushButton_trackPath->setEnabled(false);
		});

		connect(ui.pushButton_smartTest, &QPushButton::clicked, [=]() {
			ChangeSystemMode(5);

            CleanAllSetSystemModeButton();
            ui.pushButton_smartTest->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
            ui.pushButton_smartTest->setEnabled(false);
		});
	}


/**************************************************视频监控界面********************************************************/
	{
		videoTimer = NULL;
		vzbox_online_status = false;
		display_video_windows_num_ = FOUR_WINDOWS;
		video_display_label = NULL;
		p_snap_result_buff_ui = NULL;
		p_snap_result_item = NULL;
		p_display_warning_list_ui = NULL;

		ui.toolButton_oneWindow->setEnabled(true);
		ui.toolButton_fourWindows->setEnabled(false);
		ui.toolButton_nineWindows->setEnabled(true);

		ui.toolButton_devStatus->setEnabled(false);
		ui.toolButton_devStatus->setIcon(QIcon("./icon/devOffline.jpg"));
		ui.toolButton_devStatus->setText(QString::fromLocal8Bit("设备离线"));
		ui.toolButton_warningInfo->setIcon(QIcon("./icon/warnning.jpg"));
		ui.toolButton_warningInfo->setEnabled(false);
		ui.toolButton_warningInfo->setText("");

		ui.listWidget_nowSnap->setViewMode(QListView::IconMode);
		ui.listWidget_nowSnap->setResizeMode(QListWidget::Adjust);
		ui.listWidget_nowSnap->setMovement(QListWidget::Static);
		ui.listWidget_nowSnap->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

		ui.listWidget_nowRecognize->setViewMode(QListView::IconMode);
		ui.listWidget_nowRecognize->setResizeMode(QListWidget::Adjust);
		ui.listWidget_nowRecognize->setMovement(QListWidget::Static);
		ui.listWidget_nowRecognize->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

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

		p_snap_result_buff_ui = new DisplaySnapResult[CAMERA_SNAP_RESULT_MAX_NUMS];
		p_snap_result_item = new QListWidgetItem[CAMERA_SNAP_RESULT_MAX_NUMS];
		p_recognize_result_buff_ui = new DisplaySnapRecognizeResult[CAMERA_SNAP_RECOGNIZE_MAX_NUMS];
		p_recognize_result_item = new QListWidgetItem[CAMERA_SNAP_RECOGNIZE_MAX_NUMS];

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

		connect(this, &MainMenu::ShowSnapItem, this, &MainMenu::DisplaySnapInformation);

		//处理报警跳转画轨迹
		connect(ui.toolButton_warningInfo, &QToolButton::clicked, this, &MainMenu::DealShowWarningUserList);
	}
    
/**************************************************相机配置界面********************************************************/
	{
        p_add_camera_ui = NULL;

        connect(ui.listWidget_cameraManageList, &QListWidget::itemClicked, this, &MainMenu::CurrentSelectCameraItem);
        connect(ui.toolButton_addCamera, &QPushButton::clicked, this, &MainMenu::AddOneConnectCamera);
        connect(ui.pushButton_saveCameraManage, &QPushButton::clicked, this, &MainMenu::ModifySelectedCamera);
        connect(ui.toolButton_delCamera, &QPushButton::clicked, this, &MainMenu::DeleteSelectedCamera);
        connect(ui.toolButton_refreshCamera, &QPushButton::clicked, this, &MainMenu::UpdateConnectedCameraInfoMap);

	}

/***************************************************人脸库界面********************************************************/
	{
		ui.listWidget_userInfoList->setViewMode(QListView::IconMode);
		ui.listWidget_userInfoList->setResizeMode(QListWidget::Adjust);
		ui.listWidget_userInfoList->setMovement(QListWidget::Static);
		ui.listWidget_userInfoList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        p_operator_user_ui = NULL;
        p_operator_lib_ui = NULL;
		//p_user_info_item = NULL;
		//p_user_list_item = NULL;
		//p_user_info_item = new DisplayUserInfoItem[PAGE_USERINFO_NUM];
		//p_user_list_item = new QListWidgetItem[PAGE_USERINFO_NUM];

		group_cur_id_ = ~0;
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

		connect(ui.pushButton_userListHead, &QPushButton::clicked, [=]() {
			user_list_cur_page_num_ = 1;
			DisplaynPageUserInfoList(group_cur_id_, user_list_cur_page_num_);
		});

		connect(ui.pushButton_userListTail, &QPushButton::clicked, [=]() {
			user_list_cur_page_num_ = user_list_cur_page_total_;
			DisplaynPageUserInfoList(group_cur_id_, user_list_cur_page_num_);
		});
        connect(ui.pushButton_addOneUser, &QPushButton::clicked, this, &MainMenu::AddOneUserInformation);
        connect(ui.pushButton_modifyUser, &QPushButton::clicked, this, &MainMenu::ModifyOneUserInformation);
		connect(ui.listWidget_userGroupList, &QListWidget::itemClicked, this, &MainMenu::CurrentSelectFaceLib);

        connect(ui.toolButton_addLibrary, &QToolButton::clicked, this, &MainMenu::AddOneFaceLibInfo);
        connect(ui.toolButton_editLibrary, &QToolButton::clicked, this, &MainMenu::ModifyOneFaceLibInfo);
        connect(ui.toolButton_delLibrary, &QToolButton::clicked, this, &MainMenu::DeleteOneFaceLib);
	}

/**************************************************抓拍查询界面********************************************************/
	{

	}

/**************************************************轨迹查看界面********************************************************/
	{
		ui.label_buildingMap->setStyleSheet(BUILDING_MAP_LABEL_STYLE);
		ui.lineEdit_cameraPositionName->setPlaceholderText(QString::fromLocal8Bit("为相机安装位置设置名称，非必须"));
		ui.lineEdit_searchPersonTrack->setPlaceholderText(QString::fromLocal8Bit("必须是已存在用户，才可绘制轨迹"));
		ui.lineEdit_buildingMapName->setPlaceholderText(QString::fromLocal8Bit("新建地图名称，不可重复"));
		ui.checkBox_openRunTrackAllTimes->setCheckState(Qt::Checked);
		ui.dateTimeEdit_runTrackStartTime->setEnabled(false);
		ui.dateTimeEdit_runTrackEndTime->setEnabled(false);
		QDateTime date_time = QDateTime::currentDateTime();
		ui.dateTimeEdit_runTrackEndTime->setDateTime(date_time);
		ui.dateTimeEdit_runTrackStartTime->setDateTime(date_time.addDays(-1));


		place_camera_num = 0;
		connect(ui.pushButton_loadBuildingImage, &QPushButton::clicked, this, &MainMenu::LoadBuildingMapImage);
		connect(ui.pushButton_cleanDrawedTrack, &QPushButton::clicked, ui.label_buildingMap, &DealBuildingMap::CleanDrawedRunTrack);
		connect(ui.pushButton_placeCamera, &QPushButton::clicked, this, &MainMenu::DealPlaceOneCamera);
		connect(ui.pushButton_cacelPlaceCamera, &QPushButton::clicked, ui.label_buildingMap, &DealBuildingMap::CalcelOneRecodePoint);
		connect(ui.label_buildingMap, &DealBuildingMap::CalcelOnePlacedCamera,   this, &MainMenu::CalcelPlacedOneCamera);
		connect(ui.label_buildingMap, &DealBuildingMap::CurrentPlaceCameraPoint, this, &MainMenu::DealPlaceCameraPosition);
		connect(ui.label_buildingMap, &DealBuildingMap::SendShowErrorMsg, this, &MainMenu::DealShowErrorMsg);
		connect(ui.label_buildingMap, &DealBuildingMap::AdjustLinkTrackPosition, this, &MainMenu::DealAdjustLinkTrackPosition);
		connect(ui.pushButton_saveBuildingMap, &QPushButton::clicked, this, &MainMenu::SaveNewBuildingMap);
		connect(ui.pushButton_deleteExistBuildingMap, &QPushButton::clicked, this, &MainMenu::DeleteExistedBuildingMap);
		connect(ui.comboBox_existedBuindingMap, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, &MainMenu::LoadExistedBuildingMap);
		connect(ui.pushButton_resetCameraPlace, &QPushButton::clicked, this, &MainMenu::ResetCreateNewBuildingMap);
		connect(ui.comboBox_selectCamera, &QComboBox::currentTextChanged, this, &MainMenu::ShowCurrentSelectCameraId);
		connect(ui.pushButton_drawPersonTrack, &QPushButton::clicked, this, &MainMenu::DrawAfterUserRecgedTrack);
		connect(ui.checkBox_openRunTrackAllTimes, &QCheckBox::stateChanged, this, &MainMenu::DealDrawUserRunTrackTimeMode);
		connect(ui.pushButton_referPersonLinkTrack, &QPushButton::clicked, ui.label_buildingMap, &DealBuildingMap::ShowListTrackWindow);		
		UpdateExistedBuildingMapList();
	}

/**************************************************智能测试界面********************************************************/
	{
        ui.treeWidget_detectResult->setHeaderLabels(QStringList() << QString::fromLocal8Bit("属性") << QString::fromLocal8Bit("详细信息"));
        ui.treeWidget_recognizeResult->setHeaderLabels(QStringList() << QString::fromLocal8Bit("属性") << QString::fromLocal8Bit("详细信息"));

        ui.pushButton_viewRecognizeResult->setEnabled(false);
        ui.pushButton_viewDetectResult->setEnabled(false);

        CreateAItestEngine();
        p_display_recognize_ui = NULL;       
        p_display_detect_ui = NULL;

        connect(ui.pushButton_loadCompareImage1,  &QPushButton::clicked, this, &MainMenu::LoadCompareImg1);
        connect(ui.pushButton_loadCompareImage2,  &QPushButton::clicked, this, &MainMenu::LoadCompareImg2);
        connect(ui.pushButton_compareFace,        &QPushButton::clicked, this, &MainMenu::DealFaceCompare);
        connect(ui.pushButton_loadDetectImage,    &QPushButton::clicked, this, &MainMenu::LoadDetectImg);
        connect(ui.pushButton_detectFace,         &QPushButton::clicked, this, &MainMenu::DealFaceDetect);
        connect(ui.pushButton_loadRecognizeImage, &QPushButton::clicked, this, &MainMenu::LoadRecognizeImg);
        connect(ui.pushButton_recognizeFace,      &QPushButton::clicked, this, &MainMenu::DealFaceRecognize);
        connect(ui.pushButton_viewDetectResult,   &QPushButton::clicked, this, &MainMenu::ShowFaceDetectResult);
        connect(ui.pushButton_viewRecognizeResult,&QPushButton::clicked, this, &MainMenu::ShowFaceRecognizeResult);
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

    if (p_display_recognize_ui != NULL)
    {
        delete p_display_recognize_ui;
        p_display_recognize_ui = NULL;
        ui.pushButton_viewRecognizeResult->setEnabled(false);
    }

    if (p_display_detect_ui != NULL)
    {
        ui.pushButton_viewDetectResult->setEnabled(false);
        delete p_display_detect_ui;
        p_display_detect_ui = NULL;
    }

	if (p_add_camera_ui != NULL)
	{
		delete p_add_camera_ui;
		p_add_camera_ui = NULL;
	}

	if (p_operator_user_ui != NULL)
	{
		delete p_operator_user_ui;
		p_operator_user_ui = NULL;
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

//打开设备，成功打开设备后，对每个系统功能做初始化
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

	ui.toolButton_devStatus->setEnabled(true);
	ui.toolButton_devStatus->setIcon(QIcon("./icon/devOnline.jpg"));
	ui.toolButton_devStatus->setText(QString::fromLocal8Bit("设备在线"));

	SystemAllInit();
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
	ui.listWidget_cameraManageList->clear();
    connected_camera_map.clear();
	camera_list_buff.clear();
	CloseAllVideoDisplay();
	ClearVideoFrameCache();

	ui.toolButton_devStatus->setEnabled(false);
	ui.toolButton_devStatus->setIcon(QIcon("./icon/devOffline.jpg"));
	ui.toolButton_devStatus->setText(QString::fromLocal8Bit("设备离线"));
	ui.toolButton_warningInfo->setEnabled(false);
	ui.toolButton_warningInfo->setText("");
}

//系统全部功能初始化，在成功连接设备后执行
void MainMenu::SystemAllInit()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

	UpdateConnectedCameraInfoMap();
	RefreshVideoDisplayWindow();
	RefreshUserGroupList();
	RefreshUserInfoList();

	//设置抓拍回调显示
	//int ret = VzLPRClient_SetFaceResultCallBack(vzbox_handle_, &MainMenu::CameraSnapCallBack, (void *)this);
	//if (ret != VZSDK_SUCCESS)
	//{
	//	qDebug() << "set snap callback falied...";
	//	//return;
	//}

	//设置抓拍识别回调显示
	int ret = VzLPRClient_SetFaceResultExCallBack(vzbox_handle_, &MainMenu::CameraRecognizeCallBack, (void *)this);
	if (ret != VZSDK_SUCCESS)
	{
		qDebug() << "set recognize callback falied...";
		//return;
	}
}

//刷新显示的相机列表（同时刷新在线监控和相机管理界面）
void MainMenu::RefreshDisplayCameraList()
{
	ui.listWidget_CameraList->clear();
    ui.listWidget_cameraManageList->clear();

    auto it = camera_list_buff.begin();
    while (camera_list_buff.end() != it)
    {
        ui.listWidget_CameraList->addItem(*it);
        ui.listWidget_cameraManageList->addItem(*it);
        it++;
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

//保存抓拍与识别结果信息到本地文件中
void MainMenu::SaveSnapRecgInfo(QString &path, QString & info)
{
	QFile file(path);
	bool ret = file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	if (!ret)
	{
		qDebug() << "open recg_info file failed...";
	}

	QString write_context;
	if (file.size() <= 0)
	{
		write_context = QString("{") + info + QString("}");
	}
	else
	{
		write_context = QString(",{") + info + QString("}");
		file.seek(file.size());
	}

	file.write(write_context.toUtf8());
	file.close();
}

//通过通道号查询相机信息
void MainMenu::QueryCameraInfoByChanid(int channel_id, int & cam_id, QString & cam_ip)
{
	auto it = connected_camera_map.begin();
	while (it != connected_camera_map.end())
	{
		if (it.value().channel_id == channel_id)
		{
			cam_id = it.value().camera_id;
			cam_ip = QString(it.value().camera_item.ip);
			break;
		}
		++it;
	}
}

//黑名单报警，记录用户名，并激活跳转功能
void MainMenu::WarningRecordUsername(QString &name)
{
	warning_user_list.push_back(name);
	ui.toolButton_warningInfo->setIcon(QIcon("./icon/warnning.jpg"));
	QString warn_info = QString::fromLocal8Bit("报警:\"") + name 
		+ QString::fromLocal8Bit("\"出现!");
	ui.toolButton_warningInfo->setText(warn_info);
	ui.toolButton_warningInfo->setEnabled(true);
}

//处理点击报警按钮，显示报警用户列表
void MainMenu::DealShowWarningUserList()
{
	if (warning_user_list.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("报警名单为空！"));
		return;
	}

	if (p_display_warning_list_ui == NULL)
	{
		p_display_warning_list_ui = new ShowWarningUserList;
		connect(p_display_warning_list_ui, &ShowWarningUserList::SignalWarningUser, this, &MainMenu::TurnToTrackPathUi);
		connect(p_display_warning_list_ui, &ShowWarningUserList::SignalRefreshList, [=]() {
			p_display_warning_list_ui->SetUserList(warning_user_list);
			p_display_warning_list_ui->ShowUserList();
		});
	}
	p_display_warning_list_ui->move(this->x() + this->width(), this->y());
	p_display_warning_list_ui->resize(300, this->height());
	p_display_warning_list_ui->show();
	
	p_display_warning_list_ui->SetUserList(warning_user_list);
	p_display_warning_list_ui->ShowUserList();
}

//跳转到行人轨迹界面，并直接准备对应用户轨迹
void MainMenu::TurnToTrackPathUi(QString user_name)
{
	ChangeSystemMode(4);

	CleanAllSetSystemModeButton();
	ui.pushButton_trackPath->setStyleSheet(SYSTEM_MODE_BUTTON_DISABLE_STYLE);
	ui.pushButton_trackPath->setEnabled(false);

	ui.lineEdit_searchPersonTrack->clear();
	ui.lineEdit_searchPersonTrack->setText(user_name);
}

//相机视频回调缓存（通过定时器中断回调显示，效果相对较流畅）
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

//实时视频回调函数（传输显示label上下文，效果较差。暂时没有使用）
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

//相机抓拍结果回调函数
void MainMenu::CameraSnapCallBack(VzLPRClientHandle handle, TH_FaceResult * face_result, void * pUserData)
{
	static int chn_i = 0;
	MainMenu *p_menu = (MainMenu*)pUserData;

	int i = 0;
	//for (; i < face_result->person_num; i++)
	{
		FaceSnapInfo face_info;
		face_info.sex = face_result->sex;
		face_info.age = face_result->age;
		face_info.have_glasses = face_result->have_glasses;
		face_info.have_hat = face_result->have_hat;
		face_info.msec = face_result->msec;
		face_info.confidence = face_result->face_items[i].confidence;
		strcpy(face_info.datetime, face_result->datetime);

		QImage img;
		QString face_path(CAMERA_SNAP_IMAGE_PATH);
		//face_path.append(QString("/face%1_%2.jpg").arg(i).arg(face_result->msec));
		face_path.append(QString("/face.jpg"));
		FILE *face_data = fopen(face_path.toStdString().c_str(), "wb+");
		if (face_data)
		{
			fwrite(face_result->face_imgs[i].img_buf, sizeof(unsigned char), 
				   face_result->face_imgs[i].img_len, face_data);
			fclose(face_data);
		}

		//emit p_menu->ShowSnapItem(&face_info, &face_path);
		//p_menu->DisplaySnapInformation(face_info, face_path);
		//DisplaySnapResult  *p_snap_result = new DisplaySnapResult;
		//p_snap_result->SetShowData(face_info, face_path);

		//QListWidgetItem *item = new QListWidgetItem;
		//item->setSizeHint(SNAP_ITEM_SIZE);
		//p_menu->ui.listWidget_nowSnap->addItem(item);
		//p_menu->ui.listWidget_nowSnap->setItemWidget(item, p_snap_result);
		//p_menu->ui.listWidget_nowSnap->scrollToBottom();


		//p_menu->p_snap_result_buff_ui = new DisplaySnapResult;
		//p_menu->p_snap_result_item = new QListWidgetItem(p_menu->ui.listWidget_nowSnap);
		//p_menu->p_snap_result_buff_ui->SetShowData(face_info, face_path);
		//p_menu->p_snap_result_item->setSizeHint(SNAP_ITEM_SIZE);
		//p_menu->ui.listWidget_nowSnap->addItem(p_menu->p_snap_result_item);
		//p_menu->ui.listWidget_nowSnap->setItemWidget(p_menu->p_snap_result_item, 
		//	                                         p_menu->p_snap_result_buff_ui);


		p_menu->p_snap_result_buff_ui[chn_i].SetShowData(face_info, face_path);
		p_menu->p_snap_result_item[chn_i].setSizeHint(SNAP_ITEM_SIZE);
		p_menu->ui.listWidget_nowSnap->addItem(&p_menu->p_snap_result_item[chn_i]);
		p_menu->ui.listWidget_nowSnap->setItemWidget(&p_menu->p_snap_result_item[chn_i], 
			                                         &p_menu->p_snap_result_buff_ui[chn_i]);
		p_menu->ui.listWidget_nowSnap->scrollToBottom();

		//p_menu->p_snap_result_buff_ui[chn_i].SetShowData(face_info, face_path);
		//QListWidgetItem *snap_item = new QListWidgetItem;
		//snap_item->setSizeHint(SNAP_ITEM_SIZE);
		//p_menu->ui.listWidget_nowSnap->addItem(snap_item);
		//p_menu->ui.listWidget_nowSnap->setItemWidget(snap_item,
		//	&p_menu->p_snap_result_buff_ui[chn_i]);
		//p_menu->ui.listWidget_nowSnap->scrollToBottom();

		chn_i++;
		if (chn_i >= CAMERA_SNAP_RESULT_MAX_NUMS)
		{
			chn_i = 0;
			//p_menu->CleanSnapAndRecgResultList();
			//p_menu->ui.listWidget_nowSnap->clear();
		}
	}
}

//抓拍识别结果回调函数
void MainMenu::CameraRecognizeCallBack(VzLPRClientHandle handle, TH_FaceResultEx * face_result, void * pUserData)
{
	static int recg_i = 0;
	static int snap_i = 0;
	//qDebug() << "recognize success:" << recg_i << "  person_num:" << face_result->snap_num;
	MainMenu *p_menu = (MainMenu*)pUserData;
	int i = 0;
	//for (; i < face_result->num; i++)
	{		
		FaceRecognizeInfo face_info;

		face_info.channel_id = face_result->face_items[i].channel_id;
		face_info.num = face_result->num;
		face_info.msec = face_result->msec;
		face_info.recg_face_score = face_result->face_items[i].recg_face_score;
		face_info.recg_face_id = face_result->face_items[i].face_id;
		face_info.recg_face_lib_id = face_result->face_items[i].recg_face_lib_id;
		strcpy(face_info.recg_people_name, face_result->face_items[i].recg_people_name);
		strcpy(face_info.recg_img_url, face_result->face_items[i].recg_img_url);
		strcpy(face_info.recg_birthday, face_result->face_items[i].recg_birthday);
		strcpy(face_info.recg_prov, face_result->face_items[i].recg_prov);
		strcpy(face_info.datetime, face_result->datetime);

		FaceSnapInfo snap_info;
		snap_info.sex = face_result->face_items[i].sex;
		snap_info.age = face_result->face_items[i].age;
		snap_info.have_glasses = face_result->face_items[i].have_glasses;
		snap_info.have_hat = face_result->face_items[i].have_hat;
		snap_info.msec = face_result->msec;
		snap_info.confidence = face_result->face_items[i].confidence;
		strcpy(snap_info.datetime, face_result->datetime);

		//加载抓拍图
		QString face_snap_path(CAMERA_SNAP_IMAGE_NAME);
		//face_snap_path.append(QString("/snap_recg.jpg"));
		//face_snap_path.append(QString("/snap%1_%2.jpg").arg(face_info.recg_face_lib_id).arg(face_info.recg_face_id));
		FILE *snap_data = fopen(face_snap_path.toStdString().c_str(), "wb+");
		if (snap_data)
		{
			fwrite(face_result->face_imgs[i].img_buf, sizeof(unsigned char),
				face_result->face_imgs[i].img_len, snap_data);
			fclose(snap_data);
		}

		//加载抓拍大图
		QString big_face_snap_path(CAMERA_SNAP_BIG_IMAGE_NAME);
		FILE *big_snap_data = fopen(big_face_snap_path.toStdString().c_str(), "wb+");
		if (big_snap_data)
		{
			fwrite(face_result->snap_img[i].img_buf, sizeof(unsigned char),
				face_result->snap_img[i].img_len, big_snap_data);
			fclose(big_snap_data);
		}

		//根据识别结果判断是否开启人脸识别，未开启时不加载识别结果
		if (face_info.recg_img_url != NULL && face_info.recg_face_score > 0)
		{
			QDateTime date_time = QDateTime::currentDateTime();
			// 字符串格式化
			//QString times_tamp = date_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
			// 获取毫秒值
			//int ms = date_time.time().msec();
			// 转换成时间戳
			qint64 epoch_time = date_time.toMSecsSinceEpoch();
			//qDebug() << "times_tamp:" << times_tamp << "  ms:" << ms << "   epoch_time:" << epoch_time;

			//加载库图片
			QString face_recg_path = QString(CAMERA_LIBRARY_IMAGE_NAME);
			FILE *face_data = fopen(face_recg_path.toStdString().c_str(), "wb+");
			if (face_data)
			{
				int face_size = 1024 * 1024;
				char*p_face = (char *)malloc(face_size);
				memset(p_face, 0, face_size);				
				int ret = VzClient_LoadFaceImageByPath(p_menu->vzbox_handle_, face_info.recg_img_url, p_face, &face_size);
				if (ret != VZSDK_SUCCESS)
				{
					fclose(face_data);
					free(p_face);
					//continue;
				}
				else
				{
					fwrite(p_face, sizeof(char), face_size, face_data);
					fclose(face_data);
					free(p_face);
				}
			}

			//显示抓拍识别结果
			p_menu->p_recognize_result_buff_ui[recg_i].SetShowRecognizeResult(face_info, face_snap_path, face_recg_path);
			p_menu->p_recognize_result_item[recg_i].setSizeHint(SNAP_RECG_ITEM_SIZE);
			p_menu->ui.listWidget_nowRecognize->addItem(&p_menu->p_recognize_result_item[recg_i]);
			p_menu->ui.listWidget_nowRecognize->setItemWidget(&p_menu->p_recognize_result_item[recg_i],
				&p_menu->p_recognize_result_buff_ui[recg_i]);
			p_menu->ui.listWidget_nowRecognize->scrollToBottom();

			recg_i++;
			if (recg_i >= CAMERA_SNAP_RECOGNIZE_MAX_NUMS)
			{
				recg_i = 0;
				//p_menu->ui.listWidget_nowSnap->clear();
			}
#if OPEN_SAVE_RECG_RET
			if (strlen(face_info.recg_people_name) != 0)
			{
				QString recg_dir = QString(CAMERA_RECG_IMAGE_PATH) + QString('/') + QString::fromLocal8Bit(face_info.recg_people_name);
				if (!FileOperator::DirIsExisted(recg_dir))
				{
					qDebug() << "test recg dir failed...";
				}
				else
				{
					//检测文件夹是否创建
					QString path = recg_dir + QString('/') + QString::fromLocal8Bit(face_info.recg_people_name) + ".json";
					QString context = QString("\"") + QString(BODY_RECG_TIME) + QString("\":\"%1\",\n\"").arg(epoch_time);
					context.append(BODY_RECG_CAMID);
					context.append("\":");
					int camera_id = 0;
					QString camera_ip;
					p_menu->QueryCameraInfoByChanid(face_result->channel_id, camera_id, camera_ip);
					context.append(QString("%1,\n\"").arg(camera_id));
					context.append(QString(BODY_RECG_CAMIP) + QString("\":\""));
					context.append(camera_ip + QString("\""));

					p_menu->SaveSnapRecgInfo(path, context);

					//存储抓拍大图
					QString big_path = recg_dir + QString("/%1_big.jpg").arg(epoch_time);
					QFile big_snap_file(big_face_snap_path);
					big_snap_file.copy(big_path);

					//存储抓拍小图
					QString copy_path = recg_dir + QString("/%1.jpg").arg(epoch_time);
					QFile snap_file(face_snap_path);
					snap_file.copy(copy_path);
				}
			}

			//查看识别到的人员所对应库的类型
			int db_type = p_menu->QueryFaceLibTypeById(face_result->face_items[i].recg_face_lib_id);
			if (db_type == 2)   //黑名单用户
			{
				QString name_ = QString::fromLocal8Bit(face_info.recg_people_name);
				p_menu->WarningRecordUsername(name_);
			}
#endif
		}

		//显示抓拍结果
		p_menu->p_snap_result_buff_ui[snap_i].SetShowData(snap_info, face_snap_path);
		p_menu->p_snap_result_item[snap_i].setSizeHint(SNAP_ITEM_SIZE);
		p_menu->ui.listWidget_nowSnap->addItem(&p_menu->p_snap_result_item[snap_i]);
		p_menu->ui.listWidget_nowSnap->setItemWidget(&p_menu->p_snap_result_item[snap_i],
			&p_menu->p_snap_result_buff_ui[snap_i]);
		p_menu->ui.listWidget_nowSnap->scrollToBottom();

		snap_i++;
		if (snap_i >= CAMERA_SNAP_RESULT_MAX_NUMS)
		{
			snap_i = 0;
			//p_menu->ui.listWidget_nowSnap->clear();
		}
	}
}

//切换系统功能模式（通过索引切换）
void MainMenu::ChangeSystemMode(int index)
{
    ui.stackedWidget_systemMode->setCurrentIndex(index);
}

//将设置系统模式的按钮全部清空
void MainMenu::CleanAllSetSystemModeButton()
{
    ui.pushButton_onlineMonitoring->setStyleSheet(SYSTEM_MODE_BUTTON_ENABLE_STYLE);
    ui.pushButton_cameraManage->setStyleSheet(SYSTEM_MODE_BUTTON_ENABLE_STYLE);
    ui.pushButton_libraryManage->setStyleSheet(SYSTEM_MODE_BUTTON_ENABLE_STYLE);
    ui.pushButton_snapResult->setStyleSheet(SYSTEM_MODE_BUTTON_ENABLE_STYLE);
    ui.pushButton_trackPath->setStyleSheet(SYSTEM_MODE_BUTTON_ENABLE_STYLE);
    ui.pushButton_smartTest->setStyleSheet(SYSTEM_MODE_BUTTON_ENABLE_STYLE);

    ui.pushButton_onlineMonitoring->setEnabled(true);
    ui.pushButton_cameraManage-> setEnabled(true);
    ui.pushButton_libraryManage->setEnabled(true);
    ui.pushButton_snapResult->setEnabled(true);
    ui.pushButton_trackPath->setEnabled(true);
    ui.pushButton_smartTest->setEnabled(true);
}

//显示抓拍信息
void MainMenu::DisplaySnapInformation(FaceSnapInfo *face_info, QString *image_path)
{
	qDebug() << "callback emit signal...";
	DisplaySnapResult  *p_snap_result = new DisplaySnapResult;
	p_snap_result->SetShowData(*face_info, *image_path);

	QListWidgetItem *item = new QListWidgetItem;
	item->setSizeHint(SNAP_ITEM_SIZE);
	ui.listWidget_nowSnap->addItem(item);
	//ui.listWidget_nowSnap->setItemWidget(item, p_snap_result);
	//ui.listWidget_nowSnap->scrollToBottom();
}

//清空抓拍和抓拍识别列表框
void MainMenu::CleanSnapAndRecgResultList()
{
	for (int i = ui.listWidget_nowSnap->count() - 1; i >= 0; i--)
	{
		QListWidgetItem *item = ui.listWidget_nowSnap->item(i);
		ui.listWidget_nowSnap->removeItemWidget(item);
		delete item;
	}

	for (int i = ui.listWidget_nowRecognize->count() - 1; i >= 0; i--)
	{
		QListWidgetItem *item = ui.listWidget_nowRecognize->item(i);
		ui.listWidget_nowRecognize->removeItemWidget(item);
		delete item;
	}
}

//刷新人脸库列表,同时刷新显示用户人脸信息
void MainMenu::RefreshUserGroupList()
{
	if (!vzbox_online_status)
		return;

	RefreshFaceLibInfoMap();
	ui.listWidget_userGroupList->clear();

	auto it = face_lib_info_map.begin();
	while (it != face_lib_info_map.end())
	{
		ui.listWidget_userGroupList->addItem(it.key());
		++it;
	}
}

//刷新记录人脸数据库信息的map
void MainMenu::RefreshFaceLibInfoMap()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

	VZ_FACE_LIB_RESULT face_lib_list;
	int ret = VzClient_SearchFaceRecgLib(vzbox_handle_, &face_lib_list);
	if (ret != VZSDK_SUCCESS)
	{
		return;
	}
	face_lib_info_map.clear();
	for (int i = 0; i < face_lib_list.lib_count; i++)
	{
		FaceLibInfo lib_info;

		QString lib_name = QString::fromLocal8Bit(face_lib_list.lib_items[i].name);
		lib_info.threshold_value = face_lib_list.lib_items[i].threshold_value;
		lib_info.id = face_lib_list.lib_items[i].id;
		lib_info.enable = face_lib_list.lib_items[i].enable;
		lib_info.lib_type = face_lib_list.lib_items[i].lib_type;
		strcpy(lib_info.name, face_lib_list.lib_items[i].name);
		strcpy(lib_info.remark, face_lib_list.lib_items[i].remark);
		face_lib_info_map[lib_name] = lib_info;
	}
}

//刷新显示用户信息列表
void MainMenu::RefreshUserInfoList()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

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

//按页显示用户信息
void MainMenu::DisplaynPageUserInfoList(int group_id, int page_num)
{
	CleanAllUserInfoItem();

    int cur_page_num = PAGE_SUB_NUM;
	do
	{
		VZ_FACE_LIB_SEARCH_CONDITION condition = { 0 };
		condition.page_num = page_num * PAGE_SUB_NUM - cur_page_num + 1;
		condition.page_count = PAGE_USER_NUM_BACE;

		VZ_FACE_USER_RESULT cur_group_toal_user_info_ = { 0 };
		int ret = VzClient_SearchFaceRecgUser(vzbox_handle_, &condition, group_id, &cur_group_toal_user_info_);
		if (ret != VZSDK_SUCCESS)
		{
			msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取用户信息失败！"));
			return;
		}

		if (page_num == 1 && cur_page_num == PAGE_SUB_NUM)
		{
			//更新当前库的总人数
			group_cur_total_face_num_ = cur_group_toal_user_info_.total_count;
			//计算当前库中有多少页用户
			if (cur_group_toal_user_info_.total_count % PAGE_USERINFO_NUM != 0)
			{
				user_list_cur_page_total_ = group_cur_total_face_num_ / PAGE_USERINFO_NUM + 1;
			}
			else
			{
				user_list_cur_page_total_ = group_cur_total_face_num_ / PAGE_USERINFO_NUM;
			}
		}

		qDebug() << "All user:" << group_cur_total_face_num_;
		qDebug() << "Cur face:" << cur_group_toal_user_info_.face_count;
        user_detail_info_map.clear();
		for (int i = 0; i < cur_group_toal_user_info_.face_count; i++)
		{
			QImage img;
			QString face_path(FACE_IMAGE_CACHE_PATH);
			face_path.append(QString("/face%1.jpg").arg(cur_group_toal_user_info_.face_items[i].pic_index));
			FILE *face_data = fopen(face_path.toStdString().c_str(), "wb+");
			if (face_data)   //通过url获取用户人脸图
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
            UserInfo user_info;
            strcpy(user_info.user_name, cur_group_toal_user_info_.face_items[i].user_name);
            strcpy(user_info.db_name, cur_group_toal_user_info_.face_items[i].db_name);
            strcpy(user_info.card_number, cur_group_toal_user_info_.face_items[i].card_number);
            strcpy(user_info.group_id, cur_group_toal_user_info_.face_items[i].group_id);
            strcpy(user_info.phone, cur_group_toal_user_info_.face_items[i].phone);
            strcpy(user_info.province, cur_group_toal_user_info_.face_items[i].province);
            strcpy(user_info.city, cur_group_toal_user_info_.face_items[i].city);
            strcpy(user_info.birthday, cur_group_toal_user_info_.face_items[i].birthday);
            strcpy(user_info.address, cur_group_toal_user_info_.face_items[i].address);
            strcpy(user_info.img_url, cur_group_toal_user_info_.face_items[i].img_url);
            user_info.face_id = cur_group_toal_user_info_.face_items[i].face_id;
            user_info.pic_index = cur_group_toal_user_info_.face_items[i].pic_index;
            user_info.user_type = cur_group_toal_user_info_.face_items[i].user_type;
            user_info.sex = cur_group_toal_user_info_.face_items[i].sex;
            int pic_idx = user_info.pic_index;
            user_detail_info_map[pic_idx] = user_info;

			QListWidgetItem *p_user_list_item = new QListWidgetItem(ui.listWidget_userInfoList);
			DisplayUserInfoItem *p_user_info_item = new DisplayUserInfoItem(QString::fromLocal8Bit(cur_group_toal_user_info_.face_items[i].user_name), 
				img, cur_group_toal_user_info_.face_items[i].pic_index);
			connect(p_user_info_item, &DisplayUserInfoItem::DelUserInfoSignal, this, &MainMenu::DealDeleteOneUser);
			p_user_list_item->setSizeHint(USERINFO_ITEM_SIZE);
			ui.listWidget_userInfoList->addItem(p_user_list_item);
			ui.listWidget_userInfoList->setItemWidget(p_user_list_item, p_user_info_item);
		}
		if (cur_group_toal_user_info_.face_count < PAGE_USER_NUM_BACE)
		{
			break;
		}
	} while (cur_page_num-- > 1);

	ui.label_totalUserInfo->clear();
	ui.label_totalUserInfo->setText(QString::fromLocal8Bit("共 %2 页     %1 条记录    当前第 %3 页").arg(group_cur_total_face_num_)
									.arg(user_list_cur_page_total_).arg(user_list_cur_page_num_));
}

//当前选中的人脸库,需要记录并刷新库中人员信息
void MainMenu::CurrentSelectFaceLib(QListWidgetItem * item)
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

	QString lib_name = item->text();
	auto it = face_lib_info_map.find(lib_name);
	if (it == face_lib_info_map.end())
	{
		return;
	}

	group_cur_id_ = it.value().id;
	user_list_cur_page_num_ = 1;
	DisplaynPageUserInfoList(group_cur_id_);
}

//操作一个用户信息（添加一个用户或者修改用户信息，通过传参来区分）
void MainMenu::AddOneUserInformation()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

    if (p_operator_user_ui == NULL)
    {
        p_operator_user_ui = new UserInformationOperator(ADD_USER);
    }

    connect(p_operator_user_ui, &UserInformationOperator::OperUserInfo, this, &MainMenu::DealOperatorUserInfo);
    connect(p_operator_user_ui, &UserInformationOperator::CancelOperUser, [=]() {
        delete p_operator_user_ui;
        p_operator_user_ui = NULL;
    });

    p_operator_user_ui->show();
}

//修改用户信息
void MainMenu::ModifyOneUserInformation()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

    UserInfo user;
    if (p_operator_user_ui == NULL)
    {
        p_operator_user_ui = new UserInformationOperator(user, MODIFY_USER);
    }

    connect(p_operator_user_ui, &UserInformationOperator::OperUserInfo, this, &MainMenu::DealOperatorUserInfo);
    connect(p_operator_user_ui, &UserInformationOperator::CancelOperUser, [=]() {
        delete p_operator_user_ui;
        p_operator_user_ui = NULL;
    });

    p_operator_user_ui->show();
}

//处理操作用户信息（添加用户或者修改了用户的信息）
void MainMenu::DealOperatorUserInfo(UserInfo & user, USER_OPER oper)
{
    //调用user相关SDK
	int ret = ~0;
	qDebug() << "ret:" << ret;
	VZ_FACE_USER_RESULT user_info = { 0 };
	user_info.face_count = 1;
	user_info.total_count = 1;
	user_info.face_items[0].sex = user.sex;
	strcpy(user_info.face_items[0].user_name, user.user_name);
	strcpy(user_info.face_items[0].birthday, user.birthday);
	strcpy(user_info.face_items[0].phone, user.phone);
	strcpy(user_info.face_items[0].province, user.province);
	strcpy(user_info.face_items[0].city, user.city);
	strcpy(user_info.face_items[0].address, user.address);
	char lib_id[50] = { 0 };
	sprintf(lib_id, "%d", group_cur_id_);
	strcpy(user_info.face_items[0].group_id, lib_id);
	qDebug() << "url:" << QString(user.img_url) << "lib_id:" << QString(lib_id);
	QImage image;
	image.load(QString(user.img_url));
	user_info.face_items[0].pic_data = (char *)image.bits();

	if (oper == ADD_USER)
	{
		ret = VzClient_FaceRecgUserAdd(vzbox_handle_, &user_info);
	}
	else if (oper == MODIFY_USER)
	{
		ret = VzClient_FaceRecgUserEdit(vzbox_handle_, &user_info);
	}

    //校验检测结果
    if (ret != VZSDK_SUCCESS)
    {
        if (oper == ADD_USER)   p_operator_user_ui->ShowMessage(ADD_USER_FAILED);
        else                    p_operator_user_ui->ShowMessage(MODIFY_USER_FAILED);

        return;
    }

    if (oper == ADD_USER)   p_operator_user_ui->ShowMessage(ADD_USER_SUCCESS);
    else                    p_operator_user_ui->ShowMessage(MODIFY_USER_SUCCESS);
    
    p_operator_user_ui->close();
    delete p_operator_user_ui;
    p_operator_user_ui = NULL;

}

//清空用户信息中的全部框
void MainMenu::CleanAllUserInfoItem()
{
	for (int i = ui.listWidget_userInfoList->count()-1; i >=0; i--)
	{
		QListWidgetItem *item = ui.listWidget_userInfoList->item(i);
		ui.listWidget_userInfoList->removeItemWidget(item);
		delete item;
	}
}

//添加一个人脸库
void MainMenu::AddOneFaceLibInfo()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

    if (p_operator_lib_ui == NULL)
    {
        p_operator_lib_ui = new FaceLibInfoOperator;
    }
	else
	{
		delete p_operator_lib_ui;
		p_operator_lib_ui = NULL;
		p_operator_lib_ui = new FaceLibInfoOperator;
	}

    p_operator_lib_ui->show();
    connect(p_operator_lib_ui, &FaceLibInfoOperator::CancelLibOper, [=]() {
        qDebug() << "main canael face lib oper";
        p_operator_lib_ui->close();
        delete p_operator_lib_ui;
        p_operator_lib_ui = NULL;
    });
    connect(p_operator_lib_ui, &FaceLibInfoOperator::SaveAddLibOper, this, &MainMenu::DealAddOneFaceLib);
 
}

//处理添加一个人脸库
void MainMenu::DealAddOneFaceLib(FaceLibInfo & face_info)
{
    if (!vzbox_online_status)
        return;

    VZ_FACE_LIB_ITEM lib_info;
    lib_info.threshold_value = face_info.threshold_value;
    lib_info.id = face_info.id;
    lib_info.enable = face_info.enable;
    lib_info.lib_type = face_info.lib_type;
    strcpy(lib_info.name, face_info.name);
    strcpy(lib_info.remark, face_info.remark);

    int ret = VzClient_FaceRecgLibOperate(vzbox_handle_, &lib_info, 1);
    if (ret == VZSDK_FAILED)
    {
        p_operator_lib_ui->ShowErrorMessage(ERROR_MSG, QString::fromLocal8Bit("新建人脸库失败！"));
        return;
    }

    p_operator_lib_ui->ShowErrorMessage(REMARK_MSG, QString::fromLocal8Bit("创建人脸库成功！"));
    RefreshUserGroupList();
	delete p_operator_lib_ui;
	p_operator_lib_ui = NULL;
}

//修改一个人脸库
void MainMenu::ModifyOneFaceLibInfo()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

    if (p_operator_lib_ui == NULL)
    {
        QListWidgetItem *item = ui.listWidget_userGroupList->currentItem();
        p_operator_lib_ui = new FaceLibInfoOperator(face_lib_info_map[item->text()]);
    }
	else
	{
		delete p_operator_lib_ui;
		p_operator_lib_ui = NULL;
		QListWidgetItem *item = ui.listWidget_userGroupList->currentItem();
		p_operator_lib_ui = new FaceLibInfoOperator(face_lib_info_map[item->text()]);	
	}

    connect(p_operator_lib_ui, &FaceLibInfoOperator::CancelLibOper, [=]() {
        delete p_operator_lib_ui;
        p_operator_lib_ui = NULL;
    });
    connect(p_operator_lib_ui, &FaceLibInfoOperator::SaveModifyLibOper, this, &MainMenu::DealModifyOneFaceLib);

    p_operator_lib_ui->show();
}

//处理修改人脸库信息
void MainMenu::DealModifyOneFaceLib(FaceLibInfo & face_info)
{
    if (!vzbox_online_status)
        return;

    VZ_FACE_LIB_ITEM lib_info;
    lib_info.threshold_value = face_info.threshold_value;
    lib_info.id = face_info.id;
    lib_info.enable = face_info.enable;
    lib_info.lib_type = face_info.lib_type;
    strcpy(lib_info.name, face_info.name);
    strcpy(lib_info.remark, face_info.remark);

    int ret = VzClient_FaceRecgLibOperate(vzbox_handle_, &lib_info, 3);
    if (ret == VZSDK_FAILED)
    {
        p_operator_lib_ui->ShowErrorMessage(ERROR_MSG, QString::fromLocal8Bit("修改人脸库失败！"));
        return;
    }

    p_operator_lib_ui->ShowErrorMessage(REMARK_MSG, QString::fromLocal8Bit("修改人脸库成功！"));
    RefreshUserGroupList();
	delete p_operator_lib_ui;
	p_operator_lib_ui = NULL;
}

//删除选中的人脸库
void MainMenu::DeleteOneFaceLib()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

    QListWidgetItem *item = ui.listWidget_userGroupList->currentItem();
    if (item->text().isEmpty())
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未选中人脸库！"));
        return;
    }

    QMessageBox::StandardButton btn_tmp = msg_box_.question(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("确定删除人脸库 %1").arg(item->text()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (btn_tmp == QMessageBox::No)
    {
        return;
    }

    auto it = face_lib_info_map.find(item->text());
    if (it == face_lib_info_map.end())
    {
        return;
    }

    VZ_FACE_LIB_ITEM lib_info;
    lib_info.threshold_value = it.value().threshold_value;
    lib_info.id = it.value().id;
    lib_info.enable = it.value().enable;
    lib_info.lib_type = it.value().lib_type;
    strcpy(lib_info.name, it.value().name);
    strcpy(lib_info.remark, it.value().remark);
    
    int ret = VzClient_FaceRecgLibOperate(vzbox_handle_, &lib_info, 2);
    if(ret!=VZSDK_SUCCESS)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("删除人脸库失败！"));
        return;
    }
    msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("删除人脸库成功！"));
    RefreshUserGroupList();

}

//通过库id查找对应库类型
int MainMenu::QueryFaceLibTypeById(int db_id)
{
	auto it = face_lib_info_map.begin();
	while (it != face_lib_info_map.end())
	{
		if (db_id == it.value().id)
		{
			return it.value().lib_type;
		}
		++it;
	}
	return -1;
}

//删除一个用户
void MainMenu::DealDeleteOneUser(int pic_idx)
{
	int ret = VzClient_FaceRecgUserDelete(vzbox_handle_, pic_idx);
	if (ret != VZSDK_SUCCESS)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("删除人脸用户失败！"));
		return;
	}

	msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("删除人脸用户成功！"));
	DisplaynPageUserInfoList(group_cur_id_, user_list_cur_page_num_);
}

//载入建筑平面图照片
void MainMenu::LoadBuildingMapImage()
{	
	QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片"), OPEN_IMAGE_DIR,
		tr("Images (*.png *.jpg);; All files (*.*)"));
	
	if (!img_path.isEmpty())
	{
		ResetCreateNewBuildingMap();
		ui.label_buildingMap->CleanSelectedBuildingMap();
		ui.comboBox_existedBuindingMap->setCurrentIndex(0);

		building_map_image_path = img_path;
		QPixmap pix(building_map_image_path);
		float scaled_x = pix.width() * 1.0 / ui.label_buildingMap->width();
		float scaled_y = pix.height() * 1.0 / ui.label_buildingMap->height();
		ui.label_buildingMap->SetScaledParam(scaled_x, scaled_y);
		ui.label_buildingMap->setPixmap(pix.scaled(ui.label_buildingMap->width(), ui.label_buildingMap->height()));		
		ui.label_buildingMap->SetOperatorMode(BUILDING_CREATE_MAP);
	}
	else
	{
		ui.label_buildingMap->clear();
		building_map_image_path.clear();
		ui.label_buildingMap->SetOperatorMode(BUILDING_SHOW_MAP);
		ui.label_buildingMap->CleanSelectedBuildingMap();
		ui.comboBox_existedBuindingMap->setCurrentIndex(0);
	}
}

//窗口变化时，建筑图像刷新显示
void MainMenu::RefreshBuildMapDisplay()
{
	if (!building_map_image_path.isEmpty())
	{
		QPixmap pix(building_map_image_path);
		float scaled_x = pix.width() * 1.0 / ui.label_buildingMap->width();
		float scaled_y = pix.height() * 1.0 / ui.label_buildingMap->height();
		ui.label_buildingMap->SetScaledParam(scaled_x, scaled_y);
		ui.label_buildingMap->setPixmap(pix.scaled(ui.label_buildingMap->width(), ui.label_buildingMap->height()));
	}
}

//处理放置相机事件
void MainMenu::DealPlaceCameraPosition(QPoint pt)
{
	if (pt == QPoint(0, 0))
	{
		return;
	}

	QString camera_ip = ui.comboBox_selectCamera->currentText();
	QString camera_place_name = ui.lineEdit_cameraPositionName->text();

	place_camera_num++;
	QString cur_camera_place_info;

	if (place_camera_num <= 1)
	{
		cur_camera_place_info.append(QString("{\n  \"camera_id\":%1,\n").arg(connected_camera_map[camera_ip].camera_id));
	}
	else
	{
		cur_camera_place_info.append(QString(",{\n  \"camera_id\":%1,\n").arg(connected_camera_map[camera_ip].camera_id));
	}

	cur_camera_place_info.append(QString("  \"camera_ip\":\"%1\",\n").arg(camera_ip));
	cur_camera_place_info.append(QString("  \"position_name\":\"%1\",\n").arg(camera_place_name));
	cur_camera_place_info.append(QString("  \"camera_position_x\":%1,\n").arg(pt.x()));
	cur_camera_place_info.append(QString("  \"camera_position_y\":%1\n}").arg(pt.y()));

	ui.textEdit_cameraPlaceInfo->append(cur_camera_place_info);
	place_camera_info_buff.push_back(cur_camera_place_info);
	ui.lineEdit_cameraPositionName->clear();
}

//放置一个相机
void MainMenu::DealPlaceOneCamera()
{
	if (ui.comboBox_selectCamera->currentText().isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请选择相机后再放置相机！"));
		return;
	}
	ui.label_buildingMap->RecodeCurrentCameraPoint();
}

//刷新显示放置相机位置的信息
void MainMenu::RefreshShowPlaceCameraInfo()
{
	if (place_camera_info_buff.size() != place_camera_num)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("放置相机信息与数量不符！"));
	}

	ui.textEdit_cameraPlaceInfo->clear();
	auto it = place_camera_info_buff.begin();
	while (it != place_camera_info_buff.end())
	{
		ui.textEdit_cameraPlaceInfo->append(*it);
		++it;
	}
}

//撤销一个已经放置的相机
void MainMenu::CalcelPlacedOneCamera()
{
	if (place_camera_num > 0)
	{
		place_camera_num--;
		auto it = place_camera_info_buff.end();
		it--;
		place_camera_info_buff.erase(it);
	}
	RefreshShowPlaceCameraInfo();
}

//重置新建地图
void MainMenu::ResetCreateNewBuildingMap()
{
	CleanCreatingNewBuinldingMap();
}

//保存新建立的建筑物地图
void MainMenu::SaveNewBuildingMap()
{
	if (ui.lineEdit_buildingMapName->text().isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请设置新地图的名称！"));
		return;
	}
	if (ui.textEdit_cameraPlaceInfo->toPlainText().isEmpty() || building_map_image_path.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("没有载入文件或者没有放置相机！"));
		return;
	}

	//获取框中数据，并构建json格式
	QString building_map_name = ui.lineEdit_buildingMapName->text();
	QString place_camera_info = QString("{\"camera_info\":[") + ui.textEdit_cameraPlaceInfo->toPlainText() + QString("],");
	place_camera_info.append("\"camera_name\":\"");
	place_camera_info.append(building_map_name);
	place_camera_info.append("\",");
	place_camera_info.append("\"camera_num\":");
	place_camera_info.append(QString("%1").arg(place_camera_num));
	place_camera_info.append("}");

	//新地图存储目录
	QString save_path = QString(BUILDING_MAP_FILE_PATH) + QString('\\') + building_map_name;
	QDir dir;
	if (!dir.exists(save_path))   //目录不存在时，创建目录
	{
		bool res = dir.mkpath(save_path);
	}
	else
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图%1已存在，请重新设置！").arg(building_map_name));
		return;
	}

	QString file_path = save_path + QString("/%1.json").arg(building_map_name);
	QFile file(file_path);
	bool ret = file.open(QIODevice::WriteOnly | QIODevice::Text);
	if (!ret)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图信息 %1 保存失败！").arg(building_map_name));
		return;		
	}
	file.write(place_camera_info.toUtf8());
	file.close();

	QPixmap pix(building_map_image_path);
	QString image_path = save_path + QString("/%1.jpg").arg(building_map_name);
	ret = pix.save(image_path);
	if (!ret)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图图片 %1 保存失败！").arg(building_map_name));
		return;
	}

	msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("地图创建成功！"));
	UpdateExistedBuildingMapList();
	CleanCreatingNewBuinldingMap();
	ui.lineEdit_buildingMapName->clear();
}

//清空正在创建的地图
void MainMenu::CleanCreatingNewBuinldingMap()
{
	building_map_image_path.clear();
	place_camera_num = 0;
	place_camera_info_buff.clear();
	ui.textEdit_cameraPlaceInfo->clear();
	ui.label_buildingMap->clear();
	ui.label_buildingMap->CleanPlaceingNewBuinldingMap();
	ui.lineEdit_buildingMapName->clear();
	ui.lineEdit_cameraPositionName->clear();
}

//删除已保存的地图
void MainMenu::DeleteExistedBuildingMap()
{
	QString cur_map = ui.comboBox_existedBuindingMap->currentText();
	if (cur_map.compare(QString::fromLocal8Bit("请选择")) == 0 || cur_map.isEmpty())
	{
		return;
	}
	QString del_map = QString(BUILDING_MAP_FILE_PATH) + QString('\\') + cur_map;
	QDir dir;
	dir.setPath(del_map);
	dir.removeRecursively();
	msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("删除地图 %1 成功！").arg(cur_map));
	UpdateExistedBuildingMapList();
	ui.label_buildingMap->clear();
	ui.label_buildingMap->CleanPlaceingNewBuinldingMap();
}

//更新已有的地图列表
void MainMenu::UpdateExistedBuildingMapList()
{
	existed_building_map.clear();
	ui.comboBox_existedBuindingMap->clear();

	QDir dir(BUILDING_MAP_FILE_PATH);
	dir.setFilter(QDir::Dirs);
	foreach(QFileInfo fullDir, dir.entryInfoList())
	{
		if (fullDir.fileName() == "." || fullDir.fileName() == "..") continue;
		existed_building_map.push_back(fullDir.fileName());
	}
	ui.comboBox_existedBuindingMap->addItem(QString::fromLocal8Bit("请选择"));
	auto it = existed_building_map.begin();
	while (it != existed_building_map.end())
	{
		ui.comboBox_existedBuindingMap->addItem(*it);
		++it;
	}
}

//更新已有的行人轨迹图
void MainMenu::UpdateSavedPersonTrackList()
{

}

//刷新已连接的相机下拉框列表
void MainMenu::UpdateConnectedCameraList()
{
	ui.comboBox_selectCamera->clear();

	auto it = camera_list_buff.begin();
	while (it != camera_list_buff.end())
	{
		ui.comboBox_selectCamera->addItem(*it);
		++it;
	}
}

//处理绘制行人轨迹按钮
void MainMenu::DrawAfterUserRecgedTrack()
{
	if (ui.lineEdit_searchPersonTrack->text().isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请输入要绘制轨迹的用户名！"));
		return;
	}
	if (ui.comboBox_existedBuindingMap->currentIndex()==0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请选择地图后再绘制轨迹！"));
		return;
	}

	QString user_name = ui.lineEdit_searchPersonTrack->text();
	DrawUserRunTrack(user_name);
}

//显示当前选中的相机ID
void MainMenu::ShowCurrentSelectCameraId(const QString &cam_ip)
{
	auto it = connected_camera_map.find(cam_ip);
	if (it == connected_camera_map.end())
	{
		return;
	}

	ui.label_placeCameraId->setText(QString("%1").arg(it.value().camera_id));
}

//加载已有的楼宇地图
void MainMenu::LoadExistedBuildingMap(const QString &select)
{
	if (select.compare(QString::fromLocal8Bit("请选择")) == 0 || select.isEmpty())
	{
		return;
	}

	CleanCreatingNewBuinldingMap();
	QString cur_map = QString(BUILDING_MAP_FILE_PATH) + QString('\\') + select;
	QString cur_image = cur_map + QString("/%1.jpg").arg(select);
	QString cur_info= cur_map + QString("/%1.json").arg(select);

	QFile file(cur_info);
	int ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!ret)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图文件已损坏或不存在！"));
		return;
	}
	QByteArray sinfo = file.readAll();
	if (sinfo.isEmpty())
	{
		file.close();
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图文件已损坏！"));
		return;
	}
	file.close();

	building_map_image_path = cur_image;
	QString camera_info(sinfo);
	QPixmap pix(cur_image);
	float scaled_x = pix.width() * 1.0 / ui.label_buildingMap->width();
	float scaled_y = pix.height() * 1.0 / ui.label_buildingMap->height();
	ui.label_buildingMap->SetScaledParam(scaled_x, scaled_y);
	ui.label_buildingMap->setPixmap(pix.scaled(ui.label_buildingMap->width(), ui.label_buildingMap->height()));
	ui.label_buildingMap->SetOperatorMode(BUILDING_SHOW_MAP);
	ui.label_buildingMap->DealSelectedBuildingMap(camera_info);
}

//绘制对应用户的抓拍轨迹
void MainMenu::DrawUserRunTrack(const QString & user_name)
{
	if (user_name.isEmpty())
	{
		return;
	}

	ReadSnapRectJsonFile(user_name);
}

//读取抓拍识别的json文件，根据文件内容绘制轨迹图
void MainMenu::ReadSnapRectJsonFile(const QString & user_name)
{
	QString user_recg_dir = QString(CAMERA_RECG_IMAGE_PATH) + QString('/') + user_name + QString('/') + user_name;
	user_recg_dir.append(".json");
	QFile file(user_recg_dir);
	int ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!ret)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("查找该用户抓拍记录失败！"));
		return;
	}

	QByteArray sinfo = file.readAll();
	if (sinfo.isEmpty())
	{
		file.close();
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("该用户抓拍记录为空！"));
		return;
	}
	file.close();

	QString snap_recg_info;

	snap_recg_info.append(QString("{\"")+QString(BODY_RECG_NAME)+ QString("\":"));
	snap_recg_info.append(QString("\"")+user_name+ QString("\","));

	snap_recg_info.append(QString("\"")+QString(BODY_RECG_INFO)+ QString("\":"));
	snap_recg_info.append("[");
	snap_recg_info.append(sinfo);
	snap_recg_info.append("]}");

	if (ui.checkBox_openRunTrackAllTimes->isChecked())
	{
		ui.label_buildingMap->SetDrawTrackData(BUILDING_SHOW_TRACK, snap_recg_info);
	}
	else
	{
		qint64 time_start = ui.dateTimeEdit_runTrackStartTime->dateTime().toMSecsSinceEpoch();
		qint64 time_end = ui.dateTimeEdit_runTrackEndTime->dateTime().toMSecsSinceEpoch();
		qDebug() << "start_ time:" << ui.dateTimeEdit_runTrackStartTime->dateTime() 
			     << "  end_ time:" << ui.dateTimeEdit_runTrackEndTime->dateTime();
		qDebug() << "start time:" << time_start << "  end time:" << time_end;
		if (time_start > time_end)
		{
			msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("开始时间不能大于结束时间！"));
			return;
		}
		ui.label_buildingMap->SetDrawTrackData(BUILDING_SHOW_TRACK, snap_recg_info, time_start, time_end);
	}
}

//切换绘制轨迹时间选择，可以绘制全部，也可绘制一定时间段
void MainMenu::DealDrawUserRunTrackTimeMode(int status)
{
	if (status == Qt::Checked)
	{
		ui.dateTimeEdit_runTrackStartTime->setEnabled(false);
		ui.dateTimeEdit_runTrackEndTime->setEnabled(false);
	}
	else if (status == Qt::Unchecked)
	{
		ui.dateTimeEdit_runTrackStartTime->setEnabled(true);
		ui.dateTimeEdit_runTrackEndTime->setEnabled(true);
		QDateTime date_time = QDateTime::currentDateTime();
		ui.dateTimeEdit_runTrackEndTime->setDateTime(date_time);
		ui.dateTimeEdit_runTrackStartTime->setDateTime(date_time.addDays(-1));
	}
}

//显示绘制轨迹可能出现的错误
void MainMenu::DealShowErrorMsg(int msg_idx)
{
	if (error_camera_position_info == msg_idx)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图文件中相机信息被损坏！"));
	}
	else if (error_camera_position_json == msg_idx)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("地图文件信息被损坏！"));
	}
	else if (error_snap_recg_user_json == msg_idx)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("抓拍与识别信息被损坏！"));
	}
	else if (error_snap_recg_user_null == msg_idx)
	{
		if (ui.checkBox_openRunTrackAllTimes->isChecked())
		{
			msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("绘制的用户记录为空！"));
		}
		else
		{
			msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("绘制的时间段内用户记录为空！"));
		}
	}
	else if (error_snap_recg_track_null == msg_idx)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("用户轨迹为空，请绘制完成后再查看链式图！"));
	}
}

//调整链式轨迹界面，紧贴主界面显示
void MainMenu::DealAdjustLinkTrackPosition()
{
	ui.label_buildingMap->MoveListTrackWindow(this->x() + this->width(), this->y(), 300, this->height());
}

//更新已连接的相机map
void MainMenu::UpdateConnectedCameraInfoMap()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

	VZ_BOX_CAM_GROUP camera_list;
	int ret = VzClient_GetCamGroupParam(vzbox_handle_, &camera_list);
	if (ret != VZSDK_SUCCESS)
	{
		qDebug() << QString::fromLocal8Bit("获取相机属性失败");
		return;
	}

	connected_camera_map.clear();
	for (int i = 0; i < camera_list.cam_count; ++i)
	{
		QString cam_ip(camera_list.cam_items[i].ip);
		CameraAttribute cam_attri;
		cam_attri.camera_id = 0;
		cam_attri.channel_id = camera_list.cam_items[i].chn_id + 1;

		strcpy(cam_attri.camera_item.ip, camera_list.cam_items[i].ip);
		strcpy(cam_attri.camera_item.username, camera_list.cam_items[i].username);
		strcpy(cam_attri.camera_item.password, camera_list.cam_items[i].password);
		strcpy(cam_attri.camera_item.rtsp_url, camera_list.cam_items[i].rtsp_url);
		strcpy(cam_attri.camera_item.rtsp_url_sub, camera_list.cam_items[i].rtsp_url_sub);
		strcpy(cam_attri.camera_item.type, camera_list.cam_items[i].type);

		cam_attri.camera_item.http_port = camera_list.cam_items[i].http_port;
		cam_attri.camera_item.rtsp_port = camera_list.cam_items[i].rtsp_port;
		cam_attri.channel_id = camera_list.cam_items[i].chn_id;
		cam_attri.camera_item.enable_snaps = camera_list.cam_items[i].enable_snaps;
		cam_attri.camera_item.enable_video = camera_list.cam_items[i].enable_video;

		connected_camera_map[cam_ip] = cam_attri;
	}
	UpdateConnectedCameraIpList();
    RefreshDisplayCameraList();
	ReadCameraConfigParamFile();
	UpdateConnectedCameraList();
}

//刷新已连接相机IP列表
void MainMenu::UpdateConnectedCameraIpList()
{
	camera_list_buff.clear();

	auto it = connected_camera_map.begin();
	while (it != connected_camera_map.end())
	{
		camera_list_buff.push_back(it.key());
		++it;
	}
}

//读取相机配置信息文件
void MainMenu::ReadCameraConfigParamFile()
{
    QString cam_cfg_path = QString(CAMERA_CONFIG_PARAM_PATH) + QString('/') + QString(CAMERA_CONFIG_PARAM_NAME);
    QFile file(cam_cfg_path);
    int ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!ret)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取相机配置失败！"));
        return;
    }
    QByteArray sinfo = file.readAll();
    file.close();

	QString  cam_cfg(sinfo);
	Json::Value jcam_cfg;
	if (String2Json(cam_cfg.toStdString(), jcam_cfg) == false)
	{
		return;
	}

	int size = jcam_cfg[BODY_CAM_NUM].asInt();
	if (size != jcam_cfg[BODY_CAM_ATT].size())
	{
		return;
	}

	for (int i = 0; i < size; i++)
	{
		QString cam_ip = QString(jcam_cfg[BODY_CAM_ATT][i][BODY_CAM_IP].asString().c_str());
		auto it = connected_camera_map.find(cam_ip);
		if (it == connected_camera_map.end())
		{
			continue;
		}

		int  cam_id = jcam_cfg[BODY_CAM_ATT][i][BODY_CAM_ID].asInt();
		it.value().camera_id = cam_id;
	}
}

//保存相机配置文件
void MainMenu::SaveCameraConfigParamFile()
{
    QDir dir;
    if (!dir.exists(QString(CAMERA_CONFIG_PARAM_PATH)))   //目录不存在时，创建目录
    {
        dir.mkpath(QString(CAMERA_CONFIG_PARAM_PATH));
    }

	QString cam_cfg = QString(CAMERA_CONFIG_PARAM_PATH) + QString('/') + QString(CAMERA_CONFIG_PARAM_NAME);
	QFile file(cam_cfg);
	file.remove();
	bool ret = file.open(QIODevice::WriteOnly | QIODevice::Text);
	if (!ret)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("相机配置信息保存失败！"));
		return;
	}
	if (connected_camera_map.size() == 0)
	{
		file.close();
		return;
	}

	QString jcam_cfg;
	jcam_cfg.append(QString("{\n  \"camera_num\":%1,").arg(connected_camera_map.size()));
	jcam_cfg.append("\n  \"camera_attribute\":[{");
	QMap<QString, CameraAttribute>::iterator it = connected_camera_map.begin();
	while (it != connected_camera_map.end())
	{
		if (it == connected_camera_map.begin())
		{
			jcam_cfg.append(QString("\n    \"cam_id\":%1,").arg(it.value().camera_id));
		}
		else
		{
			jcam_cfg.append(QString(",\n  {\n    \"cam_id\":%1,").arg(it.value().camera_id));
		}

		jcam_cfg.append(QString("\n    \"chn_id\":%1,").arg(it.value().channel_id));
		jcam_cfg.append(QString("\n    \"http_port\":%1,").arg(it.value().camera_item.http_port));
		jcam_cfg.append(QString("\n    \"cam_ip\":\"%1\",").arg(it.key()));
		jcam_cfg.append(QString("\n    \"cam_name\":\"%1\",").arg(it.value().camera_item.name));
		jcam_cfg.append(QString("\n    \"cam_type\":\"%1\",").arg(it.value().camera_item.type));
		jcam_cfg.append(QString("\n    \"user_name\":\"%1\",").arg(it.value().camera_item.username));
		jcam_cfg.append(QString("\n    \"password\":\"%1\",").arg(it.value().camera_item.password));
		jcam_cfg.append(QString("\n    \"rtsp_url\":\"%1\",").arg(it.value().camera_item.rtsp_url));
		jcam_cfg.append(QString("\n    \"rtsp_url_sub\":\"%1\"}").arg(it.value().camera_item.rtsp_url_sub));

		++it;
	}
	jcam_cfg.append("\n  ]\n}");
	file.write(jcam_cfg.toUtf8());
	file.close();
}

//当前选中的IP相机属性
void MainMenu::CurrentSelectCameraItem(QListWidgetItem * item)
{
    cur_selected_camera_ip = item->text();
    auto it = connected_camera_map.find(cur_selected_camera_ip);
    if (it == connected_camera_map.end())
    {
        return;
    }

    //将相机参数加载到界面中
    ui.lineEdit_cameraManageId->setText(QString("%1").arg(it.value().camera_id));
    ui.lineEdit_cameraManageChannel->setText(QString("%1").arg(it.value().channel_id));
    ui.lineEdit_cameraManageIp->setText(QString(it.key()));
    ui.lineEdit_cameraManageHttpPort->setText(QString("%1").arg(it.value().camera_item.http_port));
    ui.comboBox_protocolType->setCurrentText(QString(it.value().camera_item.type));
    ui.lineEdit_cameraManageUserName->setText(QString(it.value().camera_item.username));
    ui.lineEdit_cameraManagePassword->setText(QString(it.value().camera_item.password));
    ui.lineEdit_cameraRtspAddress->setText(QString(it.value().camera_item.rtsp_url));
    ui.lineEdit_cameraRtspSubAddress->setText(QString(it.value().camera_item.rtsp_url_sub));
	Qt::CheckState status = it.value().camera_item.enable_snaps ? status = Qt::Checked : Qt::Unchecked;
	ui.checkBox_imageStream->setCheckState(status);
	status = it.value().camera_item.enable_video ? status = Qt::Checked : Qt::Unchecked;
	ui.checkBox_videoStream->setCheckState(status);
}

//添加一个相机
void MainMenu::AddOneConnectCamera()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

    if (p_add_camera_ui == NULL)
    {
        p_add_camera_ui = new AddCamera;
    }
    connect(p_add_camera_ui, &AddCamera::CancelAddCamera, [=]() {
        delete p_add_camera_ui;
        p_add_camera_ui = NULL;
    });
    connect(p_add_camera_ui, &AddCamera::ConnectAddCamera, this, &MainMenu::CheckAddCameraIsExisted);
    p_add_camera_ui->show();

}

//校验添加的相机是否存在
void MainMenu::CheckAddCameraIsExisted(CameraAttribute & cam_param)
{
	auto it = connected_camera_map.find(QString(cam_param.camera_item.ip));
	if (it != connected_camera_map.end())
	{
		p_add_camera_ui->ShowMessage(ADD_CAMERA_EXISTED);
		return;
	}

	int ret = VzClient_BoxAddCam(vzbox_handle_, &cam_param.camera_item);
	if (ret != VZSDK_SUCCESS)
	{
		p_add_camera_ui->ShowMessage(ADD_CAMERA_FAILED);
		return;
	}

    //假如校验成功
    p_add_camera_ui->ShowMessage(ADD_CAMERA_SUCCESS);
    p_add_camera_ui->close();
    delete p_add_camera_ui;
    p_add_camera_ui = NULL;
    UpdateConnectedCameraInfoMap();
    SaveCameraConfigParamFile();
}

//修改已连接的相机
void MainMenu::ModifySelectedCamera()
{
	int cam_id = 0;
	if (!ui.lineEdit_cameraManageId->text().isEmpty())
	{
		cam_id = ui.lineEdit_cameraManageId->text().toInt();
	}
	//qDebug() << "cur cam_id:" << cam_id;
	bool enable_image = ui.checkBox_imageStream->isChecked();
	bool enable_video = ui.checkBox_videoStream->isChecked();
		
	auto it = connected_camera_map.find(cur_selected_camera_ip);
	if (it == connected_camera_map.end())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("修改相机属性失败！"));
		return;
	}
	
	it.value().camera_id = cam_id;
	it.value().camera_item.enable_snaps = enable_image;
	it.value().camera_item.enable_video = enable_video;

	int ret = VzClient_BoxSetCam(vzbox_handle_, it.value().channel_id, &it.value().camera_item);
	if (ret != VZSDK_SUCCESS)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("修改相机属性失败！"));
		return;
	}

	msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("修改%1相机属性成功！").arg(cur_selected_camera_ip));
    //修改相机参数后刷新相机列表
	SaveCameraConfigParamFile();
    UpdateConnectedCameraInfoMap();
}

//删除已连接的相机
void MainMenu::DeleteSelectedCamera()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(VIDEO_DEVICE_OFFLINE));
		return;
	}

	auto it = connected_camera_map.find(cur_selected_camera_ip);
	if (it == connected_camera_map.end())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未选中相机！"));
		return;
	}
	const char *cam_ip = cur_selected_camera_ip.toStdString().c_str();
	int ret = VzClient_BoxRemoveCams(vzbox_handle_, &cam_ip, 1);
	if (ret != VZSDK_SUCCESS)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("删除相机失败！"));
		return;
	}

	msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("移除%1相机成功！").arg(cur_selected_camera_ip));
	//connected_camera_map.erase(it);
    //删除相机后刷新相机列表
	SaveCameraConfigParamFile();
    UpdateConnectedCameraInfoMap();
}

//创建智能测试引擎
void MainMenu::CreateAItestEngine()
{
    pFaceEngine = FaceEngineClass::GetInstance();
}

//载入人脸对比图片1
void MainMenu::LoadCompareImg1()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    if (!img_path.isEmpty())
    {
        comparePath1 = img_path;
        QPixmap pix;
        Geometric_Scaling_Image(comparePath1, ui.label_compareImage1->width(), ui.label_compareImage1->height(), pix);
        ui.label_compareImage1->setPixmap(pix);
        ui.toolButton_compareFaceResult->setText(" ");
    }
    else
    {
        ui.label_compareImage1->clear();
        comparePath1.clear();
    }
}

//载入人脸对比图片2
void MainMenu::LoadCompareImg2()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    if (!img_path.isEmpty())
    {
        comparePath2 = img_path;
        QPixmap pix;
        Geometric_Scaling_Image(comparePath2, ui.label_compareImage2->width(), ui.label_compareImage2->height(), pix);
        ui.label_compareImage2->setPixmap(pix);
        ui.toolButton_compareFaceResult->setText(" ");
    }
    else
    {
        ui.label_compareImage2->clear();
        comparePath2.clear();
    }
}

//人脸对比处理
void MainMenu::DealFaceCompare()
{
    //qDebug() << QString::fromLocal8Bit("人脸比对");
    IplImage* img1 = cvLoadImage(comparePath1.toLocal8Bit());

    if (img1 == NULL)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("照片一为空！"));
        return;
    }
    ASF_MultiFaceInfo faceDetectResult;
    int res = pFaceEngine->FacesDetectTask(img1, &faceDetectResult);
    ASF_FaceFeature feature;
    res = pFaceEngine->FaceFeatureExtract(img1, faceDetectResult, &feature);

    ASF_FaceFeature feature_copy;
    feature_copy.feature = (MByte*)malloc(feature.featureSize);
    memcpy(feature_copy.feature, feature.feature, feature.featureSize);
    feature_copy.featureSize = feature.featureSize;

    IplImage* img2 = cvLoadImage(comparePath2.toLocal8Bit());
    if (img2 == NULL)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("照片二为空！"));
        return;
    }
    ASF_MultiFaceInfo faceDetectResult2;
    res = pFaceEngine->FacesDetectTask(img2, &faceDetectResult2);
    res = pFaceEngine->FaceFeatureExtract(img2, faceDetectResult2, &feature);

    MFloat confidenceValue;
    res = pFaceEngine->FaceCompareTask(feature, feature_copy, &confidenceValue);

    QString str = QString("%1:%2%").arg(QString::fromLocal8Bit("相似度")).arg(QString::number(confidenceValue * 100, 'f', 2));

    ui.toolButton_compareFaceResult->setStyleSheet("background-color:white;color:red");
    ui.toolButton_compareFaceResult->setText(str);
    free(feature_copy.feature);
    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
}

//载入人脸检测图片
void MainMenu::LoadDetectImg()
{
    ui.treeWidget_detectResult->clear();
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    if (!img_path.isEmpty())
    {
        detectPath = img_path;
        QPixmap pix;
        Geometric_Scaling_Image(detectPath, ui.label_detectImage->width(), ui.label_detectImage->height(), pix);
        ui.label_detectImage->setPixmap(pix);
    }
    else
    {
        ui.label_detectImage->clear();
        detectPath.clear();
    }

    if (p_display_detect_ui != NULL)
    {
        ui.pushButton_viewDetectResult->setEnabled(false);
        delete p_display_detect_ui;
        p_display_detect_ui = NULL;
    }
}

//人脸检测处理
void MainMenu::DealFaceDetect()
{
    //qDebug() << QString::fromLocal8Bit("人脸检测");

    ui.treeWidget_detectResult->clear();
  
    IplImage* img = cvLoadImage(detectPath.toLocal8Bit());
    if (img == NULL)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("照片为空！"));
        return;
    }

    FaceDetectResult face_detect_result;
    int res = pFaceEngine->FacesDetectTask(img, &face_detect_result, detectImage);

    QString detPath = QString(FACE_IMAGE_CACHE_PATH) + "/detectImage.jpg";
    cv::imwrite(detPath.toStdString(), detectImage);
    detectPath = detPath;
    //ui.label_detectImage->clear();
    //QPixmap pix;
    //Geometric_Scaling_Image(detPath, ui.label_detectImage->width(), ui.label_detectImage->height(), pix);
    //ui.label_detectImage->setPixmap(pix);

    QTreeWidgetItem *num_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("人脸个数")
        << QString::number(face_detect_result.detectInfo.faceNum));
    ui.treeWidget_detectResult->addTopLevelItem(num_item);

    for (int i = 0; i < face_detect_result.detectInfo.faceNum; i++)
    {
        //人脸信息根节点
        QString str = QString("%1%2%3").arg(QString::fromLocal8Bit("人脸")).arg(QString::number(i + 1)).arg(QString::fromLocal8Bit("信息"));
        QTreeWidgetItem *info_item = new QTreeWidgetItem(QStringList() << str);
        ui.treeWidget_detectResult->addTopLevelItem(info_item);

        //性别
        QString sex_str;
        if (face_detect_result.genderInfo.genderArray[i] == 0)
        {
            sex_str = QString("%1").arg(QString::fromLocal8Bit("男"));
        }
        else if (face_detect_result.genderInfo.genderArray[i] == 1)
        {
            sex_str = QString("%1").arg(QString::fromLocal8Bit("女"));
        }
        else
        {
            sex_str = QString("%1").arg(QString::fromLocal8Bit("未知"));
        }
        QTreeWidgetItem *sex_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("性别") << sex_str);
        info_item->addChild(sex_item);

        //年龄信息
        QTreeWidgetItem *age_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("年龄")
            << QString::number(face_detect_result.ageInfo.ageArray[i]));
        info_item->addChild(age_item);

        //人脸位置信息
        QTreeWidgetItem *location_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("人脸位置"));
        info_item->addChild(location_item);

        QTreeWidgetItem *left_location_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("left")
            << QString::number(face_detect_result.detectInfo.faceRect[i].left));
        location_item->addChild(left_location_item);

        QTreeWidgetItem *right_location_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("right")
            << QString::number(face_detect_result.detectInfo.faceRect[i].right));
        location_item->addChild(right_location_item);

        QTreeWidgetItem *top_location_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("top")
            << QString::number(face_detect_result.detectInfo.faceRect[i].top));
        location_item->addChild(top_location_item);

        QTreeWidgetItem *bottom_location_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("bottom")
            << QString::number(face_detect_result.detectInfo.faceRect[i].bottom));
        location_item->addChild(bottom_location_item);

        //人脸3D信息
        QTreeWidgetItem *angle_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("3D信息"));
        info_item->addChild(angle_item);

        QTreeWidgetItem *pitch_angle_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("pitch")
            << QString::number(face_detect_result.angleInfo.pitch[i], 'f', 2));
        angle_item->addChild(pitch_angle_item);

        QTreeWidgetItem *roll_angle_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("roll")
            << QString::number(face_detect_result.angleInfo.roll[i], 'f', 2));
        angle_item->addChild(roll_angle_item);

        QTreeWidgetItem *yaw_angle_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("yaw")
            << QString::number(face_detect_result.angleInfo.yaw[i], 'f', 2));
        angle_item->addChild(yaw_angle_item);
    }
    //ui.button_display_result->setEnabled(true);
    cvReleaseImage(&img);
    ui.pushButton_viewDetectResult->setEnabled(true);
}

//显示人脸检测大图
void MainMenu::ShowFaceDetectResult()
{
    if (p_display_detect_ui == NULL)
    {
        p_display_detect_ui = new DisplayDetectResult(detectPath);   //这里需要传参，识别结果图像
    }

    p_display_detect_ui->show();
}

//载入人脸识别图片
void MainMenu::LoadRecognizeImg()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    if (!img_path.isEmpty())
    {
        recognizePath = img_path;
        QPixmap pix;
        Geometric_Scaling_Image(recognizePath, ui.label_recognizeImage->width(), ui.label_recognizeImage->height(), pix);
        ui.label_recognizeImage->setPixmap(pix);
        //ui.toolButton_compareFaceResult->setText(" ");
    }
    else
    {
        ui.label_recognizeImage->clear();
        recognizePath.clear();
    }

    if (p_display_recognize_ui != NULL)
    {
        delete p_display_recognize_ui;
        p_display_recognize_ui = NULL;
        ui.pushButton_viewRecognizeResult->setEnabled(false);
    }
}

//人脸识别处理
void MainMenu::DealFaceRecognize()
{

    ui.pushButton_viewRecognizeResult->setEnabled(true);
}

//显示人脸识别结果
void MainMenu::ShowFaceRecognizeResult()
{
    if (p_display_recognize_ui == NULL)
    {
        p_display_recognize_ui = new DisplayRecognizeResult;   //这里需要传参，识别结果数据
    }

    p_display_recognize_ui->show();
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
	//qDebug() << "-----------MainMenu resize event";
	RefreshVideoDisplayWindow();
	RefreshBuildMapDisplay();
}

//关闭系统主窗口
void MainMenu::closeEvent(QCloseEvent * event)
{

}

//事件过滤器
bool MainMenu::eventFilter(QObject * watched, QEvent * event)
{
	//if (watched == ui.label_buildingMap)
	//{
	//	//if (event->type() == QEvent::Paint)
	//	//{
	//	//	//ui.label_buildingMap->paintEvent((QPaintEvent*)event);
	//	//	//DisplayPlacedCameraPosition();
	//	//	return true;
	//	//}
	//	//else 
	//	if (event->type() == QEvent::Resize)
	//	{
	//		qDebug() << "-----------MainMenu eventFilter event";
	//		return true;
	//	}
	//} 

	return QWidget::eventFilter(watched, event);
}

//处理鼠标单击视频窗口
void MainMenu::DealSingleClickedVideoLabel(int chn)
{
	//qDebug() << "mouse single clicked video" << chn;
	ChangeOneVideoStyle(chn);
}

//处理鼠标双击视频窗口
void MainMenu::DealDoubleClickedVideoLabel(int chn)
{
	//qDebug() << "mouse double clicked video" << chn;
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






