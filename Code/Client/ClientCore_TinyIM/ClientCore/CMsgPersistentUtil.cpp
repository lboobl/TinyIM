/**
 * @file CMsgPersistentUtil.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 消息持久化实现
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "CMsgPersistentUtil.h"
#include "fmt/format.h"
#include "CFileUtil.h"
#include <time.h>
/**
 * @brief 初始化数据库
 * 
 * @return true 
 * @return false 
 */

std::shared_ptr<spdlog::logger> CMsgPersistentUtil::ms_logger=nullptr;

/**
 * @brief 初始化预编译的SQL
 * 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::InitDataBase()
{
	if (nullptr == ms_logger)
	{
		ms_logger = spdlog::default_logger();
	}
	CFileUtil fileUtil;
	std::string strDbName = fileUtil.GetCurDir()+m_strUserId + ".db";
	m_pDb = new SQLite::Database(strDbName.data(),SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
	if (m_pDb->tableExists("T_FRIEND_CHAT_MSG"))
	{
		LOG_INFO(ms_logger, "TABLE T_FRIEND_CHAT_MSG is Exist [ {} {} ]", __FILE__, __LINE__);
	}
	else
	{
		std::string strSQL = R"(CREATE TABLE T_FRIEND_CHAT_MSG(
		F_MSG_ID TEXT,
		F_MSG_TYPE TEXT,
        F_FROM_ID TEXT,
		F_TO_ID TEXT,
		F_MSG_CONTEXT TEXT,
        F_OTHER_INFO TEXT,
		F_READ_FLAG TEXT,
		F_CREATE_TIME TEXT))";
		m_pFriendChatCreate = new SQLite::Statement(*m_pDb, strSQL);
		m_pFriendChatCreate->executeStep();
		// Reset the query to be able to use it again later
		m_pFriendChatCreate->reset();
	}


	if (m_pDb->tableExists("T_GROUP_CHAT_MSG"))
	{
		LOG_INFO(ms_logger, "TABLE T_GROUP_CHAT_MSG is Exist [ {} {} ]", __FILE__, __LINE__);
	}
	else
	{
		std::string strSQL = R"(CREATE TABLE T_GROUP_CHAT_MSG(
		F_MSG_ID TEXT,
		F_MSG_TYPE TEXT,
        F_USER_ID TEXT,
		F_GROUP_ID TEXT,
		F_MSG_CONTEXT TEXT,
        F_OTHER_INFO TEXT,
		F_READ_FLAG TEXT,
		F_CREATE_TIME TEXT))";
		m_pGroupChatCreate = new SQLite::Statement(*m_pDb, strSQL);
		m_pGroupChatCreate->executeStep();
		// Reset the query to be able to use it again later
		m_pGroupChatCreate->reset();
	}


	if (m_pDb->tableExists("T_FILE_HASH"))
	{
		LOG_INFO(ms_logger, "TABLE F_FILE_HASH is Exist [ {} {} ]", __FILE__, __LINE__);
	}
	else
	{
		try {
			std::string strSQL = R"(CREATE TABLE T_FILE_HASH(
			F_FILE_PATH	TEXT,
			F_FILE_HASH	TEXT))";
			m_pFileHashCreate = new SQLite::Statement(*m_pDb, strSQL);
			m_pFileHashCreate->executeStep();
			// Reset the query to be able to use it again later
			m_pFileHashCreate->reset();
		}
		catch (SQLite::Exception& ec)
		{
			LOG_ERR(ms_logger, "{} {} {} {}", ec.getErrorCode(), ec.getErrorStr(), ec.getExtendedErrorCode(), ec.what());
		}

	}

	if(nullptr == m_pGroupChatSelect)
	{
		std::string strSQL = R"(SELECT * FROM T_GROUP_CHAT_MSG WHERE F_READ_FLAG="UN_READ")";

		m_pGroupChatSelect = new SQLite::Statement(*m_pDb, strSQL);
	}

	if (nullptr == m_pGroupChatUpdate)
	{
		std::string strSqlTemplate = R"(UPDATE T_GROUP_CHAT_MSG SET F_READ_FLAG="READ" WHERE F_MSG_ID=?)";
		m_pGroupChatUpdate = new SQLite::Statement(*m_pDb, strSqlTemplate);
	}

	if (nullptr == m_pGroupChatInsert)
	{
		std::string strSQLTemplate = R"(INSERT INTO T_GROUP_CHAT_MSG VALUES (?,?,?,?,?,?,?,?) )";
		m_pGroupChatInsert = new SQLite::Statement(*m_pDb, strSQLTemplate);
	}

	if (nullptr == m_pGroupChatSelectByWord)
	{
		std::string strSqlTemplate = R"(SELECT * FROM T_GROUP_CHAT_MSG WHERE F_MSG_CONTEXT LIKE "%?%")";
		m_pGroupChatSelectByWord = new SQLite::Statement(*m_pDb, strSqlTemplate);
	}

	if (nullptr == m_pFriendChatSelectByWord)
	{
		std::string strSqlTemplate = R"(SELECT * FROM T_FRIEND_CHAT_MSG WHERE F_MSG_CONTEXT LIKE "%?%")";
		m_pFriendChatSelectByWord = new SQLite::Statement(*m_pDb, strSqlTemplate);
	}

	//Group Msg History
	{
		if (nullptr == m_pGroupChatSelectRangeFirst)
		{
			std::string strSqlTemplate = R"(SELECT * FROM T_GROUP_CHAT_MSG WHERE F_GROUP_ID=? LIMIT 15)";
			m_pGroupChatSelectRangeFirst = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}

		if (nullptr == m_pGroupChatSelectRangeLast)
		{
			std::string strSqlTemplate = R"(SELECT * FROM (SELECT * FROM T_GROUP_CHAT_MSG WHERE F_GROUP_ID=? ORDER BY F_MSG_ID DESC LIMIT 15) )";
			m_pGroupChatSelectRangeLast = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}

		if (nullptr == m_pGroupChatSelectRangePrev)
		{
			std::string strSqlTemplate = R"(SELECT * FROM T_GROUP_CHAT_MSG WHERE F_GROUP_ID=? LIMIT 15)";
			m_pGroupChatSelectRangePrev = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}

		if (nullptr == m_pGroupChatSelectRangeNext)
		{
			std::string strSqlTemplate = R"(SELECT * FROM T_GROUP_CHAT_MSG WHERE F_GROUP_ID=? LIMIT 15)";
			m_pGroupChatSelectRangeNext = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}
	}

	{
		if (nullptr == m_pFriendChatSelectRangeFirst)
		{
			std::string strSqlTemplate = R"(SELECT * FROM T_FRIEND_CHAT_MSG WHERE (F_FROM_ID=? AND F_TO_ID=?) OR (F_FROM_ID=? AND F_TO_ID=?) LIMIT 15)";
			m_pFriendChatSelectRangeFirst = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}

		if (nullptr == m_pFriendChatSelectRangeLast)
		{
			std::string strSqlTemplate = R"(SELECT * FROM (SELECT * FROM T_FRIEND_CHAT_MSG WHERE (F_FROM_ID=? AND F_TO_ID=?) OR (F_FROM_ID=? AND F_TO_ID=?) ORDER BY F_MSG_ID DESC LIMIT 15) ORDER BY F_MSG_ID)";
			m_pFriendChatSelectRangeLast = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}

		if (nullptr == m_pFriendChatSelectRangePrev)
		{
			std::string strSqlTemplate = R"(SELECT * FROM T_FRIEND_CHAT_MSG WHERE ( ((F_FROM_ID=? AND F_TO_ID=?) OR (F_FROM_ID=? AND F_TO_ID=?)) AND F_MSG_ID < ? ) LIMIT 15)";
			m_pFriendChatSelectRangePrev = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}

		if (nullptr == m_pFriendChatSelectRangeNext)
		{
			std::string strSqlTemplate = R"(SELECT * FROM T_FRIEND_CHAT_MSG WHERE ( ((F_FROM_ID=? AND F_TO_ID=?) OR (F_FROM_ID=? AND F_TO_ID=?)) AND F_MSG_ID > ? ) LIMIT 15)";
			m_pFriendChatSelectRangeNext = new SQLite::Statement(*m_pDb, strSqlTemplate);
		}
	}

	{
		if (nullptr == m_pFileHashInsert)
		{
			std::string strSQL = R"(INSERT INTO T_FILE_HASH(F_FILE_PATH,F_FILE_HASH) VALUES(?,?))";

			m_pFileHashInsert = new SQLite::Statement(*m_pDb, strSQL);
		}

		if (nullptr == m_pFileHashSelect)
		{
			std::string strSQL = R"(SELECT F_FILE_PATH FROM T_FILE_HASH WHERE F_FILE_HASH=?)";

			m_pFileHashSelect = new SQLite::Statement(*m_pDb, strSQL);
		}

		if (nullptr == m_pFileHashDelete)
		{
			std::string strSQL = R"(DELETE FROM T_FILE_HASH WHERE F_FILE_HASH=?)";

			m_pFileHashDelete = new SQLite::Statement(*m_pDb, strSQL);
		}
	}
	
	{
		std::string strSQLTemplate = R"(INSERT INTO T_FRIEND_CHAT_MSG VALUES(?,'E_CHAT_TEXT_TYPE',?,?,?,?,'UNREAD',?))";
		m_pFriendChatInsert = new SQLite::Statement(*m_pDb, strSQLTemplate);
	}
	{
		std::string strSqlTemplate = R"(UPDATE T_FRIEND_CHAT_MSG SET F_READ_FLAG="READ" WHERE F_MSG_ID=?)";
		m_pFriendChatUpdate = new SQLite::Statement(*m_pDb, strSqlTemplate);
	}
	{
		std::string strSQL = R"(SELECT * FROM T_FRIEND_CHAT_MSG WHERE F_READ_FLAG="UNREAD";)";

		m_pFriendChatUnReadSelect = new SQLite::Statement(*m_pDb, strSQL);
	}
	return true;
}


/**
 * @brief 保存添加好友的请求
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_AddFriendRecvReqMsg(const AddFriendRecvReqMsg& msg)
{
	auto item = m_AddFriendRecvReqMsgMap.find(msg.m_strMsgId);
	if (item != m_AddFriendRecvReqMsgMap.end())
	{
		return true;
	}
	else
	{
		m_AddFriendRecvReqMsgMap.insert(std::pair<std::string, AddFriendRecvReqMsg>(msg.m_strMsgId,msg));
		return true;
	}

}

/**
 * @brief 获取好友接收请求消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Get_AddFriendRecvReqMsg(AddFriendRecvReqMsg& msg)
{
	auto item = m_AddFriendRecvReqMsgMap.find(msg.m_strMsgId);
	if (item != m_AddFriendRecvReqMsgMap.end())
	{
		msg = item->second;
		return true;
	}
	else
	{
		if (!m_AddFriendRecvReqMsgMap.empty())
		{
			msg = m_AddFriendRecvReqMsgMap.begin()->second;
			return true;
		}
	}
	return false;
}

/**
 * @brief 删除添加好友消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Remove_AddFriendRecvReqMsg(const AddFriendRecvReqMsg& msg)
{
	m_AddFriendRecvReqMsgMap.erase(msg.m_strMsgId);
	return true;
}

/**
 * @brief 保存添加好友通知请求消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_AddFriendNotifyReqMsg(const AddFriendNotifyReqMsg& msg)
{
	auto item = m_AddFriendNotifyReqMsgMap.find(msg.m_strMsgId);
	if (item != m_AddFriendNotifyReqMsgMap.end())
	{
		return true;
	}
	else
	{
		m_AddFriendNotifyReqMsgMap.insert(std::pair<std::string, AddFriendNotifyReqMsg>(msg.m_strMsgId, msg));
	}
	return false;
}

/**
 * @brief 获取添加好友请求消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Get_AddFriendNotifyReqMsg(AddFriendNotifyReqMsg& msg)
{
	auto item = m_AddFriendNotifyReqMsgMap.find(msg.m_strMsgId);
	if (item != m_AddFriendNotifyReqMsgMap.end())
	{
		msg = item->second;
		return true;
	}
	else
	{
		if (!m_AddFriendNotifyReqMsgMap.empty())
		{
			msg = m_AddFriendNotifyReqMsgMap.begin()->second;
			return true;
		}
	}
	return false;
}

/**
 * @brief 移除添加好友请求消息,作为消息聊天记录
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Remove_AddFriendNotifyReqMsg(const AddFriendNotifyReqMsg& msg)
{
	m_AddFriendNotifyReqMsgMap.erase(msg.m_strMsgId);
	return true;
}

/**
 * @brief 保存发送好友了聊天的回复消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_FriendChatSendTxtRspMsg(const FriendChatMsg_s& msg)
{
	bool bResult = false;
	try {
		m_pFriendChatInsert->bind(1, msg.m_strChatMsgId);
		m_pFriendChatInsert->bind(2, msg.m_strSenderId);
		m_pFriendChatInsert->bind(3, msg.m_strReceiverId);
		m_pFriendChatInsert->bind(4, msg.m_strContext);
		m_pFriendChatInsert->bind(5, msg.m_fontInfo.ToString());
		m_pFriendChatInsert->bind(6, msg.m_strMsgTime);
		m_pFriendChatInsert->exec();
		m_pFriendChatInsert->reset();
		bResult = true;
	}
	catch (std::exception& ex)
	{
		LOG_ERR(ms_logger, "Msg:{} SQL:{} [{} {}]", ex.what(), m_pFriendChatInsert->getQuery(),__FILENAME__, __LINE__);
	}

	return bResult;
}
/**
 * @brief 保存接收到的好友文本消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_FriendChatRecvTxtReqMsg(const FriendChatMsg_s& msg)
{
	std::string strSQLTemplate = R"(INSERT INTO T_FRIEND_CHAT_MSG VALUES("{0}","{1}","{2}","{3}","{4}","{5}","UNREAD","{6}"))";
	std::string strSql = fmt::format(strSQLTemplate,
		msg.m_strChatMsgId,
		ChatType(CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE),
		msg.m_strSenderId,
		msg.m_strReceiverId,
		msg.m_strContext,
		msg.m_fontInfo.ToString(),
		std::to_string(time(nullptr)));
	SQLite::Statement*   m_pInsertQuery = new SQLite::Statement(*m_pDb, strSql);
	bool bResult = m_pInsertQuery->executeStep();
	// Reset the query to be able to use it again later
	m_pInsertQuery->reset();

	return bResult;
}

/**
 * @brief 获取好友聊天消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Get_FriendChatRecvTxtReqMsg(FriendChatMsg_s& msg)
{
	bool bResult = false;
	try
	{
		bResult = m_pFriendChatUnReadSelect->executeStep();
		if (bResult)
		{
			msg.m_strChatMsgId = m_pFriendChatUnReadSelect->getColumn(0).getString();
			msg.m_strSenderId = m_pFriendChatUnReadSelect->getColumn(2).getString();
			msg.m_strReceiverId = m_pFriendChatUnReadSelect->getColumn(3).getString();
			msg.m_strContext = m_pFriendChatUnReadSelect->getColumn(4).getString();
		}
		m_pFriendChatUnReadSelect->reset();
	}
	catch (std::exception ex)
	{
		LOG_ERR(ms_logger, "EC:{} SQL:{} [{} {}]", ex.what(), m_pFriendChatUnReadSelect->getQuery(), __FILENAME__, __LINE__);
		bResult = false;
	}
	return bResult;
}

/**
 * @brief 更新好友聊天的文本消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Update_FriendChatRecvTxtReqMsg(const FriendChatMsg_s& msg)
{
	bool bResult = false;
	try {
		m_pFriendChatUpdate->bind(1, msg.m_strChatMsgId);
		bResult = m_pFriendChatUpdate->executeStep();
		m_pFriendChatUpdate->reset();
	}
	catch (std::exception ex)
	{
		LOG_ERR(ms_logger, "EC:{} SQL:{} [{} {}]", ex.what(), m_pFriendChatUpdate->getQuery(), __FILENAME__, __LINE__);
	}
	return bResult;
}


/**
 * @brief 保存收到的群组文本消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_RecvGroupTextMsgReqMsg(const SendGroupTextMsgRspMsg& msg)
{
	if (nullptr != m_pGroupChatInsert)
	{
		SQLite::bind(*m_pGroupChatInsert,
			msg.m_strMsgId,
			ChatType(CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE),
			msg.m_strSenderId,
			msg.m_strGroupId,
			msg.m_strContext,
			"OTHER_INFO",
			"READ",
			msg.m_strMsgTime);
		int nNb = m_pGroupChatInsert->exec();
		m_pGroupChatInsert->reset();
		if (nNb != 0)
		{
			return true;
		}
		else
		{
			std::string strMsg = m_pGroupChatInsert->getErrorMsg();
			LOG_ERR(ms_logger, "{} [{} {}]", strMsg, __FILENAME__, __LINE__);
			// Reset the query to be able to use it again later

			return false;
		}
	}
	else
	{
		return false;
	}
}
//RecvGroupTextMsgReqMsg
/**
 * @brief 保存群组聊天的文本消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_RecvGroupTextMsgReqMsg(const RecvGroupTextMsgReqMsg& msg)
{
	if (nullptr != m_pGroupChatInsert)
	{
		SQLite::bind(*m_pGroupChatInsert,
			msg.m_strMsgId,
			ChatType(CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE),
			msg.m_strSenderId,
			msg.m_strGroupId,
			msg.m_strContext,
			msg.m_stFontInfo.ToString(),
			"UN_READ",
			msg.m_strMsgTime);
		int nNb = m_pGroupChatInsert->exec();
		m_pGroupChatInsert->reset();
		if (nNb != 0)
		{
			return true;
		}
		else
		{
			std::string strMsg = m_pGroupChatInsert->getErrorMsg();
			LOG_ERR(ms_logger, "{} [{} {}]", strMsg, __FILENAME__, __LINE__);
			// Reset the query to be able to use it again later

			return false;
		}
	}
	else
	{
		return false;
	}

}

/**
 * @brief 获取收到的群聊文本消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Get_RecvGroupTextMsgReqMsg(RecvGroupTextMsgReqMsg& msg)
{
	bool bResult = m_pGroupChatSelect->executeStep();
	if (bResult)
	{
		msg.m_strMsgId = m_pGroupChatSelect->getColumn(0).getString();
		msg.m_strGroupId = m_pGroupChatSelect->getColumn(2).getString();
		msg.m_strSenderId = m_pGroupChatSelect->getColumn(3).getString();
		msg.m_strContext = m_pGroupChatSelect->getColumn(4).getString();
	}
	if (!bResult)
	{
		std::string strMsg = m_pGroupChatSelect->getErrorMsg();
		LOG_ERR(ms_logger, "{} [{} {}]", strMsg, __FILENAME__, __LINE__);
	}
	// Reset the query to be able to use it again later
	m_pGroupChatSelect->reset();
	return bResult;
}

/**
 * @brief 更新收到的群聊文本消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Update_RecvGroupTextMsgReqMsg(const RecvGroupTextMsgReqMsg& msg)
{
	SQLite::bind(*m_pGroupChatUpdate, msg.m_strMsgId);
	int nResult = m_pGroupChatUpdate->exec();
	m_pGroupChatUpdate->reset();
	return nResult != 0;
}


/**
 * @brief 保存收到好友发送文件的请求消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_FriendRecvFileMsgReqMsg(const FriendRecvFileMsgReqMsg& msg)
{
	m_FriendRecvFileMsgReqMsgMap.insert(std::pair<std::string,FriendRecvFileMsgReqMsg>(msg.m_strUserId,msg));
	return true;
}

/**
 * @brief 获取收到好友发送文件的请求消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Get_FriendRecvFileMsgReqMsg(FriendRecvFileMsgReqMsg& msg)
{
	if (!m_FriendRecvFileMsgReqMsgMap.empty())
	{
		msg = m_FriendRecvFileMsgReqMsgMap.begin()->second;
	}
	return true;
}

/**
 * @brief 更新收到好友发送文件的请求消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Update_FriendRecvFileMsgReqMsg(const FriendRecvFileMsgReqMsg& /*msg*/)
{
	if (!m_FriendRecvFileMsgReqMsgMap.empty())
	{
		m_FriendRecvFileMsgReqMsgMap.erase(m_FriendRecvFileMsgReqMsgMap.begin());
	}
	return true;
}


/**
 * @brief 保存好友接收文件的通知消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Save_FriendNotifyFileMsgReqMsg(const FriendNotifyFileMsgReqMsg& msg)
{
	m_FriendNotifyFileMsgReqMsgMap.insert({msg.m_strUserId, msg});
	return false;
}

/**
 * @brief 获取好友接收文件的通知消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Get_FriendNotifyFileMsgReqMsg(FriendNotifyFileMsgReqMsg& msg)
{
	if (m_FriendNotifyFileMsgReqMsgMap.empty())
	{
		return false;
	}
	msg = (m_FriendNotifyFileMsgReqMsgMap.begin()->second);
	return true;
}

/**
 * @brief 更新好友接收文件的通知消息
 * 
 * @param msg 
 * @return true 
 * @return false 
 */
bool CMsgPersistentUtil::Update_FriendNotifyFileMsgReqMsg(const FriendNotifyFileMsgReqMsg& /*msg*/)
{
	if (m_FriendNotifyFileMsgReqMsgMap.empty())
	{
		return false;
	}
	m_FriendNotifyFileMsgReqMsgMap.erase(m_FriendNotifyFileMsgReqMsgMap.begin());
	return true;
}


/**
 * @brief 根据选项对获取好友聊天记录的请求进行分发
 * 
 * @param reqMsg 
 * @return std::vector<FriendChatMsg_s> 
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_FriendChatHistory(const GetFriendChatHistoryReq& reqMsg)
{
	switch (reqMsg.m_eDirection)
	{
	case HISTORY_DIRECTION::E_FIRST_MSG:
	{
		return Get_FriendChatHistoryFirst(reqMsg);
	}break;
	case HISTORY_DIRECTION::E_LAST_MSG:
	{
		return Get_FriendChatHistoryLast(reqMsg);
	}break;
	case HISTORY_DIRECTION::E_NEXT_MSG:
	{
		return Get_FriendChatHistoryNext(reqMsg);
	}break;
	case HISTORY_DIRECTION::E_PREV_MSG:
	{
		return Get_FriendChatHistoryPrev(reqMsg);
	}break;
	default:
	{
		return Get_FriendChatHistoryLast(reqMsg);
	}break;
	}
}

/**
 * @brief 根据选项对获取群组聊天记录的请求进行分发
 * 
 * @param reqMsg 
 * @return std::vector<SendGroupTextMsgRspMsg> 
 */
std::vector<SendGroupTextMsgRspMsg>  CMsgPersistentUtil::Get_GroupChatHistory(const GetGroupChatHistoryReq&  reqMsg)
{
	switch (reqMsg.m_eDirection)
	{
	case HISTORY_DIRECTION::E_FIRST_MSG:
	{
		return Get_GroupChatHistoryFirst(reqMsg);
	}break;
	case HISTORY_DIRECTION::E_LAST_MSG:
	{
		return Get_GroupChatHistoryLast(reqMsg);
	}break;
	case HISTORY_DIRECTION::E_NEXT_MSG:
	{
		return Get_GroupChatHistoryNext(reqMsg);
	}break;
	case HISTORY_DIRECTION::E_PREV_MSG:
	{
		return Get_GroupChatHistoryPrev(reqMsg);
	}break;
	default:
	{
		return Get_GroupChatHistoryLast(reqMsg);
	}break;
	}
}


/**
 * @brief 获取好友消息记录的搜索结果
 * 
 * @param reqMsg 
 * @return std::vector<FriendChatMsg_s> 
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_SearchFriendChatMsg(const SearchChatHistoryReq&  reqMsg)
{
	std::vector<FriendChatMsg_s> result;
	SQLite::bind(*m_pFriendChatSelectByWord, reqMsg.m_strSearchWord);
	
	FriendChatMsg_s rspMsg;
	while (m_pFriendChatSelectByWord->executeStep())
	{
		//int nColumCount = m_pFriendChatSelectByWord->getColumnCount();
		rspMsg.m_strChatMsgId = m_pFriendChatSelectByWord->getColumn(0).getString();
		rspMsg.m_strSenderId = m_pFriendChatSelectByWord->getColumn(2).getString();
		rspMsg.m_strReceiverId = m_pFriendChatSelectByWord->getColumn(3).getString();
		rspMsg.m_strContext = m_pFriendChatSelectByWord->getColumn(4).getString();
		rspMsg.m_fontInfo.FromString(m_pFriendChatSelectByWord->getColumn(5).getString());
		rspMsg.m_strMsgTime = m_pFriendChatSelectByWord->getColumn(6).getString();

		result.push_back(rspMsg);
	}
	return result;
}


/**
 * @brief 获取群聊消息记录的搜索结果
 * 
 * @param reqMsg 
 * @return std::vector<SendGroupTextMsgRspMsg> 
 */
std::vector<SendGroupTextMsgRspMsg> CMsgPersistentUtil::Get_SearchGroupChatMsg(const SearchChatHistoryReq&  reqMsg)
{
	std::vector<SendGroupTextMsgRspMsg> result;
	SQLite::bind(*m_pGroupChatSelectByWord, reqMsg.m_strSearchWord);

	SendGroupTextMsgRspMsg rspMsg;
	while (m_pGroupChatSelectByWord->executeStep())
	{
		//int nColumCount = m_pGroupChatSelectByWord->getColumnCount();
		rspMsg.m_strMsgId = m_pGroupChatSelectByWord->getColumn(0).getString();
		rspMsg.m_strSenderId = m_pGroupChatSelectByWord->getColumn(2).getString();
		rspMsg.m_strGroupId = m_pGroupChatSelectByWord->getColumn(3).getString();
		rspMsg.m_strContext = m_pGroupChatSelectByWord->getColumn(4).getString();
		rspMsg.m_fontInfo.FromString(m_pGroupChatSelectByWord->getColumn(5).getString());
		rspMsg.m_strMsgTime = m_pGroupChatSelectByWord->getColumn(6).getString();

		result.push_back(rspMsg);
	}
	return result;
}

/**
 * @brief 获取好友聊天记录的首页
 * 
 * @param reqMsg 
 * @return std::vector<FriendChatMsg_s> 
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_FriendChatHistoryFirst(const GetFriendChatHistoryReq& reqMsg)
{
	std::vector<FriendChatMsg_s> result;
	try {
		SQLite::bind(*m_pFriendChatSelectRangeFirst,
			reqMsg.m_strUserId,
			reqMsg.m_strFriendId,
			reqMsg.m_strFriendId,
			reqMsg.m_strUserId);
		result = Get_FriendChatHistoryCore(m_pFriendChatSelectRangeFirst);
		LOG_INFO(ms_logger, "SQL:{} SQL2:{}", m_pFriendChatSelectRangeFirst->getQuery(), m_pFriendChatSelectRangeFirst->getExpandedSQL());
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pFriendChatSelectRangeFirst->getQuery());
	}
	m_pFriendChatSelectRangeFirst->reset();
	return result;
}

/**
 * @brief 获取群组聊天消息记录的首页
 * 
 * @param reqMsg 
 * @return std::vector<SendGroupTextMsgRspMsg> 
 */
std::vector<SendGroupTextMsgRspMsg>  CMsgPersistentUtil::Get_GroupChatHistoryFirst(const GetGroupChatHistoryReq&  reqMsg)
{
	std::vector<SendGroupTextMsgRspMsg> result;
	try {
		SQLite::bind(*m_pGroupChatSelectRangeFirst,
			reqMsg.m_strGroupId);
		result = Get_GroupChatHistoryCore(m_pGroupChatSelectRangeFirst);
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pGroupChatSelectRangeFirst->getQuery());
	}
	m_pGroupChatSelectRangeFirst->reset();
	return result;
}

/**
 * @brief 获取好友聊天记录的最后一页
 * 
 * @param reqMsg 
 * @return std::vector<FriendChatMsg_s> 
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_FriendChatHistoryLast(const GetFriendChatHistoryReq& reqMsg)
{
	std::vector<FriendChatMsg_s> result;
	try {
		SQLite::bind(*m_pFriendChatSelectRangeLast,
			reqMsg.m_strUserId,
			reqMsg.m_strFriendId,
			reqMsg.m_strFriendId,
			reqMsg.m_strUserId);
		result = Get_FriendChatHistoryCore(m_pFriendChatSelectRangeLast);
		LOG_INFO(ms_logger, "SQL:{} SQL2:{}",m_pFriendChatSelectRangeLast->getQuery(), m_pFriendChatSelectRangeLast->getExpandedSQL());
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pFriendChatSelectRangeLast->getQuery());
	}
	m_pFriendChatSelectRangeLast->reset();
	return result;
}


/**
 * @brief 获取群组聊天记录的最后一页
 * 
 * @param reqMsg 
 * @return std::vector<SendGroupTextMsgRspMsg> 
 */
std::vector<SendGroupTextMsgRspMsg>  CMsgPersistentUtil::Get_GroupChatHistoryLast(const GetGroupChatHistoryReq&  reqMsg)
{
	std::vector<SendGroupTextMsgRspMsg> result;
	try {
		SQLite::bind(*m_pGroupChatSelectRangeLast,
			reqMsg.m_strGroupId);
		result = Get_GroupChatHistoryCore(m_pGroupChatSelectRangeLast);
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pGroupChatSelectRangeLast->getQuery());
	}
	m_pGroupChatSelectRangeFirst->reset();
	return result;
}

/**
 * @brief 获取好友聊天记录的上一页
 * 
 * @param reqMsg 
 * @return std::vector<FriendChatMsg_s> 
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_FriendChatHistoryPrev(const GetFriendChatHistoryReq& reqMsg)
{
	std::vector<FriendChatMsg_s> result;
	try {
		SQLite::bind(*m_pFriendChatSelectRangePrev,
			reqMsg.m_strUserId,
			reqMsg.m_strFriendId,
			reqMsg.m_strFriendId,
			reqMsg.m_strUserId,
			reqMsg.m_strChatMsgId);
		result = Get_FriendChatHistoryCore(m_pFriendChatSelectRangePrev);
		LOG_INFO(ms_logger, "SQL:{} SQL2:{}", m_pFriendChatSelectRangePrev->getQuery(), m_pFriendChatSelectRangePrev->getExpandedSQL());
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pFriendChatSelectRangePrev->getQuery());
	}
	m_pFriendChatSelectRangePrev->reset();
	return result;
}

/**
 * @brief 获取群组聊天消息记录的上一页
 * 
 * @param reqMsg 
 * @return std::vector<SendGroupTextMsgRspMsg> 
 */
std::vector<SendGroupTextMsgRspMsg>  CMsgPersistentUtil::Get_GroupChatHistoryPrev(const GetGroupChatHistoryReq&  reqMsg)
{

	std::vector<SendGroupTextMsgRspMsg> result;
	try {
		SQLite::bind(*m_pGroupChatSelectRangePrev,
			reqMsg.m_strGroupId);
		result = Get_GroupChatHistoryCore(m_pGroupChatSelectRangePrev);
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pGroupChatSelectRangePrev->getQuery());
	}
	m_pGroupChatSelectRangePrev->reset();
	return result;
	//return Get_GroupChatHistoryCore(m_pGroupChatSelectRangePrev);
}

/**
 * @brief 获取好友聊天消息记录的下一页
 * 
 * @param reqMsg 好友聊天消息的请求
 * @return std::vector<FriendChatMsg_s> 好友聊天消息的数组
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_FriendChatHistoryNext(const GetFriendChatHistoryReq& reqMsg)
{
	std::vector<FriendChatMsg_s> result;
	try {
		SQLite::bind(*m_pFriendChatSelectRangeNext,
			reqMsg.m_strUserId,
			reqMsg.m_strFriendId,
			reqMsg.m_strFriendId,
			reqMsg.m_strUserId,
			reqMsg.m_strChatMsgId);
		result = Get_FriendChatHistoryCore(m_pFriendChatSelectRangeNext);
		LOG_INFO(ms_logger, "SQL:{} SQL2:{}", m_pFriendChatSelectRangeNext->getQuery(), m_pFriendChatSelectRangeNext->getExpandedSQL());
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pFriendChatSelectRangeNext->getQuery());
	}
	m_pFriendChatSelectRangeNext->reset();
	return result;
}

/**
 * @brief 获取群组聊天的下一页记录
 * 
 * @param reqMsg 获取群组聊天请求消息
 * @return std::vector<SendGroupTextMsgRspMsg> 群组聊天消息历史记录
 */
std::vector<SendGroupTextMsgRspMsg>  CMsgPersistentUtil::Get_GroupChatHistoryNext(const GetGroupChatHistoryReq&  reqMsg)
{
	//return Get_GroupChatHistoryCore(m_pGroupChatSelectRangeNext);
	std::vector<SendGroupTextMsgRspMsg> result;
	try {
		SQLite::bind(*m_pGroupChatSelectRangeNext,
			reqMsg.m_strGroupId);
		result = Get_GroupChatHistoryCore(m_pGroupChatSelectRangeNext);
	}
	catch (SQLite::Exception& ec) {
		result.clear();
		LOG_ERR(ms_logger, " EC: {} EC message:{} SQL:{}", ec.getErrorCode(), ec.getErrorStr(), m_pGroupChatSelectRangeNext->getQuery());
	}
	m_pGroupChatSelectRangeNext->reset();
	return result;
}

/**
 * @brief 获取好友聊天记录的核心函数
 * 
 * @param pState 预编译的SQL对象
 * @return std::vector<FriendChatMsg_s> 好友聊天记录数组
 */
std::vector<FriendChatMsg_s> CMsgPersistentUtil::Get_FriendChatHistoryCore(SQLite::Statement* pState)
{
	std::vector<FriendChatMsg_s> result;
	FriendChatMsg_s rspMsg;
	while (pState->executeStep())
	{
		//int nColumCount = pState->getColumnCount();
		rspMsg.m_strChatMsgId = pState->getColumn(0).getString();
		rspMsg.m_strSenderId = pState->getColumn(2).getString();
		rspMsg.m_strReceiverId = pState->getColumn(3).getString();
		rspMsg.m_strContext = pState->getColumn(4).getString();
		rspMsg.m_fontInfo.FromString(pState->getColumn(5).getString());
		rspMsg.m_strMsgTime = pState->getColumn(7).getString();

		result.push_back(rspMsg);
	}
	return result;
}

/**
 * @brief 获取群聊消息历史的核心函数
 * 
 * @param pState 预编译的SQL对象
 * @return std::vector<SendGroupTextMsgRspMsg> 群聊消息记录数组
 */
std::vector<SendGroupTextMsgRspMsg>  CMsgPersistentUtil::Get_GroupChatHistoryCore(SQLite::Statement* pState)
{
	std::vector<SendGroupTextMsgRspMsg> result;
	SendGroupTextMsgRspMsg rspMsg;
	while (pState->executeStep())
	{
		//int nColumCount = pState->getColumnCount();
		rspMsg.m_strMsgId = pState->getColumn(0).getString();
		rspMsg.m_strSenderId = pState->getColumn(2).getString();
		rspMsg.m_strGroupId = pState->getColumn(3).getString();
		rspMsg.m_strContext = pState->getColumn(4).getString();
		rspMsg.m_fontInfo.FromString(pState->getColumn(5).getString());
		rspMsg.m_strMsgTime = pState->getColumn(7).getString();

		result.push_back(rspMsg);
	}

	return result;
}

/**
 * @brief 保存文件名称和hash值的对应关系
 * 
 * @param strFileName 文件名称
 * @param strFileHash 文件hash值
 * @return true 保存成功
 * @return false 保存失败
 */
bool CMsgPersistentUtil::Save_FileHash(const std::string strFileName, const std::string strFileHash)
{
	SQLite::bind(*m_pFileHashInsert, 
		strFileName, 
		strFileHash);
	m_pFileHashInsert->exec();
	m_pFileHashInsert->reset();
	return true;
}

/**
 * @brief 获取某个hash值对应的文件名称
 * 
 * @param strFileHash 文件hash值 
 * @return std::string 文件名称
 */
std::string CMsgPersistentUtil::Get_FileByHash(const std::string strFileHash)
{
	SQLite::bind(*m_pFileHashSelect, strFileHash);
	std::string strFile;
	while (m_pFileHashSelect->executeStep())
	{
		strFile = m_pFileHashSelect->getColumn(0).getString();
		break;
	}
	m_pFileHashSelect->reset();
	return strFile;
}


/**
 * @brief 删除对应Hash值的文件记录
 * 
 * @param strFileHash 文件的Hash值
 * @return true 删除成功
 * @return false 删除失败
 */
bool CMsgPersistentUtil::Delete_FileByHash(const std::string strFileHash)
{
	SQLite::bind(*m_pFileHashDelete, strFileHash);
	std::string strFile;
	if (m_pFileHashDelete->exec())
	{

	}
	m_pFileHashDelete->reset();
	return true;
}