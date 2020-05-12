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
			int G = (pY[yIdx] - 16) - 0.69825 * (pV[uIdx] - 128) - 0.33557 * (pU[vIdx] - 128);                         // g����
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
		ui.listWidget_userInfoList->setViewMode(QListView::IconMode);
		ui.listWidget_userInfoList->setResizeMode(QListWidget::Adjust);
		ui.listWidget_userInfoList->setMovement(QListWidget::Static);

		
	}

/***************************************************���������********************************************************/
	{

	}

/**************************************************ץ�Ĳ�ѯ����********************************************************/
	{

	}

/**************************************************�켣�鿴����********************************************************/
	{

	}

/**************************************************���ܲ��Խ���********************************************************/
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
		if (video_data_size[*chnnal_id] != pFrame->height * pFrame->width * 3)  //��ֹframe_size�仯���µĳ������
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
}

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
	RefreshVideoDisplayWindow();
}

//�ر�ϵͳ������
void MainMenu::closeEvent(QCloseEvent * event)
{

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






