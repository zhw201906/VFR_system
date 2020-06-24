#include "DisplaySnapRecognizeResult.h"

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

DisplaySnapRecognizeResult::DisplaySnapRecognizeResult(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog);
}

DisplaySnapRecognizeResult::~DisplaySnapRecognizeResult()
{

}

void DisplaySnapRecognizeResult::SetShowRecognizeResult(FaceRecognizeInfo & face, QString & snap_path, QString & recg_path)
{
	if (!snap_path.isEmpty())
	{
		QPixmap pix;
		Geometric_Scaling_Image(snap_path, ui.label_imageSnap->width(), ui.label_imageSnap->height(), pix);
		ui.label_imageSnap->setPixmap(pix);
	}
	if (!recg_path.isEmpty())
	{
		QPixmap pix;
		Geometric_Scaling_Image(recg_path, ui.label_imageRecognize->width(), ui.label_imageRecognize->height(), pix);
		ui.label_imageRecognize->setPixmap(pix);
	}

	ui.label_name->setText(QString::fromLocal8Bit(face.recg_people_name));
	ui.label_birth->setText(QString::fromLocal8Bit(face.recg_birthday));
	ui.label_city->setText(QString::fromLocal8Bit(face.recg_prov));
	ui.label_score->setText(QString("%1%").arg(face.recg_face_score));
}
