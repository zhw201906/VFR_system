#pragma once
#ifndef  MAIN_MENU_H_
#define  MAIN_MENU_H_

#include <QWidget>
#include "ui_MainMenu.h"

#include "DisplayVideoLabel.h"
#include "sdkHeadFile.h"
#include "DisplayVideoThread.h"
#include "DisplayUserInfoItem.h"
#include "face_operation.h"
#include "DealBuildingMap.h"
#include "comDefine.h"
#include "AddCamera.h"
#include "UserInformationOperator.h"
#include "DisplayRecognizeResult.h"
#include "DisplayDetectResult.h"
#include "DisplaySnapResult.h"
#include "DisplaySnapRecognizeResult.h"



#define  CAMERA_NUM_LIMIT      9
#define  FRAME_NUM_SIZE_LIMIT  3

#define  PAGE_USERINFO_NUM     25
#define  USERINFO_ITEM_SIZE    QSize(195, 245)

#define  VIDEO_FRAME_RATE      25
#define  ONE_WINDOWS    1
#define  FOUR_WINDOWS   2
#define  NINE_WINDOWS   3

#define  CAMERA_POSITION_SIZE  20


#define   BODY_CAM_ID         "cam_id"
#define   BODY_CAM_IP         "cam_ip"
#define   BODY_CAM_NUM        "camera_num"
#define   BODY_CAM_ATT        "camera_attribute"



typedef struct {


}UserGroupInfo;

class MainMenu : public QWidget
{
	Q_OBJECT


public:
	MainMenu(QWidget *parent = Q_NULLPTR);
	~MainMenu();

    void SetIconInit();
	void DealOpenVzbox();
	void DealCloseVzbox();
	void RefreshDisplayCameraList();


	//在线视频
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
	static void CameraSnapCallBack(VzLPRClientHandle handle, TH_FaceResult* face_result, void* pUserData);
	//void (__STDCALL *VZLPRC_FACE_RESULT_CALLBACK)(VzLPRClientHandle handle, TH_FaceResult* face_result, void* pUserData);

    void ChangeSystemMode(int index);
    void CleanAllSetSystemModeButton();

	//人脸库管理
	void RefreshUserGroupList();
	void RefreshUserInfoList();
	void DisplaynPageUserInfoList(int group_id = 1, int page_num = 1);
    void AddOneUserInformation();
    void ModifyOneUserInformation();
    void DealOperatorUserInfo(UserInfo &user, USER_OPER oper = ADD_USER);


	//行人轨迹
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


	//相机管理
	void UpdateConnectedCameraInfoMap();
	void UpdateConnectedCameraIpList();
	void ReadCameraConfigParamFile();
	void SaveCameraConfigParamFile();
    void CurrentSelectCameraItem(QListWidgetItem *item);
    void AddOneConnectCamera();
    void CheckAddCameraIsExisted(CameraAttribute &cam_param);
    void ModifySelectedCamera();
    void DeleteSelectedCamera();


	//智能测试
    void CreateAItestEngine();
    void LoadCompareImg1();
    void LoadCompareImg2();
    void DealFaceCompare();
    void LoadDetectImg();
    void DealFaceDetect();
    void ShowFaceDetectResult();
    void LoadRecognizeImg();
    void DealFaceRecognize();
    void ShowFaceRecognizeResult();

protected:
    void paintEvent(QPaintEvent *event);     //绘图
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

	VzLPRClientHandle vzbox_handle_;                        //盒子操作句柄
	VzLPRClientHandle camera_handle_[CAMERA_NUM_LIMIT];     //相机操作句柄数组

	QString vzbox_ip_;				   //盒子IP地址
	QString vzbox_port_;			   //盒子端口号
	QString vzbox_user_name_;		   //盒子用户名
	QString vzbox_password_;		   //盒子密码
	bool    vzbox_online_status;	   //盒子是否在线

	int     display_video_windows_num_;			//显示视频窗口的个数
	DisplayVideoLabel  *video_display_label;	//初始化
	QVector<QString>   camera_list_buff;        //相机列表，通过IP来记录         

	DisplaySnapResult  *p_snap_result_buff_ui;
	QListWidgetItem    *p_snap_result_item;
	DisplaySnapRecognizeResult  *p_recognize_result_buff_ui;
	QListWidgetItem             *p_recognize_result_item;

	QTimer  video_show_timer_;
	bool    video_register_finished_;
	int     video_index_;

	DisplayUserInfoItem *p_user_info_item;
	QListWidgetItem     *p_user_list_item;
	int                  group_cur_id_;
	int                  user_list_cur_page_num_;
	int                  user_list_cur_page_total_;
	VZ_FACE_USER_RESULT  cur_group_toal_user_info_;
    UserInformationOperator   *p_operator_user_ui;



	QString              building_map_image_path;
	int                  place_camera_num;
	QVector<QString>     place_camera_info_buff;
	QVector<QString>     existed_building_map;
	QVector<QString>     saved_person_track;




	//typedef  VZ_BOX_CAM_GROUP  CameraAttribute;
	QMap<QString, CameraAttribute> connected_camera_map;    //记录已连接的相机IP--->attribute
    QString              cur_selected_camera_ip;
    AddCamera            *p_add_camera_ui;


private:
	QMutex  get_frame_mutex_; 
    FaceEngineClass *pFaceEngine;
    QString comparePath1;
    QString comparePath2;
    QString detectPath;
    QString recognizePath;

    cv::Mat detectImage;
    cv::Mat recognizeImage;
    DisplayRecognizeResult  *p_display_recognize_ui;
    DisplayDetectResult     *p_display_detect_ui;
};


#endif