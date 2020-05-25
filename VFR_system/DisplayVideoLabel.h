#pragma once

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QDebug>

class DisplayVideoLabel : public QLabel
{
	Q_OBJECT

signals:
	void singleClickedMouse(int chn);
	void doubleClickedMouse(int chn);

public:
	DisplayVideoLabel();
	DisplayVideoLabel(QWidget *parent);
	~DisplayVideoLabel();

	void  setChannelId(int chn);
	void  setDisplayImage(QImage &src);
	void  mousePressEvent(QMouseEvent *);
	void  mouseDoubleClickEvent(QMouseEvent *);

	//void  paintEvent(QPaintEvent *event);

private:
	int  chnId;
	QImage dis_image;
};
