/**
 * Windows平台用编码格式相互转换类, EncodingUtil.cpp
 * zhangyl 2017.03.29
 **/
//#include "stdafx.h"

//#include <iconv.h>          //linux only
#ifdef _WIN32
#include <Windows.h>
#else
#include <locale.h>
#include <locale>
#include <codecvt>
#include <string>
#endif
#include "EncodingUtil.h"


#ifdef _WIN32

/**
 * @brief ascii的字符串转为Unicode编码
 * 
 * @param lpszStr ascii字符串
 * @return wchar_t* 转换后的unicode字符串(使用完毕后要释放)
 */
wchar_t* EncodeUtil::AnsiToUnicode(const char* lpszStr)
{
    wchar_t* lpUnicode;
    int nLen;

    if (NULL == lpszStr)
        return NULL;

    nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, NULL, 0);
    if (0 == nLen)
        return NULL;

    lpUnicode = new wchar_t[nLen + 1];
    if (NULL == lpUnicode)
        return NULL;

    memset(lpUnicode, 0, sizeof(wchar_t)* (nLen + 1));
    nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        return NULL;
    }

    return lpUnicode;
}


/**
 * @brief unicode字符串转为ascii字符串
 * 
 * @param lpszStr unicode的字符串
 * @return char* 转换后的ascii字符串(使用完毕后要释放)
 */
char* EncodeUtil::UnicodeToAnsi(const wchar_t* lpszStr)
{
    char* lpAnsi;
    int nLen;

    if (NULL == lpszStr)
        return NULL;

    nLen = ::WideCharToMultiByte(CP_ACP, 0, lpszStr, -1, NULL, 0, NULL, NULL);
    if (0 == nLen)
        return NULL;

    lpAnsi = new char[nLen + 1];
    if (NULL == lpAnsi)
        return NULL;

    memset(lpAnsi, 0, nLen + 1);
    nLen = ::WideCharToMultiByte(CP_ACP, 0, lpszStr, -1, lpAnsi, nLen, NULL, NULL);
    if (0 == nLen)
    {
        delete[]lpAnsi;
        return NULL;
    }

    return lpAnsi;
}

/**
 * @brief ascii格式的字符串转为utf8格式的字符串
 * 
 * @param lpszStr ascii格式的字符
 * @return char* utf8格式的字符串
 */
char* EncodeUtil::AnsiToUtf8(const char* lpszStr)
{
    wchar_t* lpUnicode;
    char* lpUtf8;
    int nLen;

    if (NULL == lpszStr)
        return NULL;

    nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, NULL, NULL);
    if (0 == nLen)
        return NULL;

    lpUnicode = new wchar_t[nLen + 1];
    if (NULL == lpUnicode)
        return NULL;

    memset(lpUnicode, 0, sizeof(wchar_t)* (nLen + 1));
    nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        return NULL;
    }

    nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, NULL, 0, NULL, NULL);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        return NULL;
    }

    lpUtf8 = new char[nLen + 1];
    if (NULL == lpUtf8)
    {
        delete[]lpUnicode;
        return NULL;
    }

    memset(lpUtf8, 0, nLen + 1);
    nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, lpUtf8, nLen, NULL, NULL);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        delete[]lpUtf8;
        return NULL;
    }

    delete[]lpUnicode;

    return lpUtf8;
}


/**
 * @brief utf8格式的字符串转为ascii格式的字符串
 * 
 * @param lpszStr utf8格式的字符串
 * @return char* ascii格式的字符串
 */
char* EncodeUtil::Utf8ToAnsi(const char* lpszStr)
{
    wchar_t* lpUnicode;
    char* lpAnsi;
    int nLen;

    if (NULL == lpszStr)
        return NULL;

    nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, NULL, NULL);
    if (0 == nLen)
        return NULL;

    lpUnicode = new wchar_t[nLen + 1];
    if (NULL == lpUnicode)
        return NULL;

    memset(lpUnicode, 0, sizeof(wchar_t)* (nLen + 1));
    nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        return NULL;
    }

    nLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, NULL, 0, NULL, NULL);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        return NULL;
    }

    lpAnsi = new char[nLen + 1];
    if (NULL == lpAnsi)
    {
        delete[]lpUnicode;
        return NULL;
    }

    memset(lpAnsi, 0, nLen + 1);
    nLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, lpAnsi, nLen, NULL, NULL);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        delete[]lpAnsi;
        return NULL;
    }

    delete[]lpUnicode;

    return lpAnsi;
}


/**
 * @brief Unicode格式的字符串转为utf8格式的字符串
 * 
 * @param lpszStr unicode格式的字符串
 * @return char* utf8格式的字符串
 */
char* EncodeUtil::UnicodeToUtf8(const wchar_t* lpszStr)
{
    char* lpUtf8;
    int nLen;

    if (NULL == lpszStr)
        return NULL;

    nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpszStr, -1, NULL, 0, NULL, NULL);
    if (0 == nLen)
        return NULL;

    lpUtf8 = new char[nLen + 1];
    if (NULL == lpUtf8)
        return NULL;

    memset(lpUtf8, 0, nLen + 1);
    nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpszStr, -1, lpUtf8, nLen, NULL, NULL);
    if (0 == nLen)
    {
        delete[]lpUtf8;
        return NULL;
    }

    return lpUtf8;
}


/**
 * @brief utf8格式的字符串转为unicode格式的字符串
 * 
 * @param lpszStr utf8格式的字符串
 * @return wchar_t* unicode格式的字符串
 */
wchar_t* EncodeUtil::Utf8ToUnicode(const char* lpszStr)
{
    wchar_t* lpUnicode;
    int nLen;

    if (NULL == lpszStr)
        return NULL;

    nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, NULL, 0);
    if (0 == nLen)
        return NULL;

    lpUnicode = new wchar_t[nLen + 1];
    if (NULL == lpUnicode)
        return NULL;

    memset(lpUnicode, 0, sizeof(wchar_t)* (nLen + 1));
    nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen)
    {
        delete[]lpUnicode;
        return NULL;
    }

    return lpUnicode;
}

/**
 * @brief ascii格式的字符串转为unicode格式的
 * 
 * @param lpszAnsi ascii格式的字符串
 * @param lpszUnicode 转换后的unicode格式的字符串
 * @param nLen ascii的字符串的长度
 * @return true 转换成功
 * @return false 转换失败
 */
bool EncodeUtil::AnsiToUnicode(const char* lpszAnsi, wchar_t* lpszUnicode, int nLen)
{
    int nRet = ::MultiByteToWideChar(CP_ACP, 0, lpszAnsi, -1, lpszUnicode, nLen);
    return (0 == nRet) ? FALSE : TRUE;
}

/**
 * @brief unicode格式的字符串转为ascii格式的字符串
 * 
 * @param lpszUnicode unicode字符串
 * @param lpszAnsi 转换后的ascii字符串
 * @param nLen unicode字符串的长度
 * @return true 转换成功
 * @return false 转换失败
 */
bool EncodeUtil::UnicodeToAnsi(const wchar_t* lpszUnicode, char* lpszAnsi, int nLen)
{
    int nRet = ::WideCharToMultiByte(CP_ACP, 0, lpszUnicode, -1, lpszAnsi, nLen, NULL, NULL);
    return (0 == nRet) ? FALSE : TRUE;
}

/**
 * @brief ascii格式转为utf8格式的字符串
 * 
 * @param lpszAnsi ascii格式的字符串
 * @param lpszUtf8 utf8格式的字符串
 * @param nLen 
 * @return true 转换成功 
 * @return false 转换失败
 */
bool EncodeUtil::AnsiToUtf8(const char* lpszAnsi, char* lpszUtf8, int nLen)
{
    wchar_t* lpszUnicode = EncodeUtil::AnsiToUnicode(lpszAnsi);
    if (NULL == lpszUnicode)
        return FALSE;

    int nRet = EncodeUtil::UnicodeToUtf8(lpszUnicode, lpszUtf8, nLen);

    delete[]lpszUnicode;

    return (0 == nRet) ? FALSE : TRUE;
}

bool EncodeUtil::Utf8ToAnsi(const char* lpszUtf8, char* lpszAnsi, int nLen)
{
    wchar_t* lpszUnicode = EncodeUtil::Utf8ToUnicode(lpszUtf8);
    if (NULL == lpszUnicode)
        return FALSE;

    int nRet = UnicodeToAnsi(lpszUnicode, lpszAnsi, nLen);

    delete[]lpszUnicode;

    return (0 == nRet) ? FALSE : TRUE;
}

bool EncodeUtil::UnicodeToUtf8(const wchar_t* lpszUnicode, char* lpszUtf8, int nLen)
{
    int nRet = ::WideCharToMultiByte(CP_UTF8, 0, lpszUnicode, -1, lpszUtf8, nLen, NULL, NULL);
    return (0 == nRet) ? FALSE : TRUE;
}

bool EncodeUtil::Utf8ToUnicode(const char* lpszUtf8, wchar_t* lpszUnicode, int nLen)
{
    int nRet = ::MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, lpszUnicode, nLen);
    return (0 == nRet) ? FALSE : TRUE;
}


/**
 * @brief ascii格式的字符串转为unicode格式的字符串
 * 
 * @param strAnsi ascii格式的字符串
 * @return std::wstring unicode格式的字符串
 */
std::wstring EncodeUtil::AnsiToUnicode(const std::string& strAnsi)
{
    std::wstring strUnicode;

   wchar_t* lpszUnicode = EncodeUtil::AnsiToUnicode(strAnsi.c_str());
    if (lpszUnicode != NULL)
    {
        strUnicode = lpszUnicode;
        delete[]lpszUnicode;
    }

    return strUnicode;
}

/**
 * @brief unicode格式的字符串转为ascii格式的字符串
 * 
 * @param strUnicode unicode格式的字符串
 * @return std::string ascii格式的字符串
 */
std::string EncodeUtil::UnicodeToAnsi(const std::wstring& strUnicode)
{
    std::string strAnsi;

    char* lpszAnsi = UnicodeToAnsi(strUnicode.c_str());
    if (lpszAnsi != NULL)
    {
        strAnsi = lpszAnsi;
        delete[]lpszAnsi;
    }

    return strAnsi;
}

/**
 * @brief ascii格式的字符串转为utf8格式
 * 
 * @param strAnsi ascii字符串
 * @return std::string utf8格式的字符串
 */
std::string EncodeUtil::AnsiToUtf8(const std::string& strAnsi)
{
    std::string strUtf8;

    char* lpszUtf8 = AnsiToUtf8(strAnsi.c_str());
    if (lpszUtf8 != NULL)
    {
        strUtf8 = lpszUtf8;
        delete[]lpszUtf8;
    }

    return strUtf8;
}

/**
 * @brief utf8格式的字符串转为ascii格式
 * 
 * @param strUtf8 utf8格式的字符串
 * @return std::string ascii格式的字符串
 */
std::string EncodeUtil::Utf8ToAnsi(const std::string& strUtf8)
{
    std::string strAnsi;

   char* lpszAnsi = Utf8ToAnsi(strUtf8.c_str());
    if (lpszAnsi != NULL)
    {
        strAnsi = lpszAnsi;
        delete[]lpszAnsi;
    }

    return strAnsi;
}

/**
 * @brief Unicode格式字符串转为utf8格式
 * 
 * @param strUnicode unicode格式的字符串
 * @return std::string utf8格式的字符串
 */
std::string EncodeUtil::UnicodeToUtf8(const std::wstring& strUnicode)
{
    std::string strUtf8;

    char* lpszUtf8 = EncodeUtil::UnicodeToUtf8(strUnicode.c_str());
    if (lpszUtf8 != NULL)
    {
        strUtf8 = lpszUtf8;
        delete[]lpszUtf8;
    }

    return strUtf8;
}


/**
 * @brief UTF8格式的字符串转为unicode格式
 * 
 * @param strUtf8 utf8格式的字符串
 * @return std::wstring unicode格式的字符串
 */
std::wstring EncodeUtil::Utf8ToUnicode(const std::string& strUtf8)
{
    std::wstring strUnicode;

    wchar_t* lpszUnicode = EncodeUtil::Utf8ToUnicode(strUtf8.c_str());
    if (lpszUnicode != NULL)
    {
        strUnicode = lpszUnicode;
        delete[]lpszUnicode;
    }

    return strUnicode;
}
#else
/**
 * @brief ascii格式的字符串转为unicode格式的
 * 
 * @param strAnsi ascii格式的字符串
 * @return std::wstring unicode格式的字符串
 */
std::wstring EncodeUtil::AnsiToUnicode(const std::string& strAnsi)
{
    return Utf8ToUnicode(strAnsi);
}

/**
 * @brief unicode格式的字符串转为ascii格式的
 * 
 * @param strUnicode unicode格式的字符串
 * @return std::string ascii格式的字符串
 */
std::string EncodeUtil::UnicodeToAnsi(const std::wstring& strUnicode)
{
    return UnicodeToUtf8(strUnicode);
}

/**
 * @brief ascii格式的字符串转为utf8格式的
 * 
 * @param strAnsi ascii格式的字符串
 * @return std::string utf8格式的字符串
 */
std::string EncodeUtil::AnsiToUtf8(const std::string& strAnsi)
{
    return UnicodeToUtf8(AnsiToUnicode(strAnsi));
}

/**
 * @brief utf8格式的字符串转为ascii格式的
 * 
 * @param strUtf8 utf8格式的字符串
 * @return std::string ascii格式的字符串
 */
std::string EncodeUtil::Utf8ToAnsi(const std::string& strUtf8)
{
   return UnicodeToAnsi(Utf8ToUnicode(strUtf8));
}

/**
 * @brief unicode编码的字符串转为utf8编码的字符串
 * 
 * @param strUnicode unicode编码的字符串
 * @return std::string utf8编码的字符串
 */
std::string EncodeUtil::UnicodeToUtf8(const std::wstring& strUnicode)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX,wchar_t> convertX;    
    return convertX.to_bytes(strUnicode);
}


/**
 * @brief unicode编码的字符串转为utf8编码的字符串
 * 
 * @param strUtf8 utf8编码的字符串
 * @return std::wstring 
 */
std::wstring EncodeUtil::Utf8ToUnicode(const std::string& strUtf8)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX,wchar_t> convertX;
    return convertX.from_bytes(strUtf8);
}
#endif