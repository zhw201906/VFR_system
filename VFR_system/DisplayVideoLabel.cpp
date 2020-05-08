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

void DisplayVideoLabel::mousePressEvent(QMouseEvent *)
{
	emit singleClickedMouse(chnId);
}

void DisplayVideoLabel::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClickedMouse(chnId);
}
