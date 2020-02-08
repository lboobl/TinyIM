/**
 * @file MainFunc.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 
 * @version 0.1
 * @date 2020-01-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "MainFunc.h"
#include <iostream>
#include <memory>
#include "CMediumServerMultiCast.h"
#include "DaemonSvcApp.h"
#include "CServerSess.h"
#include "CommonFunction.h"
#include "asio_common.h"
#include "Log.h"
#include "planform.h"
#include "CommonConfig.h"
using asio::ip::tcp;
using namespace std;
static asio::io_service IoService;

/**
 * @brief 根据参数解析结果运行程序
 * 
 * @param result 参数解析的结果
 */
void RunProgram(ParseParamResult result)
{
	std::string strcfg, errinfo;
	load_txtfile(result.m_cfgFile.c_str(), strcfg);
	std::cout << strcfg << std::endl;
	//std::cout << strcfg << std::endl;
	if (!strcfg.length()) {
		printf("no Configure\n");
		return;
	}
	auto cfg = json11::Json::parse(strcfg, errinfo, json11::JsonParse::COMMENTS);
	if (!errinfo.empty())
	{
		std::cout << "Config Error:" << errinfo << std::endl;
		return;
	}
	
	auto logger = CreateLogger(cfg);
	if (!logger)
	{
		std::cout << "Can not Create Logger" << std::endl;
		return;
	}
	ClientCore::CClientSess::ms_loger = logger;
	ClientCore::CServerSess::ms_loger = logger;
	ClientCore::CMediumServer::ms_loger = logger;
	ClientCore::CHttpServer::ms_loger = logger;
	ClientCore::CUdpClient::ms_loger = logger;
	//MediumServer::CClientSessManager::ms_loger = logger;

	
	auto server = std::make_shared<ClientCore::CMediumServer>(IoService);
	LOG_INFO(logger, "starting Server [{} {}] ", __FILENAME__, __LINE__);

	std::error_code ec;
	server->loadConfig(cfg, ec);
	server->start([logger](const std::error_code& ec) {
		LOG_INFO(logger, "starting Server2 [{} {}] ", __FILENAME__, __LINE__);
		if (!ec)
		{
			LOG_INFO(logger, "The Service is up and running [{} {}] ", __FILENAME__, __LINE__);
		}
		else
		{
			LOG_ERR(logger, "ERRORFail to start error:{} [{} {}] ", ec.message(), __FILENAME__, __LINE__);
		}
	});
	try {
		IoService.run();
	}
	catch (std::exception ec)
	{
		
	}
}

/**
 * @brief 停止程序的运行
 * 
 */
void StopProgram()
{
	try
	{
		IoService.stop();
	}
	catch (std::exception ec)
	{

	}
}