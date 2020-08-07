#pragma once

#include <QWidget>
#include "ui_ShowUserListTrack.h"
#include "comDefine.h"
#include "comDataStruct.h"
#include <QStringList>
#include <QTableWidgetItem>
#include <QTableWidget>

#include "ShowSnapRecgImage.h"

#define  TABLE_COLS    3

typedef struct AdjoinPosition_ {
	QPoint start;
	QPoint end;
	QString url;
}AdjoinPosition;

class ShowUserListTrack : public QWidget
{
	Q_OBJECT

public:
	ShowUserListTrack(QMap<QString, CameraPositionInfo>& track_map, QString& dir, QWidget *parent = Q_NULLPTR);
	~ShowUserListTrack();

	void DealClickedItem(QTableWidgetItem *item);
	void DealClickedItemCell(int row, int col);

signals:
	void CurrentClickedLinkTrack(QPoint start, QPoint end);
	void CloseSignals();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
	Ui::ShowUserListTrack ui;
	QVector<AdjoinPosition> link_track_buff;

	ShowSnapRecgImage *p_show_snap_image;
};
