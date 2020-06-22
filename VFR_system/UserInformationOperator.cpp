#include "UserInformationOperator.h"

//等比缩放图像
//参数1：图像路径（IN）
//参数2：空间宽度（IN）
//参数3：控件高度（IN）
//参数4：缩放后结果图（OUT）
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

    ui.label_userInforOperator->setText(QString::fromLocal8Bit("添加新用户"));

    connect(ui.pushButton_loadUserImage, &QPushButton::clicked, this, &UserInformationOperator::LoadUserImage);
    connect(ui.pushButton_cancelUser, &QPushButton::clicked, this, &UserInformationOperator::QuitOperUser);
    connect(ui.pushButton_saveUser, &QPushButton::clicked, this, &UserInformationOperator::EnsureOperUser);

    //修改用户信息时，需要先加载用户信息
}

UserInformationOperator::UserInformationOperator(UserInfo & user, USER_OPER oper)
    :oper_mode(oper)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    //修改时需要将原本参数加载到界面对应框中显示

    ui.label_userInforOperator->setText(QString::fromLocal8Bit("修改用户信息"));

    connect(ui.pushButton_loadUserImage, &QPushButton::clicked, this, &UserInformationOperator::LoadUserImage);
    connect(ui.pushButton_cancelUser, &QPushButton::clicked, this, &UserInformationOperator::QuitOperUser);
    connect(ui.pushButton_saveUser, &QPushButton::clicked, this, &UserInformationOperator::EnsureOperUser);

}

UserInformationOperator::~UserInformationOperator()
{

}

void UserInformationOperator::LoadUserImage()
{
    QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片"), OPEN_IMAGE_DIR,
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
        msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("添加新用户成功！"));
    }
    else if (msg == ADD_USER_FAILED)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("添加新用户失败，请稍后重试！"));
    }
    else if (msg == MODIFY_USER_SUCCESS)
    {
        msg_box_.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("修改用户信息成功！"));
    }
    else if (msg == MODIFY_USER_FAILED)
    {
        msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("修改用户信息失败，请稍后重试！"));
    }
}

void UserInformationOperator::closeEvent(QCloseEvent * event)
{
    //emit CancelOperUser();
}
