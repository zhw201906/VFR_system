#pragma once

#include <QWidget>
#include "ui_FaceLibInfoOperator.h"
#include "comDefine.h"
#include "comDataStruct.h"

enum FACE_LIB_OPER { ADD_FACE_LIB, MODIFY_FACE_LIB };
//enum FAFE_LIB_OPER_RESULT{ADD_LIB_SUCCESS,};
enum ERROR_LIB_OPER_MSG { ERROR_MSG, REMARK_MSG };

class FaceLibInfoOperator : public QWidget
{
    Q_OBJECT

public:
    FaceLibInfoOperator(QWidget *parent = Q_NULLPTR);
    FaceLibInfoOperator(FaceLibInfo &lib_info, QWidget *parent = Q_NULLPTR);

    ~FaceLibInfoOperator();

	inline FACE_LIB_OPER GetCurOperMode()const { return oper_mode; };

    void ShowErrorMessage(ERROR_LIB_OPER_MSG type, QString &msg);

    void SaveOper();
    void CancelOper();

    void ThresholdChanged(int value);

signals:
    void CancelLibOper();
    void SaveModifyLibOper(FaceLibInfo &lib_info);
    void SaveAddLibOper(FaceLibInfo &lib_info);

private:
    Ui::FaceLibInfoOperator ui;
    FACE_LIB_OPER  oper_mode;
    QMessageBox msg_box_;
};
