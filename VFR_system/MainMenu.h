#pragma once

#include <QWidget>
#include "ui_MainMenu.h"
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QVector>

#include "DisplayVideoLabel.h"
#include "sdkHeadFile.h"

#define  CAMERA_NUM_LIMIT  9

#define  ONE_WINDOWS    1
#define  FOUR_WINDOWS   2
#define  NINE_WINDOWS   3

#define  DISPLAY_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);background-color: rgb(200, 200, 200);}"
#define  ClICKED_LABEL_STYLE   "QLabel{border:2px solid rgb(255, 0, 0);background-color: rgb(200, 200, 200);}"

class MainMenu : public QWidget
{
	Q_OBJECT

public:
	MainMenu(QWidget *parent = Q_NULLPTR);
	~MainMenu();

	void DealOpenVzbox();
	void DealCloseVzbox();
	void RefreshCameraList();

	void RefreshVideoDisplayWindow();
	void RefreshVideoDisplayStyle();
	void CleanAllDisplayWindows();
	void OneWindowsDisplay();
	void FourWindowsDisplay();
	void NineWindowsDisplay();
	void ShowOneChnVideo(int chnId);
	void ChangeOneVideoStyle(int chnId);

	void VideoFrameCallBack(VzLPRClientHandle handle, void *pUserData, const VzYUV420P *pFrame);

	void resizeEvent(QResizeEvent *event);

public slots:
	void DealSingleClickedVideoLabel(int chn);
	void DealDoubleClickedVideoLabel(int chn);
	void DealAutoPlayAllVideo();

public:
	Ui::MainMenu ui;

	QTimer *videoTimer;
	QMessageBox msg_box_;

	VzLPRClientHandle vzbox_handle_;                        //���Ӳ������
	VzLPRClientHandle camera_handle_[CAMERA_NUM_LIMIT];     //��������������

	QString vzbox_ip_;              //����IP��ַ
	QString vzbox_port_;            //���Ӷ˿ں�
	QString vzbox_user_name_;       //�����û���
	QString vzbox_password_;        //��������
	bool    vzbox_online_status;    //�����Ƿ�����

	int     display_video_windows_num_;        //��ʾ��Ƶ���ڵĸ���
	DisplayVideoLabel  *video_display_label;   //��ʼ��
	QVector<QString> camera_list_buff;         //����б�ͨ��IP����¼         

	QTimer  video_show_timer_;
};


