/**
 * @file Updater.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 自动升级类头文件
 * @version 0.1
 * @date 2019-08-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_UPDATER_H_
#define _TINY_IM_UPDATER_H_
#include <vector>
#include "Path.h"

class CFileTaskThread;
//自动升级类
class Updater
{
public:
    Updater(CFileTaskThread* pFileTask);
	~Updater();

	BOOL IsNeedUpdate();

public:
    CFileTaskThread*	    m_pFileTask; //文件下载任务线程
	std::vector<CString>	m_aryUpdateFileList; //需要更新的文件的列表
};
#endif