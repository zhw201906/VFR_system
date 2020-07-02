#include "FaceLibInfoOperator.h"
#include <QDebug>
FaceLibInfoOperator::FaceLibInfoOperator(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    oper_mode = ADD_FACE_LIB;
    ui.label_libOperator->setText(QString::fromLocal8Bit("添加库"));
    ui.label_libId->hide();
    ui.label_libIdIcon->hide();  

    connect(ui.pushButton_cancel, &QPushButton::clicked, this, &FaceLibInfoOperator::CancelOper);
    connect(ui.pushButton_save, &QPushButton::clicked, this, &FaceLibInfoOperator::SaveOper);
    connect(ui.horizontalSlider_threshold, &QSlider::valueChanged, this, &FaceLibInfoOperator::ThresholdChanged);

	ui.comboBox_libStatus->setCurrentIndex(1);
    ui.horizontalSlider_threshold->setValue(40);
}

FaceLibInfoOperator::FaceLibInfoOperator(FaceLibInfo & lib_info, QWidget * parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    oper_mode = MODIFY_FACE_LIB;
    ui.label_libOperator->setText(QString::fromLocal8Bit("修改库信息"));

    //修改库信息时，需要先加载原来的库信息

    connect(ui.pushButton_cancel, &QPushButton::clicked, this, &FaceLibInfoOperator::CancelOper);
    connect(ui.pushButton_save, &QPushButton::clicked, this, &FaceLibInfoOperator::SaveOper);
    connect(ui.horizontalSlider_threshold, &QSlider::valueChanged, this, &FaceLibInfoOperator::ThresholdChanged);

	ui.lineEdit_libName->setText(QString::fromLocal8Bit(lib_info.name));
	ui.textEdit_remark->setText(QString::fromLocal8Bit(lib_info.remark));
	ui.horizontalSlider_threshold->setValue(lib_info.threshold_value);
	ui.comboBox_libType->setCurrentIndex(lib_info.lib_type);
	ui.comboBox_libStatus->setCurrentIndex(lib_info.enable);
	ui.label_libId->setText(QString("%1").arg(lib_info.id));
}

FaceLibInfoOperator::~FaceLibInfoOperator()
{
}

void FaceLibInfoOperator::ShowErrorMessage(ERROR_LIB_OPER_MSG type, QString & msg)
{
    if (type == REMARK_MSG)
    {
        msg_box_.information(this, QString::fromLocal8Bit("提示"), msg);
    }
    else if (type == ERROR_MSG)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), msg);
    }
}

void FaceLibInfoOperator::SaveOper()
{
    FaceLibInfo face_info;
    face_info.enable = ui.comboBox_libStatus->currentIndex();
    face_info.threshold_value = ui.horizontalSlider_threshold->value();
    face_info.lib_type = ui.comboBox_libType->currentIndex();
    strcpy(face_info.name, ui.lineEdit_libName->text().toLocal8Bit());
    strcpy(face_info.remark, ui.textEdit_remark->toPlainText().toLocal8Bit());

    if (oper_mode == ADD_FACE_LIB)
    {
        emit SaveAddLibOper(face_info);
    }
    else if (oper_mode == MODIFY_FACE_LIB)
    {
        face_info.id = ui.label_libId->text().toInt();
        emit SaveModifyLibOper(face_info);
    }
}

void FaceLibInfoOperator::CancelOper()
{
    emit CancelLibOper();
}

void FaceLibInfoOperator::ThresholdChanged(int value)
{
    ui.label_threshold->setText(QString("%1").arg(value));
}
