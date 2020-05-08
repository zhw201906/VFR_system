#pragma once

#include <QWidget>
#include <QLabel>

class DisplayVideoLabel : public QLabel
{
	Q_OBJECT

public:
	DisplayVideoLabel(QWidget *parent);
	~DisplayVideoLabel();
};
