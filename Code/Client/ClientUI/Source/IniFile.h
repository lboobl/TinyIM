
/**
 * @brief Ini文件操作类
 * 
 */



#ifndef _TINY_IM_C_INI_FILE_H_ 
#define _TINY_IM_C_INI_FILE_H_

class CIniFile
{
public:
	CIniFile();
	~CIniFile();

public:
	UINT ReadInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, long nDefault, LPCTSTR lpFileName);
	BOOL ReadString(LPCTSTR lpAppName, LPCTSTR lpKeyName,LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName);
	BOOL WriteInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, long nValue, LPCTSTR lpFileName);
	BOOL WriteString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName);
};
#endif 
