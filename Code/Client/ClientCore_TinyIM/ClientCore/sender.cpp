////
//// sender.cpp
//// ~~~~~~~~~~
////
//// Copyright (c) - Christopher M. Kohlhoff (chris at kohlhoff dot com)
////
//// Distributed under the Boost Software License, Version 1.0. (See accompanying
//// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////
//
//#include <iostream>
//#include <sstream>
//#include <string>
//#include "asio.hpp"
//
//constexpr short multicast_port = 1;
//constexpr int max_message_count = ;
//
//class sender
//{
//public:
//  sender(asio::io_context& io_context,
//      const asio::ip::address& multicast_address)
//    : endpoint_(multicast_address, multicast_port),
//      socket_(io_context, endpoint_.protocol()),
//      timer_(io_context),
//      message_count_(0)
//  {
//    do_send();
//  }
//
//private:
//  void do_send()
//  {
//    std::ostringstream os;
//    os << "Message " << message_count_++;
//    message_ = os.str();
//
//    socket_.async_send_to(
//        asio::buffer(message_), endpoint_,
//        [this](std::error_code ec, std::size_t /*length*/)
//        {
//          if (!ec && message_count_ < max_message_count)
//            do_timeout();
//        });
//  }
//
//  void do_timeout()
//  {
//    timer_.expires_after(std::chrono::seconds(1));
//    timer_.async_wait(
//        [this](std::error_code ec)
//        {
//          if (!ec)
//            do_send();
//        });
//  }
//
//private:
//  asio::ip::udp::endpoint endpoint_;
//  asio::ip::udp::socket socket_;
//  asio::steady_timer timer_;
//  int message_count_;
//  std::string message_;
//};
//
//int main(int argc, char* argv[])
//{
//  try
//  {
//    if (argc != 2)
//    {
//      std::cerr << "Usage: sender <multicast_address>\n";
//      std::cerr << "  For IPv4, try:\n";
//      std::cerr << "    sender 9.5.0.1\n";
//      std::cerr << "  For IPv6, try:\n";
//      std::cerr << "    sender ff:::\n";
//      return 1;
//    }
//
//    asio::io_context io_context;
//    sender s(io_context, asio::ip::make_address(argv[1]));
//    io_context.run();
//  }
//  catch (std::exception& e)
//  {
//    std::cerr << "Exception: " << e.what() << "\n";
//  }
//
//  return 0;
//}


#include <stdio.h>
//#include <arpa/inet.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define N 128
int main(int argc, const char *argv[])
{
	WSAData wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 0;
	}
	int sockfd;
	struct sockaddr_in broadcastaddr;
	int addrlen = sizeof(broadcastaddr);
	char buf[N] = {};

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s ip port\n", argv[0]);
		exit(1);
	}

	//��һ��:�����׽���
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		//errlog("fail to socket");
	}

	//�ڶ���:���㲥������Ϣ�ṹ��
	//inet_addr:�����ʮ����ip��ַת��Ϊ�����ֽ������������
	//htons:�������ֽ���ת��Ϊ�����ֽ���
	//atoi:���������ַ���ת��Ϊ��������
	broadcastaddr.sin_family = AF_INET;
	broadcastaddr.sin_addr.s_addr = inet_addr(argv[1]);
	broadcastaddr.sin_port = htons(atoi(argv[2]));

	//������:����Ϊ�������͹㲥Ȩ��
	int on = 1;
	if (setsockopt(sockfd, SOL_SOCKET, \
		SO_BROADCAST,(const char*)(&on), sizeof(on)) < 0)
	{
		//errlog("fail to setsockopt");
	}

	while (1)
	{
		fgets(buf, N, stdin);
		buf[strlen(buf) - 1] = '\0';

		if (sendto(sockfd, buf, N, 0, \
			(struct sockaddr *)&broadcastaddr, addrlen) < 0)
		{
			//errlog("fail to sendto");
		}

	}

	closesocket(sockfd);
	WSACleanup();
	return 0;

}