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

#include "FaceAlgServer.h"

#define   VIDEO_DIR   "../../video"
#define   VIDEO_FRAME_RATE    30

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


private:
    Ui::VFR_algorithmClass ui;

	cv::VideoCapture video_cap;
	QTimer  video_play_timer;

	bool  is_recg;
	bool  is_snap;
	bool  is_save;
};
