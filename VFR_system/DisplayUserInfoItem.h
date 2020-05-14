#pragma once

#include <QWidget>
#include "ui_DisplayUserInfoItem.h"
#include <QImage>
#include <QPixmap>
#include <QString>

class DisplayUserInfoItem : public QWidget
{
	Q_OBJECT

public:
	DisplayUserInfoItem(QWidget *parent = Q_NULLPTR);
	DisplayUserInfoItem(QString userName,QImage &userImage);
	~DisplayUserInfoItem();

	void  SetDisplayItemInfo(QString userName, QImage &userImage);

signals:
	void  EditUserInfoSignal();
	void  DelUserInfoSignal();
	void  CheckedSignal();
	void  UncheckedSignal();


private:
	Ui::DisplayUserInfoItem ui;
};
