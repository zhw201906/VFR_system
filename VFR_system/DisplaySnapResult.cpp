#include "DisplaySnapResult.h"
#include <QDebug>

//�ȱ�����ͼ��
//����1��ͼ��·����IN��
//����2���ռ��ȣ�IN��
//����3���ؼ��߶ȣ�IN��
//����4�����ź���ͼ��OUT��
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

const char *age[]  = { "","����","����","����","����","δ֪" };
const char *sex[]  = { "��","Ů","δ֪" };
const char *have[] = { "��","��","δ֪" };

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
