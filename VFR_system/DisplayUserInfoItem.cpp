#include "DisplayUserInfoItem.h"
#include <QDebug>

//�ȱ�����ͼ��
//����1��ͼ��·����IN��
//����2���ռ��ȣ�IN��
//����3���ؼ��߶ȣ�IN��
//����4�����ź���ͼ��OUT��
static void Geometric_Scaling_Image(const QImage &src, const int control_width, const int control_height, QPixmap &dst_pix)
{
	QPixmap pix_src = QPixmap::fromImage(src);
	double ratio_w = pix_src.width() * 1.0 / control_width;
	double ratio_h = pix_src.height() * 1.0 / control_height;

	if (ratio_w >= ratio_h)
		dst_pix = pix_src.scaled(pix_src.width() / ratio_w, pix_src.height() / ratio_w);
	else
		dst_pix = pix_src.scaled(pix_src.width() / ratio_h, pix_src.height() / ratio_h);
}

DisplayUserInfoItem::DisplayUserInfoItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.toolButton_edit->setIcon(QIcon("./icon/edit.jpg"));
	ui.toolButton_del->setIcon(QIcon("./icon/del.jpg"));

	//�л�ѡ��״̬
	connect(ui.checkBox_userName, &QCheckBox::stateChanged, [=](int state) {
		if (Qt::Checked == state)
		{
			emit CheckedSignal();
		}
		else if (Qt::Unchecked == state)
		{
			emit UncheckedSignal();
		}
	});

	//�༭��ť
	connect(ui.toolButton_edit, &QToolButton::clicked, [=]() {
		emit EditUserInfoSignal();
	});

	//ɾ����ť
	connect(ui.toolButton_del, &QToolButton::clicked, [=]() {
		emit DelUserInfoSignal();
	});
}

DisplayUserInfoItem::DisplayUserInfoItem(QString userName, QImage &userImage)
{
	ui.setupUi(this);

	ui.toolButton_edit->setIcon(QIcon("./icon/edit.jpg"));
	ui.toolButton_del->setIcon(QIcon("./icon/del.jpg"));

	if (!userImage.isNull())
	{
		qDebug() << "image is not NULL";
		QPixmap  show_pix;	
		Geometric_Scaling_Image(userImage, ui.label_image->width(), ui.label_image->height(), show_pix);
		ui.label_image->setPixmap(show_pix);
	}

	ui.checkBox_userName->setText(userName);

	//�л�ѡ��״̬
	connect(ui.checkBox_userName, &QCheckBox::stateChanged, [=](int state) {
		if (Qt::Checked == state)
		{
			emit CheckedSignal();
		}
		else if (Qt::Unchecked == state)
		{
			emit UncheckedSignal();
		}
	});

	//�༭��ť
	connect(ui.toolButton_edit, &QToolButton::clicked, [=]() {
		emit EditUserInfoSignal();
	});

	//ɾ����ť
	connect(ui.toolButton_del, &QToolButton::clicked, [=]() {
		emit DelUserInfoSignal();
	});
}

DisplayUserInfoItem::~DisplayUserInfoItem()
{
}

void DisplayUserInfoItem::SetDisplayItemInfo(QString userName, QImage & userImage)
{
	if (!userImage.isNull())
	{
		qDebug() << "image is not NULL";
		QPixmap  show_pix;
		Geometric_Scaling_Image(userImage, ui.label_image->width(), ui.label_image->height(), show_pix);
		ui.label_image->clear();
		ui.label_image->setPixmap(show_pix);
	}

	ui.checkBox_userName->setText(userName);
}
