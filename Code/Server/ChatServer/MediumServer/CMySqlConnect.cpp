/**
 * @file CMySqlConnect.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 处理数据库操作的相关代码
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "CMySqlConnect.h"
#include "fmt/format.h"
#include <iostream>
std::shared_ptr<spdlog::logger> CMySqlConnect::m_loger=nullptr;

/**
 * @brief 构造函数
 * 
 */
CMySqlConnect::CMySqlConnect()
{
		if ((m_mysql = mysql_init(nullptr))) {
			LOG_INFO(m_loger, "mysql_init()succeed [ {}  {}]", __FILENAME__, __LINE__);
		}
		else {
			LOG_INFO(m_loger,"mysql_init()failed  [{} {}]", __FILENAME__, __LINE__);
		}
		int nResult = mysql_options(m_mysql,MYSQL_READ_DEFAULT_GROUP,"ChatServer");
		LOG_INFO(m_loger,"mysql_options() {}  [{} {}]",nResult, __FILENAME__, __LINE__);
		if (0 == mysql_library_init(0, NULL, NULL)) {
			LOG_INFO(m_loger,"mysql_library_init()succeed  [{} {}]", __FILENAME__, __LINE__);
		}
		else {
			LOG_INFO(m_loger, "mysql_library_init()failed [{} {}]", __FILENAME__, __LINE__);
		}

		m_pFriendChatInsertStmt = nullptr;
		m_pSelectUserByNameStmt = nullptr;
		m_pSelectAllUserNameStmt = nullptr;
}

/**
 * @brief 析构函数
 * 
 */
CMySqlConnect::~CMySqlConnect()
{
    mysql_close(m_mysql);
}


/**
 * @brief 创建数据库对应的表格
 * 
 * @return true 创建成功
 * @return false 创建失败
 */
bool CMySqlConnect::CreateTable()
{
	if (m_mysql)
	{
		//TABLE T_ADD_FRIEND_MSG
		{
			std::string strSql = R"( CREATE TABLE `T_ADD_FRIEND_MSG`  (
			`F_INDEX` int(255) NOT NULL AUTO_INCREMENT,
			`F_MSG_ID` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL COMMENT '消息ID',
			`F_USER_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL COMMENT '请求添加好友的用户',
			`F_FRIEND_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL COMMENT '被添加好友的用户',
			`F_ADD_FRIEND_STATUS` enum('UN_KNOWN', 'UN_READ', 'READ_UN_NOTIFY', 'NOTIFY') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'UN_KNOWN' COMMENT '添加消息的状态，\r\n未读----->已读未通知------>已通知',
			`F_FRIEND_OPTION` enum('UN_KNOWN', 'AGREE_ADD', 'REFUSE_ADD') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'UN_KNOWN' COMMENT '同意，未同意',
			`F_CREATE_TIME` datetime(0) DEFAULT NULL COMMENT '创建的时间',
			`F_OPTION_TIME` datetime(0) DEFAULT NULL COMMENT '对方回复的时间',
			`F_NOTIFY_TIME` datetime(0) DEFAULT NULL COMMENT '通知发送方的时间',
			PRIMARY KEY(`F_INDEX`) USING BTREE
			) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic; )";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_ADD_FRIEND SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_ADD_FRIEND has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}

		//T_ADD_INVATE_GROUP_RELATION_MSG
		{
			std::string strSql = R"( CREATE TABLE `T_ADD_INVATE_GROUP_RELATION_MSG`  (
				`F_INDEX` int(255) NOT NULL,
				`F_MSG_ID` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
				`F_USER_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
				`F_GROUP_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
				`F_CREATE_TIME` datetime(0) DEFAULT NULL,
				`F_OPTION_TIME` datetime(0) DEFAULT NULL,
				`F_NOTIFY_TIME` datetime(0) DEFAULT NULL,
				PRIMARY KEY (`F_INDEX`, `F_MSG_ID`) USING BTREE
			) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_ADD_INVATE_GROUP_RELATION_MSG SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_ADD_INVATE_GROUP_RELATION_MSG has Exists {} {} [{} {}]", errCode, errMsg, __FILENAME__, __LINE__);
				
			}
		}

		//T_FILE_HASH
		{
			std::string strSql = R"( CREATE TABLE `T_FILE_HASH`  (
			  `F_INDEX` int(255) UNSIGNED NOT NULL AUTO_INCREMENT,
			  `F_USER_ID` char(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
			  `F_FILE_NAME` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
			  `F_FILE_HASH` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
			  `F_CREATE_TIME` datetime(0) DEFAULT NULL,
			  PRIMARY KEY (`F_INDEX`) USING BTREE
			) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_FILE_HASH SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_FILE_HASH has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}

		//T_FRIEND_CHAT_MSG
		{
			std::string strSql = u8R"( CREATE TABLE `T_FRIEND_CHAT_MSG`  (
			  `F_INDEX` bigint(255) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增ID，由数据库产生',
			  `F_MSG_ID` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '消息ID,由程序生成',
			  `F_MSG_TYPE` char(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '消息类型',
			  `F_FROM_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '消息发送者ID',
			  `F_TO_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '消息接收者ID',
			  `F_MSG_CONTEXT` text CHARACTER SET utf8 COLLATE utf8_bin NOT NULL COMMENT '消息内容',
			  `F_OTHER_INFO` text CHARACTER SET utf8 COLLATE utf8_bin COMMENT '消息的其他部分的信息，比如文本消息的字体信息',
			  `F_READ_FLAG` enum('UNREAD','READ') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT 'UNREAD' COMMENT '信息是否被读取,\'READ\',\'UNREAD\'',
			  `F_CREATE_TIME` timestamp(0) DEFAULT CURRENT_TIMESTAMP COMMENT '信息创建时间',
			  `F_READ_TIME` timestamp(0) DEFAULT CURRENT_TIMESTAMP COMMENT '信息读取时间',
			  PRIMARY KEY (`F_INDEX`) USING BTREE
			) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_FRIEND_CHAT_MSG SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_FRIEND_CHAT_MSG has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}


		//T_FRIEND_RELATION
		{
			std::string strSql = u8R"( CREATE TABLE `T_FRIEND_RELATION`  (
  `F_INDEX` int(255) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `F_USER_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '用户ID,外键到T_USER表的F_USER_ID',
  `F_TEAM_ID` char(8) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT '1000000' COMMENT '分组ID',
  `F_FRIEND_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '好友ID,外键到T_USER表的F_USER_ID',
  `F_STATUS` enum('FRIEND','BLACK') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL COMMENT '好友状态,\'FRIEND\',\'BLACK\'',
  `F_CREATE_TIME` datetime(0) DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `F_UPDATE_TIME` datetime(0) DEFAULT CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`F_INDEX`, `F_USER_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_FRIEND_RELATION SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_FRIEND_RELATION has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}

		//T_GROUP
		{
			std::string strSql = u8R"( CREATE TABLE `T_GROUP`  (
				  `F_INDEX` int(255) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
				  `F_GROUP_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '群组ID',
				  `F_GROUP_NAME` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'https://www.dennisthink.com' COMMENT '群组名称',
				  `F_CREATE_TIME` datetime(0) DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
				  `F_GROUP_INFO` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'https://www.dennisthink.com' COMMENT '群组简介',
				  PRIMARY KEY (`F_INDEX`, `F_GROUP_ID`) USING BTREE
				) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_GROUP SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_GROUP has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}

		//T_GROUP_RELATION
		{
			std::string strSql = u8R"( CREATE TABLE `T_GROUP_RELATION`  (
			  `F_INDEX` int(255) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增ID',
			  `F_GROUP_ID` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '群组ID',
			  `F_USER_ID` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '用户ID',
			  `F_ROLE_TYPE` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '用户角色，\'OWNER\',\'MANAGER\',\'MEMBER\' ',
			  `F_LAST_READ_MSG_ID` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '00000000' COMMENT '阅读的最后一个消息的编号',
			  `F_CREATE_TIME` datetime(0) DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
			  PRIMARY KEY (`F_INDEX`) USING BTREE
			) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_GROUP_RELATION SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_GROUP_RELATION has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}


		//T_USER
		{
			std::string strSql = u8R"( CREATE TABLE `T_USER`  (
  `F_INDEX` int(255) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '数据记录索引，自增',
  `F_USER_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '00000000' COMMENT '用户ID',
  `F_USER_NAME` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '11111111' COMMENT '用户名',
  `F_PASS_WORD` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '11111111' COMMENT '用户密码',
  `F_ADDRESS` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'TesT_USER_ADDRESS' COMMENT '用户住址',
  `F_BIRTH_DATE` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '19901010' COMMENT '出生日期',
  `F_EMAIL_ADDR` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'test@dennisthink.com' COMMENT '电子邮箱地址',
  `F_NICK_NAME` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'DennisThink.com' COMMENT '用户昵称',
  `F_CREATE_TIME` timestamp(0) DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `F_UPDATE_TIME` timestamp(0) DEFAULT CURRENT_TIMESTAMP COMMENT '最后更新时间',
  `F_SIGNATURE` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '个人博客 https://www.dennisthink.com/' COMMENT '用户签名',
  `F_ON_LINE_STATE` enum('ON_LINE','OFF_LINE') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'OFF_LINE' COMMENT '用户在线状态，分为在线和离线',
  `F_FACE_ID` int(11) DEFAULT 2 COMMENT '头像编号',
  `F_GENDER` enum('MALE','FEMALE','UN_KNOWN') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT 'UN_KNOWN' COMMENT '用户性别',
  PRIMARY KEY (`F_INDEX`, `F_USER_ID`) USING BTREE,
  UNIQUE INDEX `F_USER_NAME_INDEX`(`F_USER_NAME`) USING BTREE COMMENT '用户名做唯一索引'
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_USER SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_USER has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}

		//T_USER_TEAM
		{
			std::string strSql = u8R"( CREATE TABLE `T_USER_TEAM`  (
  `F_INDEX` int(255) NOT NULL AUTO_INCREMENT COMMENT '唯一ID',
  `F_USER_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '用户ID',
  `F_TEAM_ID` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '分组编号',
  `F_TEAM_NAME` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT '',
  `F_CREATE_TIME` datetime(0) DEFAULT CURRENT_TIMESTAMP COMMENT '创建日期',
  PRIMARY KEY (`F_INDEX`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;)";
			int res = mysql_query(m_mysql, strSql.c_str());//查询
			int errCode = mysql_errno(m_mysql);
			std::string errMsg = mysql_error(m_mysql);
			if (!res)
			{
				LOG_INFO(m_loger, "CREATE TABLE T_USER_TEAM SUCCEED {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
			}
			else
			{
				LOG_INFO(m_loger, "TABLE T_USER_TEAM has Exists {} {} [{} {}]",errCode,errMsg,__FILENAME__,__LINE__);
				
			}
		}
	}
	return true;
}
/**
 * @brief 连接到Mysql服务器
 * 
 * @param userName 数据库用户名
 * @param passwd 数据库密码
 * @param database 数据库名称
 * @param strHost 数据库服务器IP
 * @param port 数据库服务器断开
 * @return true 连接成功
 * @return false 连接失败
 */
bool CMySqlConnect::ConnectToServer(const std::string userName,
                                    const std::string passwd,
                                    const std::string database,
                                    const std::string strHost,const int port)
{
	try{
		if (NULL != mysql_real_connect(m_mysql, strHost.c_str(),userName.c_str(),passwd.c_str(), database.c_str(), 0,NULL, 0))
		{
			LOG_INFO(m_loger, "mysql_real_connect() succeed [{} {}]", __FILENAME__, __LINE__);
			int nResult = mysql_set_character_set(m_mysql, "UTF8");
			LOG_INFO(m_loger, "{}  succeed [{} {}]", nResult, __FILENAME__, __LINE__);
			return CreateTable();
		}
		else
		{
			LOG_ERR(m_loger, "mysql_real_connect() {} failed[{} {}]",port,__FILENAME__, __LINE__);
            return false;
		}
	}
	catch(std::exception ec)
	{
		LOG_INFO(m_loger,"MySql EC:{} ",ec.what());
		return false;
	}
}

/**
 * @brief 根据用户名获取用户的密码
 * 
 * @param userName 用户名
 * @param bean 用户名和密码的结构体
 * @return true 获取成功
 * @return false 获取失败
 */
bool CMySqlConnect::SelectUserByName(const std::string userName, T_USER_BEAN& bean)
{

	if (nullptr == m_pSelectUserByNameStmt)
	{

		m_pSelectUserByNameStmt = mysql_stmt_init(m_mysql);
		if (!m_pSelectUserByNameStmt)
		{
			return false;
		}
		std::string strSql = "SELECT F_USER_ID,F_USER_NAME,F_PASS_WORD FROM T_USER WHERE F_USER_NAME=?;";
		if (mysql_stmt_prepare(m_pSelectUserByNameStmt, strSql.c_str(), static_cast<unsigned long>(strSql.size())))
		{
			LOG_WARN(m_loger, "{} {} [{} {}]", mysql_stmt_error(m_pSelectUserByNameStmt), mysql_stmt_errno(m_pSelectUserByNameStmt), __FILENAME__, __LINE__);
			return false;
		}
	}

	MYSQL_BIND paramBind[1];
	memset(paramBind, 0, sizeof(paramBind));
	{
		char buff[32] = { 0 };
		#ifdef _WIN32
		strcpy_s(buff, userName.c_str());
		#else
		strcpy(buff, userName.c_str());
		#endif
		paramBind[0].buffer_type = MYSQL_TYPE_STRING;
		paramBind[0].buffer = buff;
		paramBind[0].buffer_length = static_cast<unsigned long>(userName.length());

		if (mysql_stmt_bind_param(m_pSelectUserByNameStmt, paramBind))
		{
			LOG_WARN(m_loger, "{} {} [{} {}]", mysql_stmt_error(m_pSelectUserByNameStmt), mysql_stmt_errno(m_pSelectUserByNameStmt),__FILENAME__,__LINE__);
			return false;
		}

		
		{
			unsigned long length[4];
	
			bool       is_null[4];
			bool       error[4];
			MYSQL_BIND resultBind[3];
			memset(resultBind, 0, sizeof(resultBind));

			char resultBuf[3][128] = { 0 };
			
			resultBind[0].buffer_type = MYSQL_TYPE_STRING;
			resultBind[0].buffer = resultBuf[0];
			resultBind[0].buffer_length = 64;
			resultBind[0].length = &length[0];
			resultBind[0].is_null = &is_null[0];
			resultBind[0].error = &error[0];

			resultBind[1].buffer_type = MYSQL_TYPE_STRING;
			resultBind[1].buffer = resultBuf[1];
			resultBind[1].buffer_length = 64;
			resultBind[1].length = &length[1];
			resultBind[1].is_null = &is_null[1];
			resultBind[1].error = &error[1];

			resultBind[2].buffer_type = MYSQL_TYPE_STRING;
			resultBind[2].buffer = resultBuf[2];
			resultBind[2].buffer_length = 64;
			resultBind[2].length = &length[2];
			resultBind[2].is_null = &is_null[2];
			resultBind[2].error = &error[2];

			try {
				if (mysql_stmt_bind_result(m_pSelectUserByNameStmt, resultBind))
				{
					return false;
				}
				if (mysql_stmt_execute(m_pSelectUserByNameStmt))
				{
					return false;
				}
				if (mysql_stmt_store_result(m_pSelectUserByNameStmt))
				{
					return false;
				}
				while (true)
				{
					int nState = mysql_stmt_fetch(m_pSelectUserByNameStmt);
					if (nState == 1 || nState == MYSQL_NO_DATA) {
						return false;
					}
					else
					{
						bean.m_strF_USER_ID = std::string(static_cast<char*>(resultBind[0].buffer), *(resultBind[0].length));
						bean.m_strF_USER_NAME = std::string(static_cast<char*>(resultBind[1].buffer), *(resultBind[1].length));
						bean.m_strF_PASS_WORD = std::string(static_cast<char*>(resultBind[2].buffer), *(resultBind[2].length));
					}
					break;
				}
			}
			catch (std::exception ec)
			{
				LOG_ERR(m_loger, "EC:{} [{} {}]", ec.what(), __FILENAME__, __LINE__);
			}
		}
		
	}
	return true;
	//MYSQL_RES *result;
	//MYSQL_ROW sql_row;
	//int res = 0;
	//bool bResult = false;
	//constexpr char * strTemplate2 = "SELECT F_USER_ID,F_USER_NAME,F_PASS_WORD FROM T_USER WHERE F_USER_NAME='{0}';";
	//std::string strSql = fmt::format(strTemplate2, userName);
	//LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	//res = mysql_query(m_mysql, strSql.c_str());
	//if (!res)
	//{
	//	result = mysql_store_result(m_mysql);
	//	if (result)
	//	{
	//		while (sql_row = mysql_fetch_row(result))
	//		{
	//			bean.m_strF_USER_ID = sql_row[0];
	//			bean.m_strF_USER_NAME = sql_row[1];
	//			bean.m_strF_PASS_WORD = sql_row[2];
	//			bResult = true;
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	return false;
	//}
	//if (result != NULL)
	//{
	//	mysql_free_result(result);
	//}
 //   return bResult;
}


/**
 * @brief 判断用户是否存在
 * 
 * @param userName 用户名
 * @return true 存在
 * @return false 不存在
 */
bool CMySqlConnect::IsUserExist(const std::string strUserId)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_USER_ID FROM T_USER WHERE F_USER_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strUserId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql  ,__FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	bool bExist = false;
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				bExist = true;
				break;
			}
		}
	}
	else
	{
		return bExist;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return bExist;
}


/**
 * @brief 判断userName 和friendName是不是好友
 * 
 * @param userName 用户名
 * @param friendName 好友用户名
 * @return true friendName是userName的好友
 * @return false 不是好友
 */
bool CMySqlConnect::IsFriend(const std::string userId,const std::string friendId)
{
	MYSQL_RES *result=nullptr;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_STATUS FROM T_FRIEND_RELATION WHERE F_USER_ID='{0}' AND F_FRIEND_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, userId, friendId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	bool bExist = false;
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				bExist = true;
				break;
			}
		}
	}
	else
	{
		return bExist;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return bExist;
}

/**
 * @brief 更新用户的密码
 * 
 * @param bean 用户名和密码的结构体
 * @return true 更新成功
 * @return false 更新失败
 */
bool CMySqlConnect::UpdateUser(const T_USER_BEAN& bean)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_USER SET F_PASS_WORD='{0}' WHERE F_USER_NAME='{1}';";
	std::string strSql = fmt::format(strTemplate2, bean.m_strF_PASS_WORD, bean.m_strF_USER_NAME);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * @brief 更新用户在线状态
 * 
 * @param strUserId 用户ID
 * @param type 在线状态
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateUserOnlineState(const std::string strUserId, const CLIENT_ONLINE_TYPE type)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_USER SET F_ON_LINE_STATE='{0}' WHERE F_USER_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, OnLineType(type), strUserId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief 删除某个用户
 * 
 * @param userName 待删除的用户名
 * @return true 删除成功
 * @return false 删除失败
 */
bool CMySqlConnect::DeleteUser(const std::string strUserId)
{
	int res = 0;
	constexpr char strTemplate2[] = "DELETE FROM T_USER WHERE F_USER_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strUserId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}


/**
 * @brief 插入用户信息到数据库
 * 
 * @param bean 用户名和密码的结构体
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertUser(const T_USER_BEAN& bean)
{
	if (bean.IsValid())
	{
		int res = 0;
		constexpr char strTemplate2[] = "INSERT INTO T_USER(F_USER_ID,F_USER_NAME,F_PASS_WORD,F_NICK_NAME) VALUES('{0}','{1}','{2}','{3}');";
		std::string strSql = fmt::format(strTemplate2, bean.m_strF_USER_ID, bean.m_strF_USER_NAME, bean.m_strF_PASS_WORD, bean.m_strF_NICK_NAME);

		LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
		res = mysql_query(m_mysql, strSql.c_str());//查询
		if (!res)
		{
			return true;
		}
		else
		{
			LOG_ERR(m_loger, "ERR:{} [{}  {} ]", mysql_errno(m_mysql),__FILENAME__, __LINE__);
			return false;
		}
	}
	else
	{
		LOG_ERR(m_loger, "Bean Is Not Valid [{} {}]", __FILENAME__, __LINE__);
		return false;
	}
	
}

/**
 * @brief 根据用户名查找用户的其他信息
 * 
 * @param userName 用户名
 * @param bean 用户的其他信息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectUserInfoByName(const std::string userId, T_USER_INFO_BEAN& bean)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_USER_ID,\
F_USER_NAME,\
F_ADDRESS,\
F_BIRTH_DATE,\
F_EMAIL_ADDR,\
F_NICK_NAME,\
F_SIGNATURE,\
F_FACE_ID,\
F_ON_LINE_STATE \
FROM T_USER WHERE F_USER_ID='{0}' OR F_USER_NAME='{0}';";
	
	std::string strSql = fmt::format(strTemplate2, userId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_USER_ID = sql_row[0];
				bean.m_strF_USER_NAME = sql_row[1];
				bean.m_strF_ADDRESS = sql_row[2];
				bean.m_strF_BIRTH_DATE = sql_row[3];
				bean.m_strF_EMAIL_ADDR = sql_row[4];
				bean.m_strF_NICK_NAME = sql_row[5];
				bean.m_strF_SIGNATURE = sql_row[6];
				bean.m_strF_FACE_ID = sql_row[7];
				bean.m_eOnlineState = OnLineType(sql_row[8]);
				break;
			}
		}
	}
	else
	{
		return false;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return true;
}


/**
 * @brief 更新用户的其他信息
 * 
 * @param bean 用户的其他信息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateUserInfo(const T_USER_INFO_BEAN& bean)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_USER SET F_ADDRESS='{0}',F_BIRTH_DATE='{1}',F_EMAIL_ADDR='{2}',F_NICK_NAME='{3}',F_SIGNATURE='{4}',F_FACE_ID='{5}' WHERE F_USER_NAME='{6}';";
	std::string strSql = fmt::format(strTemplate2,bean.m_strF_ADDRESS,
		bean.m_strF_BIRTH_DATE,
		bean.m_strF_EMAIL_ADDR,
		bean.m_strF_NICK_NAME,
		bean.m_strF_SIGNATURE,
		bean.m_strF_FACE_ID,
		bean.m_strF_USER_NAME);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

/**
 * @brief 插入用户的其他信息
 * 
 * @param bean 用户的其他信息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertUserInfo(const T_USER_INFO_BEAN& bean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_USER(\
F_USER_NAME,\
F_ADDRESS,\
F_BIRTH_DATE,\
F_EMAIL_ADDR,\
F_NICK_NAME,\
F_SIGNATURE,\
F_FACE_ID) VALUES('{0}','{1}','{2},'{3}','{4}','{5}','{6}');";
	std::string strSql = fmt::format(strTemplate2, bean.m_strF_USER_NAME, bean.m_strF_ADDRESS,bean.m_strF_BIRTH_DATE,bean.m_strF_EMAIL_ADDR,bean.m_strF_NICK_NAME,bean.m_strF_SIGNATURE,bean.m_strF_FACE_ID);

	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}

}


/**
 * @brief 插入好友文本聊天消息到数据库
 * 
 * @param chatMsg 聊天消息
 * @return true 插入成功
 * @return false 失败
 */
bool CMySqlConnect::InsertFriendChatMsg(const T_USER_CHAT_MSG& chatMsg) {

	if (nullptr == m_pFriendChatInsertStmt)
	{
		m_pFriendChatInsertStmt = mysql_stmt_init(m_mysql); 
		if (!m_pFriendChatInsertStmt)
		{
			return false;
		}
		std::string strSql = "INSERT INTO T_FRIEND_CHAT_MSG(F_MSG_ID,\
F_MSG_TYPE,\
F_FROM_ID,\
F_TO_ID,\
F_MSG_CONTEXT,\
F_OTHER_INFO) VALUES(?,?,?,?,?,?);";
		if (mysql_stmt_prepare(m_pFriendChatInsertStmt, strSql.c_str(), static_cast<unsigned long>(strSql.size())))
		{
			return false;
		}
	}

	MYSQL_BIND paramBind[6];
	memset(paramBind, 0, sizeof(paramBind));
	{
		paramBind[0].buffer_type = MYSQL_TYPE_STRING;
		paramBind[0].buffer = (void*)(chatMsg.m_strF_MSG_ID.c_str());
		paramBind[0].buffer_length = static_cast<unsigned long>(chatMsg.m_strF_MSG_ID.length());

		paramBind[1].buffer_type = MYSQL_TYPE_STRING;
		std::string strMsgType = ChatType(chatMsg.m_eChatMsgType);
		paramBind[1].buffer = (void*)(strMsgType.c_str());
		paramBind[1].buffer_length = static_cast<unsigned long>(strMsgType.length());

		paramBind[2].buffer_type = MYSQL_TYPE_STRING;
		paramBind[2].buffer = (void*)(chatMsg.m_strF_FROM_ID.c_str());
		paramBind[2].buffer_length = static_cast<unsigned long>(chatMsg.m_strF_FROM_ID.length());

		paramBind[3].buffer_type = MYSQL_TYPE_STRING;
		paramBind[3].buffer = (void*)(chatMsg.m_strF_TO_ID.c_str());
		paramBind[3].buffer_length = static_cast<unsigned long>(chatMsg.m_strF_TO_ID.length());

		paramBind[4].buffer_type = MYSQL_TYPE_STRING;
		paramBind[4].buffer = (void*)(chatMsg.m_strF_MSG_CONTEXT.c_str());
		paramBind[4].buffer_length = static_cast<unsigned long>(chatMsg.m_strF_MSG_CONTEXT.length());

		paramBind[5].buffer_type = MYSQL_TYPE_STRING;
		paramBind[5].buffer = (void*)(chatMsg.m_strF_OTHER_INFO.c_str());
		paramBind[5].buffer_length = static_cast<unsigned long>(chatMsg.m_strF_OTHER_INFO.length());

		if (mysql_stmt_bind_param(m_pFriendChatInsertStmt, paramBind))
		{
			return false;
		}
		try {
			if (mysql_stmt_execute(m_pFriendChatInsertStmt))
			{
				return false;
			}
		}
		catch (std::exception ec)
		{
			LOG_ERR(m_loger, "EC:{} [{} {}]", ec.what(), __FILENAME__, __LINE__);
		}
	}
	return true;
}


/**
 * @brief 根据消息ID更新消息的状态
 * 
 * @param strMsgId 消息ID
 * @param msgState 消息的状态，为未读('UN_READ')或者已读('READ')
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateFriendChatMsgState(const std::string& strMsgId, const std::string msgState)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_FRIEND_CHAT_MSG SET F_READ_FLAG='{0}',F_READ_TIME=now() WHERE F_MSG_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, msgState, strMsgId);
	
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{
		auto nRows = mysql_affected_rows(m_mysql);
		LOG_INFO(m_loger, "SQL:{}  Rows:{} [{}  {} ]", strSql, nRows,__FILENAME__, __LINE__);
		return true;
	}
	else
	{
		LOG_ERR(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
		return false;
	}

}

/**
 * @brief 从队列中获取未读消息
 * 
 * @param strUserId 用户ID
 * @param chatMsg 未读消息
 * @return true 获取成功
 * @return false 获取失败
 */
bool CMySqlConnect::UnReadFromQueue(const std::string strUserId, T_USER_CHAT_MSG& chatMsg)
{
	auto item = m_unReadChatMsg.find(strUserId);
	if (item != m_unReadChatMsg.end())
	{
		if (!m_unReadChatMsg[strUserId].empty())
		{
			chatMsg = m_unReadChatMsg[strUserId].front();
			m_unReadChatMsg[strUserId].pop();
			return true;
		}
	}
	return false;
}

/**
 * @brief 判断用户是否有好友未读消息
 * 
 * @param strUserId 用户ID
 * @return true 有未读消息
 * @return false 没有未读消息
 */
bool CMySqlConnect::HaveUnReadMsg(const std::string strUserId)
{
	auto item = m_unReadChatMsg.find(strUserId);
	if (item != m_unReadChatMsg.end())
	{
		if (!(m_unReadChatMsg[strUserId].empty()))
		{
			return true;
		}
	}
	return SaveMsgToQueue(strUserId);
}
/**
 * @brief 选择未读的聊天消息
 * 
 * @param strToID 接收方用户名
 * @param chatMsg 聊天消息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectUnReadFriendChatMsg(const std::string& strToID, T_USER_CHAT_MSG& chatMsgNew)
{
	if (UnReadFromQueue(strToID, chatMsgNew))
	{
		return true;
	}
	else
	{
		SaveMsgToQueue(strToID);
	}
	return UnReadFromQueue(strToID, chatMsgNew);
}

/**
 * @brief 从数据库中读取未读消息保存到未读队列
 * 
 * @param strUserId 用户ID
 * @return true 保存成功
 * @return false 保存失败
 */
bool CMySqlConnect::SaveMsgToQueue(const std::string strUserId)
{
	std::queue<T_USER_CHAT_MSG> chatMsgQueue;
	bool bResult = false;
	MYSQL_RES *result = nullptr;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_MSG_ID,F_MSG_TYPE,F_FROM_ID,F_TO_ID,F_MSG_CONTEXT,F_OTHER_INFO,F_CREATE_TIME FROM T_FRIEND_CHAT_MSG WHERE (F_TO_ID={0} AND F_READ_FLAG='UNREAD');";
	std::string strSql = fmt::format(strTemplate2, strUserId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			T_USER_CHAT_MSG chatMsg;
			while ((sql_row = mysql_fetch_row(result)))
			{
				chatMsg.m_strF_MSG_ID = (sql_row[0]);
				chatMsg.m_eChatMsgType = ChatType(sql_row[1]);
				chatMsg.m_strF_FROM_ID = sql_row[2];
				chatMsg.m_strF_TO_ID = sql_row[3];
				chatMsg.m_strF_MSG_CONTEXT = sql_row[4];
				chatMsg.m_strF_OTHER_INFO = sql_row[5];
				chatMsg.m_strF_CREATE_TIME = sql_row[6];
				chatMsgQueue.push(chatMsg);
				bResult = true;
			}
		}
		mysql_free_result(result);
	}
	m_unReadChatMsg.erase(strUserId);
	m_unReadChatMsg.insert({ strUserId,chatMsgQueue });

	return bResult;
}
/**
 * @brief 删除好友聊天消息
 * 
 * @param msgId 消息ID
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::DeleteFriendChatMsg(const uint64_t msgId){
	int res = 0;
	constexpr char strTemplate2[] = "DELETE FROM T_FRIEND_CHAT_MSG WHERE F_MSG_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, msgId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 获取用户的好友列表
 * 
 * @param strUser 用户ID
 * @param friendList 好友列表
 * @return true 获取成功
 * @return false 回去失败
 */
bool CMySqlConnect::GetUserFriendList(const std::string strUser, std::vector<T_FRIEND_RELATION_BEAN>& friendList)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_TEAM_ID,\
F_FRIEND_ID,\
F_STATUS \
FROM T_FRIEND_RELATION WHERE F_USER_ID='{0}' ORDER BY F_TEAM_ID DESC;";
	std::string strSql = fmt::format(strTemplate2, strUser);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			T_FRIEND_RELATION_BEAN bean;
			bean.m_strF_USER_ID = strUser;
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_TEAM_ID = sql_row[0];
				bean.m_strF_FRIEND_ID = sql_row[1];
				bean.m_eF_STATUS = E_FRIEND_RELATION::E_FRIEND_TYPE;
				friendList.push_back(bean);
				bResult = true;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}
/**
 * @brief 获取某个用户的好友ID列表
 * 
 * @param strUserName 用户名
 * @param friendList 好友列表
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::GetUserFriendList(const std::string strUserName,std::vector<std::string>& friendList)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_FRIEND_ID FROM T_FRIEND_RELATION WHERE F_USER_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strUserName);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				std::string strResult = sql_row[0];
				friendList.push_back(strResult);
				bResult = true;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}

/**
 * @brief 插入好友关系,添加好友的时候使用
 * 
 * @param bean 好友关系bean
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertFriendRelation(const T_FRIEND_RELATION_BEAN& bean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_FRIEND_RELATION(F_USER_ID,\
F_FRIEND_ID,\
F_TEAM_ID,\
F_STATUS,\
F_CREATE_TIME) VALUES('{0}','{1}','{2}','{3}',now());";
	std::string strSql = fmt::format(strTemplate2,bean.m_strF_USER_ID,bean.m_strF_FRIEND_ID,bean.m_strF_TEAM_ID,FriendRelation(bean.m_eF_STATUS));
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 插入好友关系
 * 
 * @param strUser 用户名
 * @param strFriend 好友用户名
 * @param relationType 关系类型(好友或者黑名单)
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertFriendRelation(const std::string strUser, const std::string strFriend, const E_FRIEND_RELATION relationType)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_FRIEND_RELATION(F_USER_ID,\
F_FRIEND_ID,\
F_TEAM_ID,\
F_STATUS,F_CREATE_TIME) VALUES('{0}','{1}','10000000','{2}',now());";
	std::string strRelation = "FRIEND";
	if (relationType == E_FRIEND_RELATION::E_BLACK_TYPE)
	{
		strRelation = "BLACK";
	}
	std::string strSql = fmt::format(strTemplate2, strUser, strFriend, strRelation);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		LOG_ERR(m_loger, "{} [{} {}]", strSql, __FILENAME__, __LINE__);
		return false;
	}
	return true;
}

/**
 * @brief 删除好友关系
 * 
 * @param strUser 用户名
 * @param strFriend 好友用户名
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::DeleteFriendRelation(const std::string strUser, const std::string strFriend)
{
	int res = 0;
	constexpr char strTemplate2[] = "DELETE FROM T_FRIEND_RELATION WHERE F_USER_ID='{0}' AND F_FRIEND_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, strUser,strFriend);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 更新好友的分组ID
 * 
 * TODO: 可以删除
 * @param bean 
 * @return true 成功 
 * @return false 失败
 */
bool CMySqlConnect::UpdateFriendRelation(const T_FRIEND_RELATION_BEAN& bean)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_FRIEND_RELATION SET F_TEAM_ID='{0}' WHERE F_USER_ID='{1}' AND F_FRIEND_ID='{2}';";
	std::string strSql = fmt::format(strTemplate2,bean.m_strF_TEAM_ID,bean.m_strF_USER_ID,bean.m_strF_FRIEND_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
	return false;
}
/**
 * @brief 查询A用户和B用户的好友关系
 * 
 * @param strUser 用户名
 * @param strFriend 好友用户名
 * @param relationType 关系类型
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectFriendRelation(const std::string strUser, const std::string strFriend, E_FRIEND_RELATION& relationType)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_STATUS FROM T_FRIEND_RELATION WHERE F_USER_NAME='{0}' AND F_FRIEND_NAME='{1}';";
	std::string strSql = fmt::format(strTemplate2, strUser, strFriend);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	const std::string strFriendRelation = "FRIEND";
	const std::string strBlackRelation = "BLACK";

	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				if(sql_row[0]==strFriendRelation)
				{
					relationType = E_FRIEND_RELATION::E_FRIEND_TYPE;
				}
				else if(sql_row[0]==strBlackRelation)
				{
					relationType = E_FRIEND_RELATION::E_BLACK_TYPE;
				}
				else
				{
					relationType = E_FRIEND_RELATION::E_STRANGER_TYPE;
				}
				bResult = true;
				break;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}

/**
 * @brief 修改用户的分组ID,删除用户分组的时候，将分组的所有成员移动到我的好友分组
 * 
 * @param strUser 用户ID
 * @param strOldTeamId 旧分组ID
 * @param strNewTeamId 新的分组ID
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateFriendTeamId(const std::string strUser, const std::string strOldTeamId, const std::string strNewTeamId)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_FRIEND_RELATION SET F_TEAM_ID='{0}' WHERE F_USER_ID='{1}' AND F_TEAM_ID='{2}';";
	std::string strSql = fmt::format(strTemplate2,strNewTeamId,strUser,strOldTeamId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}
/**
 * @brief 更新好友关系
 * 
 * @param strUser 用户名
 * @param strFriend 好友用户名
 * @param relationType 好友类型 
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateFriendRelation(const std::string strUser, const std::string strFriend, const E_FRIEND_RELATION relationType)
{
	DeleteFriendRelation(strUser, strFriend);
	return InsertFriendRelation(strUser, strFriend, relationType);
}


/**
 * @brief 插入添加好友的消息
 * 
 * @param msgBean 添加好友消息的内容
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertAddFriendMsg(const T_ADD_FRIEND_MSG_BEAN msgBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_ADD_FRIEND_MSG(F_MSG_ID,\
F_USER_ID,\
F_FRIEND_ID,\
F_ADD_FRIEND_STATUS,\
F_FRIEND_OPTION,F_CREATE_TIME)\
VALUES('{0}','{1}','{2}','{3}','{4}',now());";

	std::string strSql = fmt::format(strTemplate2, 
								msgBean.m_strF_MSG_ID,
								msgBean.m_strF_USER_ID,
								msgBean.m_strF_FRIEND_ID,
								FriendStatus(msgBean.m_eF_ADD_FRIEND_STATUS),
		                        FriendOption(msgBean.m_eF_FRIEND_OPTION));
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 选择没有读取的添加好友消息[发送方发起了申请，但是接收方还没有收到的]
 * 
 * @param strUserName 用户名
 * @param msgBean 添加好友消息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectUnReadAddFriendMsg(const std::string strUserName, T_ADD_FRIEND_MSG_BEAN& msgBean)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_MSG_ID,F_USER_ID FROM T_ADD_FRIEND_MSG WHERE F_FRIEND_ID='{0}' AND F_ADD_FRIEND_STATUS='UN_READ';";
	std::string strSql = fmt::format(strTemplate2, strUserName);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				msgBean.m_strF_MSG_ID = sql_row[0];
				msgBean.m_strF_FRIEND_ID = sql_row[1];
				msgBean.m_strF_USER_ID = strUserName;
				bResult = true;
				break;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}


/**
 * @brief 选择没有通知的添加好友消息[被添加方已经回复，但是发送方还没有收到通知]
 * 
 * @param strUserName 用户名
 * @param msgBean 添加好友消息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectUnNotifyAddFriendMsg(const std::string strUserName, T_ADD_FRIEND_MSG_BEAN& msgBean)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_MSG_ID,\
F_FRIEND_ID,F_FRIEND_OPTION  FROM T_ADD_FRIEND_MSG WHERE F_USER_ID='{0}' AND F_ADD_FRIEND_STATUS='READ_UN_NOTIFY';";
	std::string strSql = fmt::format(strTemplate2, strUserName);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				msgBean.m_strF_MSG_ID = sql_row[0];
				msgBean.m_strF_USER_ID = strUserName;
				msgBean.m_strF_FRIEND_ID = sql_row[1];
				msgBean.m_eF_FRIEND_OPTION = FriendOption(sql_row[2]);
				bResult = true;
				break;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}

/**
 * @brief 更新添加好友消息为通知的状态[发送方已经接收到添加好友的通知]
 * 
 * @param strMsgId 消息ID
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateToNotifyAddFriendMsg(const std::string strMsgId)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_ADD_FRIEND_MSG SET F_ADD_FRIEND_STATUS='NOTIFY',F_NOTIFY_TIME=now() WHERE F_MSG_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strMsgId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 更新添加好友消息的状态为已读但是未通知的状态
 * 
 * @param strMsgId 消息ID
 * @param option 用户选择
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateToReadUnNotifyAddFriendMsg(const std::string strMsgId,const E_FRIEND_OPTION option)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_ADD_FRIEND_MSG SET F_ADD_FRIEND_STATUS='READ_UN_NOTIFY',F_FRIEND_OPTION='{0}',F_OPTION_TIME=now() WHERE F_MSG_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, FriendOption(option), strMsgId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 插入用户分组
 * 
 * @param teamBean 用户分组
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertUserTeam(const T_USER_TEAM_BEAN& teamBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_USER_TEAM(F_USER_ID,F_TEAM_ID,F_TEAM_NAME,F_CREATE_TIME) VALUES('{0}','{1}','{2}',now());";
	std::string strSql = fmt::format(strTemplate2, teamBean.m_strF_USER_ID,teamBean.m_strF_TEAM_ID,teamBean.m_strF_TEAM_NAME);

	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		int nError = mysql_errno(m_mysql);
		LOG_INFO(m_loger, "SQL_ERROR:{} [{}  {} ]", nError, __FILENAME__, __LINE__);
		return false;
	}
	return true;
}


/**
 * @brief 删除用户分组
 * 
 * @param teamBean 用户分组
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::DeleteUserTeam(const T_USER_TEAM_BEAN& teamBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "DELETE FROM T_USER_TEAM WHERE F_TEAM_ID='{0}' AND F_USER_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, teamBean.m_strF_TEAM_ID,teamBean.m_strF_USER_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 选择某个用户的某个分组的所有好友
 * 
 * @param strUserName 用户ID
 * @param strTeamId 分组ID
 * @param teamBeans 所有好友的数组
 * @return true 
 * @return false 
 */
bool CMySqlConnect::SelectUserByTeamId(const std::string strUserName, const std::string strTeamId, std::vector<T_FRIEND_RELATION_BEAN>& teamBeans)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_FRIEND_ID,F_TEAM_ID, FROM T_FRIEND_RELATION WHERE F_USER_ID='{0}' AND F_TEAM_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, strUserName,strTeamId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			T_FRIEND_RELATION_BEAN bean;
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_FRIEND_ID = sql_row[0];
				bean.m_strF_TEAM_ID = sql_row[1];
				teamBeans.push_back(bean);
				bResult = true;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}
/**
 * @brief 查找用户的所有分组
 * 
 * @param strUserName 用户名
 * @param teamBeans 该用户的所有分组
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectUserTeams(const std::string strUserName, std::vector<T_USER_TEAM_BEAN>& teamBeans)
{
	bool bResult = false;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_USER_ID,F_TEAM_ID,F_TEAM_NAME FROM T_USER_TEAM WHERE F_USER_ID='{0}' ORDER BY F_TEAM_ID DESC;";
	std::string strSql = fmt::format(strTemplate2, strUserName);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			T_USER_TEAM_BEAN bean;
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_USER_ID = sql_row[0];
				bean.m_strF_TEAM_ID = sql_row[1];
				bean.m_strF_TEAM_NAME = sql_row[2];
				teamBeans.push_back(bean);
				bResult = true;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		return bResult;
	}
	return bResult;
}


/**
 * @brief 更新用户的分组名
 * 
 * @param teamBean 用户分组信息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateUserTeamName(const T_USER_TEAM_BEAN& teamBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_USER_TEAM SET F_TEAM_NAME='{0}' WHERE F_TEAM_ID='{1}' AND F_USER_ID='{2}";
	std::string strSql = fmt::format(strTemplate2, teamBean.m_strF_TEAM_NAME,teamBean.m_strF_TEAM_ID, teamBean.m_strF_USER_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}


/**
 * @brief 创建群聊
 * 
 * @param groupBean 群聊信息 
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertGroup(const T_GROUP_BEAN& groupBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_GROUP(F_GROUP_ID,F_GROUP_NAME,F_CREATE_TIME,F_GROUP_INFO) VALUES('{0}','{1}',now(),'{2}');";
	std::string strSql = fmt::format(strTemplate2, groupBean.m_strF_GROUP_ID, groupBean.m_strF_GROUP_NAME, "NO INFO");

	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 根据群组ID获取群组的基本信息
 * 
 * @param groupId 群组ID
 * @param groupBean 
 * @return true 
 * @return false 
 */
bool CMySqlConnect::SelectGroupById(const std::string groupId, T_GROUP_BEAN& groupBean)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	bool bResult = false;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_GROUP_ID,F_GROUP_NAME FROM T_GROUP WHERE F_GROUP_ID = '{0}';";
	std::string strSql = fmt::format(strTemplate2, groupId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	T_GROUP_BEAN bean;
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				groupBean.m_strF_GROUP_ID = sql_row[0];
				groupBean.m_strF_GROUP_NAME = sql_row[1];
				bResult = true;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return bResult;
}


/**
 * @brief 解散群聊
 * 
 * @param strGroupId 群聊ID  
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::DeleteGroup(const std::string strGroupId)
{
	int res = 0;
	constexpr char strTemplate2[] = "DELETE FROM T_GROUP WHERE F_GROUP_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strGroupId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 根据群组名查找群组
 * 
 * @param strGroupName 群组名
 * @param groupBeans 查找到的所有群组
 * @return true 查找成功
 * @return false 查找失败
 */
bool CMySqlConnect::SelectGroups(const std::string strGroupName, std::vector<T_GROUP_BEAN>& groupBeans)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	bool bResult = false;
	int res = 0;
constexpr char strTemplate2[] = "SELECT F_GROUP_ID,F_GROUP_NAME FROM T_GROUP WHERE F_GROUP_NAME LIKE '{0}' OR F_GROUP_ID LIKE '{0}';";
	std::string strSql = fmt::format(strTemplate2, strGroupName);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	T_GROUP_BEAN bean;
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_GROUP_ID = sql_row[0];
				bean.m_strF_GROUP_NAME = sql_row[1];
				groupBeans.push_back(bean);
				bResult = true;
			}
		}
	}
	else
	{
		return bResult;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return bResult;
}


/**
 * @brief 插入用户和群组的关系
 * 
 * @param memBean　用户群组消息
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::InsertGroupRelation(const T_GROUP_RELATION_BEAN& memBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_GROUP_RELATION(F_GROUP_ID,F_USER_ID,F_ROLE_TYPE,F_CREATE_TIME) VALUES('{0}','{1}','{2}',now());";
	std::string strSql = fmt::format(strTemplate2, memBean.m_strF_GROUP_ID, memBean.m_strF_USER_ID,MemberRole(memBean.m_eRole));

	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}


/**
 * @brief 更新用户和群组关系
 * 
 * @param memBean 新的用户群组关系
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::UpdateGroupRelation(const T_GROUP_RELATION_BEAN& memBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_GROUP_RELATION SET F_ROLE_TYPE='{0}' WHERE F_GROUP_ID='{1}' AND F_USER_ID='{2}';";
	std::string strSql = fmt::format(strTemplate2,MemberRole(memBean.m_eRole),memBean.m_strF_GROUP_ID, memBean.m_strF_USER_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 根据群组ID和用户ID，更新该用户的最后已读消息
 * 
 * @param memBean 传入群组ID，用户ID和已读消息ID
 * @return true 更新成功
 * @return false 更新失败
 */
bool CMySqlConnect::UpdateGroupRelationLastReadId(const T_GROUP_RELATION_BEAN& memBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "UPDATE T_GROUP_RELATION SET F_LAST_READ_MSG_ID='{0}' WHERE F_GROUP_ID='{1}' AND F_USER_ID='{2}';";
	std::string strSql = fmt::format(strTemplate2, memBean.m_strF_LAST_READ_MSG_ID, memBean.m_strF_GROUP_ID, memBean.m_strF_USER_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 删除用户和群组关系，退出群组和解散群组的时候使用
 * 
 * @param memBean 用户群组关系
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::DeleteGroupRelation(const T_GROUP_RELATION_BEAN& memBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "DELETE FROM T_GROUP_RELATION WHERE F_GROUP_ID='{0}' AND F_USER_ID='{1}';";
	std::string strSql = fmt::format(strTemplate2, memBean.m_strF_GROUP_ID,memBean.m_strF_USER_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}


/**
 * @brief 查找某个用户所在的所有群组
 * 
 * @param strUserName 用户名
 * @param memBeans 所在的全部群组
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectUserGroupRelation(const std::string strUserName, std::vector<T_GROUP_RELATION_BEAN>& memBeans)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_GROUP_ID,F_ROLE_TYPE,F_LAST_READ_MSG_ID FROM T_GROUP_RELATION WHERE F_USER_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strUserName);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			T_GROUP_RELATION_BEAN bean;
			bean.m_strF_USER_ID = strUserName;
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_GROUP_ID = sql_row[0];
				bean.m_eRole = MemberRole(sql_row[1]);
				bean.m_strF_LAST_READ_MSG_ID = sql_row[2];
				memBeans.push_back(bean);
			}
		}
	}
	else
	{
		return false;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return true;
}

/**
 * @brief 查找某个群组中的全部成员
 * 
 * @param strGroupId 群组ID
 * @param memBeans 群组的全部成员
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectGroupRelation(const std::string strGroupId, std::vector<T_GROUP_RELATION_BEAN>& memBeans)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_USER_ID,F_ROLE_TYPE,F_LAST_READ_MSG_ID FROM T_GROUP_RELATION WHERE F_GROUP_ID='{0}';";
	std::string strSql = fmt::format(strTemplate2, strGroupId);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			T_GROUP_RELATION_BEAN bean;
			while ((sql_row = mysql_fetch_row(result)))
			{
				bean.m_strF_USER_ID = sql_row[0];
				bean.m_eRole = MemberRole(sql_row[1]);
				bean.m_strF_LAST_READ_MSG_ID = sql_row[2];
				memBeans.push_back(bean);
			}
		}
	}
	else
	{
		return false;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return true;
}



/**
 * @brief 根据群组ID和消息ID,查询该群组下该消息以后的新消息，
 * 
 * @param chatMsg 传入群组ID和消息ID,同时传回消息内容和发送者
 * @return true 成功
 * @return false 失败
 */
bool CMySqlConnect::SelectGroupChatText(T_GROUP_CHAT_MSG& chatMsg)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_MSG_ID,\
F_MSG_TYPE,\
F_SENDER_ID,\
F_MSG_CONTEXT,\
F_OTHER_INFO,\
F_CREATE_TIME \
FROM T_GROUP_CHAT_MSG WHERE F_GROUP_ID='{0}' AND F_MSG_ID > '{1}' LIMIT 1;";
	std::string strSql = fmt::format(strTemplate2, chatMsg.m_strF_GROUP_ID,chatMsg.m_strF_MSG_ID);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				chatMsg.m_strF_MSG_ID = sql_row[0];
				chatMsg.m_eChatMsgType = ChatType(sql_row[1]);
				chatMsg.m_strF_SENDER_ID = sql_row[2];
				chatMsg.m_strF_MSG_CONTEXT = sql_row[3];
				chatMsg.m_strF_OTHER_INFO = sql_row[4];
				chatMsg.m_strF_CREATE_TIME = sql_row[5];
				break;
			}
		}
	}
	else
	{
		return false;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return true;
}


/**
 * @brief 插入群组聊天文本消息
 * 
 * @param chatMsg 群组聊天消息
 * @return true 插入成功
 * @return false 插入失败
 */
bool CMySqlConnect::InsertGroupChatText(const T_GROUP_CHAT_MSG& chatMsg)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_GROUP_CHAT_MSG(F_MSG_ID,\
F_MSG_TYPE,\
F_SENDER_ID,\
F_GROUP_ID,\
F_MSG_CONTEXT,\
F_OTHER_INFO,\
F_CREATE_TIME) VALUES('{0}','{1}','{2}','{3}','{4}','{5}',now());";
	std::string strSql = fmt::format(strTemplate2, chatMsg.m_strF_MSG_ID,
		ChatType(chatMsg.m_eChatMsgType), 
		chatMsg.m_strF_SENDER_ID,
		chatMsg.m_strF_GROUP_ID,
		chatMsg.m_strF_MSG_CONTEXT,
		chatMsg.m_strF_OTHER_INFO);

	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}


/**
 * @brief 插入文件Hash和文件名的关系
 * 
 * @param hashBean Hash和文件名的关系
 * @return true 插入成功
 * @return false 插入失败
 */
bool CMySqlConnect::InsertFileHash(const T_FILE_HASH_BEAN& hashBean)
{
	int res = 0;
	constexpr char strTemplate2[] = "INSERT INTO T_FILE_HASH(F_USER_ID,\
F_FILE_NAME,\
F_FILE_HASH,\
F_CREATE_TIME)\
VALUES('{0}','{1}','{2}',now());";
	std::string strSql = fmt::format(strTemplate2,
		hashBean.m_strF_USER_ID,
		hashBean.m_strF_FILE_NAME,
		hashBean.m_strF_FILE_HASH);

	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());//查询
	if (!res)
	{

	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief 根据HASH值获取文件名
 * 
 * @param hashBean 该hash值对对应的文件信息
 * @param strFileHash 文件hash值
 * @return true 存在对应的文件
 * @return false 不存在对应的文件
 */
bool CMySqlConnect::SelectFileByHash(T_FILE_HASH_BEAN& hashBean, const std::string strFileHash)
{
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res = 0;
	constexpr char strTemplate2[] = "SELECT F_USER_ID,\
F_FILE_NAME,\
F_FILE_HASH \
FROM T_FILE_HASH WHERE F_FILE_HASH='{0}' LIMIT 1;";
	std::string strSql = fmt::format(strTemplate2,strFileHash);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	bool bResult = false;
	if (!res)
	{
		result = mysql_store_result(m_mysql);
		if (result)
		{
			while ((sql_row = mysql_fetch_row(result)))
			{
				hashBean.m_strF_USER_ID = sql_row[0];
				hashBean.m_strF_FILE_NAME = sql_row[1];
				hashBean.m_strF_FILE_HASH = sql_row[2];
				bResult = true;
				break;
			}
		}
	}
	else
	{
		return false;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
	}
	return bResult;
}

/**
 * @brief 删除hash值对应的文件记录
 * 
 * @param strFileHash 文件Hash值
 * @return true 删除成功
 * @return false 删除失败
 */
bool CMySqlConnect::DeleteFileByHash(const std::string strFileHash)
{
	int res = 0;
	constexpr char strTemplate2[] = "DELETE \
FROM T_FILE_HASH WHERE F_FILE_HASH='{0}';";
	std::string strSql = fmt::format(strTemplate2, strFileHash);
	LOG_INFO(m_loger, "SQL:{} [{}  {} ]", strSql, __FILENAME__, __LINE__);
	res = mysql_query(m_mysql, strSql.c_str());
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief 获取数据库中保存的所有用户名
 * 
 * @param userNameVec 所有用户名列表
 * @return true 获取成功
 * @return false 获取失败
 */
bool CMySqlConnect::GetAllUserName(std::vector<std::string>& userNameVec)
{
	if (nullptr == m_pSelectAllUserNameStmt)
	{
		m_pSelectAllUserNameStmt = mysql_stmt_init(m_mysql);
		if (!m_pSelectAllUserNameStmt)
		{
			return false;
		}
		std::string strSql = "SELECT F_USER_NAME FROM T_USER;";
		if (mysql_stmt_prepare(m_pSelectAllUserNameStmt, strSql.c_str(), static_cast<unsigned long>(strSql.size())))
		{
			return false;
		}
	}




	{
		unsigned long length[1];

		bool       is_null[1];
		bool       error[1];
		MYSQL_BIND resultBind[1];
		memset(resultBind, 0, sizeof(resultBind));

		char resultBuf[1][64] = { 0 };

		resultBind[0].buffer_type = MYSQL_TYPE_STRING;
		resultBind[0].buffer = resultBuf[0];
		resultBind[0].buffer_length = 64;
		resultBind[0].length = &length[0];
		resultBind[0].is_null = &is_null[0];
		resultBind[0].error = &error[0];
		try {
			if (mysql_stmt_bind_result(m_pSelectAllUserNameStmt, resultBind))
			{
				return false;
			}
			if (mysql_stmt_execute(m_pSelectAllUserNameStmt))
			{
				return false;
			}
			if (mysql_stmt_store_result(m_pSelectAllUserNameStmt))
			{
				return false;
			}
			while (true)
			{
				int nState = mysql_stmt_fetch(m_pSelectAllUserNameStmt);
				if (nState == 1 || nState == MYSQL_NO_DATA) {
					break;
				}
				else
				{
					std::string strUserName = std::string(static_cast<char*>(resultBind[0].buffer), *(resultBind[0].length));
					userNameVec.push_back(strUserName);
				}
			}
		}
		catch (std::exception ec)
		{
			LOG_ERR(m_loger, "EC:{} [{} {}]", ec.what(), __FILENAME__, __LINE__);
		}
	}
	return !(userNameVec.empty());
}
