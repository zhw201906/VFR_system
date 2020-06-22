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

//��Ƶ���Ŵ��ڵ��ʱ��״̬�л�
#define  DISPLAY_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);background-color: rgb(200, 200, 200);}"
#define  ClICKED_LABEL_STYLE   "QLabel{border:2px solid rgb(255, 0, 0);background-color: rgb(200, 200, 200);}"

#define  SYSTEM_MODE_BUTTON_ENABLE_STYLE    "QPushButton{background-color:grey;}"
#define  SYSTEM_MODE_BUTTON_DISABLE_STYLE   "QPushButton{background-color:white;}"

#define  CAMERA_SNAP_RESULT_MAX_NUMS        100
#define  CAMERA_SNAP_RECOGNIZE_MAX_NUMS     100

//¥���ͼ�ؼ���ʾ�߿�
#define  BUILDING_MAP_LABEL_STYLE   "QLabel{border:1px solid rgb(0, 0, 0);}"



/*
��ʾ����ץ�ĺ�ʶ���ͼ���С��Ϊ90*120���������ܷŵ��б�ؼ���
*/




#endif
