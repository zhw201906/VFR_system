#pragma once
#ifndef  ADD_CAMERA_H_
#define  ADD_CAMERA_H_

#include <QWidget>
#include "ui_AddCamera.h"
#include "comDefine.h"
#include "comDataStruct.h"

enum INFORM { ADD_CAMERA_SUCCESS, ADD_CAMERA_FAILED };

class AddCamera : public QWidget
{
    Q_OBJECT

public:
    AddCamera(QWidget *parent = Q_NULLPTR);
    ~AddCamera();

    void  CancelAdd();
    void  ConnectCamera();
    void  ShowMessage(INFORM msg);

signals:
    void  ConnectAddCamera(CameraAttribute &cam_param);
    void  CancelAddCamera();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::AddCamera ui;
    QMessageBox msg_box_;
};


#endif