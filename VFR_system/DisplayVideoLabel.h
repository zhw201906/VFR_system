#pragma once

#include <QWidget>
#include <QLabel>

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
	void  mousePressEvent(QMouseEvent *);
	void  mouseDoubleClickEvent(QMouseEvent *);

private:
	int  chnId;
};
