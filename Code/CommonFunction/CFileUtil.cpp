#include "CFileUtil.h"
#include <fstream>
#include <stdio.h>
#ifdef _WIN32
#include  <io.h>
#include  <direct.h>
#include  <Windows.h>
#else
#include <unistd.h>
#define _access access
#define _mkdir  mkdir
#define MAX_PATH 256
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <errno.h>
#endif
#include  <stdio.h>
#include  <stdlib.h>
#include "md5.h"

/**
 * @brief 获取文件的大小
 * 
 * @param nFileSize 文件大小，返回值
 * @param strFileName 文件名称
 * @return true 获取成功
 * @return false 获取失败
 */
bool CFileUtil::GetFileSize(int& nFileSize, const std::string strFileName)
{
	FILE * pFile=nullptr;
#ifdef _WIN32
	fopen_s(&pFile,strFileName.c_str(), "rb");
#else
	pFile = fopen(strFileName.c_str(), "rb");
#endif

	if (nullptr != pFile) {
		fseek(pFile, 0L, SEEK_END);
		nFileSize = ftell(pFile);
		fclose(pFile);
		return true;
	}
	else {
		return false;
	}
}

/**
 * @brief 以只读的方式打开文件，将文件名称和文件编号关联起来
 * 
 * @param nFileId 文件ID
 * @param strFileName 文件名称
 * @return true 打开成功
 * @return false 打开失败
 */
bool CFileUtil::OpenReadFile(const int nFileId, const std::string strFileName)
{
	if (!IsFileExist(strFileName))
	{
		return false;
	}
	m_strFileNameMap.erase(nFileId);
	m_strFileNameMap.insert({ nFileId, strFileName });
	FILE * pFile=nullptr;
#ifdef _WIN32
	fopen_s(&pFile,strFileName.c_str(), "rb");
#else
	pFile = fopen(strFileName.c_str(), "rb");
#endif
	//fopen_s(&pFile,strFileName.c_str(), "rb");
	if (nullptr != pFile) {
		m_ReadFileMap.insert({ nFileId,pFile });
		return true;
	}
	else {
		return false;
	}
}


/**
 * @brief 以只写的方式打开文件
 * 
 * @param nFileId 文件ID
 * @param strFileName 文件名称
 * @return true 打开成功
 * @return false 打开失败
 */
bool CFileUtil::OpenWriteFile(const int nFileId, const std::string strFileName)
{
	if (IsFileExist(strFileName))
	{
		return false;
	}
	m_strFileNameMap.erase(nFileId);
	m_strFileNameMap.insert({ nFileId, strFileName });
	FILE * pFile=nullptr;
#ifdef _WIN32
	fopen_s(&pFile,strFileName.c_str(), "wb");
#else
	pFile = fopen(strFileName.c_str(), "wb");
#endif
	//fopen_s(&pFile,strFileName.c_str(), "wb");
	if (nullptr != pFile) {
		m_WriteFileMap.insert({ nFileId,pFile });
		return true;
	}
	else {
		return false;
	}
}

/**
 * @brief 响应文件写入数据
 * 
 * @param nFileId 文件ID
 * @param pData 数据
 * @param nDataLen 数据长度
 * @return true 成功
 * @return false 失败
 */
bool CFileUtil::OnWriteData(const int nFileId, const char * pData, const int nDataLen)
{
	auto item = m_WriteFileMap.find(nFileId);
	if (item != m_WriteFileMap.end())
	{
		auto writeSize = fwrite(pData, 1, nDataLen, item->second);
		if(static_cast<int>(writeSize) == nDataLen)
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}
	return false;
}

/**
 * @brief 根据文件ID从文件中读取数据
 * 
 * @param nFileId 文件ID
 * @param pData 读取到的文件数据
 * @param nReadLen 实际读取到的最大数据长度
 * @param nMaxDataLen 可以读取的数据的最大长度
 * @return true 读取数据成功
 * @return false 读取数据失败
 */
bool CFileUtil::OnReadData(const int nFileId, char * pData, int& nReadLen, const int nMaxDataLen)
{
	auto item = m_ReadFileMap.find(nFileId);
	if (item != m_ReadFileMap.end())
	{
		auto readSize = fread(pData, 1, nMaxDataLen, item->second);
		if (readSize <= 0)
		{
			return false;
		}
		nReadLen = static_cast<int>(readSize);
		return true;
	}
	return false;
}
std::string CFileUtil::GetFileNameFromPath(const std::string strFullPath)
{
	std::string::size_type iPos = strFullPath.find_last_of('\\') + 1;
	std::string filename = strFullPath.substr(iPos, strFullPath.length() - iPos);
	return filename;
}

std::string CFileUtil::GetFileNameExtension(const std::string strFullPath)
{
	std::string::size_type iPos = strFullPath.find_last_of('.');
	if (iPos != std::string::npos)
	{
		std::string filename = strFullPath.substr(iPos, strFullPath.length() - iPos);
		return filename;
	}
	else
	{
		return "";
	}
}
/**
 * @brief 根据文件ID关闭文件
 * 
 * @param nFileId 文件ID
 * @return true 关闭成功
 * @return false 关闭失败
 */
bool CFileUtil::OnCloseFile(const int nFileId)
{
	{
		auto item = m_ReadFileMap.find(nFileId);
		if (item != m_ReadFileMap.end())
		{
			fclose(item->second);
			m_ReadFileMap.erase(nFileId);
			return true;
		}
	}
	{
		auto item = m_WriteFileMap.find(nFileId);
		if (item != m_WriteFileMap.end())
		{
			fclose(item->second);
			m_WriteFileMap.erase(nFileId);
			return true;
		}
	}
	

	return false;
}

/**
 * @brief 根据文件名判断文件是否存在
 * 
 * @param strFileName 文件名
 * @return true 文件存在
 * @return false 文件不存在
 */
bool CFileUtil::IsFileExist(const std::string strFileName)
{
	if (_access(strFileName.c_str(), 0) == 0)
	{
		return true;
	}
	return false;
}


/**
 * @brief 
 * 
 * @param strFileName 
 * @return true 
 * @return false 
 */
bool CFileUtil::RemoveFile(const std::string strFileName)
{
	if (remove(strFileName.c_str()) == 0)
	{
		return true;
	}
	return false;
}

/**
 * @brief 判断文件夹是否存在
 * 
 * @param strFolder 文件夹名称
 * @return true 是文件夹
 * @return false 不是文件夹
 */
bool CFileUtil::IsFolder(const std::string strFolder)
{
	if (_access(strFolder.c_str(), 0) == 0)
	{
		return true;
	}
	return false;
}

/**
 * @brief 创建文件夹
 * 
 * @param strFolder 文件夹路径
 * @return true 创建成功
 * @return false 创建失败
 */
bool CFileUtil::CreateFolder(const std::string strFolder)
{
	#ifdef _WIN32
	if(_mkdir(strFolder.c_str())==0)
	{
		return true;
	}
	else
	{
		return false;
	}
	#else
	std::string strFolderNew=strFolder;
	size_t pre=0,pos;
    std::string dir;
    int mdret;

    if(strFolderNew[strFolderNew.size()-1]!='/'){
        // force trailing / so we can handle everything in loop
        strFolderNew+='/';
    }
	mode_t mode =0755;
    while((pos=strFolderNew.find_first_of('/',pre))!=std::string::npos){
        dir=strFolderNew.substr(0,pos++);
        pre=pos;
        if(dir.size()==0) continue; // if leading / first time is 0 length
        if((mdret=::mkdir(dir.c_str(),mode)) && errno!=EEXIST){
            return mdret;
        }
    }
    return mdret;
	#endif
}


/**
 * @brief 删除文件夹
 * 
 * @param strFolder 文件
 * @return true 成功
 * @return false 失败
 */
bool CFileUtil::RemoveFolder(const std::string strFolder)
{
	#ifdef _WIN32
	if(_rmdir(strFolder.c_str()) == 0)
	{
		return true;
	}
	return false;
	#else
	if(unlink(strFolder.c_str()))
	{
		return true;
	}
	return false;
	#endif
}


/**
 * @brief 
 * 
 * @param nFileId 
 * @param FileName 
 * @return true 
 * @return false 
 */
/*bool CFileUtil::CreateFileByName(const int nFileId, const std::string FileName)
{
	return false;
}*/

/**
 * @brief 计算文件的HASH值
 * 
 * @param strFileName 文件名称
 * @return std::string 文件的HASH值
 */
std::string CFileUtil::CalcHash(const std::string strFileName)
{
	MD5 hashUtil;
	int fileId = rand();
	if (OpenReadFile(fileId, strFileName))
	{
		char buff[1024] = { 0 };
		int nReadLen = 0;
		while (OnReadData(fileId, buff, nReadLen, 1024))
		{
			hashUtil.update(buff, nReadLen);
		}
		OnCloseFile(fileId);
		auto result = hashUtil.finalize();
		return result.hexdigest();
	}
	return "";
}

std::string CFileUtil::GetCurDir()
{
	char chpath[MAX_PATH];
	#ifdef _WIN32
	GetModuleFileName(NULL, (LPSTR)chpath, sizeof(chpath));
	#else
	getcwd(chpath,MAX_PATH);
	#endif
	std::string strPath = std::string(chpath);
	strPath = strPath.substr(0, strPath.rfind("\\") + 1);
	return strPath;
}

std::string CFileUtil::GetFileName(const int nFileId)
{
	auto item = m_strFileNameMap.find(nFileId);
	if (item != m_strFileNameMap.end())
	{
		return item->second;
	}
	else
	{
		return "";
	}
}

bool CFileUtil::UtilCopy(const std::string strSrcName, const std::string strDstName)
{
	#ifdef _WIN32
	CopyFileA(strSrcName.c_str(), strDstName.c_str(), TRUE);
	#else
	if(strSrcName.empty() || strDstName.empty())
	{
		return false;
	}
	#endif
	return true;
}