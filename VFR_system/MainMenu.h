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

	VzLPRClientHandle vzbox_handle_;                        //盒子操作句柄
	VzLPRClientHandle camera_handle_[CAMERA_NUM_LIMIT];     //相机操作句柄数组

	QString vzbox_ip_;              //盒子IP地址
	QString vzbox_port_;            //盒子端口号
	QString vzbox_user_name_;       //盒子用户名
	QString vzbox_password_;        //盒子密码
	bool    vzbox_online_status;    //盒子是否在线

	int     display_video_windows_num_;  //显示视频窗口的个数
	QLabel  *video_display_label;        //初始化
	QVector<QString> camera_list_buff;   //相机列表，通过IP来记录         

};
