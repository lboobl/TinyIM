
/**
 * @file CClientSess.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief ClientCore用于连接到远程服务器的绘画类
 * @version 0.1
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _MEDIUM_SERVER_C_CLIENT_SESS_H_
#define _MEDIUM_SERVER_C_CLIENT_SESS_H_
#include "asio_common.h"
#include "Log.h"
#include "CommonMsg.h"
namespace ClientCore
{
using asio::ip::tcp;
class CMediumServer;
/**
 * @brief 用于连接到远端服务器的Sess
 * 
 */
class CClientSess : public std::enable_shared_from_this<CClientSess>  {
protected:
	enum STATE
	{
		ST_CONN_FINISHED=0,
		ST_CONNING,
		ST_NOT_CONNECT
	};
    asio::io_service& m_ioService;

	std::string m_serverIp;
	int 		m_serverPort;

	//持有此连接的消息队列
	CMediumServer * m_queue;
	//tcp连接的socket
	asio::ip::tcp::socket   m_socket;

	//连接信息,需要多次获取,因此保存到变量中
	std::string m_connectInfo;

	//Sess的连接状态
	int m_bConnect=ST_NOT_CONNECT;
	
	enum { max_length = 16384, msg_max_len=8192 };
	
	//接收buf
	char m_recvbuf[max_length];
	
	//接收位置
    uint32_t m_recvpos=0;
public:
	static	std::shared_ptr<spdlog::logger> ms_loger;//会话日志
public:
	//检查是否为同一个连接
	bool is_same_connect(const std::string& strIp,const int nport)
	{
		return (( m_serverIp.compare(strIp)==0) && 
			(nport == m_serverPort ));
	}
	//获取连接的ip地址和端口号,格式为 xx.xx.xx.xx:port
	std::string GetConnectInfo(){return m_connectInfo;}

	bool SendKeepAlive();
	
	

    CClientSess(asio::io_service& ioService,std::string& strIp,int port, CMediumServer* queue);
	virtual ~CClientSess();
    int StartConnect();
    int StopConnect();
	bool IsConnect(){return m_bConnect == ST_CONN_FINISHED;}
	bool SendMsg(std::shared_ptr<TransBaseMsg_t> pMsg);
	bool SendMsg(const BaseMsg* pMsg);
	void SetUserName(const std::string strUserName)
	{
		m_strUserName = strUserName;
	}

	std::string UserName()const {
		return m_strUserName;
	}

	void SetUserId(const std::string strUserId) {
		m_strUserId = strUserId;
	}

	std::string UserId() const {
		return m_strUserId;
	}
private:
	std::string GetConnectInfo() const;
	int do_read();
	void handle_message(const TransBaseMsg_t& msg);

	
	void handleKeepAliveReq(const KeepAliveReqMsg& reqMsg);
	
	void handleKeepAliveRsp(const KeepAliveRspMsg& rspMsg);

	void OnSocketError();
	std::string m_strUserName;
	std::string m_strUserId;
};
using CClientSess_SHARED_PTR = std::shared_ptr<CClientSess>;
}

#endif //CAPNSSESS
