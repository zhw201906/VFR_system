#pragma once

#include <QWidget>
#include "ui_UserInformationOperator.h"
#include "comDefine.h"
#include "comDataStruct.h"

enum USER_OPER{ADD_USER, MODIFY_USER};
enum OPERATE_RESULT { ADD_USER_SUCCESS, ADD_USER_FAILED, MODIFY_USER_SUCCESS, MODIFY_USER_FAILED };



class UserInformationOperator : public QWidget
{
    Q_OBJECT

public:
    UserInformationOperator(USER_OPER oper = ADD_USER, QWidget *parent = Q_NULLPTR);
    UserInformationOperator(UserInfo &user, USER_OPER oper = ADD_USER);

    ~UserInformationOperator();

    void  LoadUserImage();
    void  EnsureOperUser();
    void  QuitOperUser();
    void  ShowMessage(OPERATE_RESULT msg);

protected:
    virtual void closeEvent(QCloseEvent *event);

signals:
    void  OperUserInfo(UserInfo &user, USER_OPER oper = ADD_USER);
    void  CancelOperUser();

private:
    Ui::UserInformationOperator ui;
    USER_OPER  oper_mode;
    QString    user_image_path;
    QMessageBox msg_box_;
};
