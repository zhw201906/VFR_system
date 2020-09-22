#pragma once

#include <QtWidgets/QWidget>
#include "ui_VFR_algorithm.h"

#include <opencv2/opencv.hpp>

#include <QPushButton>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QTimer>
#include <QCheckBox>
#include <QVector>
#include <QQueue>
#include <QList>
#include <QLabel>

#include <thread>
#include <mutex>

#include "FaceAlgServer.h"

#define   VIDEO_DIR   "../../video"
#define   VIDEO_FRAME_RATE    30
#define   SNAP_FACE_SHOW_SIZE  QSize(75, 80)

class VFR_algorithm : public QWidget
{
    Q_OBJECT

public:
    VFR_algorithm(QWidget *parent = Q_NULLPTR);

	void SlotSelectVideo();
	void SlotOpenCamera();
	void SlotPlayVideo();

	void SlotShowRecg(int state);
	void SlotOpenSnap(int state);
	void SlotSaveSnap(int state);
	void SlotAreaCount(int state);

	static void DealSnapCallBack(const cv::Mat &img, void *puser);
    static void DealFaceAlgThread();

private:
    Ui::VFR_algorithmClass ui;

	cv::VideoCapture video_cap;
	QTimer  video_play_timer;

	bool  is_recg;
	bool  is_snap;
	bool  is_save;
	bool  is_count;

    char  alg_mode;
    
};
