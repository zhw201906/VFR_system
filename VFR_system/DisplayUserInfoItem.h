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
	DisplayUserInfoItem(QString userName, QImage &userImage, int idx);
	~DisplayUserInfoItem();

	void  SetDisplayItemInfo(QString userName, QImage &userImage);
	int   GetObjectNum() { return object_num; };

signals:
	void  EditUserInfoSignal();
	void  DelUserInfoSignal(int idx);
	void  CheckedSignal();
	void  UncheckedSignal();


private:
	Ui::DisplayUserInfoItem ui;
	static int object_num;
	int pic_index;
};

