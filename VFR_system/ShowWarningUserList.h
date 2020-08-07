#pragma once

#include <QWidget>
#include "ui_ShowWarningUserList.h"
#include "comDefine.h"

class ShowWarningUserList : public QWidget
{
	Q_OBJECT

public:
	ShowWarningUserList(QWidget *parent = Q_NULLPTR);
	~ShowWarningUserList();

	void SetUserList(QVector<QString>&buff);
	void ShowUserList();

	void DealClickedListItem(QListWidgetItem *item);

signals:
	void SignalWarningUser(QString name);
	void SignalRefreshList();

private:
	Ui::ShowWarningUserList ui;
	QVector<QString> warning_user_list;
};
