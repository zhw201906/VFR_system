#pragma once

#include <QWidget>
#include "ui_MainMenu.h"
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QVector>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <functional> 
#include <QImage>

#include "DisplayVideoLabel.h"
#include "sdkHeadFile.h"
#include "DisplayVideoThread.h"
#include "DisplayUserInfoItem.h"

#define  CAMERA_NUM_LIMIT      9
#define  FRAME_NUM_SIZE_LIMIT  3

#define  PAGE_USERINFO_NUM     50
#define  USERINFO_ITEM_SIZE    QSize(198, 261)

#define  VIDEO_FRAME_RATE      25
#define  ONE_WINDOWS    1
#define  FOUR_WINDOWS   2
#define  NINE_WINDOWS   3

#define  DISPLAY_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);background-color: rgb(200, 200, 200);}"
#define  ClICKED_LABEL_STYLE   "QLabel{border:2px solid rgb(255, 0, 0);background-color: rgb(200, 200, 200);}"


typedef struct user_group_info {


}UserGroupInfo;


typedef struct user_info {


}UserInfo;

class MainMenu : public QWidget
{
	Q_OBJECT


public:
	MainMenu(QWidget *parent = Q_NULLPTR);
	~MainMenu();

    void SetIconInit();
	void DealOpenVzbox();
	void DealCloseVzbox();
	void RefreshCameraList();

	void RefreshVideoDisplayWindow();
	void RefreshVideoDisplayStyle();
	void AutoPlayAllVideo();
	void CloseAllVideoDisplay();
	void CloseAllCameraHandle();
	void CleanAllDisplayWindows();
	void OneWindowsDisplay();
	void FourWindowsDisplay();
	void NineWindowsDisplay();
	void ShowOneChnVideo(int chnId);
	void ChangeOneVideoStyle(int chnId);

	static void VideoFrameCallBack(VzLPRClientHandle handle, void *pUserData, const VzYUV420P *pFrame);

    void ChangeSystemMode(int index);


	void RefreshUserGroupList();
	void RefreshUserInfoList();


    void paintEvent(QPaintEvent *event);     //��ͼ
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);

public slots:
	void DealSingleClickedVideoLabel(int chn);
	void DealDoubleClickedVideoLabel(int chn);
	void DealAutoPlayAllVideo();
	void DealStopPlayAllVideo();
	void DealPlayVideoTimer();

public:
	Ui::MainMenu ui;

	QTimer *videoTimer;
	QMessageBox msg_box_;

	VzLPRClientHandle vzbox_handle_;                        //���Ӳ������
	VzLPRClientHandle camera_handle_[CAMERA_NUM_LIMIT];     //��������������

	QString vzbox_ip_;				   //����IP��ַ
	QString vzbox_port_;			   //���Ӷ˿ں�
	QString vzbox_user_name_;		   //�����û���
	QString vzbox_password_;		   //��������
	bool    vzbox_online_status;	   //�����Ƿ�����

	int     display_video_windows_num_;			//��ʾ��Ƶ���ڵĸ���
	DisplayVideoLabel  *video_display_label;	//��ʼ��
	QVector<QString> camera_list_buff;			//����б�ͨ��IP����¼         


	QTimer  video_show_timer_;
	bool    video_register_finished_;
	int     video_index_;

	DisplayUserInfoItem *p_user_info_item;
	QListWidgetItem     *p_user_list_item;
private:
	QMutex  get_frame_mutex_; 


};


