#include "DealBuildingMap.h"

DealBuildingMap::DealBuildingMap(QWidget *parent)
	: QLabel(parent)
{
	qDebug() << "-----------DealBuildingMap build finished";
}

DealBuildingMap::~DealBuildingMap()
{
}

void DealBuildingMap::mousePressEvent(QMouseEvent * ev)
{
	qDebug() << "-----------enter mouse event";

	if (ev->button() == Qt::LeftButton)
	{
		qDebug() << "-----------leftButton pressed" << "x:" << ev->x() << "y" << ev->y();

	}
}
