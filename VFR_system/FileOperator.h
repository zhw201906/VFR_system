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

	//����ļ����Ƿ���ڣ��ڶ�������Ϊtrue�Զ�������Ϊfalse���򲻻ᴴ��
	static bool DirIsExisted(QString &path, bool bcreate = true);
	//����ļ��Ƿ����
	static bool FileIsExisted(QString &path);
	
};

#endif