#pragma once
#ifndef COMDEFINE_H_
#define COMDEFINE_H_

#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QVector>
#include <QQueue>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <functional> 
#include <QImage>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QCheckBox>
#include <QFont>
#include <QPushButton>
#include <QSet>
#include <QMoveEvent>

#define  OPEN_IMAGE_DIR          "d:/vz_box"
//#define  OPEN_IMAGE_DIR          "d:/test_image"

#define  FACE_IMAGE_CACHE_PATH   "../imgcache"

#define  BUILDING_MAP_FILE_PATH  "./comtrack/buildingmap"
#define  PERSON_TRACK_FILE_PATH  "./comtrack/persontrack"

#define  CAMERA_CONFIG_PARAM_PATH  "./config/cameracfgparam"
#define  CAMERA_CONFIG_PARAM_NAME  "cameracfg.json"

#define  SNAP_ITEM_SIZE            QSize(215, 140)
#define  SNAP_RECG_ITEM_SIZE       QSize(225, 165)
#define  CAMERA_SNAP_IMAGE_PATH    "./comtrack/camerasnapimage"
#define  CAMERA_RECG_IMAGE_PATH    "./comtrack/camerarecgimage"
#define  CAMERA_LIBRARY_IMAGE_NAME "./comtrack/cacheImage/lib_face.jpg"
#define  CAMERA_SNAP_IMAGE_NAME    "./comtrack/cacheImage/snap_face.jpg"
#define  CAMERA_SNAP_BIG_IMAGE_NAME  "./comtrack/cacheImage/big_snap_face.jpg"

#define  OPEN_SAVE_RECG_RET   0

//视频播放窗口点击时的状态切换
#define  DISPLAY_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);background-color: rgb(200, 200, 200);}"
#define  ClICKED_LABEL_STYLE   "QLabel{border:2px solid rgb(255, 0, 0);background-color: rgb(200, 200, 200);}"

#define  SYSTEM_MODE_BUTTON_ENABLE_STYLE    "QPushButton{background-color:grey;}"
#define  SYSTEM_MODE_BUTTON_DISABLE_STYLE   "QPushButton{background-color:white;}"

#define  CAMERA_SNAP_RESULT_MAX_NUMS        500
#define  CAMERA_SNAP_RECOGNIZE_MAX_NUMS     500

//楼宇地图控件显示边框
#define  BUILDING_MAP_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);}"



/*
显示最新抓拍和识别的图像大小均为90*120，这样才能放到列表控件中
*/


#define   BODY_RECG_NAME       "recg_name"
#define   BODY_RECG_INFO       "recg_info"
#define   BODY_RECG_CAMID      "camera_id"
#define   BODY_RECG_CAMIP      "camera_ip"
#define   BODY_RECG_TIME       "time_stamp"




#endif
