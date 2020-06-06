#include "MainMenu.h"
#include <QPushButton>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>

QMutex video_cache_mutex;
unsigned char  *video_data[CAMERA_NUM_LIMIT];               //�洢ԭ��ͼ������ָ��
unsigned int   video_data_size[CAMERA_NUM_LIMIT] = { 0 };   //��¼ԭʼ���ݴ�С����̬����ռ䣬��ֹ�ڴ�Ƿ�����
unsigned int   video_chnId[CAMERA_NUM_LIMIT] = { 0 };       //��¼���ͨ����
QQueue<QImage> video_frame_cache_[CAMERA_NUM_LIMIT];        //��Ƶ����

//�ȱ�����ͼ��
//����1��ͼ��·����IN��
//����2���ռ��ȣ�IN��
//����3���ؼ��߶ȣ�IN��
//����4�����ź���ͼ��OUT��
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

			int R = (pY[yIdx] - 16) + 1.370805 * (pV[uIdx] - 128);                                                     // r����	
			int G = (pY[yIdx] - 16) - 0.69825  * (pV[uIdx] - 128) - 0.33557 * (pU[vIdx] - 128);                        // g����
			int B = (pY[yIdx] - 16) + 1.733221 * (pU[vIdx] - 128);                                                     // b����

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

//����������
void ClearVideoFrameCache()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_frame_cache_[i].clear();
		video_data_size[i] = 0;
	}
}

//�����湹�캯��
MainMenu::MainMenu(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

/**************************************************ϵͳ������ȫ�ֳ�ʼ��********************************************************/
	{
		//��ʼ����ʾͼ��
		SetIconInit();
		ChangeSystemMode(0);

        //Ϊ��ͼ��ʾ�ؼ���װ�¼�������
        ui.label_showBuildingMap->installEventFilter(this);

		//ͨ����ť�л�ϵͳ����
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


/**************************************************��Ƶ��ؽ���********************************************************/
	{
		videoTimer = NULL;
		vzbox_online_status = false;
		display_video_windows_num_ = FOUR_WINDOWS;
		video_display_label = NULL;

		ui.toolButton_oneWindow->setEnabled(true);
		ui.toolButton_fourWindows->setEnabled(false);
		ui.toolButton_nineWindows->setEnabled(true);

		//��ʼ����ʾ��Ƶ����label
		video_display_label = new DisplayVideoLabel[CAMERA_NUM_LIMIT];
		if (video_display_label == NULL)
		{
			msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��Ƶ��ʾ�ؼ���ʼ��ʧ�ܣ�"));
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

		//���豸��ť
		connect(ui.pushButton_openDev, &QPushButton::clicked, this, &MainMenu::DealOpenVzbox);

		//�ر��豸��ť
		connect(ui.pushButton_closeDev, &QPushButton::clicked, this, &MainMenu::DealCloseVzbox);

		//��ʾ��Ƶ����
		RefreshVideoDisplayWindow();

		//�޸Ĵ�����ʾ��Ƶ�ĸ���   
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

		//һ������ȫ����Ƶ
		connect(ui.pushButton_allVideoPlay, &QPushButton::clicked, this, &MainMenu::DealAutoPlayAllVideo);

		//ֹͣ���ŵ�ȫ����Ƶ
		connect(ui.pushButton_allVideoStop, &QPushButton::clicked, this, &MainMenu::DealStopPlayAllVideo);

		//������Ƶ����ʱ����
		connect(&video_show_timer_, &QTimer::timeout, this, &MainMenu::DealPlayVideoTimer);
	}
    
/**************************************************������ý���********************************************************/
	{
		
		
	}

/***************************************************���������********************************************************/
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

/**************************************************ץ�Ĳ�ѯ����********************************************************/
	{

	}

/**************************************************�켣�鿴����********************************************************/
	{

        connect(ui.pushButton_loadMapImage, &QPushButton::clicked, this, &MainMenu::LoadBuildingMapImage);
<<<<<<< HEAD
        //connect(ui.label_showBuildingMap,&BuildingMapLabel::selectPosition,this,&)
=======
>>>>>>> 3a392791294542a0460206aadfa70ef24ce26518
	}

/**************************************************���ܲ��Խ���********************************************************/
	{
        ui.treeWidget_detectResult->setHeaderLabels(QStringList() << QString::fromLocal8Bit("����") << QString::fromLocal8Bit("��ϸ��Ϣ"));
        ui.treeWidget_recognizeResult->setHeaderLabels(QStringList() << QString::fromLocal8Bit("����") << QString::fromLocal8Bit("��ϸ��Ϣ"));

        CreateAItestEngine();

        connect(ui.pushButton_loadCompareImage1,  &QPushButton::clicked, this, &MainMenu::LoadCompareImg1);
        connect(ui.pushButton_loadCompareImage2,  &QPushButton::clicked, this, &MainMenu::LoadCompareImg2);
        connect(ui.pushButton_compareFace,        &QPushButton::clicked, this, &MainMenu::DealFaceCompare);
        connect(ui.pushButton_loadDetectImage,    &QPushButton::clicked, this, &MainMenu::LoadDetectImg);
        connect(ui.pushButton_detectFace,         &QPushButton::clicked, this, &MainMenu::DealFaceDetect);
        connect(ui.pushButton_loadRecognizeImage, &QPushButton::clicked, this, &MainMenu::LoadRecognizeImg);
        connect(ui.pushButton_recognizeFace,      &QPushButton::clicked, this, &MainMenu::DealFaceRecognize);
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

//���ð�ťͼ��
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

//���豸
void MainMenu::DealOpenVzbox()
{
	if (vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�豸�Ѵ򿪣�"));
		return;
	}
	vzbox_ip_ = ui.lineEdit_devIp->text();
	vzbox_port_ = ui.lineEdit_devPort->text();
	vzbox_user_name_ = ui.lineEdit_userName->text();
	vzbox_password_ = ui.lineEdit_password->text();
	if (vzbox_ip_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("IP������Ϊ�գ�"));
		return;
	}

	if (vzbox_port_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�˿ںŲ�����Ϊ�գ�"));
		return;
	}

	if (vzbox_user_name_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�û���������Ϊ�գ�"));
		return;
	}

	if (vzbox_password_.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("���벻����Ϊ�գ�"));
		return;
	}

	int res = VzLPRClient_Setup();
	if (res != 0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�豸��ʼ��ʧ�ܣ�"));
		return;
	}

	vzbox_handle_ = VzLPRClient_Open(vzbox_ip_.toUtf8(),
		vzbox_port_.toInt(),
		vzbox_user_name_.toUtf8(),
		vzbox_password_.toUtf8());
	if (vzbox_handle_ == 0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("���豸ʧ�ܣ�"));
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

//�ر��豸
void MainMenu::DealCloseVzbox()
{
	if (vzbox_handle_ == 0)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("û�д򿪵��豸��"));
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

//ˢ������б�
void MainMenu::RefreshCameraList()
{
	VZ_BOX_CAM_GROUP camera_list;
	int ret = VzClient_GetCamGroupParam(vzbox_handle_, &camera_list);
	if (ret != VZSDK_SUCCESS)
	{
		qDebug() << QString::fromLocal8Bit("��ȡ����б�ʧ��");
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

//ˢ����Ƶ��ʾ���ڣ���ʾ��ʽ��
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

//ˢ����Ƶ��ʾ������ʽ������������
void MainMenu::RefreshVideoDisplayStyle()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].setStyleSheet(DISPLAY_LABEL_STYLE);
	}
}

//�����ѹ��ص�ȫ���������Ƶ
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

//�ر�������ʾ����Ƶ
void MainMenu::CloseAllVideoDisplay()
{
	CloseAllCameraHandle();
	RefreshVideoDisplayWindow();
}

//�ر������Ѵ򿪵����
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

//���������Ƶ��ʾ
void MainMenu::CleanAllDisplayWindows()
{
	for (int i = 0; i < CAMERA_NUM_LIMIT; i++)
	{
		video_display_label[i].hide();
	}
}

//��������ʾ��Ƶģʽ������ʾ��1·��
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

//�Ĵ�����ʾ��Ƶģʽ����ʾǰ4·��
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

//�Ŵ�����ʾ��Ƶģʽ����ʾǰ9·��
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

//��ʾһ·��Ƶ����chnIdָ����
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

//�ı�һ·��Ƶ������ʽ����chnIdָ����
void MainMenu::ChangeOneVideoStyle(int chnId)
{
	RefreshVideoDisplayStyle();
	video_display_label[chnId].setStyleSheet(ClICKED_LABEL_STYLE);
}

//�����Ƶ�ص�����
void MainMenu::VideoFrameCallBack(VzLPRClientHandle handle, void * pUserData, const VzYUV420P * pFrame)
{
	int* chnnal_id = (int *)pUserData;
	//qDebug() << "static VideoFrameCallBack chnnal:" << *chnnal_id;

	//Ϊframe����ռ�
	if (video_data[*chnnal_id] == NULL)
	{
		qDebug() << "malloc chnnalId:" << *chnnal_id;
		video_data_size[*chnnal_id] = pFrame->height * pFrame->width * 3;
		video_data[*chnnal_id] = (unsigned char*)malloc(video_data_size[*chnnal_id]);
		memset(video_data[*chnnal_id], 0, video_data_size[*chnnal_id]);
	}
	else
	{
		if (video_data_size[*chnnal_id] != pFrame->height * pFrame->width * 3)  //������֤����Ŀռ䣬��ֹframe_size�仯���µĳ������
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

	//Ϊframe����ռ�
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

//�л�ϵͳ����ģʽ��ͨ�������л���
void MainMenu::ChangeSystemMode(int index)
{
    ui.stackedWidget_systemMode->setCurrentIndex(index);
}

//ˢ���������б�
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

//ˢ���û���Ϣ�б�
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
		ui.label_totalUserInfo->setText(QString::fromLocal8Bit("�� %1 ����¼").arg(user_face_info.total_count));
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
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ȡ�û���Ϣʧ�ܣ�"));
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
	ui.label_totalUserInfo->setText(QString::fromLocal8Bit("��%2 ҳ     %1 ����¼    ��ǰ %3 ҳ").arg(cur_group_toal_user_info_.total_count)
									.arg(user_list_cur_page_total_).arg(user_list_cur_page_num_));

}

//�������ܲ�������
void MainMenu::CreateAItestEngine()
{
    pFaceEngine = FaceEngineClass::GetInstance();
}

//���������Ա�ͼƬ1
void MainMenu::LoadCompareImg1()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��ͼƬ"), OPEN_IMAGE_DIR,
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

//���������Ա�ͼƬ2
void MainMenu::LoadCompareImg2()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��ͼƬ"), OPEN_IMAGE_DIR,
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

//�����Աȴ���
void MainMenu::DealFaceCompare()
{
    qDebug() << QString::fromLocal8Bit("�����ȶ�");
    IplImage* img1 = cvLoadImage(comparePath1.toLocal8Bit());

    if (img1 == NULL)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ƬһΪ�գ�"));
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
        msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��Ƭ��Ϊ�գ�"));
        return;
    }
    ASF_MultiFaceInfo faceDetectResult2;
    res = pFaceEngine->FacesDetectTask(img2, &faceDetectResult2);
    res = pFaceEngine->FaceFeatureExtract(img2, faceDetectResult2, &feature);

    MFloat confidenceValue;
    res = pFaceEngine->FaceCompareTask(feature, feature_copy, &confidenceValue);

    QString str = QString("%1:%2%").arg(QString::fromLocal8Bit("���ƶ�")).arg(QString::number(confidenceValue * 100, 'f', 2));

    ui.toolButton_compareFaceResult->setStyleSheet("background-color:white;color:red");
    ui.toolButton_compareFaceResult->setText(str);
    free(feature_copy.feature);
    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
}

//�����������ͼƬ
void MainMenu::LoadDetectImg()
{
    ui.treeWidget_detectResult->clear();
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��ͼƬ"), OPEN_IMAGE_DIR,
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
}

//������⴦��
void MainMenu::DealFaceDetect()
{
    qDebug() << QString::fromLocal8Bit("�������");

    ui.treeWidget_detectResult->clear();

    //if (display_detect_result_ != NULL)
    //{
    //    delete display_detect_result_;
    //    display_detect_result_ = NULL;
    //}

    IplImage* img = cvLoadImage(detectPath.toLocal8Bit());
    if (img == NULL)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ƬΪ�գ�"));
        return;
    }

    FaceDetectResult face_detect_result;
    int res = pFaceEngine->FacesDetectTask(img, &face_detect_result, detectImage);

    QString detPath = "./faceImageCache/detectImage.jpg";
    cv::imwrite(detPath.toStdString(), detectImage);
    ui.label_detectImage->clear();
    QPixmap pix;
    Geometric_Scaling_Image(detPath, ui.label_detectImage->width(), ui.label_detectImage->height(), pix);
    ui.label_detectImage->setPixmap(pix);

    QTreeWidgetItem *num_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("��������")
        << QString::number(face_detect_result.detectInfo.faceNum));
    ui.treeWidget_detectResult->addTopLevelItem(num_item);

    for (int i = 0; i < face_detect_result.detectInfo.faceNum; i++)
    {
        //������Ϣ���ڵ�
        QString str = QString("%1%2%3").arg(QString::fromLocal8Bit("����")).arg(QString::number(i + 1)).arg(QString::fromLocal8Bit("��Ϣ"));
        QTreeWidgetItem *info_item = new QTreeWidgetItem(QStringList() << str);
        ui.treeWidget_detectResult->addTopLevelItem(info_item);

        //�Ա�
        QString sex_str;
        if (face_detect_result.genderInfo.genderArray[i] == 0)
        {
            sex_str = QString("%1").arg(QString::fromLocal8Bit("��"));
        }
        else if (face_detect_result.genderInfo.genderArray[i] == 1)
        {
            sex_str = QString("%1").arg(QString::fromLocal8Bit("Ů"));
        }
        else
        {
            sex_str = QString("%1").arg(QString::fromLocal8Bit("δ֪"));
        }
        QTreeWidgetItem *sex_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("�Ա�") << sex_str);
        info_item->addChild(sex_item);

        //������Ϣ
        QTreeWidgetItem *age_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("����")
            << QString::number(face_detect_result.ageInfo.ageArray[i]));
        info_item->addChild(age_item);

        //����λ����Ϣ
        QTreeWidgetItem *location_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("����λ��"));
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

        //����3D��Ϣ
        QTreeWidgetItem *angle_item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("3D��Ϣ"));
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
    cvReleaseImage(&img);
}

//��������ʶ��ͼƬ
void MainMenu::LoadRecognizeImg()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��ͼƬ"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    if (!img_path.isEmpty())
    {
        recognizePath = img_path;
        QPixmap pix;
        Geometric_Scaling_Image(recognizePath, ui.label_recognizeImage->width(), ui.label_recognizeImage->height(), pix);
        ui.label_recognizeImage->setPixmap(pix);
    }
    else
    {
        ui.label_recognizeImage->clear();
        recognizePath.clear();
    }
}

//����ʶ����
void MainMenu::DealFaceRecognize()
{
}

//����¥��ƽ��ͼ
void MainMenu::LoadBuildingMapImage()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��ͼƬ"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    newBuildingMapPath = img_path;

<<<<<<< HEAD
    qDebug() << "main before show   w:" << ui.label_showBuildingMap->width() << "  h:" << ui.label_showBuildingMap->width();
    
    ui.label_showBuildingMap->SetShowImage(newBuildingMapPath);

    qDebug() << "main after  show   w:" << ui.label_showBuildingMap->width() << "  h:" << ui.label_showBuildingMap->width();

    QImage img(newBuildingMapPath);
    float scaled_x = img.width()  * 1.0 / ui.label_showBuildingMap->width();
    float scaled_y = img.height() * 1.0 / ui.label_showBuildingMap->height();
    ui.label_showBuildingMap->SetImageScaled(scaled_x, scaled_y);
=======
    ui.label_showBuildingMap->SetShowImage(newBuildingMapPath);
    QImage img(newBuildingMapPath);
    float scaled_x = img.width()  / ui.label_showBuildingMap->width();
    float scaled_y = img.height() / ui.label_showBuildingMap->height();
    ui.label_showBuildingMap->SetImageScaled(scaled_x, scaled_y);
    //if (!img_path.isEmpty())
    //{
    //    QImage img(newBuildingMapPath);
    //    ui.label_showBuildingMap->setPixmap(QPixmap::fromImage(img.scaled(ui.label_showBuildingMap->width(),ui.label_showBuildingMap->height())));
    //}
    //else
    //{
    //    ui.label_showBuildingMap->clear();
    //    newBuildingMapPath.clear();
    //}
>>>>>>> 3a392791294542a0460206aadfa70ef24ce26518
}

//ˢ����ʾ��ͼ
void MainMenu::RefreshBuildMap()
{
    ui.label_showBuildingMap->RefreshDisplayImage();
    QImage img(newBuildingMapPath);
<<<<<<< HEAD
    float scaled_x = img.width()  * 1.0 / ui.label_showBuildingMap->width();
    float scaled_y = img.height() * 1.0 / ui.label_showBuildingMap->height();
    ui.label_showBuildingMap->SetImageScaled(scaled_x, scaled_y);
}

void MainMenu::DealPlaceCamera(QPoint pt)
{
}

=======
    float scaled_x = img.width() / ui.label_showBuildingMap->width();
    float scaled_y = img.height() / ui.label_showBuildingMap->height();
    ui.label_showBuildingMap->SetImageScaled(scaled_x, scaled_y);
}

>>>>>>> 3a392791294542a0460206aadfa70ef24ce26518
//����ϵͳ����
void MainMenu::paintEvent(QPaintEvent * event)
{
    //QPainter painter(this);
    //QPixmap pix;
    //pix.load("./icon/MainBackGround.jpg");
    ////ָ������
    //painter.drawPixmap(0, 0, this->width(), this->height(), pix);
}

//�ı��Сʱˢ����Ƶ����
void MainMenu::resizeEvent(QResizeEvent * event)
{
    qDebug() << "Mainmenu windows resize...";
	RefreshVideoDisplayWindow();
<<<<<<< HEAD
    RefreshBuildMap();
=======
    //RefreshBuildMap();
>>>>>>> 3a392791294542a0460206aadfa70ef24ce26518
}

//�ر�ϵͳ������
void MainMenu::closeEvent(QCloseEvent * event)
{

}

//�¼�������
bool MainMenu::eventFilter(QObject * watched, QEvent * event)
{
    return false;
}

//������굥����Ƶ����
void MainMenu::DealSingleClickedVideoLabel(int chn)
{
	qDebug() << "mouse single clicked video" << chn;
	ChangeOneVideoStyle(chn);
}

//�������˫����Ƶ����
void MainMenu::DealDoubleClickedVideoLabel(int chn)
{
	qDebug() << "mouse double clicked video" << chn;
	ShowOneChnVideo(chn);
}

//һ������ȫ����Ƶ
void MainMenu::DealAutoPlayAllVideo()
{
	if (!vzbox_online_status)
	{
		msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("û�д򿪵��豸��"));
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

//ֹͣ���ŵ���Ƶ�������ر��Ѵ򿪵����
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

//ͨ����ʱ�����жϣ���ʾ�����Ƶ
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






