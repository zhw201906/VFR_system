#include "UserInformationOperator.h"

//�ȱ�����ͼ��
//����1��ͼ��·����IN��
//����2���ռ��ȣ�IN��
//����3���ؼ��߶ȣ�IN��
//����4�����ź���ͼ��OUT��
static void Geometric_Scaling_Image(const QString path, const int control_width, const int control_height, QPixmap &dst_pix)
{
    QPixmap pix;
    pix.load(path);

    double ratio_w = pix.width() * 1.0 / control_width;
    double ratio_h = pix.height() * 1.0 / control_height;

    if (ratio_w >= ratio_h)
        pix = pix.scaled(pix.width() / ratio_w, pix.height() / ratio_w);
    else
        pix = pix.scaled(pix.width() / ratio_h, pix.height() / ratio_h);

    dst_pix = pix;
}

UserInformationOperator::UserInformationOperator(USER_OPER oper, QWidget *parent)
    : QWidget(parent), oper_mode(oper)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    ui.label_userInforOperator->setText(QString::fromLocal8Bit("������û�"));

    connect(ui.pushButton_loadUserImage, &QPushButton::clicked, this, &UserInformationOperator::LoadUserImage);
    connect(ui.pushButton_cancelUser, &QPushButton::clicked, this, &UserInformationOperator::QuitOperUser);
    connect(ui.pushButton_saveUser, &QPushButton::clicked, this, &UserInformationOperator::EnsureOperUser);

    //�޸��û���Ϣʱ����Ҫ�ȼ����û���Ϣ
}

UserInformationOperator::UserInformationOperator(UserInfo & user, USER_OPER oper)
    :oper_mode(oper)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    //�޸�ʱ��Ҫ��ԭ���������ص������Ӧ������ʾ

    ui.label_userInforOperator->setText(QString::fromLocal8Bit("�޸��û���Ϣ"));

    connect(ui.pushButton_loadUserImage, &QPushButton::clicked, this, &UserInformationOperator::LoadUserImage);
    connect(ui.pushButton_cancelUser, &QPushButton::clicked, this, &UserInformationOperator::QuitOperUser);
    connect(ui.pushButton_saveUser, &QPushButton::clicked, this, &UserInformationOperator::EnsureOperUser);

}

UserInformationOperator::~UserInformationOperator()
{

}

void UserInformationOperator::LoadUserImage()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��ͼƬ"), OPEN_IMAGE_DIR,
        tr("Images (*.png *.jpg);; All files (*.*)"));

    if (img_path.isEmpty())
    {
        return;
    }
    user_image_path = img_path;
    QPixmap pix;
    Geometric_Scaling_Image(user_image_path, ui.label_userImage->width(), ui.label_userImage->height(), pix);
    ui.label_userImage->setPixmap(pix);
}

void UserInformationOperator::EnsureOperUser()
{
    UserInfo user_info;

    emit OperUserInfo(user_info, oper_mode);
}

void UserInformationOperator::QuitOperUser()
{
    this->close();
    emit CancelOperUser();
}

void UserInformationOperator::ShowMessage(OPERATE_RESULT msg)
{
    if (msg == ADD_USER_SUCCESS)
    {
        msg_box_.information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("������û��ɹ���"));
    }
    else if (msg == ADD_USER_FAILED)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("������û�ʧ�ܣ����Ժ����ԣ�"));
    }
    else if (msg == MODIFY_USER_SUCCESS)
    {
        msg_box_.information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�޸��û���Ϣ�ɹ���"));
    }
    else if (msg == MODIFY_USER_FAILED)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�޸��û���Ϣʧ�ܣ����Ժ����ԣ�"));
    }
}

void UserInformationOperator::closeEvent(QCloseEvent * event)
{
    //emit CancelOperUser();
}
