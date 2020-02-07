
/**
 * @file UdpReciver.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief UDP多播示例程序的接收代码
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
#endif 

#include<iostream>
#include<conio.h>


#define MYPORT 3345    // the port users will be connecting to


int main(int argc, char * argv[])
{

#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	char broadcast = '1';

	//     This option is needed on the socket in order to be able to receive broadcast messages
	//   If not set the receiver will not receive broadcast messages in the local network.
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
	{
		std::cout << "Error in setting Broadcast option";
		closesocket(sock);
		return 0;
	}

	struct sockaddr_in Recv_addr;
	struct sockaddr_in Sender_addr;

	int len = sizeof(struct sockaddr_in);

	char recvbuff[128] = { 0 };
	int recvbufflen = 128;

	char sendMSG[] = "Broadcast UDP Sender https://www.dennisthink.com/";

	Recv_addr.sin_family = AF_INET;
	Recv_addr.sin_port = htons(MYPORT);
	Recv_addr.sin_addr.s_addr = INADDR_ANY;



	if (bind(sock, (sockaddr*)&Recv_addr, sizeof(Recv_addr)) < 0)
	{
#ifdef _WIN32_
		std::cout << "Error in BINDING" << WSAGetLastError();
		_getch();
		closesocket(sock);
#endif
		return 0;
	}


	while (true)
	{
		recvfrom(sock, recvbuff, recvbufflen, 0, (sockaddr *)&Sender_addr, &len);
		std::cout << "\n\n\tRECV FROM:(" << inet_ntoa(Sender_addr.sin_addr) << ":" << Sender_addr.sin_port << ") MSG: " << recvbuff << std::endl;
		if (sendto(sock, recvbuff, len, 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
		{
			std::cout << "Error in Sending." << WSAGetLastError();
			std::cout << "\n\n\t\t Press any key to continue....";
			_getch();
			closesocket(sock);
			return 0;
		}
		else
		{
			std::cout << "\n\n\n\tREADER sends the broadcast message Successfully";
		}
	}

	std::cout << "\n\n\tpress any key to CONT...";
	_getch();

#ifdef WIN32
	closesocket(sock);
	WSACleanup();
#endif 

	return 0;
}


