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
