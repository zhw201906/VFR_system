#include "VFR_system.h"
#include <QPainter>
#include <QPushButton>

VFR_system::VFR_system(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowIcon(QIcon("./icon/icon.jpg"));

	main_menu_Handle_ = NULL;

	

	//退出系统按钮
	connect(ui.pushButton_exit, &QPushButton::clicked, [=]() {
		this->close();
	});

	//登录系统按钮
	connect(ui.pushButton_login, &QPushButton::clicked, [=]() {
		DealLoginTask();
	});
}

VFR_system::~VFR_system()
{
}

void VFR_system::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	QPixmap pix;
	pix.load("./icon/login.jpg");
	//指定长宽
	painter.drawPixmap(0, 0, this->width(), this->height(), pix);
}

void VFR_system::DealLoginTask()
{
	if (main_menu_Handle_ == NULL) 
	{
		main_menu_Handle_ = new MainMenu();
	}

	main_menu_Handle_->show();
	this->hide();
	main_menu_Handle_->RefreshVideoDisplayWindow();
}
