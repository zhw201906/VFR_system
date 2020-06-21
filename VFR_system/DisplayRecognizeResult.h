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
    DisplayRecognizeResult(QWidget *parent = Q_NULLPTR);
    ~DisplayRecognizeResult();

    void  DisplaySelectedUserInfo(int imgn);

private:
    Ui::DisplayRecognizeResult ui;
    int  cur_selected_image;
};
