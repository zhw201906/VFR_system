#include "ShowWarningUserList.h"

ShowWarningUserList::ShowWarningUserList(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.listWidget_warningList, &QListWidget::itemClicked, this, &ShowWarningUserList::DealClickedListItem);
	connect(ui.pushButton_refreshList, &QPushButton::clicked, [=]() {
		emit SignalRefreshList();
	});
	connect(ui.pushButton_closeList, &QPushButton::clicked, [=]() {
		this->close();
	});
}

ShowWarningUserList::~ShowWarningUserList()
{
}

void ShowWarningUserList::SetUserList(QVector<QString>& buff)
{
	warning_user_list.clear();
	auto it = buff.rbegin();
	while (it != buff.rend())
	{
		warning_user_list.push_back(*it);
		++it;
	}
}

void ShowWarningUserList::ShowUserList()
{
	ui.listWidget_warningList->clear();

	auto it = warning_user_list.begin();
	while (it != warning_user_list.end())
	{
		ui.listWidget_warningList->addItem(*it);
		++it;
	}
}

void ShowWarningUserList::DealClickedListItem(QListWidgetItem * item)
{
	emit SignalWarningUser(item->text());
}
