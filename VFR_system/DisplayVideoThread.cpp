#include "DisplayVideoThread.h"

DisplayVideoThread::DisplayVideoThread(QObject *parent)
	: QObject(parent)
{

}

DisplayVideoThread::DisplayVideoThread(QQueue<QImage>* pCache)
{
	video_cache_ = pCache;
}

DisplayVideoThread::~DisplayVideoThread()
{

}

void DisplayVideoThread::DisplayThread()
{
	while (1)
	{

	}
}
