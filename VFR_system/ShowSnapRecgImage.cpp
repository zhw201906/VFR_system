#include "ShowSnapRecgImage.h"


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

ShowSnapRecgImage::ShowSnapRecgImage(QString &dir, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

	small_path = dir + QString(".jpg");
	big_path   = dir + QString("_big.jpg");
}

ShowSnapRecgImage::~ShowSnapRecgImage()
{

}

//设置显示图像的路径
void ShowSnapRecgImage::SetShowImagePath(QString & dir)
{
	small_path = dir + QString(".jpg");
	big_path   = dir + QString("_big.jpg");
	
	ui.label_smallImage->clear();
	ui.label_bigImage->clear();

	QPixmap small_pix, big_pix;
	Geometric_Scaling_Image(small_path, ui.label_smallImage->width(), ui.label_smallImage->height(), small_pix);
	Geometric_Scaling_Image(big_path, ui.label_bigImage->width(), ui.label_bigImage->height(), big_pix);

	ui.label_smallImage->setPixmap(small_pix);
	ui.label_bigImage->setPixmap(big_pix);
}

void ShowSnapRecgImage::resizeEvent(QResizeEvent * event)
{
	ui.label_smallImage->clear();
	ui.label_bigImage->clear();

	QPixmap small_pix, big_pix;
	Geometric_Scaling_Image(small_path, ui.label_smallImage->width(), ui.label_smallImage->height(), small_pix);
	Geometric_Scaling_Image(big_path, ui.label_bigImage->width(), ui.label_bigImage->height(), big_pix);

	ui.label_smallImage->setPixmap(small_pix);
	ui.label_bigImage->setPixmap(big_pix);
}

