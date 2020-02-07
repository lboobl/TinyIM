/**
 * @file UdpSender.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief UDP多播示例程序的发送代码
 * @version 0.1
 * @date 2020-02-06
 *
 * @copyright Copyright (c) 2020
 *
 * //http://www.cs.ubbcluj.ro/~dadi/compnet/labs/lab3/udp-broadcast.html
 */

#ifdef WIN32
#include"winsock2.h"
#pragma comment(lib, "Ws2_32.lib")
#else
#endif
#include "CommonMsg.h"
#include<iostream>
#include<conio.h>

const int DEFAULT_PORT = 3345;

int main(int argc, char * argv[])
{

	int port = DEFAULT_PORT;
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	//设置广播选项

	char broadcast = '1';
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
	{
		std::cout << "Error in setting Broadcast option" << std::endl;
		closesocket(sock);
		return -1;
	}

	struct sockaddr_in Recv_addr;
	int len = sizeof(struct sockaddr_in);
	char sendMSG[128] = "Broadcast UDP Sender https://www.dennisthink.com/";

	const int recvBuffLen = 128;
	char recvbuff[recvBuffLen] = "";

	Recv_addr.sin_family = AF_INET;
	Recv_addr.sin_port = htons(DEFAULT_PORT);

	//  Recv_addr.sin_addr.s_addr  = INADDR_BROADCAST; // this isq equiv to 255.255.255.255
	// better use subnet broadcast (for our subnet is 172.30.255.255)

	char ipAddress[64] = { 0 };
	std::cout << "Input Broad Ip Address Like(172.30.255.255):" << std::endl;
	std::cin >> ipAddress;
	Recv_addr.sin_addr.s_addr = inet_addr(ipAddress);
	
	KeepAliveReqMsg reqMsg;
	reqMsg.m_strClientId = "UdpMultiCastDemo";
	TransBaseMsg_t trans(reqMsg.GetMsgType(), reqMsg.ToString());
	while (true)
	{
		memcpy(sendMSG, trans.GetData(), trans.GetSize());
		sendto(sock, sendMSG, trans.GetSize(), 0, (sockaddr *)&Recv_addr, sizeof(Recv_addr));
		recvfrom(sock, recvbuff, recvBuffLen, 0, (sockaddr *)&Recv_addr, &len);
		std::cout << "\n\n\tRECV FROM:(" << inet_ntoa(Recv_addr.sin_addr) << ":" << Recv_addr.sin_port << ") MSG: " << recvbuff << std::endl;
		memset(recvbuff, 0, sizeof(recvbuff));
		memset(sendMSG, 0, sizeof(sendMSG));
		std::cout << " Input SendMsg: ";
		std::cin >> sendMSG;
	}

#ifdef WIN32
	closesocket(sock);
	WSACleanup();
#endif

	return 0;
}


