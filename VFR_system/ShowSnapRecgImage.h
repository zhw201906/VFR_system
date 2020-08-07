#pragma once

#include <QWidget>
#include "ui_ShowSnapRecgImage.h"

#include "comDefine.h"
#include "QPixmap"

class ShowSnapRecgImage : public QWidget
{
	Q_OBJECT

public:
	ShowSnapRecgImage(QString &dir, QWidget *parent = Q_NULLPTR);
	~ShowSnapRecgImage();

	void SetShowImagePath(QString &dir);

protected:
	virtual void resizeEvent(QResizeEvent *event);

private:
	Ui::ShowSnapRecgImage ui;
	QString big_path;
	QString small_path;

};
