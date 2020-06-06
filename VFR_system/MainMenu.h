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
#include <QFileDialog>
#include <QPoint>

#include "DisplayVideoLabel.h"
#include "sdkHeadFile.h"
#include "DisplayVideoThread.h"
#include "DisplayUserInfoItem.h"
#include "face_operation.h"

#define  CAMERA_NUM_LIMIT      9
#define  FRAME_NUM_SIZE_LIMIT  3

#define  PAGE_USERINFO_NUM     25
#define  USERINFO_ITEM_SIZE    QSize(195, 245)

#define  VIDEO_FRAME_RATE      25
#define  ONE_WINDOWS    1
#define  FOUR_WINDOWS   2
#define  NINE_WINDOWS   3

#define  DISPLAY_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);background-color: rgb(200, 200, 200);}"
#define  ClICKED_LABEL_STYLE   "QLabel{border:2px solid rgb(255, 0, 0);background-color: rgb(200, 200, 200);}"

#define  OPEN_IMAGE_DIR   "d:/test_image"

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
	static void CameraFrameCallBack(VzLPRClientHandle handle, void *pUserData, const VzYUV420P *pFrame);

    void ChangeSystemMode(int index);


	void RefreshUserGroupList();
	void RefreshUserInfoList();

	void DisplaynPageUserInfoList(int group_id = 1, int page_num = 1);


    void CreateAItestEngine();
    void LoadCompareImg1();
    void LoadCompareImg2();
    void DealFaceCompare();
    void LoadDetectImg();
    void DealFaceDetect();
    void LoadRecognizeImg();
    void DealFaceRecognize();


    void LoadBuildingMapImage();
    void RefreshBuildMap();
<<<<<<< HEAD
    void DealPlaceCamera(QPoint pt);
    


    virtual void paintEvent(QPaintEvent *event);     //绘图
    virtual void resizeEvent(QResizeEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);
=======


    void paintEvent(QPaintEvent *event);     //绘图
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
>>>>>>> 3a392791294542a0460206aadfa70ef24ce26518

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

	VzLPRClientHandle vzbox_handle_;                        //盒子操作句柄
	VzLPRClientHandle camera_handle_[CAMERA_NUM_LIMIT];     //相机操作句柄数组

	QString vzbox_ip_;				   //盒子IP地址
	QString vzbox_port_;			   //盒子端口号
	QString vzbox_user_name_;		   //盒子用户名
	QString vzbox_password_;		   //盒子密码
	bool    vzbox_online_status;	   //盒子是否在线

	int     display_video_windows_num_;			//显示视频窗口的个数
	DisplayVideoLabel  *video_display_label;	//初始化
	QVector<QString> camera_list_buff;			//相机列表，通过IP来记录         


	QTimer  video_show_timer_;
	bool    video_register_finished_;
	int     video_index_;

	DisplayUserInfoItem *p_user_info_item;
	QListWidgetItem     *p_user_list_item;
	int                  group_cur_id_;
	int                  user_list_cur_page_num_;
	int                  user_list_cur_page_total_;
	VZ_FACE_USER_RESULT  cur_group_toal_user_info_;

private:
	QMutex  get_frame_mutex_; 
    FaceEngineClass *pFaceEngine;
    QString comparePath1;
    QString comparePath2;
    QString detectPath;
    QString recognizePath;
    QString newBuildingMapPath;

    cv::Mat detectImage;
    cv::Mat recognizeImage;

};


