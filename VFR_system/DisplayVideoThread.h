#pragma once

#include <QObject>
#include <QVector>

class DisplayVideoThread : public QObject
{
	Q_OBJECT

public:
	DisplayVideoThread(QObject *parent);
	DisplayVideoThread(QQueue<QImage> *pCache);
	~DisplayVideoThread();

	void  DisplayThread();

signals:
	void  DisplayVideoSignal(int chnnalId);


private:
	QQueue<QImage> *video_cache_ = NULL;
	QVector<int>   camera_id_buff_;

};
