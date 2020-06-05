#pragma once

#include <QLabel>
#include <QDebug>
#include <QMouseEvent>

class DealBuildingMap : public QLabel
{
	Q_OBJECT

public:
	DealBuildingMap(QWidget *parent);
	~DealBuildingMap();

	virtual void mousePressEvent(QMouseEvent *ev);
};
