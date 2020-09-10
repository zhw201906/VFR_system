#pragma once

#include <QLabel>
#include "comDefine.h"
#include "comDataStruct.h"


class DisplayRecgFaceImage : public QLabel
{
    Q_OBJECT

#define  SELECTED_LABEL_STYLE   "QLabel{border:2px solid rgb(255, 0, 0);}"
#define  FREE_LABEL_STYLE       "QLabel{border:2px solid rgb(0, 0, 0);}"

public:
    DisplayRecgFaceImage(QWidget *parent);
    ~DisplayRecgFaceImage();

    void SetLabelStyle(QString style);

protected:
    virtual void mousePressEvent(QMouseEvent *ev);

signals:
    void  SelectedCurrentLabel();
};
