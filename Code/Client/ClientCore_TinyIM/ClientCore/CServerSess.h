
/**
 * @file CServerSess.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用于处理客户端连接上来的会话
 * @version 0.1
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _MEDIUM_SERVER_C_SERVER_SESS_H_
#define _MEDIUM_SERVER_C_SERVER_SESS_H_
#include "json11.hpp"
#include "CommonMsg.h"
#include "asio_common.h"
#include "Log.h"

using tcp=asio::ip::tcp;
namespace ClientCore
{
/**
 * @brief 用于接收客户端连接的Sess
 * 
 */
class CServerSess : public std::enable_shared_from_this<CServerSess>
{
public:
	using SESS_CALL_BACK = std::function<void(std::shared_ptr<CServerSess> pSess, const TransBaseMsg_t& pMsg)>;
	CServerSess(tcp::socket socket, SESS_CALL_BACK&& callBack) : m_callBack(callBack),m_socket(std::move(socket)),m_bConnect(true) {
		{
		
			m_connectInfo.clear();
			m_connectInfo = m_connectInfo + m_socket.remote_endpoint().address().to_v4().to_string() + ":" + std::to_string(m_socket.remote_endpoint().port());
			m_connectInfo += "--->";
			m_connectInfo = m_connectInfo + m_socket.local_endpoint().address().to_v4().to_string() + ":" + std::to_string(m_socket.local_endpoint().port());
		}
	}
	static std::shared_ptr<spdlog::logger> ms_loger;
private:
    //套接字
    tcp::socket m_socket;
	SESS_CALL_BACK m_callBack;
    //需要告知

    //是否已连接
	bool m_bConnect = false;


    enum { max_length = 16384, msg_max_len=8192 };

    //接收消息的buf    
    char m_recvbuf[max_length];
    
    //已经接收的消息的长度
    uint32_t m_recvpos=0;
public:
    /**
     * @brief 发送消息函数，所有的消息需要转为TransBaseMsg_t的类型来进行发送。
     * 
     * @param msg 待发送的消息
     */
    void SendMsg(std::shared_ptr<TransBaseMsg_t> msg){
		if (IsConnected())
		{
			auto self = shared_from_this();
			asio::async_write(m_socket, asio::buffer(msg->GetData(), msg->GetSize()), [this, self, msg](std::error_code ec, std::size_t length) {
				if (!ec && length > 0)
				{
					LOG_INFO(ms_loger, "TCP Send To GUI Succeed: MsgType{} Msg:{} [{} {}]",MsgType(msg->GetType()),msg->to_string(),__FILENAME__,__LINE__);
				}
				else
				{
					LOG_INFO(ms_loger, "TCP Send To GUI Failed: MsgType{} Msg:{} [{} {}]", MsgType(msg->GetType()), msg->to_string(), __FILENAME__, __LINE__);
					StopConnect();
				}
			});
		}
		else
		{
			LOG_ERR(ms_loger, "Socket:{} Connect Closed", m_connectInfo);
		}
    }

	/**
	* @brief 发送消息函数，所有的消息需要转为TransBaseMsg_t的类型来进行发送。
	*
	* @param msg 待发送的消息
	*/
	void SendMsg(const BaseMsg* pMsg) {
		auto msg = std::make_shared<TransBaseMsg_t>(pMsg->GetMsgType(), pMsg->ToString());
		SendMsg(msg);
	}

	//void HandleErrorCode(const std::error_code& ec) {
//
//	}

	void StopConnect();
   

	void SetUserId(const std::string strUserId) {
		m_strUserId = strUserId;
	}

	std::string UserId() const {
		return m_strUserId;
	}

	void SetUserName(const std::string strUserName) {
		m_strUserName = strUserName;
	}

	std::string UserName() const {
		return m_strUserName;
	}
    
    virtual ~CServerSess(){
    }
    
    void Start() {
		LOG_INFO(ms_loger,"Start Receive Socket ");
        m_recvpos=0;
        do_read();
    }
	
	bool IsConnected()
	{
		return true;
	}

	std::string GetConnectInfo() {
		return m_connectInfo;
	}
private:
    
    /**
     * @brief 接收数据，在Start中被调用，通过asio的回调函数，实现连续接收
     *        非阻塞，不需要修改
     * 
     */
	void do_read();


    /**
     * @brief 处理消息的函数，由do_read在判断消息完整以后调用，在此函数中完成消息的分发处理，
     *        代码的示例中给出了KeepAliveReqMsg和KeepAliveRspMsg的处理示例
     * 
     * @param msg 接收到的一条消息
     */
    void handle_message(const TransBaseMsg_t& msg);


    /**
     * @brief 处理KeepAliveRsp消息，由handle_message 调用
     * 
     * @param rspMsg KeepAliveRsp消息
     */
    void handleKeepAliveRsp(const KeepAliveRspMsg& rspMsg);
    
    /**
     * @brief 处理KeepAliveReq消息
     * 
     * @param rspMsg KeepAliveReq消息
     */
    void handleKeepAliveReq(const KeepAliveReqMsg& reqMsg);

private:
	std::string m_connectInfo;
	std::string m_strUserId;
	std::string m_strUserName;
};
using CServerSess_SHARED_PTR = std::shared_ptr<CServerSess>;
}
#endif
