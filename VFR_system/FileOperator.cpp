#include "FileOperator.h"

FileOperator::FileOperator()
{
}

FileOperator::~FileOperator()
{
}

bool FileOperator::DirIsExisted(QString & path, bool bcreate)
{
	if (path.isEmpty())
	{
		return false;
	}
	QDir dir;
	if (!dir.exists(path))   //Ŀ¼������ʱ������Ŀ¼
	{
		if (bcreate)
		{
			bool ret = dir.mkpath(path);
			return ret;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool FileOperator::FileIsExisted(QString & path)
{
	if (path.isEmpty())
	{
		return false;
	}

	QFileInfo fileInfo(path);
	if (fileInfo.isFile())
	{
		return true;
	}
	
	return false;
}
