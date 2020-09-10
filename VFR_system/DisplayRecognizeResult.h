#pragma once

#include <QWidget>
#include "ui_DisplayRecognizeResult.h"
#include "DisplayRecgFaceImage.h"
#include "comDefine.h"
#include "comDataStruct.h"


class DisplayRecognizeResult : public QWidget
{
    Q_OBJECT

public:
    //DisplayRecognizeResult(QVector<FaceRecognizeShowInfo> *pinfo = NULL,QWidget *parent = Q_NULLPTR);
    DisplayRecognizeResult(FaceRecognizeShowInfo *pinfo = NULL,QWidget *parent = Q_NULLPTR);
    ~DisplayRecognizeResult();

	void  DisplayUserInfo(FaceRecognizeShowInfo *pinfo);
    void  DisplaySelectedUserInfo(int imgn);

private:
    Ui::DisplayRecognizeResult ui;
    int  cur_selected_image;
	//QVector<FaceRecognizeShowInfo> *p_show_recg_info;
	FaceRecognizeShowInfo *p_show_recg_info;
};
