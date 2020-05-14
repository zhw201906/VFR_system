#include "DisplayUserInfoItem.h"
#include <QDebug>

//等比缩放图像
//参数1：图像路径（IN）
//参数2：空间宽度（IN）
//参数3：控件高度（IN）
//参数4：缩放后结果图（OUT）
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

	//切换选择状态
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

	//编辑按钮
	connect(ui.toolButton_edit, &QToolButton::clicked, [=]() {
		emit EditUserInfoSignal();
	});

	//删除按钮
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

	//切换选择状态
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

	//编辑按钮
	connect(ui.toolButton_edit, &QToolButton::clicked, [=]() {
		emit EditUserInfoSignal();
	});

	//删除按钮
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
