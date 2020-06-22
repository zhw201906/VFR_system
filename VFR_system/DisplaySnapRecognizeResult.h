#pragma once

#include <QWidget>
#include "ui_DisplaySnapRecognizeResult.h"
#include "comDataStruct.h"
#include "comDefine.h"

class DisplaySnapRecognizeResult : public QWidget
{
	Q_OBJECT

public:
	DisplaySnapRecognizeResult(QWidget *parent = Q_NULLPTR);
	~DisplaySnapRecognizeResult();

	void SetShowRecognizeResult(FaceRecognizeInfo &face,QString &snap_path,QString &recg_path);


private:
	Ui::DisplaySnapRecognizeResult ui;
};
