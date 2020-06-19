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
#include "DealBuildingMap.h"
#include "comDefine.h"


#define  CAMERA_NUM_LIMIT      9
#define  FRAME_NUM_SIZE_LIMIT  3

#define  PAGE_USERINFO_NUM     25
#define  USERINFO_ITEM_SIZE    QSize(195, 245)

#define  VIDEO_FRAME_RATE      25
#define  ONE_WINDOWS    1
#define  FOUR_WINDOWS   2
#define  NINE_WINDOWS   3

#define  CAMERA_POSITION_SIZE  20

#define  OPEN_IMAGE_DIR   "d:/test_image"

#define  BUILDING_MAP_FILE_PATH  "./comtrack/buildingmap"
#define  PERSON_TRACK_FILE_PATH  "./comtrack/persontrack"

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


	void LoadBuildingMapImage();
	void RefreshBuildMapDisplay();
	void DealPlaceCameraPosition(QPoint pt);
	void RefreshShowPlaceCameraInfo();
	void CalcelPlacedOneCamera();
	void ResetCreateNewBuildingMap();
	void SaveNewBuildingMap();
	void CleanCreatingNewBuinldingMap();
	void DeleteExistedBuildingMap();
	void UpdateExistedBuildingMapList();
	void UpdateSavedPersonTrackList();
	void LoadExistedBuildingMap(const QString &str);



    void CreateAItestEngine();
    void LoadCompareImg1();
    void LoadCompareImg2();
    void DealFaceCompare();
    void LoadDetectImg();
    void DealFaceDetect();
    void LoadRecognizeImg();
    void DealFaceRecognize();

protected:
    void paintEvent(QPaintEvent *event);     //��ͼ
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	bool eventFilter(QObject *watched, QEvent *event);


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
	int                  group_cur_id_;
	int                  user_list_cur_page_num_;
	int                  user_list_cur_page_total_;
	VZ_FACE_USER_RESULT  cur_group_toal_user_info_;

	QString              building_map_image_path;
	int                  place_camera_num;
	QVector<QString>     place_camera_info_buff;
	QVector<QString>     existed_building_map;
	QVector<QString>     saved_person_track;


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


