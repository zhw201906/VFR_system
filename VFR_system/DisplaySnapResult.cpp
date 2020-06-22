#include "DisplaySnapResult.h"
#include <QDebug>

//等比缩放图像
//参数1：图像路径（IN）
//参数2：空间宽度（IN）
//参数3：控件高度（IN）
//参数4：缩放后结果图（OUT）
static void Geometric_Scaling_Image(const QString path, const int control_width, const int control_height, QPixmap &dst_pix)
{
	QPixmap pix;
	pix.load(path);

	double ratio_w = pix.width() * 1.0 / control_width;
	double ratio_h = pix.height() * 1.0 / control_height;

	if (ratio_w >= ratio_h)
		pix = pix.scaled(pix.width() / ratio_w, pix.height() / ratio_w);
	else
		pix = pix.scaled(pix.width() / ratio_h, pix.height() / ratio_h);

	dst_pix = pix;
}

DisplaySnapResult::DisplaySnapResult(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Dialog);

}

DisplaySnapResult::~DisplaySnapResult()
{
}

const char *age[]  = { "","少年","青年","中年","老年","未知" };
const char *sex[]  = { "男","女","未知" };
const char *have[] = { "无","有","未知" };

void DisplaySnapResult::SetShowData(FaceSnapInfo & face_info, QString &path)
{
	if (!path.isEmpty())
	{
		QPixmap pix;
		Geometric_Scaling_Image(path, ui.label_image->width(), ui.label_image->height(), pix);
		ui.label_image->setPixmap(pix);
	}

	qDebug() << "time:" << face_info.datetime;
	QString time = QString(face_info.datetime).mid(11);
	ui.label_age->setText(QString::fromLocal8Bit(age[face_info.age]));
	ui.label_sex->setText(QString::fromLocal8Bit(sex[face_info.sex]));
	ui.label_score->setText(QString("%1").arg(face_info.confidence));
	ui.label_time->setText(time);
	ui.label_hat->setText(QString::fromLocal8Bit(have[face_info.have_hat]));
	ui.label_glasses->setText(QString::fromLocal8Bit(have[face_info.have_glasses]));
}
