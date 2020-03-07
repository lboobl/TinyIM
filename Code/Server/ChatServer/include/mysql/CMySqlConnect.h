/**
 * @file CMySqlConnect.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 数据库操作的工具类
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _DENNIS_C_MYSQL_CONNECT_H_
#define _DENNIS_C_MYSQL_CONNECT_H_
#include "mysql.h"
#include <string>
#include <queue>
#include <map>
#include "CMysqlStruct.h"
#include "Log.h"
class CMySqlConnect
{
public:
    explicit CMySqlConnect();
    virtual ~CMySqlConnect();
    //bool ConnectToServer();
    bool ConnectToServer(const std::string userName,
                         const std::string passwd,
                         const std::string database,
                         const std::string strHost,
                         const int port=3306);

	bool CreateTable();
    
	//用户最基本的操作 begin
    bool SelectUserByName(const std::string userName,T_USER_BEAN& bean);
    
	bool UpdateUser(const T_USER_BEAN& bean);
    
	bool DeleteUser(const std::string strUserId);

    bool InsertUser(const T_USER_BEAN& bean);
	
	bool IsUserExist(const std::string strUserId);
	//用户最基本的操作 end

	// 用户基本信息的操作 begin
	bool SelectUserInfoByName(const std::string userName, T_USER_INFO_BEAN& bean);
	bool UpdateUserInfo(const T_USER_INFO_BEAN& bean);
	bool UpdateUserOnlineState(const std::string strUserId, const CLIENT_STATE);
	bool InsertUserInfo(const T_USER_INFO_BEAN& bean);
	bool GetAllUserName(std::vector<std::string>& userNameVec);
	// 用户基本信息的操作 end

	// 添加好友的操作 begin
	bool InsertAddFriendMsg(const T_ADD_FRIEND_MSG_BEAN msgBean);
	bool SelectUnReadAddFriendMsg(const std::string strUserName, T_ADD_FRIEND_MSG_BEAN& msgBean);
	bool SelectUnNotifyAddFriendMsg(const std::string strUserName, T_ADD_FRIEND_MSG_BEAN& msgBean);
	bool UpdateToNotifyAddFriendMsg(const std::string strMsgId);
	bool UpdateToReadUnNotifyAddFriendMsg(const std::string strMsgId, const E_FRIEND_OPTION option);
	// 添加好友的操作 end

	// 好友关系的基本操作 begin
	bool IsFriend(const std::string strUser, const std::string strFriend);
	bool InsertFriendRelation(const std::string strUser, const std::string strFriend, const E_FRIEND_RELATION relationType);
	bool DeleteFriendRelation(const std::string strUser, const std::string strFriend);
	bool SelectFriendRelation(const std::string strUser, const std::string strFriend, E_FRIEND_RELATION& relationType);
	bool UpdateFriendRelation(const std::string strUser, const std::string strFriend, const E_FRIEND_RELATION relationType);
	bool UpdateFriendRelation(const T_FRIEND_RELATION_BEAN& bean);
	bool UpdateFriendTeamId(const std::string strUser, const std::string strOldTeamId, const std::string strNewTeamId);
	bool InsertFriendRelation(const T_FRIEND_RELATION_BEAN& bean);
	bool GetUserFriendList(const std::string strUser, std::vector<std::string>& friendList);
	bool GetUserFriendList(const std::string strUser, std::vector<T_FRIEND_RELATION_BEAN>& friendList);
	// 好友关系的基本操作 end


	// 好友聊天消息的操作 begin
	bool InsertFriendChatMsg(const T_USER_CHAT_MSG& chatMsg);
	bool SelectUnReadFriendChatMsg(const std::string& strToId, T_USER_CHAT_MSG& chatMsg);
	bool HaveUnReadMsg(const std::string strUserId);
	bool UpdateFriendChatMsgState(const std::string& strMsgId, const std::string msgState);
	bool DeleteFriendChatMsg(const uint64_t msgId);
	// 好友聊天消息的操作 end

	// 好友分组的基本操作 begin
	bool InsertUserTeam(const T_USER_TEAM_BEAN& teamBean);
	bool DeleteUserTeam(const T_USER_TEAM_BEAN& teamBean);
	bool SelectUserTeams(const std::string strUserName, std::vector<T_USER_TEAM_BEAN>& teamBeans);
	bool SelectUserByTeamId(const std::string strUserName,const std::string strTeamId,std::vector<T_FRIEND_RELATION_BEAN>& teamBeans);
	bool UpdateUserTeamName(const T_USER_TEAM_BEAN& teamBean);
	// 好友分组的基本操作 end

	//  群组信息的基本操作 begin 
	bool InsertGroup(const T_GROUP_BEAN& groupBean);
	bool DeleteGroup(const std::string strGroupId);
	bool SelectGroupById(const std::string groupId, T_GROUP_BEAN& groupBean);
	bool SelectGroups(const std::string strGroupName, std::vector<T_GROUP_BEAN>& groupBeans);
	// 群组信息的基本操作 end

	// 群组成员基本操作 begin
	bool InsertGroupRelation(const T_GROUP_RELATION_BEAN& memBean);

	bool UpdateGroupRelation(const T_GROUP_RELATION_BEAN& memBean);

	bool UpdateGroupRelationLastReadId(const T_GROUP_RELATION_BEAN& memBean);

	bool DeleteGroupRelation(const T_GROUP_RELATION_BEAN& memBean);

	bool SelectUserGroupRelation(const std::string strUserName, std::vector<T_GROUP_RELATION_BEAN>& memBeans);

	bool SelectGroupRelation(const std::string strGroupId, std::vector<T_GROUP_RELATION_BEAN>& memBeans);
	
	bool SelectGroupUsers(const std::string strGroupId, std::vector<std::string> groupUsers);

	bool SelectGroupUserLastId(const std::string strUserId, const std::string strGroupId, std::string& strLastReadId);
	// 群组成员基本操作 end

	// 群聊消息基本操作 Begin
	bool InsertGroupChatText(const T_GROUP_CHAT_MSG& chatMsg);
	
	bool SelectGroupChatText(T_GROUP_CHAT_MSG& chatMsg);

	bool DeleteGroupChatText(const std::string m_strMsgId);
	// 群聊消息基本操作 End
	

	//  文件Hash的基本操作 begin
	//bool InsertFileHash(const T_FILE_HASH_BEAN& hashBean);
	//bool SelectFileByHash(T_FILE_HASH_BEAN& hashBean, const std::string strFileHash);
	//bool DeleteFileByHash(const std::string strFileHash);
	//  文件hash的基本操作 end

	static std::shared_ptr<spdlog::logger> m_loger;
private:
	bool SaveMsgToQueue(const std::string strUserId);
	bool UnReadFromQueue(const std::string strUserId, T_USER_CHAT_MSG& chatMsg);
    MYSQL* m_mysql;
	std::map<std::string,std::queue<T_USER_CHAT_MSG>> m_unReadChatMsg;
	MYSQL_STMT* m_pFriendChatInsertStmt;
	MYSQL_STMT* m_pSelectUserByNameStmt;
	MYSQL_STMT* m_pSelectAllUserNameStmt;

};
#endif