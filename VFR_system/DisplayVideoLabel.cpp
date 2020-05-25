#include "DisplayVideoLabel.h"

DisplayVideoLabel::DisplayVideoLabel()
{
	chnId = -1;
}

DisplayVideoLabel::DisplayVideoLabel(QWidget *parent)
	: QLabel(parent)
{
	chnId = -1;
}

DisplayVideoLabel::~DisplayVideoLabel()
{
}

void DisplayVideoLabel::setChannelId(int chn)
{
	chnId = chn;
}

void DisplayVideoLabel::setDisplayImage(QImage & src)
{
	dis_image = src.copy();
	update();
}

void DisplayVideoLabel::mousePressEvent(QMouseEvent *)
{
	emit singleClickedMouse(chnId);
}

void DisplayVideoLabel::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClickedMouse(chnId);
}

//void DisplayVideoLabel::paintEvent(QPaintEvent * event)
//{
//	QPainter painter(this);
//
//	qDebug() << "paint event" << chnId << ": " << this->x() << "  ," << this->y() << "  ," << this->width() << "  ," << this->height();
//	//QRect rect(this->x(), this->y(), this->width(), this->height());
//	QRect rect(0, 0, this->width(), this->height());
//	painter.drawImage(rect, dis_image);
//}
