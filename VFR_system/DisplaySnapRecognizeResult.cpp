#include "DisplaySnapRecognizeResult.h"

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
