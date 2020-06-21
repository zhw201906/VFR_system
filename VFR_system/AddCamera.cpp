#include "AddCamera.h"

AddCamera::AddCamera(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    ui.lineEdit_cameraManageId->setPlaceholderText(QString::fromLocal8Bit("�û��Զ������id"));
    ui.lineEdit_cameraManageIp->setPlaceholderText(QString::fromLocal8Bit("���IP"));
    ui.lineEdit_cameraManageHttpPort->setPlaceholderText(QString::fromLocal8Bit("�˿ں�"));
    ui.lineEdit_cameraManageUserName->setPlaceholderText(QString::fromLocal8Bit("��¼����û���"));
    ui.lineEdit_cameraManagePassword->setPlaceholderText(QString::fromLocal8Bit("��¼�������"));
    ui.lineEdit_cameraRtspAddress->setPlaceholderText(QString::fromLocal8Bit("rtsp��������ַ"));
    ui.lineEdit_cameraRtspSubAddress->setPlaceholderText(QString::fromLocal8Bit("rtsp��������ַ"));

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
        msg_box_.information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�������ɹ���"));
    }
    else if (msg == ADD_CAMERA_FAILED)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�������ʧ�ܣ������������"));
    }
}

void AddCamera::closeEvent(QCloseEvent *event)
{
    emit CancelAddCamera();
}
