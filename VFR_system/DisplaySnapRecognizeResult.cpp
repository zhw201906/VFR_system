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

}
