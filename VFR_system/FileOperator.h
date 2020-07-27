#pragma once
#ifndef  FILE_OPERATOR_H_
#define  FILE_OPERATOR_H_

#include <QObject>
#include <QDir>
#include <QFileInfo>

#include "comDefine.h"

class FileOperator
{
	//Q_OBJECT

public:
	FileOperator();
	~FileOperator();

	//检测文件夹是否存在，第二个参数为true自动创建，为false，则不会创建
	static bool DirIsExisted(QString &path, bool bcreate = true);
	//检测文件是否存在
	static bool FileIsExisted(QString &path);
	
};

#endif