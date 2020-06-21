#include "AddCamera.h"

AddCamera::AddCamera(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    ui.lineEdit_cameraManageId->setPlaceholderText(QString::fromLocal8Bit("用户自定义相机id"));
    ui.lineEdit_cameraManageIp->setPlaceholderText(QString::fromLocal8Bit("相机IP"));
    ui.lineEdit_cameraManageHttpPort->setPlaceholderText(QString::fromLocal8Bit("端口号"));
    ui.lineEdit_cameraManageUserName->setPlaceholderText(QString::fromLocal8Bit("登录相机用户名"));
    ui.lineEdit_cameraManagePassword->setPlaceholderText(QString::fromLocal8Bit("登录相机密码"));
    ui.lineEdit_cameraRtspAddress->setPlaceholderText(QString::fromLocal8Bit("rtsp主码流地址"));
    ui.lineEdit_cameraRtspSubAddress->setPlaceholderText(QString::fromLocal8Bit("rtsp子码流地址"));

    connect(ui.pushButton_cancelCameraManage, &QPushButton::clicked, this, &AddCamera::CancelAdd);
    connect(ui.pushButton_saveCameraManage, &QPushButton::clicked, this, &AddCamera::ConnectCamera);

}

AddCamera::~AddCamera()
{
}

void AddCamera::CancelAdd()
{
    this->close();
    emit CancelAddCamera();
}

void AddCamera::ConnectCamera()
{
    //qstring 
    CameraAttribute cam_atr;

    emit ConnectAddCamera(cam_atr);
}

void AddCamera::ShowMessage(INFORM msg)
{
    if (msg == ADD_CAMERA_SUCCESS)
    {
        msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("添加相机成功！"));
    }
    else if (msg == ADD_CAMERA_FAILED)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("相机连接失败，请检查后重连！"));
    }
}

void AddCamera::closeEvent(QCloseEvent *event)
{
    emit CancelAddCamera();
}
