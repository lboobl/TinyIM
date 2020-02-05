////
//// receiver.cpp
//// ~~~~~~~~~~~~
////
//// Copyright (c) - Christopher M. Kohlhoff (chris at kohlhoff dot com)
////
//// Distributed under the Boost Software License, Version 1.0. (See accompanying
//// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////
//
//#include <array>
//#include <iostream>
//#include <string>
//#include "asio.hpp"
//
//constexpr short multicast_port = 1;
//
//class receiver
//{
//public:
//  receiver(asio::io_context& io_context,
//      const asio::ip::address& listen_address,
//      const asio::ip::address& multicast_address)
//    : socket_(io_context)
//  {
//    // Create the socket so that multiple may be bound to the same address.
//    asio::ip::udp::endpoint listen_endpoint(
//        listen_address, multicast_port);
//    socket_.open(listen_endpoint.protocol());
//    socket_.set_option(asio::ip::udp::socket::reuse_address(true));
//	socket_.set_option(asio::ip::multicast::enable_loopback(true));
//	socket_.set_option(asio::ip::multicast::hops(4));
//    socket_.bind(listen_endpoint);
//    // Join the multicast group.
//    socket_.set_option(
//        asio::ip::multicast::join_group(multicast_address.to_v4));
//
//    do_receive();
//  }
//
//private:
//  void do_receive()
//  {
//    socket_.async_receive_from(
//        asio::buffer(data_), sender_endpoint_,
//        [this](std::error_code ec, std::size_t length)
//        {
//          if (!ec)
//          {
//            std::cout.write(data_.data(), length);
//            std::cout << std::endl;
//
//            do_receive();
//          }
//        });
//  }
//
//  asio::ip::udp::socket socket_;
//  asio::ip::udp::endpoint sender_endpoint_;
//  std::array<char, > data_;
//};
//
//int main(int argc, char* argv[])
//{
//  try
//  {
//    if (argc != 3)
//    {
//      std::cerr << "Usage: receiver <listen_address> <multicast_address>\n";
//      std::cerr << "  For IPv4, try:\n";
//      std::cerr << "    receiver 0.0.0.0 9.5.0.1\n";
//      std::cerr << "  For IPv6, try:\n";
//      std::cerr << "    receiver 0::0 ff:::\n";
//      return 1;
//    }
//
//    asio::io_context io_context;
//    receiver r(io_context,
//        asio::ip::make_address(argv[1]),
//        asio::ip::make_address(argv[2]));
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
	struct sockaddr_in broadcastaddr, addr;
	int addrlen = sizeof(broadcastaddr);
	char buf[N] = {};

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s ip port\n", argv[0]);
		exit(1);
	}

	//第一步:创建套接字
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		//errlog("fail to socket");
	}

	//第二步:填充广播网络信息结构体
	//inet_addr:将点分十进制ip地址转化为网络字节序的整型数据
	//htons:将主机字节序转化为网络字节序
	//atoi:将数字型字符串转化为整型数据
	broadcastaddr.sin_family = AF_INET;
	broadcastaddr.sin_addr.s_addr = inet_addr(argv[1]);
	broadcastaddr.sin_port = htons(atoi(argv[2]));

	//第三步:将套接字与服务器网络信息结构体绑定
	if (bind(sockfd, (struct sockaddr *)&broadcastaddr, addrlen) < 0)
	{
		//errlog("fail to bind");
	}

	int bytes;
	while (1)
	{
		if ((bytes = recvfrom(sockfd, buf, N, 0, \
			(struct sockaddr *)&addr, &addrlen)) < 0)
		{
			//errlog("fail to recvfrom");
		}
		else
		{
			printf("ip: %s, port: %d\n", \
				inet_ntoa(addr.sin_addr), \
				ntohs(addr.sin_port));

			printf("broadcast : %s\n", buf);

		}
	}

	closesocket(sockfd);
	WSACleanup();
	return 0;
}
