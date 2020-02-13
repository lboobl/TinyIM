
/**
 * @file CMsgPersistentUtil.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 客户端消息持久化的类
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _DENNIS_THINK_C_MSG_PERSISTENT_UTIL_H_
#define _DENNIS_THINK_C_MSG_PERSISTENT_UTIL_H_
#include <memory>
#include "CommonMsg.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"
#include "Log.h"

class CMsgPersistentUtil: public std::enable_shared_from_this<CMsgPersistentUtil>
{
public:
	bool InitDataBase();

	//AddFriendRecvReqMsg
	bool Save_AddFriendRecvReqMsg(const AddFriendRecvReqMsg& msg);

	bool Get_AddFriendRecvReqMsg(AddFriendRecvReqMsg& msg);

	bool Remove_AddFriendRecvReqMsg(const AddFriendRecvReqMsg& msg);

	//AddFriendNotifyReqMsg
	bool Save_AddFriendNotifyReqMsg(const AddFriendNotifyReqMsg& msg);

	bool Get_AddFriendNotifyReqMsg(AddFriendNotifyReqMsg& msg);

	bool Remove_AddFriendNotifyReqMsg(const AddFriendNotifyReqMsg& msg);

	//FriendChatRecvTxtReqMsg
	bool Save_FriendChatSendTxtRspMsg(const FriendChatMsg_s& msg);

	bool Get_FriendChatRecvTxtReqMsg(FriendChatMsg_s& msg);

	bool Update_FriendChatRecvTxtReqMsg(const FriendChatMsg_s& msg);

	//FriendRecvFileMsgReqMsg
	bool Save_FriendRecvFileMsgReqMsg(const FriendRecvFileMsgReqMsg& msg);

	bool Get_FriendRecvFileMsgReqMsg(FriendRecvFileMsgReqMsg& msg);

	bool Update_FriendRecvFileMsgReqMsg(const FriendRecvFileMsgReqMsg& msg);


	//FriendNotifyFileMsgReqMsg
	bool Save_FriendNotifyFileMsgReqMsg(const FriendNotifyFileMsgReqMsg& msg);

	bool Get_FriendNotifyFileMsgReqMsg(FriendNotifyFileMsgReqMsg& msg);

	bool Update_FriendNotifyFileMsgReqMsg(const FriendNotifyFileMsgReqMsg& msg);

	std::vector<FriendChatMsg_s> Get_FriendChatHistory(const GetFriendChatHistoryReq& reqMsg);
	std::vector<SendGroupTextMsgRspMsg>  Get_GroupChatHistory(const GetGroupChatHistoryReq&  reqMsg);
 
	std::vector<FriendChatMsg_s> Get_SearchFriendChatMsg(const SearchChatHistoryReq&  reqMsg);
	std::vector<SendGroupTextMsgRspMsg> Get_SearchGroupChatMsg(const SearchChatHistoryReq&  reqMsg);

	//RecvGroupTextMsgReqMsg
	bool Save_RecvGroupTextMsgReqMsg(const RecvGroupTextMsgReqMsg& msg);

	bool Get_RecvGroupTextMsgReqMsg(RecvGroupTextMsgReqMsg& msg);

	bool Update_RecvGroupTextMsgReqMsg(const RecvGroupTextMsgReqMsg& msg);

	bool Save_RecvGroupTextMsgReqMsg(const SendGroupTextMsgRspMsg& msg);

	bool Save_FileHash(const std::string strFileName, const std::string strFileHash);
	std::string Get_FileByHash(const std::string strFileHash);
	bool Delete_FileByHash(const std::string strFileHash);
	static std::shared_ptr<spdlog::logger> ms_logger;
	CMsgPersistentUtil(const std::string strUserId) {
		m_strUserId = strUserId;
	}
	bool Save_FriendChatRecvTxtReqMsg(const FriendChatMsg_s& msg);
private:
	std::string m_strUserId;
	std::map<std::string, AddFriendRecvReqMsg> m_AddFriendRecvReqMsgMap;
	std::map<std::string, AddFriendNotifyReqMsg> m_AddFriendNotifyReqMsgMap;
	std::map<std::string, FriendChatRecvTxtReqMsg> m_FriendChatRecvTxtReqMsgMap;
	std::map<std::string, RecvGroupTextMsgReqMsg> m_RecvGroupTextMsgReqMsgMap;
	std::map<std::string, FriendRecvFileMsgReqMsg> m_FriendRecvFileMsgReqMsgMap;
	std::map<std::string, FriendNotifyFileMsgReqMsg> m_FriendNotifyFileMsgReqMsgMap;

	std::vector<FriendChatMsg_s> Get_FriendChatHistoryFirst(const GetFriendChatHistoryReq& reqMsg);
	std::vector<SendGroupTextMsgRspMsg>  Get_GroupChatHistoryFirst(const GetGroupChatHistoryReq&  reqMsg);

	std::vector<FriendChatMsg_s> Get_FriendChatHistoryLast(const GetFriendChatHistoryReq& reqMsg);
	std::vector<SendGroupTextMsgRspMsg>  Get_GroupChatHistoryLast(const GetGroupChatHistoryReq&  reqMsg);

	std::vector<FriendChatMsg_s> Get_FriendChatHistoryPrev(const GetFriendChatHistoryReq& reqMsg);
	std::vector<SendGroupTextMsgRspMsg>  Get_GroupChatHistoryPrev(const GetGroupChatHistoryReq&  reqMsg);

	std::vector<FriendChatMsg_s> Get_FriendChatHistoryNext(const GetFriendChatHistoryReq& reqMsg);
	std::vector<SendGroupTextMsgRspMsg>  Get_GroupChatHistoryNext(const GetGroupChatHistoryReq&  reqMsg);

	std::vector<FriendChatMsg_s> Get_FriendChatHistoryCore(SQLite::Statement* pState);
	std::vector<SendGroupTextMsgRspMsg>  Get_GroupChatHistoryCore(SQLite::Statement* pState);
	SQLite::Database*    m_pDb = nullptr;    ///< Database connection
	//FriendChat Msg
	SQLite::Statement*   m_pFriendChatCreate = nullptr; ///< Database prepared SQL query

	SQLite::Statement*   m_pFriendChatInsert = nullptr;

	SQLite::Statement*   m_pFriendChatUpdate = nullptr;

	SQLite::Statement*   m_pFriendChatUnReadSelect = nullptr;
	//GroupChatMsg Msg
	SQLite::Statement*   m_pGroupChatCreate=nullptr; ///< Database prepared SQL query

	SQLite::Statement*   m_pGroupChatInsert = nullptr; ///< Database prepared SQL query

	SQLite::Statement*   m_pGroupChatSelect = nullptr; ///< Database prepared SQL query

	SQLite::Statement*   m_pGroupChatUpdate = nullptr; ///< Database prepared SQL query

	SQLite::Statement*   m_pGroupChatSelectRangeFirst = nullptr;//首页消息

	SQLite::Statement*   m_pGroupChatSelectRangeLast = nullptr;//最后一页的消息

	SQLite::Statement*   m_pGroupChatSelectRangePrev = nullptr;//上一页

	SQLite::Statement*   m_pGroupChatSelectRangeNext = nullptr;//群组消息下一页

	//好友历史消息
	SQLite::Statement*   m_pFriendChatSelectRangeFirst = nullptr;  //首页消息

	SQLite::Statement*   m_pFriendChatSelectRangeLast = nullptr;  //最后一页的消息

	SQLite::Statement*   m_pFriendChatSelectRangePrev = nullptr;  //上一页

	SQLite::Statement*   m_pFriendChatSelectRangeNext = nullptr;  //好友消息下一页

	//消息查询
	SQLite::Statement*   m_pFriendChatSelectByWord = nullptr; //好友聊天关键字查询

	SQLite::Statement*   m_pGroupChatSelectByWord = nullptr;  //群组聊天关键字查询

	//文件Hash表
	SQLite::Statement*   m_pFileHashCreate = nullptr;//文件HASH表

	SQLite::Statement*   m_pFileHashInsert = nullptr;//

	SQLite::Statement*	 m_pFileHashSelect = nullptr;

	SQLite::Statement*   m_pFileHashDelete = nullptr;

};
using CMsgPersistentUtil_SHARED_PTR = std::shared_ptr<CMsgPersistentUtil>;
#endif