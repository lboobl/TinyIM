/**
 * @file Path.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 对Windows下操作文件，文件夹的封装
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_PATH_H_
#define _TINY_IM_PATH_H_
#include "UICommonType.h"
namespace Hootina
{

class CPath
{
public:
	CPath(void);
	~CPath(void);

	static WString GetAppPath();												// 获取应用程序执行路径
	static std::string GetAppPathAscii();										// 获取应用程序执行路径
	static WString GetCurDir();													// 获取应用程序当前目录
	static WString GetTempPath();												// 获取当前系统的临时文件夹的路径
	static WString GetTempFileName(LPCTSTR lpszFileName);						// 获取当前系统的临时文件夹的路径下的唯一命名的临时文件名(全路径)
	static WString GetRandomFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName);	// 获取随机文件名(全路径)

	static BOOL IsDirectory(LPCTSTR lpszPath);									// 检测指定路径是否目录
	static BOOL IsFileExist(LPCTSTR lpszFileName);								// 检测指定文件是否存在
	static BOOL IsDirectoryExist(LPCTSTR lpszPath);								// 检测指定目录是否存在
	static BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);

	static WString GetPathRoot(LPCTSTR lpszPath);								// 获取指定路径的根目录信息
	static WString GetDirectoryName(LPCTSTR lpszPath);							// 返回指定路径字符串的目录信息
	static WString GetFileName(LPCTSTR lpszPath);								// 返回指定路径字符串的文件名和扩展名
	static WString GetFileNameWithoutExtension(LPCTSTR lpszPath);				// 返回不具有扩展名的路径字符串的文件名
	static WString GetExtension(LPCTSTR lpszPath);								// 返回指定的路径字符串的扩展名
	static WString GetFullPath(LPCTSTR lpszPath);								// 根据指定的相对路径获取绝对路径
	static long    GetFileSize(LPCTSTR lpszPath);								// 获取文件大小
	static bool    CopyFilePath(const std::string strSrcFile, const std::string strDstFile);
};

}
#endif