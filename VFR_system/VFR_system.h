#pragma once

#include <QtWidgets/QWidget>
#include "ui_VFR_system.h"
#include "MainMenu.h"


class VFR_system : public QWidget
{
	Q_OBJECT

public:
	VFR_system(QWidget *parent = Q_NULLPTR);
	~VFR_system();
	void paintEvent(QPaintEvent *event);

	void DealLoginTask();
	//void Deal


private:
	Ui::VFR_systemClass ui;

	MainMenu *main_menu_Handle_;


};
