/**
 * @file TinyIMConfig.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief TinyIM配置对应的结构体
 * @version 0.1
 * @date 2020-04-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef _TINY_IM_CONFIG_H_
#define _TINY_IM_CONFIG_H_
#include <string>

/**
 * @brief TinyIM的服务配置
 * 
 */
class CTinyServerConfig
{
public:
	bool Valid() const 
	{
		return true;
	}
    std::string m_strClientCoreIp;
    int m_nClientCorePort;
};

/**
 * @brief 关于对话框的配置
 * 
 */
class CAboutDlgMsgConfig
{
public:
    std::string m_strAboutDlgTitle;//关于对话框的标题
    std::string m_strCompanyUrlLink;//公司网址连接
    std::string m_strCopyRight;//声明版权信息
    std::string m_strDefaultCompanyName;//公司名称
    std::string m_strAboutInfo;//关于对话框的展示信息
};


/**
 * @brief 用户设置的配置
 * 
 */
class CUserSetConfig
{

};

#endif