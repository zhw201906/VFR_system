#include "VFR_algorithm.h"

VFR_algorithm::VFR_algorithm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	is_recg = false;
	is_snap = false;
	is_save = false;

	connect(ui.pushButton_selectVideo, &QPushButton::clicked, this, &VFR_algorithm::SlotSelectVideo);
	connect(ui.pushButton_openCamera, &QPushButton::clicked, this, &VFR_algorithm::SlotOpenCamera);
	connect(ui.checkBox_snap, &QCheckBox::stateChanged, this, &VFR_algorithm::SlotOpenSnap);
	connect(ui.checkBox_saveSnap, &QCheckBox::stateChanged, this, &VFR_algorithm::SlotSaveSnap);
	connect(ui.checkBox_rect, &QCheckBox::stateChanged, this, &VFR_algorithm::SlotShowRecg);
	connect(&video_play_timer, &QTimer::timeout, this, &VFR_algorithm::SlotPlayVideo);


}

void VFR_algorithm::SlotSelectVideo()
{
	QString video_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频"), VIDEO_DIR,
		tr("video (*.avi *.mp4);; All files (*.*)"));

	if (video_path.isEmpty())
	{
		return;
	}

	ui.textBrowser_log->append(QString::fromLocal8Bit("视频路径:%1").arg(video_path));
	ui.textBrowser_log->append(QString::fromLocal8Bit("开始播放..."));

	video_cap.open(video_path.toStdString());
	video_play_timer.start((int)(1000.0 / VIDEO_FRAME_RATE));
}

void VFR_algorithm::SlotOpenCamera()
{
	video_play_timer.stop();
	video_cap.open(0);
	if (!video_cap.isOpened())
	{
		ui.textBrowser_log->append(QString::fromLocal8Bit("打开摄像头失败"));
		return;
	}
	ui.textBrowser_log->append(QString::fromLocal8Bit("摄像头开启成功..."));
	video_play_timer.start((int)(1000.0 / VIDEO_FRAME_RATE));
}

void VFR_algorithm::SlotPlayVideo()
{	
	cv::Mat in_mat;	//用于储存每一帧的图像
	video_cap >> in_mat;//读取当前帧

	if (in_mat.empty())
	{
		video_play_timer.stop();
		ui.label_video->clear();
		ui.textBrowser_log->append(QString::fromLocal8Bit("播放完毕..."));
		return;
	}

	cv::Mat frame;
	if (is_recg)
	{
		FaceAlgServer::Instance()->DetectFaceAndRect(in_mat, frame);
	}
	else
	{
		in_mat.copyTo(frame);
	}

	QImage img;
	int chana = frame.channels();
	//依据通道数不同，改变不同的装换方式
	if (3 == chana) {
		//调整通道次序
		cv::cvtColor(frame, frame, CV_BGR2RGB);
		img = QImage(static_cast<uchar *>(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
	}
	else if (4 == chana) {
		//argb
		img = QImage(static_cast<uchar *>(frame.data), frame.cols, frame.rows, QImage::Format_ARGB32);
	}
	else {
		//单通道，灰度图
		img = QImage(frame.cols, frame.rows, QImage::Format_Indexed8);
		uchar * matdata = frame.data;
		for (int row = 0; row < frame.rows; ++row) {
			uchar* rowdata = img.scanLine(row);
			memcpy(rowdata, matdata, frame.cols);
			matdata += frame.cols;
		}
	}

	ui.label_video->setPixmap(QPixmap::fromImage(img.scaled(ui.label_video->width(), ui.label_video->height())));
}

void VFR_algorithm::SlotShowRecg(int state)
{
	if (state == Qt::Checked)
	{
		is_recg = true;
	}
	else
	{
		is_recg = false;
	}
}

void VFR_algorithm::SlotOpenSnap(int state)
{
	if (state == Qt::Checked)
	{
		is_snap = true;
	}
	else
	{
		is_snap = false;
	}
}

void VFR_algorithm::SlotSaveSnap(int state)
{
	if (state == Qt::Checked)
	{
		is_save = true;
	}
	else
	{
		is_save = false;
	}
}
