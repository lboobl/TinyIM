/**
 * @file CNetUtil.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 为了支持UDP广播模式,而编写的
 * @version 0.1
 * @date 2020-04-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef  _DENNIS_THINK_C_NET_UTIL_H_
#define  _DENNIS_THINK_C_NET_UTIL_H_
#include <string>
#include <vector>
class CNetUtil
{
public:
	static std::vector<std::string> GetAllIpAddr();
	static std::vector<std::string> GetUdpMultiCastIpAddrVec();
};
#endif // ! _DENNIS_THINK_C_NET_UTIL_H_
