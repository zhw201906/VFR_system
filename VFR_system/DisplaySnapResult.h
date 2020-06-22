#pragma once

#include <QWidget>
#include "ui_DisplaySnapResult.h"
#include "comDataStruct.h"
#include "comDefine.h"

class DisplaySnapResult : public QWidget
{
	Q_OBJECT

public:
	DisplaySnapResult(QWidget *parent = Q_NULLPTR);
	~DisplaySnapResult();

	void SetShowData(FaceSnapInfo &face_info, QString &path);

private:
	Ui::DisplaySnapResult ui;
};
