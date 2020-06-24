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

	user_image_path = QString(user.img_url);
	QPixmap pix;
	Geometric_Scaling_Image(user_image_path, ui.label_userImage->width(), ui.label_userImage->height(), pix);
	ui.label_userImage->setPixmap(pix);

	ui.lineEdit_userName->setText(QString::fromLocal8Bit(user.user_name));
	//ui.dateEdit_userBirthday->setText(QString::fromLocal8Bit(user.birthday));
	ui.lineEdit_userCardId->setText(QString::fromLocal8Bit(user.card_number));
	ui.lineEdit_userPhone->setText(QString::fromLocal8Bit(user.phone));
	ui.lineEdit_userProvices->setText(QString::fromLocal8Bit(user.province));
	ui.lineEdit_userCity->setText(QString::fromLocal8Bit(user.city));
	ui.lineEdit_userAddress->setText(QString::fromLocal8Bit(user.address));

	if (user.sex == 0)
	{
		ui.comboBox_userSex->setCurrentText(QString::fromLocal8Bit("男"));
	} 
	else if(user.sex == 1)
	{
		ui.comboBox_userSex->setCurrentText(QString::fromLocal8Bit("女"));
	}
	else if (user.sex == 2)
	{
		ui.comboBox_userSex->setCurrentText(QString::fromLocal8Bit("未知"));
	}
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
	QString name = ui.lineEdit_userName->text();
	if (name.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("姓名不允许为空！"));
		return;
	}

	if (user_image_path.isEmpty())
	{
		msg_box_.critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("照片不允许为空！"));
		return;
	}

	strcpy(user_info.user_name, name.toUtf8());
	strcpy(user_info.img_url, user_image_path.toUtf8());
	//strcpy(user_info.birthday, ui.dateEdit_userBirthday->text().toUtf8());
	strcpy(user_info.card_number, ui.lineEdit_userCardId->text().toUtf8());
	strcpy(user_info.province, ui.lineEdit_userProvices->text().toUtf8());
	strcpy(user_info.city, ui.lineEdit_userCity->text().toUtf8());
	strcpy(user_info.address, ui.lineEdit_userAddress->text().toUtf8());
	strcpy(user_info.phone, ui.lineEdit_userPhone->text().toUtf8());
	
	if (ui.comboBox_userSex->currentText().compare(QString::fromLocal8Bit("男")) == 0)
	{
		user_info.sex = 0;
	}	
	else if (ui.comboBox_userSex->currentText().compare(QString::fromLocal8Bit("女")) == 0)
	{
		user_info.sex = 1;

	}
	else if (ui.comboBox_userSex->currentText().compare(QString::fromLocal8Bit("未知")) == 0)
	{
		user_info.sex = 2;

	}

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
