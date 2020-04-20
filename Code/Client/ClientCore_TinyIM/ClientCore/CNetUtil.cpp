#include "CNetUtil.h"
#ifdef WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#endif

/**
 * @brief IP地址的数据结构
 * 
 */
struct IpStruct 
{
	std::string m_strIpAddr;//IP地址
	std::string m_strNetMask;//子网掩码
	std::string m_strNetGate;//默认网关
};

/**
 * @brief 是否是一个好的ip地址
 * 
 * @param value IP地址
 * @return true 是好的ip地址
 * @return false 不是好的ip地址
 */
static bool IsGoodIpStruct(const IpStruct& value)
{
	if (value.m_strIpAddr == "0.0.0.0")
	{
		return false;
	}
	if (value.m_strNetGate == "0.0.0.0")
	{
		return false;
	}
	if (value.m_strNetMask == "0.0.0.0")
	{
		return false;
	}
	if (value.m_strIpAddr == "127.0.0.1")
	{
		return false;
	}
	return true;
}

/**
 * @brief 获取IP地址对应的UDP广播地址
 * 
 * @param value IP地址
 * @return std::string UDP广播地址
 */
std::string ConvertToUdpMultiCast(const IpStruct& value)
{
	if (IsGoodIpStruct(value))
	{
		long nIp = inet_addr(value.m_strIpAddr.c_str());
		long nMask = inet_addr(value.m_strNetMask.c_str());
		long nMaskInversion = ~nMask;
		long nIpMultiCast = nIp | nMaskInversion;
		struct in_addr inIpAddr;
		inIpAddr.S_un.S_addr = nIpMultiCast;
		std::string strResult = inet_ntoa(inIpAddr);
		return strResult;
	}
	else
	{
		return "";
	}
}

#ifdef WIN32
/**
 * @brief 获取本机网卡的所有IP地址
 * 
 * @return std::vector<IpStruct> 所有IP地址信息
 */
static std::vector<IpStruct> GetAllIpStruct()
{
	std::vector<IpStruct> result;
	do
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		UINT i;

		/* variables used to print DHCP time info */
		struct tm newtime;
		char buffer[32];
		errno_t error;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
		// Make an initial call to GetAdaptersInfo to get
		// the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL) {
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
				break;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			while (pAdapter) {
				printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
				printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
				printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
				printf("\tAdapter Addr: \t");
				for (i = 0; i < pAdapter->AddressLength; i++) {
					if (i == (pAdapter->AddressLength - 1))
						printf("%.2X\n", (int)pAdapter->Address[i]);
					else
						printf("%.2X-", (int)pAdapter->Address[i]);
				}
				printf("\tIndex: \t%d\n", pAdapter->Index);
				printf("\tType: \t");
				switch (pAdapter->Type) {
				case MIB_IF_TYPE_OTHER:
					printf("Other\n");
					break;
				case MIB_IF_TYPE_ETHERNET:
					printf("Ethernet\n");
					break;
				case MIB_IF_TYPE_TOKENRING:
					printf("Token Ring\n");
					break;
				case MIB_IF_TYPE_FDDI:
					printf("FDDI\n");
					break;
				case MIB_IF_TYPE_PPP:
					printf("PPP\n");
					break;
				case MIB_IF_TYPE_LOOPBACK:
					printf("Lookback\n");
					break;
				case MIB_IF_TYPE_SLIP:
					printf("Slip\n");
					break;
				default:
					printf("Unknown type %ld\n", pAdapter->Type);
					break;
				}

				printf("\tIP Address: \t%s\n",
					pAdapter->IpAddressList.IpAddress.String);
				printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

				printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
				printf("\t***\n");
				{
					IpStruct oneValue;
					oneValue.m_strIpAddr = pAdapter->IpAddressList.IpAddress.String;
					oneValue.m_strNetMask = pAdapter->IpAddressList.IpMask.String;
					oneValue.m_strNetGate = pAdapter->GatewayList.IpAddress.String;
					result.push_back(oneValue);
				}
				if (pAdapter->DhcpEnabled) {
					printf("\tDHCP Enabled: Yes\n");
					printf("\t  DHCP Server: \t%s\n",
						pAdapter->DhcpServer.IpAddress.String);

					printf("\t  Lease Obtained: ");
					/* Display local time */
					error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
					if (error)
						printf("Invalid Argument to _localtime32_s\n");
					else {
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
							printf("Invalid Argument to asctime_s\n");
						else
							/* asctime_s returns the string terminated by \n\0 */
							printf("%s", buffer);
					}

					printf("\t  Lease Expires:  ");
					error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
					if (error)
						printf("Invalid Argument to _localtime32_s\n");
					else {
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
							printf("Invalid Argument to asctime_s\n");
						else
							/* asctime_s returns the string terminated by \n\0 */
							printf("%s", buffer);
					}
				}
				else
					printf("\tDHCP Enabled: No\n");

				if (pAdapter->HaveWins) {
					printf("\tHave Wins: Yes\n");
					printf("\t  Primary Wins Server:    %s\n",
						pAdapter->PrimaryWinsServer.IpAddress.String);
					printf("\t  Secondary Wins Server:  %s\n",
						pAdapter->SecondaryWinsServer.IpAddress.String);
				}
				else
					printf("\tHave Wins: No\n");
				pAdapter = pAdapter->Next;
				printf("\n");
			}
		}
		else {
			printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

		}
		if (pAdapterInfo)
			FREE(pAdapterInfo);

		break;
	}while (0);
	return result;
}
#endif

/**
 * @brief 获取所有的IP地址
 * 
 * @return std::vector<std::string> 所有的IP地址
 */
std::vector<std::string> CNetUtil::GetAllIpAddr()
{
	std::vector<std::string> ipVec;
	return ipVec;
}

/**
 * @brief 获取所有的UDP广播地址
 * 
 * @return std::vector<std::string> UDP广播地址的数组
 */
std::vector<std::string> CNetUtil::GetUdpMultiCastIpAddrVec()
{
	std::vector<std::string> ipVec;
	{
		auto allIpStruct = GetAllIpStruct();
		for (auto item : allIpStruct)
		{
			if (IsGoodIpStruct(item))
			{
				ipVec.push_back(ConvertToUdpMultiCast(item));
			}
		}
	}
	return ipVec;
}