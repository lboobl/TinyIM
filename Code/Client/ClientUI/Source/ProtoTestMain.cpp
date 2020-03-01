/**
 * @file ProtoTestMain.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 协议测试的类实现
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Proto.h"
#include <thread>
#include <iostream>
int Test1()
{
	CMsgProto::Initialize();
	auto pMsg = CMsgProto::GetInstance();
	pMsg->SendRegisterSync("18333333333", "33333333", "33333333");
	pMsg->SendRegisterSync("18322222222", "22222222", "22222222");
	UserLoginRspMsg rspMsg = pMsg->SendLoginSync("18333333333", "33333333");
	for (int i = 0; i < 3; i++)
	{
		std::cout << "i  " << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	pMsg->SendFindFriendAsnyc("18322222222", "18322222222", NULL);
	pMsg->SendFindFriendAsnyc("18322222222", "18322222222", NULL);
	pMsg->SendFindFriendAsnyc("18322222222", "18322222222", NULL);
	pMsg->SendFindFriendAsnyc("18322222222", "18322222222", NULL);

	pMsg->GetFriendList();
	for (int i = 0; i < 100; i++)
	{
		std::cout << "i  " << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	pMsg->SendUserLogout();
	CMsgProto::UnInstialize();
	for (int i = 0; i < 100; i++)
	{
		std::cout << "i  " << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << "main thread" << std::endl;
	return 0;
}

int Test2()
{
	CMsgProto::Initialize();
	for (int i = 0; i < 10; i++)
	{
		std::cout << "i  " << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	CMsgProto::UnInstialize();
	for (int i = 0; i < 10; i++)
	{
		std::cout << "i  " << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << "main thread" << std::endl;
	return 0;
}

int main(int argc, char * argv[])
{
	int x = 0;
	std::cout << "Choince ";
	std::cin >> x;
	if (x == 1)
	{
		Test1();
	}
	else
	{
		Test2();
	}
	return 0;

}