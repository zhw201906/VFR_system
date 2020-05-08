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
	void OneWindowsDisplay();
	void FourWindowsDisplay();
	void NineWindowsDisplay();
	//void VideoFrameCallBack(VzLPRClientHandle handle, void *pUserData, const VZ_LPRC_IMAGE_INFO *pFrame);

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

	int     display_video_windows_num_;  //��ʾ��Ƶ���ڵĸ���
	QLabel  *video_display_label;        //��ʼ��
	QVector<QString> camera_list_buff;   //����б�ͨ��IP����¼         

};
