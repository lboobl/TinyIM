#include "CommonMsg.h"
json11::Json FriendChatMsg(const FriendChatMsg_s& chatMsg) {
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId",chatMsg.m_strChatMsgId},
			{"SenderId", chatMsg.m_strSenderId},
			{"ReciverId", chatMsg.m_strReceiverId},
			{"Context", chatMsg.m_strContext},
			{"MsgTime",chatMsg.m_strMsgTime},
			{"FontInfo",chatMsg.m_fontInfo.ToString()},
		});

	return clientObj;
}

bool FriendChatMsg(const json11::Json & msgItem,FriendChatMsg_s& chatMsg) {

	if (msgItem["MsgId"].is_string()) {
		chatMsg.m_strChatMsgId = msgItem["MsgId"].string_value();
	}
	else {
		return false;
	}
		

	if (msgItem["SenderId"].is_string()) {
		chatMsg.m_strSenderId = msgItem["SenderId"].string_value();
	}
	else {
		return false;
	}
		

	if (msgItem["ReciverId"].is_string()) {
		chatMsg.m_strReceiverId = msgItem["ReciverId"].string_value();
	}
	else {
		return false;
	}
	

	if (msgItem["MsgTime"].is_string()) {
		chatMsg.m_strMsgTime = msgItem["MsgTime"].string_value();
	}
	else {
		return false;
	}
		

	if (msgItem["Context"].is_string())
	{
		chatMsg.m_strContext = msgItem["Context"].string_value();
	}
	else {
		return false;
	}
		
	if (msgItem["FontInfo"].is_string()) {
		chatMsg.m_fontInfo.FromString(msgItem["FontInfo"].string_value());
	}
	else {
		return false;
	}

	return true;
}

json11::Json GroupChatMsg(const GroupChatMsg_s& chatMsg) {
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId",chatMsg.m_strChatMsgId},
			{"SenderId", chatMsg.m_strSenderId},
			{"GroupId", chatMsg.m_strGroupId},
			{"Context", chatMsg.m_strContext},
			{"MsgTime",chatMsg.m_strMsgTime},
			{"FontInfo",chatMsg.m_fontInfo.ToString()},
		});

	return clientObj;
}

bool GroupChatMsg(const json11::Json & msgItem, GroupChatMsg_s& chatMsg) {

	if (msgItem["MsgId"].is_string()) {
		chatMsg.m_strChatMsgId = msgItem["MsgId"].string_value();
	}
	else {
		return false;
	}


	if (msgItem["SenderId"].is_string()) {
		chatMsg.m_strSenderId = msgItem["SenderId"].string_value();
	}
	else {
		return false;
	}


	if (msgItem["GroupId"].is_string()) {
		chatMsg.m_strGroupId = msgItem["GroupId"].string_value();
	}
	else {
		return false;
	}


	if (msgItem["MsgTime"].is_string()) {
		chatMsg.m_strMsgTime = msgItem["MsgTime"].string_value();
	}
	else {
		return false;
	}


	if (msgItem["Context"].is_string())
	{
		chatMsg.m_strContext = msgItem["Context"].string_value();
	}
	else {
		return false;
	}

	if (msgItem["FontInfo"].is_string()) {
		chatMsg.m_fontInfo.FromString(msgItem["FontInfo"].string_value());
	}
	else {
		return false;
	}

	return true;
}


E_MsgType TransBaseMsg_t::GetType() const 
{
    Header* head = reinterpret_cast<Header*>(m_data);
    return static_cast<E_MsgType>(head->m_type);
}

std::size_t TransBaseMsg_t::GetSize() const
{
    Header* head = reinterpret_cast<Header*>(m_data);
    return head->m_length;
}

const char * TransBaseMsg_t::GetData() const 
{
    return m_data;
}

TransBaseMsg_t::TransBaseMsg_t(const E_MsgType& type, const std::string& strMsg)
{
    std::size_t strLen = strMsg.length();
    Header head;
    head.m_type = static_cast<uint32_t>(type);
    head.m_length = static_cast<uint32_t>(strLen + sizeof(head));
    m_data = new char[head.m_length];
    memset(m_data, 0, head.m_length);
    memcpy(m_data, &head, sizeof(head));
    memcpy(m_data + sizeof(head), strMsg.c_str(), strLen);
    m_selfData = true;
}

TransBaseMsg_t::TransBaseMsg_t(const char * data)
{
    m_data = const_cast<char*>(data);
    m_selfData = false;
}

TransBaseMsg_t::~TransBaseMsg_t()
{
    if(m_selfData)
    {
        if(nullptr != m_data)
        {
            delete m_data;
            m_data = nullptr;
        }
    }
}


std::string IpPortCfg::to_string()
{
    return m_strServerIp+":"+std::to_string(m_nPort);
}

bool IpPortCfg::Valid()
{
    if(m_strServerIp.empty())
    {
        return false;
    }		
    if(m_nPort<1024)
    {
        return false;
    }
    return true;
}



/**
 * @brief 获取消息的字符串内容
 * 
 * @return std::string 
 */
std::string TransBaseMsg_t::to_string() const
{ 
    Header* head = reinterpret_cast<Header*>(m_data);
    if (head->m_length <= 0)
    {
        return "";
    }
    else
    {
        return std::string((const char*)(m_data + sizeof(Header)), GetSize() - sizeof(Header));
    }
}


NetFailedReportMsg::NetFailedReportMsg()
{
    m_type = E_MsgType::NetFailedReport_Type;
    m_strClientId = "";
}
NetFailedReportMsg::NetFailedReportMsg(const std::string& clientId)
{
    m_type = E_MsgType::KeepAliveReq_Type;
    m_strClientId = clientId;
}

std::string NetFailedReportMsg::ToString() const
{
    using namespace json11;
    json11::Json clientObj = json11::Json::object(
    {
        {"ClientID", m_strClientId}
    });
    return clientObj.dump();
}

bool NetFailedReportMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    json11::Json json = json11::Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["ClientID"].is_string())
    {
        m_strClientId = json["ClientID"].string_value();
    }
    else
    {
        return false;
    }
    if (m_strClientId.empty())
    {
        return false;
    }
    return true;
}

NetRecoverReportMsg::NetRecoverReportMsg()
{
    m_type = E_MsgType::NetRecoverReport_Type;
    m_strClientId = "";
}
NetRecoverReportMsg::NetRecoverReportMsg(const std::string& clientId)
{
    m_type = E_MsgType::KeepAliveReq_Type;
    m_strClientId = clientId;
}


std::string NetRecoverReportMsg::ToString() const
{
    using namespace json11;
    json11::Json clientObj = json11::Json::object(
    {
        {"ClientID", m_strClientId}
    });
    return clientObj.dump();
}

bool NetRecoverReportMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    json11::Json json = json11::Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["ClientID"].is_string())
    {
        m_strClientId = json["ClientID"].string_value();
    }
    else
    {
        return false;
    }
    if (m_strClientId.empty())
    {
        return false;
    }
    return true;
}

KeepAliveReqMsg::KeepAliveReqMsg()
{
    m_type = E_MsgType::KeepAliveReq_Type;
    m_strClientId= "";
}

KeepAliveReqMsg::KeepAliveReqMsg(const std::string& clientId)
{
    m_type = E_MsgType::KeepAliveReq_Type;
    m_strClientId=clientId;
}

std::string KeepAliveReqMsg::ToString() const
{
    using namespace json11;
    json11::Json clientObj = json11::Json::object(
    {
        {"ClientID", m_strClientId}
    });
    return clientObj.dump();
}

bool KeepAliveReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    json11::Json json = json11::Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["ClientID"].is_string())
    {
        m_strClientId = json["ClientID"].string_value();
    }
    else
    {
        return false;
    }
    if (m_strClientId.empty())
    {
        return false;
    }
    return true;
}


KeepAliveRspMsg::KeepAliveRspMsg()
{
    m_type = E_MsgType::KeepAliveRsp_Type;
    m_strClientId="";
}

KeepAliveRspMsg::KeepAliveRspMsg(const std::string& clientId)
{
    m_type = E_MsgType::KeepAliveRsp_Type;
    m_strClientId=clientId;
}


std::string KeepAliveRspMsg::ToString() const
{
    using namespace json11;
    json11::Json clientObj = json11::Json::object(
    {
        {"ClientID", m_strClientId}
    });
    return clientObj.dump();
}

bool KeepAliveRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    json11::Json json = json11::Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["ClientID"].is_string())
    {
        m_strClientId = json["ClientID"].string_value();
    }
    else
    {
        return false;
    }
    if (m_strClientId.empty())
    {
        return false;
    }
    return true;
}




UserLoginReqMsg::UserLoginReqMsg()
{
    m_type = E_MsgType::UserLoginReq_Type;
    m_strUserName.clear();
    m_strPassword.clear();
    m_eOsType = CLIENT_OS_TYPE::OS_TYPE_UNKNOWN;
    m_eOnlineType = CLIENT_ONLINE_TYPE::C_ONLINE_TYPE_OFFLINE;
    m_eNetType = CLIENT_NET_TYPE::C_NET_TYPE_UNKNOWN;
}

std::string UserLoginReqMsg::ToString() const
{
    using namespace json11;
    json11::Json clientObj = json11::Json::object(
    {
        {"UserName", m_strUserName},
        {"Password", m_strPassword},
        {"MsgId", m_strMsgId},
        {"OsType", static_cast<int>(m_eOsType)},
        {"NetType", static_cast<int>(m_eNetType)},
        {"OnlineType", static_cast<int>(m_eOnlineType)},
    });

    return clientObj.dump();
}

bool UserLoginReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    json11::Json json = json11::Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Password"].is_string())
    {
        m_strPassword = json["Password"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["OsType"].is_number())
    {
        m_eOsType = static_cast<CLIENT_OS_TYPE>(json["OsType"].int_value());
    }
    else
    {
        return false;
    }

    if (json["NetType"].is_number())
    {
        m_eNetType = static_cast<CLIENT_NET_TYPE>(json["NetType"].int_value());
    }
    else
    {
        return false;
    }

    if (json["OnlineType"].is_number())
    {
        m_eOnlineType = static_cast<CLIENT_ONLINE_TYPE>(json["OnlineType"].int_value());
    }
    else
    {
        return false;
    }

    return true;
}

bool UserLoginReqMsg::Valid() const
{
    if (m_strUserName.empty())
    {
        return false;
    }

    if (m_strPassword.empty())
    {
        return false;
    }

    if (m_strMsgId.empty())
    {
        return false;
    }

    return true;
}


UserLoginRspMsg::UserLoginRspMsg()
{
    m_eErrCode = ERROR_CODE_TYPE::E_CODE_LOGIN_FAILED;
    m_type = E_MsgType::UserLoginRsp_Type;
}

std::string UserLoginRspMsg::ToString() const
{
    using namespace json11;
    Json itemObj = Json::object(
    {
        {"FriendName", m_userInfo.m_strUserName},
        {"Signature", m_userInfo.m_strSignature},
        {"NickName", m_userInfo.m_strNickName},
        {"FaceId", m_userInfo.m_strFaceId},
        {"Address", m_userInfo.m_strAddress},
        {"BirthDate", m_userInfo.m_strBirthDate},
        {"Email", m_userInfo.m_strEmail},
        {"Gender", m_userInfo.m_strGender},
    });

    json11::Json clientObj = json11::Json::object(
    {
        {"UserId", m_strUserId},
        {"UserName", m_strUserName},
        {"code", static_cast<int>(m_eErrCode)},
        {"message", m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"Info", itemObj}
    });

    return clientObj.dump();
}

bool UserLoginRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }


    if (json["code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["message"].is_string())
    {
        m_strErrMsg = json["message"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Info"].is_object())
    {
        auto item = json["Info"];
        if (item["FriendName"].is_string())
        {
            m_userInfo.m_strUserName = item["FriendName"].string_value();
        }
        else
        {
            return false;
        }

        if (item["Signature"].is_string())
        {
            m_userInfo.m_strSignature = item["Signature"].string_value();
        }
        else
        {
            return false;
        }

        if (item["NickName"].is_string())
        {
            m_userInfo.m_strNickName = item["NickName"].string_value();
        }
        else
        {
            return false;
        }

        if (item["FaceId"].is_string())
        {
            m_userInfo.m_strFaceId = item["FaceId"].string_value();
        }
        else
        {
            return false;
        }

        if (item["Address"].is_string())
        {
            m_userInfo.m_strAddress = item["Address"].string_value();
        }
        else
        {
            return false;
        }

        if (item["BirthDate"].is_string())
        {
            m_userInfo.m_strBirthDate = item["BirthDate"].string_value();
        }
        else
        {
            return false;
        }

        if (item["Email"].is_string())
        {
            m_userInfo.m_strEmail = item["Email"].string_value();
        }
        else
        {
            return false;
        }

        if (item["Gender"].is_string())
        {
            m_userInfo.m_strGender = item["Gender"].string_value();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool UserLoginRspMsg::Valid() const
{
    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strUserName.empty())
    {
        return false;
    }
    return true;
}


UserLogoutReqMsg::UserLogoutReqMsg()
{
    m_type = E_MsgType::UserLogoutReq_Type;
    m_strUserName.clear();
    m_strPassword.clear();
    m_eOsType = CLIENT_OS_TYPE::OS_TYPE_UNKNOWN;
}

std::string UserLogoutReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"UserName", m_strUserName},
        {"Password", m_strPassword},
        {"MsgId", m_strMsgId},
        {"OsType", static_cast<int>(m_eOsType)}
    });

    return clientObj.dump();
}

bool UserLogoutReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Password"].is_string())
    {
        m_strPassword = json["Password"].string_value();
    }
    else
    {
        return false;
    }


    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["OsType"].is_number())
    {
        m_eOsType = static_cast<CLIENT_OS_TYPE>(json["OsType"].int_value());
    }
    else
    {
        return false;
    }
    return true;
}

bool UserLogoutReqMsg::Valid() const
{
    if (m_strUserName.empty())
    {
        return false;
    }

    if (m_strPassword.empty())
    {
        return false;
    }

    if (m_strMsgId.empty())
    {
        return false;
    }

    return true;
}






UserLogoutRspMsg::UserLogoutRspMsg()
{
    m_type = E_MsgType::UserLogoutRsp_Type;
}
std::string  UserLogoutRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserName", m_strUserName},
        {"code", static_cast<int>(m_eErrCode)},
        {"message", m_strErrMsg},
    });

    return clientObj.dump();
}

bool  UserLogoutRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["message"].is_string())
    {
        m_strErrMsg = json["message"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}

bool  UserLogoutRspMsg::Valid() const
{
    if (m_strUserName.empty())
    {
        return false;
    }

    if (m_strMsgId.empty())
    {
        return false;
    }

    return true;
}



UserRegisterReqMsg::UserRegisterReqMsg()
{
    m_type = E_MsgType::UserRegisterReq_Type;
}

std::string UserRegisterReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserName", m_strUserName},
        {"Password", m_strPassword},
        {"NickName", m_strNickName},
    });
    return clientObj.dump();
}

bool UserRegisterReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }

    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Password"].is_string())
    {
        m_strPassword = json["Password"].string_value();
    }
    else
    {
        return false;
    }

    if (json["NickName"].is_string())
    {
        m_strNickName = json["NickName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool UserRegisterReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserName.empty())
    {
        return false;
    }

    if (m_strPassword.empty())
    {
        return false;
    }

    if (m_strNickName.empty())
    {
        return false;
    }

    return true;
}



UserRegisterRspMsg::UserRegisterRspMsg()
{
    m_type = E_MsgType::UserRegisterRsp_Type;
    m_eErrCode = ERROR_CODE_TYPE::E_CODE_NO_SUCH_USER;
}

std::string UserRegisterRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserName", m_strUserName},
        {"code", static_cast<int>(m_eErrCode)},
        {"message", m_strErrMsg},
    });

    return clientObj.dump();
}

bool UserRegisterRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["message"].is_string())
    {
        m_strErrMsg = json["message"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}

bool UserRegisterRspMsg::Valid() const
{
    if (m_strUserName.empty())
    {
        return false;
    }

    if (m_strMsgId.empty())
    {
        return false;
    }
    return true;
}



UserUnRegisterReqMsg::UserUnRegisterReqMsg()
{
    m_type = E_MsgType::UserUnRegisterReq_Type;
}

std::string UserUnRegisterReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserName", m_strUserName},
        {"Password", m_strPassword},
        {"VerifyCode", m_strVerifyCode},
    });
    return clientObj.dump();
}

bool UserUnRegisterReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Password"].is_string())
    {
        m_strPassword = json["Password"].string_value();
    }
    else
    {
        return false;
    }

    if (json["VerifyCode"].is_string())
    {
        m_strVerifyCode = json["VerifyCode"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool UserUnRegisterReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserName.empty())
    {
        return false;
    }

    if (m_strPassword.empty())
    {
        return false;
    }

    if (m_strVerifyCode.empty())
    {
        return false;
    }

    return true;
}



UserUnRegisterRspMsg::UserUnRegisterRspMsg()
{
    m_type = E_MsgType::UserUnRegisterRsp_Type;
}

std::string UserUnRegisterRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"UserName", m_strUserName},
        {"MsgId", m_strMsgId},
        {"code", static_cast<int>(m_eErrCode)},
        {"message", m_strErrMsg},
    });

    return clientObj.dump();
}

bool UserUnRegisterRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["message"].is_string())
    {
        m_strErrMsg = json["message"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserName"].is_string())
    {
        m_strUserName = json["UserName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}

bool UserUnRegisterRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserName.empty())
    {
        return false;
    }

    return true;
}



FontInfo_s::FontInfo_s()
{
	m_strFontName = "微软雅黑";
	m_nFontSize = 14;
	m_nFontStyle = 0;
}

bool FontInfo_s::IsBold() const 
{
	return (m_nFontStyle&E_FONT_BOLD) != 0;
}

bool FontInfo_s::IsItalic() const
{
	return (m_nFontStyle&E_FONT_ITALIC) != 0;
}
bool FontInfo_s::IsUnderScore() const
{
	return (m_nFontStyle&E_FONT_UNDER_SCORE) != 0;
}


void FontInfo_s::SetBold()
{
	m_nFontStyle |= E_FONT_BOLD;
}
void FontInfo_s::SetItalic()
{
	m_nFontStyle |= E_FONT_ITALIC;
}
void FontInfo_s::SetUnderScore()
{
	m_nFontStyle |= E_FONT_UNDER_SCORE;
}

bool FontInfo_s::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["FontName"].is_string())
	{
		m_strFontName = json["FontName"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FontSize"].is_number())
	{
		m_nFontSize = json["FontSize"].int_value();
	}
	else
	{
		return false;
	}

	if (json["FontColor"].is_string())
	{
		m_strFontColorHex = json["FontColor"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FontStyle"].is_number())
	{
		m_nFontStyle = json["FontStyle"].int_value();
	}
	else
	{
		return false;
	}
	return true;
}

std::string FontInfo_s::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"FontName", m_strFontName},
			{"FontSize", m_nFontSize},
			{"FontColor", m_strFontColorHex},
			{"FontStyle", m_nFontStyle},
		});

	return clientObj.dump();
}


FriendChatSendTxtReqMsg::FriendChatSendTxtReqMsg()
{
    m_type = E_MsgType::FriendChatSendTxtMsgReq_Type;
}

std::string  FriendChatSendTxtReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"SenderId", m_strSenderId},
        {"ReceiverId", m_strReceiverId},
        {"Context", m_strContext},
		{"FontInfo",m_fontInfo.ToString()},
    });

    return clientObj.dump();
}

bool  FriendChatSendTxtReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["SenderId"].is_string())
    {
        m_strSenderId = json["SenderId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["ReceiverId"].is_string())
    {
        m_strReceiverId = json["ReceiverId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["Context"].is_string())
    {
        m_strContext = json["Context"].string_value();
    }
    else
    {
        return false;
    }

	if (json["FontInfo"].is_string()) {
		if (m_fontInfo.FromString(json["FontInfo"].string_value())) {

		}
		else
		{
			return false;
		}
	}

    return true;
}

bool  FriendChatSendTxtReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strSenderId.empty())
    {
        return false;
    }

    if (m_strReceiverId.empty())
    {
        return false;
    }

    if (m_strContext.empty())
    {
        return false;
    }

    return true;
}



FriendChatSendTxtRspMsg::FriendChatSendTxtRspMsg()
{
    m_type = E_MsgType::FriendChatSendTxtMsgRsp_Type;
}

std::string FriendChatSendTxtRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message", m_strErrMsg},
		{"ChatMsg",FriendChatMsg(m_chatMsg)},
    });

    return clientObj.dump();
}

bool FriendChatSendTxtRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }


    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }

	if (json["ChatMsg"].is_object())
	{
		FriendChatMsg(json["ChatMsg"], m_chatMsg);
	}
	else
	{
		return false;
	}

    return true;
}

bool FriendChatSendTxtRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }
    return true;
}


FriendChatRecvTxtReqMsg::FriendChatRecvTxtReqMsg()
{
    m_type = E_MsgType::FriendChatReceiveTxtMsgReq_Type;
}

std::string FriendChatRecvTxtReqMsg::ToString() const
{
    using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"ChatMsg",FriendChatMsg(m_chatMsg)},
    });

    return clientObj.dump();
}

bool FriendChatRecvTxtReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();;
    }
    else
    {
        return false;
    }

	if (json["ChatMsg"].is_object()) {
		FriendChatMsg(json["ChatMsg"], m_chatMsg);
	}
    return true;
}


bool FriendChatRecvTxtReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }
    return true;
}



FriendChatRecvTxtRspMsg::FriendChatRecvTxtRspMsg()
{
    m_type = E_MsgType::FriendChatReceiveTxtMsgRsp_Type;
}

std::string FriendChatRecvTxtRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
		{"ChatMsgId",m_strChatMsgId},
    });
    return clientObj.dump();
}

bool FriendChatRecvTxtRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

	if (json["ChatMsgId"].is_string()) {
		m_strChatMsgId = json["ChatMsgId"].string_value();
	}
	else {
		return false;
	}
    return true;
}

bool FriendChatRecvTxtRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }

    return true;
}






GetFriendListReqMsg::GetFriendListReqMsg()
{
    m_type = E_MsgType::GetFriendListReq_Type;
}

std::string GetFriendListReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
    });
    return clientObj.dump();
}

bool GetFriendListReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool GetFriendListReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    return true;
}



GetFriendListRspMsg::GetFriendListRspMsg()
{
    m_type = E_MsgType::GetFriendListRsp_Type;
}

std::string GetFriendListRspMsg::ToString() const
{
    using namespace json11;


    Json::array itemArray;
	Json::array teamArray;
    for (auto teamItem : m_teamVec)
    {
		for (const auto item : teamItem.m_teamUsers)
		{
			Json itemObj = Json::object(
				{
					{"UserId", item.m_strUserId},
					{"FriendName", item.m_strUserName},
					{"Signature", item.m_strSignature},
					{"NickName", item.m_strNickName},
					{"FaceId", item.m_strFaceId},
					{"Address", item.m_strAddress},
					{"BirthDate", item.m_strBirthDate},
					{"Email", item.m_strEmail},
					{"Gender", item.m_strGender},
					{"Online",OnLineType(item.m_eOnlineState)},
				});
			itemArray.push_back(itemObj);
		}
		Json teamObj = Json::object({
			{"TeamId",teamItem.m_strTeamId},
			{"TeamName",teamItem.m_strTeamName},
			{"TeamUserList",itemArray},
		});
		teamArray.push_back(teamObj);
		itemArray.clear();
    }

    Json clientObj = Json::object(
    {
		{"Code",static_cast<int>(m_errCode)},
		{"Message",m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"TeamList", teamArray},
    });
    return clientObj.dump();
}


bool GetFriendListRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
	if (json["Code"].is_number())
	{
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_strErrMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["TeamList"].is_array())
    {
        auto teamJson = json["TeamList"];
		TeamBaseInfo teamInfo;
        for (auto teamItem : teamJson.array_items())
        {
			teamInfo.m_teamUsers.clear();
			{
				
				if (teamItem["TeamId"].is_string())
				{
					teamInfo.m_strTeamId = teamItem["TeamId"].string_value();
				}
				else
				{
					return false;
				}

				if (teamItem["TeamName"].is_string())
				{
					teamInfo.m_strTeamName = teamItem["TeamName"].string_value();
				}
				else
				{
					return false;
				}

				if (teamItem["TeamUserList"].is_array())
				{
					for (const auto item : teamItem["TeamUserList"].array_items())
					{
						UserBaseInfo info;

						if (item["UserId"].is_string())
						{
							info.m_strUserId = item["UserId"].string_value();
						}
						else
						{
							return false;
						}

						if (item["FriendName"].is_string())
						{
							info.m_strUserName = item["FriendName"].string_value();
						}
						else
						{
							return false;
						}

						if (item["Signature"].is_string())
						{
							info.m_strSignature = item["Signature"].string_value();
						}
						else
						{
							return false;
						}


						if (item["NickName"].is_string())
						{
							info.m_strNickName = item["NickName"].string_value();
						}
						else
						{
							return false;
						}

						if (item["FaceId"].is_string())
						{
							info.m_strFaceId = item["FaceId"].string_value();
						}
						else
						{
							return false;
						}

						if (item["Address"].is_string())
						{
							info.m_strAddress = item["Address"].string_value();
						}
						else
						{
							return false;
						}

						if (item["BirthDate"].is_string())
						{
							info.m_strBirthDate = item["BirthDate"].string_value();
						}
						else
						{
							return false;
						}

						if (item["Email"].is_string())
						{
							info.m_strEmail = item["Email"].string_value();
						}
						else
						{
							return false;
						}

						if (item["Gender"].is_string())
						{
							info.m_strGender = item["Gender"].string_value();
						}
						else
						{
							return false;
						}

						if (item["Online"].is_string())
						{
							info.m_eOnlineState = OnLineType(item["Online"].string_value());
						}
						else
						{
							return false;
						}

						teamInfo.m_teamUsers.push_back(info);
					}
				}
			}
			m_teamVec.push_back(teamInfo);
        }
    }
    return true;
}

bool GetFriendListRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    return true;
}


AddFriendSendReqMsg::AddFriendSendReqMsg()
{
    m_type = E_MsgType::AddFriendSendReq_Type;
}

std::string AddFriendSendReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
    });

    return clientObj.dump();
}

bool AddFriendSendReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddFriendSendReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }

    return true;
}


AddFriendSendRspMsg::AddFriendSendRspMsg()
{
    m_type = E_MsgType::AddFriendSendRsp_Type;
}

std::string AddFriendSendRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"Code", static_cast<int>(m_eErrCode)},
        {"Message", m_strErrMsg},
    });

    return clientObj.dump();
}

bool AddFriendSendRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }


    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddFriendSendRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }

    return true;
}



AddFriendRecvReqMsg::AddFriendRecvReqMsg()
{
    m_type = E_MsgType::AddFriendRecvReq_Type;
}

std::string AddFriendRecvReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
    });

    return clientObj.dump();
}

bool AddFriendRecvReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddFriendRecvReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }

    return true;
}



AddFriendRecvRspMsg::AddFriendRecvRspMsg()
{
    m_type = E_MsgType::AddFriendRecvRsp_Type;
    m_option = E_FRIEND_OPTION::E_UN_KNOWN;
}

std::string AddFriendRecvRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
		{"Code",static_cast<int>(m_errCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"Option", static_cast<int>(m_option)},
    });

    return clientObj.dump();
}

bool AddFriendRecvRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

	if (json["Code"].is_number())
	{
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}
    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Option"].is_number())
    {
        m_option = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }
    return true;
}

bool AddFriendRecvRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }
    return true;
}


AddFriendNotifyReqMsg::AddFriendNotifyReqMsg()
{
    m_type = E_MsgType::AddFriendNotifyReq_Type;
}

std::string AddFriendNotifyReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"MsgID", m_strMsgId},
        {"Option", static_cast<int>(m_option)},
    });

    return clientObj.dump();
}

bool AddFriendNotifyReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgID"].is_string())
    {
        m_strMsgId = json["MsgID"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Option"].is_number())
    {
        m_option = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }
    return true;
}

bool AddFriendNotifyReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    return true;
}

AddFriendNotifyRspMsg::AddFriendNotifyRspMsg()
{
    m_type = E_MsgType::AddFriendNotifyRsp_Type;
}

std::string AddFriendNotifyRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgID", m_strMsgId},
    });

    return clientObj.dump();
}

bool AddFriendNotifyRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["MsgID"].is_string())
    {
        m_strMsgId = json["MsgID"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddFriendNotifyRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    return true;
}



RemoveFriendReqMsg::RemoveFriendReqMsg()
{
    m_type = E_MsgType::RemoveFriendReq_Type;
}

std::string RemoveFriendReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
    });

    return clientObj.dump();
}

bool RemoveFriendReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool RemoveFriendReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }

    return true;
}

RemoveFriendRspMsg::RemoveFriendRspMsg()
{
    m_type = E_MsgType::RemoveFriendRsp_Type;
}

std::string RemoveFriendRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"Code", static_cast<int>(m_eErrCode)},
        {"Message", m_strErrMsg},
    });

    return clientObj.dump();
}

bool RemoveFriendRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }


    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool RemoveFriendRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strFriendId.empty())
    {
        return false;
    }
    return true;
}


FindFriendReqMsg::FindFriendReqMsg()
{
    m_type = E_MsgType::FindFriendReq_Type;
}

std::string FindFriendReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"WantedName", m_strWantedName},
    });

    return clientObj.dump();
}

bool FindFriendReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["WantedName"].is_string())
    {
        m_strWantedName = json["WantedName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool FindFriendReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strWantedName.empty())
    {
        return false;
    }

    return true;
}



FindFriendRspMsg::FindFriendRspMsg()
{
    m_type = E_MsgType::FindFriendRsp_Type;
}

std::string FindFriendRspMsg::ToString() const
{
    using namespace json11;


    Json::array itemArray;
    for (auto item : m_friendInfoVec)
    {
        Json itemObj = Json::object(
        {
            {"FriendId", item.m_strUserId},
            {"FriendName", item.m_strUserName},
            {"Signature", item.m_strSignature},
            {"NickName", item.m_strNickName},
            {"FaceId", item.m_strFaceId},
            {"Address", item.m_strAddress},
            {"BirthDate", item.m_strBirthDate},
            {"Email", item.m_strEmail},
            {"Gender", item.m_strGender},
        });
        itemArray.push_back(itemObj);
    }

    Json clientObj = Json::object(
    {
        {"UserId", m_strUserId},
        {"MsgId", m_strMsgId},
        {"List", itemArray},
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
    });
    return clientObj.dump();
}


bool FindFriendRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["List"].is_array())
    {
        for (auto item : json["List"].array_items())
        {
            UserBaseInfo info;

            if (item["FriendId"].is_string())
            {
                info.m_strUserId = item["FriendId"].string_value();
            }
            else
            {
                return false;
            }

            if (item["FriendName"].is_string())
            {
                info.m_strUserName = item["FriendName"].string_value();
            }
            else
            {
                return false;
            }

            if (item["Signature"].is_string())
            {
                info.m_strSignature = item["Signature"].string_value();
            }
            else
            {
                return false;
            }


            if (item["NickName"].is_string())
            {
                info.m_strNickName = item["NickName"].string_value();
            }
            else
            {
                return false;
            }

            if (item["FaceId"].is_string())
            {
                info.m_strFaceId = item["FaceId"].string_value();
            }
            else
            {
                return false;
            }

            if (item["Address"].is_string())
            {
                info.m_strAddress = item["Address"].string_value();
            }
            else
            {
                return false;
            }

            if (item["BirthDate"].is_string())
            {
                info.m_strBirthDate = item["BirthDate"].string_value();
            }
            else
            {
                return false;
            }

            if (item["Email"].is_string())
            {
                info.m_strEmail = item["Email"].string_value();
            }
            else
            {
                return false;
            }

            if (item["Gender"].is_string())
            {
                info.m_strGender = item["Gender"].string_value();
            }
            else
            {
                return false;
            }

            m_friendInfoVec.push_back(info);
        }
    }
    return true;
}


AddTeamReqMsg::AddTeamReqMsg()
{
    m_type = E_MsgType::AddTeamReq_Type;
}

std::string AddTeamReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"TeamName", m_strTeamName},
    });

    return clientObj.dump();
}

bool AddTeamReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["TeamName"].is_string())
    {
        m_strTeamName = json["TeamName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


AddTeamRspMsg::AddTeamRspMsg()
{
    m_type = E_MsgType::AddTeamRsp_Type;
}

std::string AddTeamRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
        {"Message", m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"TeamId", m_strTeamId},
        {"UserId", m_strUserId},
        {"TeamName", m_strTeamName},
    });

    return clientObj.dump();
}

bool AddTeamRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["TeamId"].is_string())
    {
        m_strTeamId = json["TeamId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["TeamName"].is_string())
    {
        m_strTeamName = json["TeamName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}



RemoveTeamReqMsg::RemoveTeamReqMsg()
{
    m_type = E_MsgType::RemoveTeamReq_Type;
}

std::string RemoveTeamReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"TeamId", m_strTeamId},
        {"UserId", m_strUserId},
    });

    return clientObj.dump();
}

bool RemoveTeamReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }


    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["TeamId"].is_string())
    {
        m_strTeamId = json["TeamId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


RemoveTeamRspMsg::RemoveTeamRspMsg()
{
    m_type = E_MsgType::RemoveTeamRsp_Type;
}

std::string RemoveTeamRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrorCode)},
        {"Message", m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"TeamId", m_strTeamId},
        {"UserId", m_strUserId},
        {"TeamName", m_strTeamName},
    });

    return clientObj.dump();
}

bool RemoveTeamRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrorCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }


    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["TeamId"].is_string())
    {
        m_strTeamId = json["TeamId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["TeamName"].is_string())
    {
        m_strTeamName = json["TeamName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


MoveFriendToTeamReqMsg::MoveFriendToTeamReqMsg()
{
    m_type = E_MsgType::MoveFriendToTeamReq_Type;
}

std::string MoveFriendToTeamReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"SrcTeamId", m_strSrcTeamId},
        {"DstTeamId", m_strDstTeamId},

    });

    return clientObj.dump();
}

bool MoveFriendToTeamReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["SrcTeamId"].is_string())
    {
        m_strSrcTeamId = json["SrcTeamId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["DstTeamId"].is_string())
    {
        m_strDstTeamId = json["DstTeamId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}



MoveFriendToTeamRspMsg::MoveFriendToTeamRspMsg()
{
    m_type = E_MsgType::MoveFriendToTeamReq_Type;
}

std::string MoveFriendToTeamRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
        {"Message", m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"SrcTeamId", m_strSrcTeamId},
        {"DstTeamId", m_strDstTeamId},

    });

    return clientObj.dump();
}

bool MoveFriendToTeamRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["SrcTeamId"].is_string())
    {
        m_strSrcTeamId = json["SrcTeamId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["DstTeamId"].is_string())
    {
        m_strDstTeamId = json["DstTeamId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}





CreateGroupReqMsg::CreateGroupReqMsg()
{
    m_type = E_MsgType::CreateGroupReq_Type;
}

std::string CreateGroupReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupName", m_strGroupName},
    });

    return clientObj.dump();
}

bool CreateGroupReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupName"].is_string())
    {
        m_strGroupName = json["GroupName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool CreateGroupReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupName.empty())
    {
        return false;
    }

    return true;
}





CreateGroupRspMsg::CreateGroupRspMsg()
{
    m_type = E_MsgType::CreateGroupRsp_Type;
}

std::string CreateGroupRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
        {"Message", m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"GroupId", m_strGroupId},
        {"GroupName", m_strGroupName},
        {"UserId", m_strUserId},
    });

    return clientObj.dump();
}

bool CreateGroupRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupName"].is_string())
    {
        m_strGroupName = json["GroupName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool CreateGroupRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    return true;
}


DestroyGroupReqMsg::DestroyGroupReqMsg()
{
    m_type = E_MsgType::DestroyGroupReq_Type;
}

std::string DestroyGroupReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"GroupId", m_strGroupId},
        {"UserId", m_strUserId},
        {"GroupName", m_strGroupName},
    });

    return clientObj.dump();
}

bool DestroyGroupReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupName"].is_string())
    {
        m_strGroupName = json["GroupName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool DestroyGroupReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    if (m_strGroupName.empty())
    {
        return false;
    }

    return true;
}



DestroyGroupRspMsg::DestroyGroupRspMsg()
{
    m_type = E_MsgType::DestroyGroupRsp_Type;
}

std::string DestroyGroupRspMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrorCode)},
        {"Message", m_strErrMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
        {"GroupName", m_strGroupName},
    });

    return clientObj.dump();
}

bool DestroyGroupRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrorCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["Message"].is_string())
    {
        m_strErrMsg = json["Message"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupName"].is_string())
    {
        m_strGroupName = json["GroupName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


bool DestroyGroupRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;
}



FindGroupReqMsg::FindGroupReqMsg()
{
    m_type = E_MsgType::FindGroupReq_Type;
}

std::string FindGroupReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupName", m_strGroupName},
    });

    return clientObj.dump();
}

bool FindGroupReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupName"].is_string())
    {
        m_strGroupName = json["GroupName"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool FindGroupReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupName.empty())
    {
        return false;
    }

    return true;
}


FindGroupRspMsg::FindGroupRspMsg()
{
    m_type = E_MsgType::FindGroupRsp_Type;
}

std::string FindGroupRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
        {"GroupName", m_strGroupName},
        {"GroupOwner", m_strGroupOwner},
    });
    return clientObj.dump();
}


bool FindGroupRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupName"].is_string())
    {
        m_strGroupName = json["GroupName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupOwner"].is_string())
    {
        m_strGroupOwner = json["GroupOwner"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}

bool FindGroupRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    if (m_strGroupName.empty())
    {
        return false;
    }

    if (m_strGroupOwner.empty())
    {
        return false;
    }

    return true;
}



AddToGroupReqMsg::AddToGroupReqMsg()
{
    m_type = E_MsgType::AddToGroupReq_Type;
}

std::string AddToGroupReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
    });

    return clientObj.dump();
}

bool AddToGroupReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddToGroupReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;
}

AddToGroupRspMsg::AddToGroupRspMsg()
{
    m_type = E_MsgType::AddToGroupRsp_Type;
}

std::string AddToGroupRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
    });
    return clientObj.dump();
}


bool AddToGroupRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}

bool AddToGroupRspMsg::Valid() const
{

    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;
}



AddToGroupRecvReqMsg::AddToGroupRecvReqMsg()
{
    m_type = E_MsgType::AddToGroupRecvReq_Type;
}

std::string AddToGroupRecvReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Admin", m_strAdminId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
    });

    return clientObj.dump();
}

bool AddToGroupRecvReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Admin"].is_string())
    {
        m_strAdminId = json["Admin"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddToGroupRecvReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strAdminId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;
}




AddToGroupRecvRspMsg::AddToGroupRecvRspMsg()
{
    m_type = E_MsgType::AddToGroupRecvRsp_Type;
}

std::string  AddToGroupRecvRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"Option", static_cast<int>(m_eOption)},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
        {"Admin", m_strAdminId},
    });
    return clientObj.dump();
}


bool  AddToGroupRecvRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Admin"].is_string())
    {
        m_strAdminId = json["Admin"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool  AddToGroupRecvRspMsg::Valid() const
{
    if (m_strAdminId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;
}



AddToGroupNotifyReqMsg::AddToGroupNotifyReqMsg()
{
    m_type = E_MsgType::AddToGroupNotifyReq_Type;
}

std::string AddToGroupNotifyReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Option", static_cast<int>(m_eOption)},
        {"Admin", m_strAdminId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
    });

    return clientObj.dump();
}

bool AddToGroupNotifyReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }


    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Admin"].is_string())
    {
        m_strAdminId = json["Admin"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddToGroupNotifyReqMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;

}


AddToGroupNotifyRspMsg::AddToGroupNotifyRspMsg()
{
    m_type = E_MsgType::AddToGroupNotifyRsp_Type;
}

std::string AddToGroupNotifyRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"Option", static_cast<int>(m_eOption)},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupId", m_strGroupId},
        {"Admin", m_strAdminId},
    });
    return clientObj.dump();
}

bool AddToGroupNotifyRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }


	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}
    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Admin"].is_string())
    {
        m_strAdminId = json["Admin"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

bool AddToGroupNotifyRspMsg::Valid() const
{
    if (m_strMsgId.empty())
    {
        return false;
    }

    if (m_strAdminId.empty())
    {
        return false;
    }
    if (m_strUserId.empty())
    {
        return false;
    }

    if (m_strGroupId.empty())
    {
        return false;
    }

    return true;
}



InviteFriendToGroupReqMsg::InviteFriendToGroupReqMsg()
{
    m_type = E_MsgType::InviteFriendToGroupReq_Type;
}

std::string InviteFriendToGroupReqMsg::ToString() const
{
    using namespace json11;
    Json clientObj = Json::object(
    {
        {"Sender", m_strSenderId},
        {"Reciver", m_strReciverId},
        {"GroupId", m_strGroupId},
    });

    return clientObj.dump();
}

bool InviteFriendToGroupReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }


    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Reciver"].is_string())
    {
        m_strReciverId = json["Reciver"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


InviteFriendToGroupRspMsg::InviteFriendToGroupRspMsg()
{
    m_type = E_MsgType::InviteFriendToGroupRsp_Type;
}

std::string InviteFriendToGroupRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Reciver", m_strReciverId},
    });
    return clientObj.dump();
}


bool InviteFriendToGroupRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Reciver"].is_string())
    {
        m_strReciverId = json["Reciver"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}

InviteFriendToGroupRecvReqMsg::InviteFriendToGroupRecvReqMsg()
{
    m_type = E_MsgType::InviteToGroupRecvReq_Type;
}

std::string InviteFriendToGroupRecvReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Reciver", m_strReciverId},
    });
    return clientObj.dump();
}


bool InviteFriendToGroupRecvReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }



    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Reciver"].is_string())
    {
        m_strReciverId = json["Reciver"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


InviteFriendToGroupRecvRspMsg::InviteFriendToGroupRecvRspMsg()
{
    m_type = E_MsgType::InviteToGroupRecvRsp_Type;
}

std::string InviteFriendToGroupRecvRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
		{"Code",static_cast<int>(m_errCode)},
		{"Message",m_errMsg},
        {"Option", static_cast<int>(m_eOption)},
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Reciver", m_strReciverId},
    });
    return clientObj.dump();
}


bool InviteFriendToGroupRecvRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

	if (json["Code"].is_number())
	{
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}




    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }


    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Reciver"].is_string())
    {
        m_strReciverId = json["Reciver"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


InviteFriendToGroupNotifyReqMsg::InviteFriendToGroupNotifyReqMsg()
{
    m_type = E_MsgType::InviteResultNotifyReq_Type;
}

std::string InviteFriendToGroupNotifyReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Option", static_cast<int>(m_eOption)},
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Reciver", m_strReciverId},
    });
    return clientObj.dump();
}


bool InviteFriendToGroupNotifyReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }


    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Reciver"].is_string())
    {
        m_strReciverId = json["Reciver"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}




QuitFromGroupReqMsg::QuitFromGroupReqMsg()
{
	m_type = E_MsgType::QuitGroupReq_Type;
}

std::string QuitFromGroupReqMsg::ToString() const
{
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"GroupId", m_strGroupId},
		});
	return clientObj.dump();
}


bool QuitFromGroupReqMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}



	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["GroupId"].is_string())
	{
		m_strGroupId = json["GroupId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}


QuitFromGroupRspMsg::QuitFromGroupRspMsg()
{
	m_type = E_MsgType::QuitGroupRsp_Type;
}

std::string QuitFromGroupRspMsg::ToString() const
{
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"Code",static_cast<int>(m_errCode)},
			{"Message",m_errMsg},
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"GroupId", m_strGroupId},
		});
	return clientObj.dump();
}


bool QuitFromGroupRspMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["Code"].is_number())
	{
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}


	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["GroupId"].is_string())
	{
		m_strGroupId = json["GroupId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}


InviteFriendToGroupNotifyRspMsg::InviteFriendToGroupNotifyRspMsg()
{
    m_type = E_MsgType::InviteResultNotifyRsp_Type;
}

std::string InviteFriendToGroupNotifyRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Reciver", m_strReciverId},
    });
    return clientObj.dump();
}


bool InviteFriendToGroupNotifyRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }



    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Reciver"].is_string())
    {
        m_strReciverId = json["Reciver"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


SendGroupTextMsgReqMsg::SendGroupTextMsgReqMsg()
{
    m_type = E_MsgType::SendGroupTextMsgReq_Type;
}

std::string SendGroupTextMsgReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Context", m_strContext},
		{"FontInfo",m_stFontInfo.ToString()},
    });
    return clientObj.dump();
}


bool SendGroupTextMsgReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }


    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Context"].is_string())
    {
        m_strContext = json["Context"].string_value();
    }
    else
    {
        return false;
    }

	if (json["FontInfo"].is_string()) {
		if (m_stFontInfo.FromString(json["FontInfo"].string_value())) {
			return true;
		} 
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
    return true;
}


SendGroupTextMsgRspMsg::SendGroupTextMsgRspMsg()
{
    m_type = E_MsgType::SendGroupTextMsgRsp_Type;
}

std::string SendGroupTextMsgRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
		{"MsgTime",m_strMsgTime},
		{"Context", m_strContext},
		{"FontInfo",m_fontInfo.ToString()},
    });
    return clientObj.dump();
}


bool SendGroupTextMsgRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["Code"].is_number())
    {
        m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
    }
    else
    {
        return false;
    }

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

	if (json["MsgTime"].is_string())
	{
		m_strMsgTime = json["MsgTime"].string_value();
	}
	else
	{
		return false;
	}

	if (json["Context"].is_string())
	{
		m_strContext = json["Context"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FontInfo"].is_string())
	{
		if (m_fontInfo.FromString(json["FontInfo"].string_value()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
    return true;
}


RecvGroupTextMsgReqMsg::RecvGroupTextMsgReqMsg()
{
    m_type = E_MsgType::RecvGroupTextMsgReq_Type;
}

std::string RecvGroupTextMsgReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
		{"UserId",m_strUserId},
        {"Sender", m_strSenderId},
        {"GroupId", m_strGroupId},
        {"Context", m_strContext},
		{"FontInfo",m_stFontInfo.ToString()},
		{"MsgTime",m_strMsgTime},
    });
    return clientObj.dump();
}


bool RecvGroupTextMsgReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }



    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Context"].is_string())
    {
        m_strContext = json["Context"].string_value();
    }
    else
    {
        return false;
    }

	if (json["MsgTime"].is_string())
	{
		m_strMsgTime = json["MsgTime"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FontInfo"].is_string())
	{
		if (m_stFontInfo.FromString(json["FontInfo"].string_value()))
		{
			return true;
		}
		else {
			return false;
		}
	}
	else
	{
		return false;
	}
    return true;
}



RecvGroupTextMsgRspMsg::RecvGroupTextMsgRspMsg()
{
    m_type = E_MsgType::RecvGroupTextMsgRsp_Type;
}

std::string RecvGroupTextMsgRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"Sender", m_strSenderId},
		{"UserId",m_strUserId},
        {"GroupId", m_strGroupId},
    });
    return clientObj.dump();
}


bool RecvGroupTextMsgRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }



    if (json["Sender"].is_string())
    {
        m_strSenderId = json["Sender"].string_value();
    }
    else
    {
        return false;
    }

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["GroupId"].is_string())
    {
        m_strGroupId = json["GroupId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}



FriendSendFileMsgReqMsg::FriendSendFileMsgReqMsg()
{
    m_type = E_MsgType::FriendSendFileMsgReq_Type;
	m_transMode = FILE_TRANS_TYPE::TCP_OFFLINE_MODE;
}

std::string FriendSendFileMsgReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileName", m_strFileName},
		{"TransMode",static_cast<int>(m_transMode)},
    });
    return clientObj.dump();
}


bool FriendSendFileMsgReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileName"].is_string())
    {
        m_strFileName = json["FileName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }


	if (json["TransMode"].is_number())
	{
		m_transMode = static_cast<FILE_TRANS_TYPE>(json["TransMode"].int_value());
	}
	else
	{
		m_transMode = FILE_TRANS_TYPE::TCP_OFFLINE_MODE;
	}

    return true;
}

FriendSendFileMsgRspMsg::FriendSendFileMsgRspMsg()
{
    m_type = E_MsgType::FriendSendFileMsgRsp_Type;
	m_transMode = FILE_TRANS_TYPE::TCP_OFFLINE_MODE;
}

std::string FriendSendFileMsgRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
		{"Code",static_cast<int>(m_eErrCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileName", m_strFileName},
		{"TransMode",static_cast<int>(m_transMode)},
    });
    return clientObj.dump();
}


bool FriendSendFileMsgRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

	if (json["Code"].is_number())
	{
		m_eErrCode = static_cast<ERROR_CODE_TYPE>(m_eErrCode);
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FileName"].is_string())
    {
        m_strFileName = json["FileName"].string_value();
    }
    else
    {
        return false;
    }

	if (json["TransMode"].is_number())
	{
		m_transMode = static_cast<FILE_TRANS_TYPE>(json["TransMode"].int_value());
	}
	else
	{
		return false;
	}

    return true;
}



FriendRecvFileMsgReqMsg::FriendRecvFileMsgReqMsg()
{
    m_type = E_MsgType::FriendRecvFileMsgReq_Type;
	m_transMode = FILE_TRANS_TYPE::TCP_OFFLINE_MODE;
}

std::string FriendRecvFileMsgReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileName", m_strFileName},
		{"TransMode",static_cast<int>(m_transMode)},

    });
    return clientObj.dump();
}


bool FriendRecvFileMsgReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
		m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileName"].is_string())
    {
        m_strFileName = json["FileName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

	if (json["TransMode"].is_number())
	{
		m_transMode = static_cast<FILE_TRANS_TYPE>(json["TransMode"].int_value());
	}
	else
	{
		return false;
	}
    return true;
}


FriendRecvFileMsgRspMsg::FriendRecvFileMsgRspMsg()
{
    m_type = E_MsgType::FriendRecvFileMsgRsp_Type;
	m_transMode = FILE_TRANS_TYPE::TCP_OFFLINE_MODE;
}

std::string FriendRecvFileMsgRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
	{
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileName", m_strFileName},
        {"FileId", m_nFileId},
        {"Option", static_cast<int>(m_eOption)},
		{"TransMode",static_cast<int>(m_transMode)},

    });
    return clientObj.dump();
}


bool FriendRecvFileMsgRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }


    if (json["FileName"].is_string())
    {
        m_strFileName = json["FileName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }


    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }

	if (json["TransMode"].is_number())
	{
		m_transMode = static_cast<FILE_TRANS_TYPE>(json["TransMode"].int_value());
	}
	else
	{
		return false;
	}

    return true;
}



FriendNotifyFileMsgReqMsg::FriendNotifyFileMsgReqMsg()
{
    m_type = E_MsgType::FriendNotifyFileMsgReq_Type;
}

std::string FriendNotifyFileMsgReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileName", m_strFileName},
        {"FileId", m_nFileId},
        {"Option", static_cast<int>(m_eOption)},
		{"TransMode",static_cast<int>(m_transMode)},

    });
    return clientObj.dump();
}


bool FriendNotifyFileMsgReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileName"].is_string())
    {
        m_strFileName = json["FileName"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["Option"].is_number())
    {
        m_eOption = static_cast<E_FRIEND_OPTION>(json["Option"].int_value());
    }
    else
    {
        return false;
    }

	if (json["TransMode"].is_number())
	{
		m_transMode = static_cast<FILE_TRANS_TYPE>(json["TransMode"].int_value());
	}
	else
	{
		return false;
	}

    return true;
}



FriendNotifyFileMsgRspMsg::FriendNotifyFileMsgRspMsg()
{
    m_type = E_MsgType::FriendNotifyFileMsgRsp_Type;
}

std::string FriendNotifyFileMsgRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
    });
    return clientObj.dump();
}


bool FriendNotifyFileMsgRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    return true;
}


GetGroupListReqMsg::GetGroupListReqMsg()
{
    m_type = E_MsgType::GetGroupListReq_Type;
}

std::string GetGroupListReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
    });
    return clientObj.dump();
}


bool GetGroupListReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    return true;
}

GetGroupListRspMsg::GetGroupListRspMsg()
{
    m_type = E_MsgType::GetGroupListRsp_Type;
}

std::string GetGroupListRspMsg::ToString() const
{
    using namespace json11;



    Json::array groupListArray;
    //Loop Group
    {
        for (auto groupItem : m_GroupList)
            //Loop BaseInfo
        {
            Json::array memList;
            for (auto userItem : groupItem.m_GroupUsers)
            {
                Json itemObj = Json::object(
                {
                    {"FriendName", userItem.m_strUserName},
                    {"Signature", userItem.m_strSignature},
                    {"NickName", userItem.m_strNickName},
                    {"FaceId", userItem.m_strFaceId},
                    {"Address", userItem.m_strAddress},
                    {"BirthDate", userItem.m_strBirthDate},
                    {"Email", userItem.m_strEmail},
                    {"Gender", userItem.m_strGender},
					{"UserId",userItem.m_strUserId},
                });
                memList.push_back(itemObj);
            }
            auto groupJson = Json::object(
            {
                {"GroupId", groupItem.m_strGroupId},
                {"GroupName", groupItem.m_strGroupName},
                {"GroupImage", groupItem.m_strGroupImage},
                {"UserList", memList},
            });
            groupListArray.push_back(groupJson);
        }
    }
    Json clientObj = Json::object(
    {
		{"Code",static_cast<int>(m_errCode)},
		{"Message",m_errMsg},
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"GroupList", groupListArray},
    });
    return clientObj.dump();
}


bool GetGroupListRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

	if (json["Code"].is_number())
	{
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }

    auto groupListJson = json["GroupList"];
    if (groupListJson.is_array())
    {
        for (auto groupItem : groupListJson.array_items())
        {

            GroupInfo groupInfo;
            //Parse Group
            if (groupItem["GroupId"].is_string())
            {
                groupInfo.m_strGroupId = groupItem["GroupId"].string_value();
            }
            else
            {
                return false;
            }

            if (groupItem["GroupName"].is_string())
            {
                groupInfo.m_strGroupName = groupItem["GroupName"].string_value();
            }
            else
            {
                return false;
            }

            if (groupItem["GroupImage"].is_string())
            {
                groupInfo.m_strGroupImage = groupItem["GroupImage"].string_value();
            }
            else
            {
                return false;
            }

            if (groupItem["UserList"].is_array())
            {
                auto memListJson = groupItem["UserList"].array_items();
                for (auto memItem : memListJson)
                {
                    UserBaseInfo info;
                    if (memItem["FriendName"].is_string())
                    {
                        info.m_strUserName = memItem["FriendName"].string_value();
                    }
                    else
                    {
                        return false;
                    }

                    if (memItem["Signature"].is_string())
                    {
                        info.m_strSignature = memItem["Signature"].string_value();
                    }
                    else
                    {
                        return false;
                    }


                    if (memItem["NickName"].is_string())
                    {
                        info.m_strNickName = memItem["NickName"].string_value();
                    }
                    else
                    {
                        return false;
                    }

                    if (memItem["FaceId"].is_string())
                    {
                        info.m_strFaceId = memItem["FaceId"].string_value();
                    }
                    else
                    {
                        return false;
                    }

                    if (memItem["Address"].is_string())
                    {
                        info.m_strAddress = memItem["Address"].string_value();
                    }
                    else
                    {
                        return false;
                    }

                    if (memItem["BirthDate"].is_string())
                    {
                        info.m_strBirthDate = memItem["BirthDate"].string_value();
                    }
                    else
                    {
                        return false;
                    }

                    if (memItem["Email"].is_string())
                    {
                        info.m_strEmail = memItem["Email"].string_value();
                    }
                    else
                    {
                        return false;
                    }

                    if (memItem["Gender"].is_string())
                    {
                        info.m_strGender = memItem["Gender"].string_value();
                    }
                    else
                    {
                        return false;
                    }

					if (memItem["UserId"].is_string())
					{
						info.m_strUserId = memItem["UserId"].string_value();
					}
					else
					{
						return false;
					}
                    groupInfo.m_GroupUsers.push_back(info);
                }
            }
            else
            {
                return false;
            }

            m_GroupList.push_back(groupInfo);
        }
    }
    else
    {
        return false;
    }
    return true;
}


FileDataSendReqMsg::FileDataSendReqMsg()
{
    m_type = E_MsgType::FileSendDataReq_Type;
}

std::string FileDataSendReqMsg::ToString() const
{
    using namespace json11;
    std::string strData(m_szData, m_nDataLength);
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileId", m_nFileId},
        {"DataTotalCount", m_nDataTotalCount},
        {"DataIndex", m_nDataIndex},
        {"DataLength", m_nDataLength},
        {"Data", StringToHex(strData)},
    });
    return clientObj.dump();
}
bool FileDataSendReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataTotalCount"].is_number())
    {
        m_nDataTotalCount = json["DataTotalCount"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataIndex"].is_number())
    {
        m_nDataIndex = json["DataIndex"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataLength"].is_number())
    {
        m_nDataLength = json["DataLength"].int_value();
    }
    else
    {
        return false;
    }

    if (json["Data"].is_string())
    {
        std::string strHex = json["Data"].string_value();
        std::string strData = HexToString(strHex);
        if (strData.length() <= 1024)
        {
            memcpy(m_szData, strData.data(), strData.length());
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}


FileDataSendRspMsg::FileDataSendRspMsg()
{
    m_type = E_MsgType::FileSendDataRsp_Type;
}

std::string FileDataSendRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileId", m_nFileId},
        {"DataTotalCount", m_nDataTotalCount},
        {"DataIndex", m_nDataIndex},
    });
    return clientObj.dump();
}


bool FileDataSendRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataTotalCount"].is_number())
    {
        m_nDataTotalCount = json["DataTotalCount"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataIndex"].is_number())
    {
        m_nDataIndex = json["DataIndex"].int_value();
    }
    else
    {
        return false;
    }

    return true;
}


FileDataRecvReqMsg::FileDataRecvReqMsg()
{
    m_type = E_MsgType::FileRecvDataReq_Type;
}

std::string FileDataRecvReqMsg::ToString() const
{
    using namespace json11;
    std::string strData(m_szData, m_nDataLength);
    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileId", m_nFileId},
        {"DataTotalCount", m_nDataTotalCount},
        {"DataIndex", m_nDataIndex},
        {"DataLength", m_nDataLength},
        {"Data", StringToHex(strData)},
    });
    return clientObj.dump();
}
bool FileDataRecvReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataTotalCount"].is_number())
    {
        m_nDataTotalCount = json["DataTotalCount"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataIndex"].is_number())
    {
        m_nDataIndex = json["DataIndex"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataLength"].is_number())
    {
        m_nDataLength = json["DataLength"].int_value();
    }
    else
    {
        return false;
    }
    if (json["Data"].is_string())
    {
        std::string strData = HexToString(json["Data"].string_value());
        if (strData.length() <= 1024)
        {
            memcpy(m_szData, strData.data(), strData.length());
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}


FileDataRecvRspMsg::FileDataRecvRspMsg()
{
    m_type = E_MsgType::FileRecvDataRsp_Type;
}

std::string FileDataRecvRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileId", m_nFileId},
        {"DataTotalCount", m_nDataTotalCount},
        {"DataIndex", m_nDataIndex},
    });
    return clientObj.dump();
}


bool FileDataRecvRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataTotalCount"].is_number())
    {
        m_nDataTotalCount = json["DataTotalCount"].int_value();
    }
    else
    {
        return false;
    }

    if (json["DataIndex"].is_number())
    {
        m_nDataIndex = json["DataIndex"].int_value();
    }
    else
    {
        return false;
    }

    return true;
}



FileVerifyReqMsg::FileVerifyReqMsg()
{
    m_type = E_MsgType::FileVerifyReq_Type;
}

std::string FileVerifyReqMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"MsgId", m_strMsgId},
        {"UserId", m_strUserId},
        {"FriendId", m_strFriendId},
        {"FileId", m_nFileId},
        {"FileName", m_strFileName},
        {"FileSize", m_nFileSize},
        {"FileHash", m_strFileHash},
    });
    return clientObj.dump();
}


bool FileVerifyReqMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }

    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

    if (json["FileSize"].is_number())
    {
        m_nFileSize = json["FileSize"].int_value();
    }
    else
    {
        return false;
    }

    if (json["FileHash"].is_string())
    {
        m_strFileHash = json["FileHash"].string_value();
    }
    else
    {
        return false;
    }

	if (json["FileName"].is_string())
	{
		m_strFileName = json["FileName"].string_value();
	}
	else
	{
		return false;
	}

    return true;
}


FileVerifyRspMsg::FileVerifyRspMsg()
{
    m_type = E_MsgType::FileVerifyRsp_Type;
}

std::string FileVerifyRspMsg::ToString() const
{
    using namespace json11;

    Json clientObj = Json::object(
    {
        {"Code", static_cast<int>(m_eErrCode)},
        {"MsgId", m_strMsgId},
        {"UserId",m_strMsgId},
        {"FriendId",m_strFriendId},
        {"FileId", m_nFileId},
        {"FileName", m_strFileName},
		{"FileHash",m_strFileHash},
    });
    return clientObj.dump();
}


bool FileVerifyRspMsg::FromString(const std::string &strJson)
{
    std::string err;
    using namespace json11;
    auto json = Json::parse(strJson, err);
    if (!err.empty())
    {
        return false;
    }
	if (json["Code"].is_number()) {
		m_eErrCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else {
		return false;
	}
    if (json["MsgId"].is_string())
    {
        m_strMsgId = json["MsgId"].string_value();
    }
    else
    {
        return false;
    }
    if (json["UserId"].is_string())
    {
        m_strUserId = json["UserId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FriendId"].is_string())
    {
        m_strFriendId = json["FriendId"].string_value();
    }
    else
    {
        return false;
    }

    if (json["FileId"].is_number())
    {
        m_nFileId = json["FileId"].int_value();
    }
    else
    {
        return false;
    }

	if (json["FileHash"].is_string()) {
		m_strFileHash = json["FileHash"].string_value();
	}
	else {
		return false;
	}
    return true;
}

UserKickOffReqMsg::UserKickOffReqMsg()
{
	m_type = E_MsgType::UserKickOffReq_Type;
}

std::string UserKickOffReqMsg::ToString() const
{
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"NewLoginIp",m_strNewLoginIp},
		});

	return clientObj.dump();
}

bool UserKickOffReqMsg::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["NewLoginIp"].is_string())
	{
		m_strNewLoginIp = json["NewLoginIp"].string_value();
	}
	else
	{
		return false;
	}

	return true;
}

UserKickOffRspMsg::UserKickOffRspMsg()
{
	m_type = E_MsgType::UserKickOffRsp_Type;
}

std::string UserKickOffRspMsg::ToString() const
{
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool UserKickOffRspMsg::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

FriendUnReadNotifyReqMsg::FriendUnReadNotifyReqMsg()
{
	m_type = E_MsgType::FriendUnReadMsgNotifyReq_Type;
}

std::string FriendUnReadNotifyReqMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool FriendUnReadNotifyReqMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

FriendUnReadNotifyRspMsg::FriendUnReadNotifyRspMsg()
{
	m_type = E_MsgType::FriendUnReadMsgNotifyRsp_Type;
}

std::string FriendUnReadNotifyRspMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool FriendUnReadNotifyRspMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

UpdateFriendListNotifyReqMsg::UpdateFriendListNotifyReqMsg()
{
	m_type = E_MsgType::UpdateFriendListNotifyReq_Type;
}

std::string UpdateFriendListNotifyReqMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool UpdateFriendListNotifyReqMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

UpdateFriendListNotifyRspMsg::UpdateFriendListNotifyRspMsg()
{
	m_type = E_MsgType::UpdateFriendListNotifyRsp_Type;
}

std::string UpdateFriendListNotifyRspMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool UpdateFriendListNotifyRspMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}


UpdateGroupListNotifyReqMsg::UpdateGroupListNotifyReqMsg()
{
	m_type = E_MsgType::UpdateGroupListNotifyReq_Type;
}

std::string UpdateGroupListNotifyReqMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool UpdateGroupListNotifyReqMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

UpdateGroupListNotifyRspMsg::UpdateGroupListNotifyRspMsg()
{
	m_type = E_MsgType::UpdateGroupListNotifyRsp_Type;
}

std::string UpdateGroupListNotifyRspMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool UpdateGroupListNotifyRspMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}



QueryUserUdpAddrReqMsg::QueryUserUdpAddrReqMsg()
{
	m_type = E_MsgType::QueryUserUdpAddrReq_Type;
}

std::string QueryUserUdpAddrReqMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"UdpUserId",m_strUdpUserId},
		});

	return clientObj.dump();
}

bool QueryUserUdpAddrReqMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UdpUserId"].is_string())
	{
		m_strUdpUserId = json["UdpUserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}




QueryUserUdpAddrRspMsg::QueryUserUdpAddrRspMsg()
{
	m_type = E_MsgType::QueryUserUdpAddrRsp_Type;
}

std::string QueryUserUdpAddrRspMsg::ToString() const {
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"Code",static_cast<int>(m_errCode)},
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"UdpUserId",m_strUdpUserId},
			{"UdpIp",m_udpEndPt.m_strServerIp},
			{"UdpPort",m_udpEndPt.m_nPort},
		});

	return clientObj.dump();
}

bool QueryUserUdpAddrRspMsg::FromString(const std::string& strJson) {
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["Code"].is_number()) {
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	
	if (json["UdpUserId"].is_string())
	{
		m_strUdpUserId = json["UdpUserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UdpIp"].is_string())
	{
		m_udpEndPt.m_strServerIp = json["UdpIp"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UdpPort"].is_number())
	{
		m_udpEndPt.m_nPort = json["UdpPort"].int_value();
	}
	else
	{
		return false;
	}
	return true;
}

NormalRspMsg::NormalRspMsg(): NormalRspMsg(ERROR_CODE_TYPE::E_CODE_SUCCEED)
{
}

NormalRspMsg::NormalRspMsg(const ERROR_CODE_TYPE type)
{
	m_errCode = type;
	m_errMsg = ErrMsg(m_errCode);

}


std::string NormalRspMsg::ToString() const
{
	using namespace json11;

	Json clientObj = Json::object(
		{
			{"Code",static_cast<int>(m_errCode)},
			{"Message",m_errMsg},
		});

	return clientObj.dump();
}

bool NormalRspMsg::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["Code"].is_number()) {
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string())
	{
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

GetFriendChatHistoryReq::GetFriendChatHistoryReq()
{
	m_type = E_MsgType::GetFriendChatHistroyReq_Type;
}

std::string GetFriendChatHistoryReq::ToString() const
{
	using namespace json11;
	Json msgJson = Json::object(
		{
			{"MsgId",m_strMsgId},
			{"UserId",m_strUserId},
			{"FriendId",m_strFriendId},
			{"ChatMsgId",m_strChatMsgId},
			{"HistoryDirection",static_cast<int>(m_eDirection)},
		});
	return msgJson.dump();
}

bool GetFriendChatHistoryReq::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string())
	{
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["ChatMsgId"].is_string()) {
		m_strChatMsgId = json["ChatMsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["HistoryDirection"].is_number()) {
		m_eDirection = static_cast<HISTORY_DIRECTION>(json["HistoryDirection"].int_value());
	}
	else {
		return false;
	}
	return true;
}

GetFriendChatHistoryRsp::GetFriendChatHistoryRsp()
{
	m_type = E_MsgType::GetFriendChatHistoryRsp_Type;
}

std::string GetFriendChatHistoryRsp::ToString() const
{
	using namespace json11;
	Json::array msgArray;
	for (const auto item : m_msgHistory)
	{
		msgArray.push_back(FriendChatMsg(item));
	}
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"FriendId",m_strFriendId},
		{"MsgHistory",msgArray},
	});
	return msgJson.dump();
}

bool GetFriendChatHistoryRsp::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string())
	{
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgHistory"].is_array())
	{
		auto jsonArray = json["MsgHistory"].array_items();
			
		FriendChatMsg_s rspMsg;
		for (auto itemJson : jsonArray)
		{
			if (FriendChatMsg(itemJson, rspMsg))
			{
				m_msgHistory.push_back(rspMsg);
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

GetGroupChatHistoryReq::GetGroupChatHistoryReq()
{
	m_type = E_MsgType::GetGroupChatHistoryReq_Type;
}

std::string GetGroupChatHistoryReq::ToString() const
{
	using namespace json11;
	Json rspJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"GroupId",m_strGroupId},
		{"ChatMsgId",m_strChatMsgId},
		{"HistoryDirection",static_cast<int>(m_eDirection)},
		});
	return rspJson.dump();
}

bool GetGroupChatHistoryReq::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["GroupId"].is_string())
	{
		m_strGroupId = json["GroupId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["ChatMsgId"].is_string()) {
		m_strChatMsgId = json["ChatMsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["HistoryDirection"].is_number()) {
		m_eDirection = static_cast<HISTORY_DIRECTION>(json["HistoryDirection"].int_value());
	}
	else {
		return false;
	}
	return true;
}

GetGroupChatHistoryRsp::GetGroupChatHistoryRsp()
{
	m_type = E_MsgType::GetGroupChatHistoryRsp_Type;
}

std::string GetGroupChatHistoryRsp::ToString() const
{
	using namespace json11;
	Json::array msgArray;
	for (const auto item : m_msgHistory)
	{
		Json itemJson = Json::object({
			{"MsgId",item.m_strMsgId},
			{"Content",item.m_strContext},
			{"FontInfo",item.m_fontInfo.ToString()},
			{"MsgTime",item.m_strMsgTime},
			{"SenderId",item.m_strSenderId},
			});
		msgArray.push_back(itemJson);
	}
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"GroupId",m_strGroupId},
		{"MsgHistory",msgArray},
		});
	return msgJson.dump();
}

bool GetGroupChatHistoryRsp::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["GroupId"].is_string())
	{
		m_strGroupId = json["GroupId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgHistory"].is_array())
	{
		auto jsonArray = json["MsgHistory"].array_items();

		SendGroupTextMsgRspMsg rspMsg;
		for (auto itemJson : jsonArray)
		{
			if (itemJson["MsgId"].is_string()) {
				rspMsg.m_strMsgId = itemJson["MsgId"].string_value();
			}
			else
			{
				break;
			}

			if (itemJson["Content"].is_string()) {
				rspMsg.m_strContext = itemJson["Content"].string_value();
			}
			else {
				break;
			}

			if (itemJson["FontInfo"].is_string()) {
				if (rspMsg.m_fontInfo.FromString(itemJson["FontInfo"].string_value())) {
				}
				else {
					break;
				}
			}
			else {
				break;
			}

			if (itemJson["MsgTime"].is_string()) {
				rspMsg.m_strMsgTime = itemJson["MsgTime"].string_value();
			}
			else
			{
				break;
			}

			if (itemJson["SenderId"].is_string()) {
				rspMsg.m_strSenderId = itemJson["SenderId"].string_value();
			}
			else {
				break;
			}

			m_msgHistory.push_back(rspMsg);
		}
	}
	return true;
}


SearchChatHistoryReq::SearchChatHistoryReq()
{
	m_type = E_MsgType::SearchChatMsgReq_Type;
}

std::string SearchChatHistoryReq::ToString() const
{
	using namespace json11;
	Json reqJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"SearchWord",m_strSearchWord},
	});
	return reqJson.dump();
}

bool SearchChatHistoryReq::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["SearchWord"].is_string()) {
		m_strSearchWord = json["SearchWord"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

SearchChatHistoryRsp::SearchChatHistoryRsp()
{
	m_type = E_MsgType::SearchChatMsgRsp_Type;
}

std::string SearchChatHistoryRsp::ToString() const
{
	using namespace json11;
	Json::array friendMsgJsonArray;
	for (const auto item : m_friendChatMsgVec)
	{
		friendMsgJsonArray.push_back(FriendChatMsg(item));
	}

	Json::array groupMsgJsonArray;
	{
		for (const auto item : m_groupChatMsgVec)
		{
			Json itemJson = Json::object({
				{"MsgId",item.m_strMsgId},
				{"Content",item.m_strContext},
				{"FontInfo",item.m_fontInfo.ToString()},
				{"MsgTime",item.m_strMsgTime},
				{"SenderId",item.m_strSenderId},
				{"GroupId",item.m_strGroupId},
				});

			groupMsgJsonArray.push_back(itemJson);
		}
	}
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"FriendMsgArray",friendMsgJsonArray},
		{"GroupMsgArray",groupMsgJsonArray},
		});
	return msgJson.dump();
}

bool SearchChatHistoryRsp::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["GroupMsgArray"].is_array())
	{
		auto jsonArray = json["GroupMsgArray"].array_items();

		SendGroupTextMsgRspMsg rspMsg;
		for (auto itemJson : jsonArray)
		{
			if (itemJson["MsgId"].is_string()) {
				rspMsg.m_strMsgId = itemJson["MsgId"].string_value();
			}
			else
			{
				break;
			}

			if (itemJson["Content"].is_string()) {
				rspMsg.m_strContext = itemJson["Content"].string_value();
			}
			else {
				break;
			}

			if (itemJson["FontInfo"].is_string()) {
				if (rspMsg.m_fontInfo.FromString(itemJson["FontInfo"].string_value())) {
				}
				else {
					break;
				}
			}
			else {
				break;
			}

			if (itemJson["MsgTime"].is_string()) {
				rspMsg.m_strMsgTime = itemJson["MsgTime"].string_value();
			}
			else
			{
				break;
			}

			if (itemJson["SenderId"].is_string()) {
				rspMsg.m_strSenderId = itemJson["SenderId"].string_value();
			}
			else {
				break;
			}

			if (itemJson["GroupId"].is_string()) {
				rspMsg.m_strGroupId = itemJson["GroupId"].string_value();
			}
			else
			{
				break;
			}

			m_groupChatMsgVec.push_back(rspMsg);
		}
	}

	if (json["FriendMsgArray"].is_array())
	{
		auto jsonArray = json["FriendMsgArray"].array_items();

		FriendChatMsg_s rspMsg;
		for (auto itemJson : jsonArray)
		{
			if (FriendChatMsg(itemJson, rspMsg))
			{
				m_friendChatMsgVec.push_back(rspMsg);
			}
		}
	}

	return true;
}
//GetFriendChatHistoryReq::GetFriendChatHistoryReq()
//{
//
//}
//
//std::string GetFriendChatHistoryReq::ToString() const
//{
//	return "";
//}
//
//bool GetFriendChatHistoryReq::FromString(const std::string& strJson)
//{
//	return false;
//}


AsyncFriendChatMsgReq::AsyncFriendChatMsgReq()
{
	m_type = E_MsgType::AsyncFriendChatMsgReq_Type;
}

std::string AsyncFriendChatMsgReq::ToString() const
{
	using namespace json11;
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"ChatMsgId",m_strChatMsgId},
		});
	return msgJson.dump();
}

bool AsyncFriendChatMsgReq::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["ChatMsgId"].is_string()) {
		m_strChatMsgId = json["ChatMsgId"].string_value();
	}
	else
	{
		return false;
	}

	return true;
}

AsyncFriendChatMsgRsp::AsyncFriendChatMsgRsp()
{
	m_type = E_MsgType::AsyncFriendChatMsgRsp_Type;
}

std::string AsyncFriendChatMsgRsp::ToString() const
{
	using namespace json11;
	
	Json::array msgArray;
	for (auto msgItem : m_chatMsgVec)
	{
		msgArray.push_back(FriendChatMsg(msgItem));
	}
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"MsgList",msgArray},
	});
	return msgJson.dump();
}

bool AsyncFriendChatMsgRsp::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["Code"].is_number()) {
		m_eCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgList"].is_array())
	{
		auto jsonArray = json["MsgList"].array_items();
	
		for (const auto msgItem : jsonArray)
		{
			FriendChatMsg_s chatMsg;
			if (FriendChatMsg(msgItem, chatMsg)) {
				m_chatMsgVec.push_back(chatMsg);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}


AsyncGroupChatMsgReq::AsyncGroupChatMsgReq()
{
	m_type = E_MsgType::AsyncGroupChatMsgReq_Type;
}

std::string AsyncGroupChatMsgReq::ToString() const
{
	using namespace json11;
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"ChatMsgId",m_strChatMsgId},
		});
	return msgJson.dump();
}

bool AsyncGroupChatMsgReq::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["ChatMsgId"].is_string()) {
		m_strChatMsgId = json["ChatMsgId"].string_value();
	}
	else
	{
		return false;
	}

	return true;
}

AsyncGroupChatMsgRsp::AsyncGroupChatMsgRsp()
{
	m_type = E_MsgType::AsyncGroupChatMsgRsp_Type;
}

std::string AsyncGroupChatMsgRsp::ToString() const
{
	using namespace json11;

	Json::array msgArray;
	for (auto msgItem : m_chatMsgVec)
	{
		msgArray.push_back(GroupChatMsg(msgItem));
	}
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"MsgList",msgArray},
		});
	return msgJson.dump();
}

bool AsyncGroupChatMsgRsp::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["Code"].is_number()) {
		m_eCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgList"].is_array())
	{
		auto jsonArray = json["MsgList"].array_items();

		for (const auto msgItem : jsonArray)
		{
			GroupChatMsg_s chatMsg;
			if (GroupChatMsg(msgItem, chatMsg)) {
				m_chatMsgVec.push_back(chatMsg);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

FileSendDataBeginReq::FileSendDataBeginReq()
{
	m_type = E_MsgType::FileSendDataBeginReq_Type;
}

std::string FileSendDataBeginReq::ToString() const 
{

	using namespace json11;
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"FriendId",m_strFriendId},
		{"FileName",m_strFileName},
		{"FileId",m_nFileId},
		{"FileHash",m_strFileHash},
		{"FileType",static_cast<int>(m_eFileType)},
		});
	return msgJson.dump();
}

bool FileSendDataBeginReq::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string()) {
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileName"].is_string()) {
		m_strFileName = json["FileName"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileId"].is_number())
	{
		m_nFileId = json["FileId"].int_value();
	}
	else
	{
		return false;
	}

	if (json["FileHash"].is_string())
	{
		m_strFileHash = json["FileHash"].string_value();
	}
	else
	{
		return false;
	}
	if (json["FileType"].is_number())
	{
		m_eFileType = static_cast<FILE_TYPE>(json["FileType"].int_value());
	}
	else
	{
		return false;
	}
	return true;
}


FileSendDataBeginRsp::FileSendDataBeginRsp()
{
	m_type = E_MsgType::FileSendDataBeginRsp_Type;
}

std::string FileSendDataBeginRsp::ToString() const
{
	using namespace json11;
	Json msgJson = Json::object({
		{"Code",static_cast<int>(m_errCode)},
		{"Message",m_errMsg},
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"FriendId",m_strFriendId},
		{"FileName",m_strFileName},
		{"FileId",m_nFileId},
		{"FileType",static_cast<int>(m_eFileType)},
	});
	return msgJson.dump();
}

bool FileSendDataBeginRsp::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["Code"].is_number()) {
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string()) {
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string()) {
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileName"].is_string()) {
		m_strFileName = json["FileName"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileId"].is_number())
	{
		m_nFileId = json["FileId"].int_value();
	}
	else
	{
		return false;
	}

	if (json["FileType"].is_number())
	{
		m_eFileType = static_cast<FILE_TYPE>(json["FileType"].int_value());
	}
	else
	{
		return false;
	}
	return true;
}

FileTransProgressNotifyReqMsg::FileTransProgressNotifyReqMsg()
{
	m_type = E_MsgType::FileTransProgressNotifyReq_Type;
}

std::string FileTransProgressNotifyReqMsg::ToString() const
{
	using namespace json11;
	Json objJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"OtherId",m_strOtherId},
		{"FileName",m_strFileName},
		{"TransPercent",m_nTransPercent},
		{"TransSpeed",m_nTransSpeed},
		{"Direction",static_cast<int>(m_eDirection)},
	});
	
	return objJson.dump();
}

bool FileTransProgressNotifyReqMsg::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["OtherId"].is_string()) {
		m_strOtherId = json["OtherId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileName"].is_string()) {
		m_strFileName = json["FileName"].string_value();
	}
	else
	{
		return false;
	}

	if (json["TransPercent"].is_number()) {
		m_nTransPercent = json["TransPercent"].int_value();
	}
	else
	{
		return false;
	}

	if (json["TransSpeed"].is_number()) {
		m_nTransSpeed = json["TransSpeed"].int_value();
	}
	else
	{
		return false;
	}

	if (json["Direction"].is_number()) {
		m_eDirection = static_cast<FILE_TRANS_DIRECTION>(json["Direction"].int_value());
	}
	else
	{
		return false;
	}

	return true;
}

FileDownLoadReqMsg::FileDownLoadReqMsg()
{
	m_type = E_MsgType::FileDownLoadReq_Type;
}

std::string FileDownLoadReqMsg::ToString() const
{
	using namespace json11;
	Json msgJson = Json::object({
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"FriendId",m_strFriendId},
		{"FileName",m_strFileName},
		{"RelateMsgId",m_strRelateMsgId},
		{"FileType",static_cast<int>(m_eFileType)},
		});
	return msgJson.dump();
}

bool FileDownLoadReqMsg::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}


	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string()) {
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileName"].is_string()) {
		m_strFileName = json["FileName"].string_value();
	}
	else
	{
		return false;
	}

	if (json["RelateMsgId"].is_string()) {
		m_strRelateMsgId = json["RelateMsgId"].string_value();
	}
	else {
		return false;
	}

	if (json["FileType"].is_number()) {
		m_eFileType = static_cast<FILE_TYPE>(json["FileType"].int_value());
	}
	else {
		return false;
	}

	return true;
}


FileDownLoadRspMsg::FileDownLoadRspMsg()
{
	m_type = E_MsgType::FileDownLoadRsp_Type;
}

std::string FileDownLoadRspMsg::ToString() const
{
	using namespace json11;
	Json msgJson = Json::object({
		{"Code",static_cast<int>(m_errCode)},
		{"Message",m_errMsg},
		{"MsgId",m_strMsgId},
		{"UserId",m_strUserId},
		{"FriendId",m_strFriendId},
		{"FileName",m_strFileName},
		{"FileHash",m_strFileHash},
		{"RelateMsgId",m_strRelateMsgId},
		{"FileType",static_cast<int>(m_eFileType)},
		});
	return msgJson.dump();
}

bool FileDownLoadRspMsg::FromString(const std::string& strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["Code"].is_number()) {
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string()) {
		m_errMsg = json["Message"].string_value();
	}
	else
	{
		return false;
	}

	if (json["MsgId"].is_string()) {
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string()) {
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string()) {
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileName"].is_string()) {
		m_strFileName = json["FileName"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FileHash"].is_string()) {
		m_strFileHash = json["FileHash"].string_value();
	}
	else
	{
		return false;
	}

	if (json["RelateMsgId"].is_string()) {
		m_strRelateMsgId = json["RelateMsgId"].string_value();
	}
	else
	{
		return false;
	}
	
	if (json["FileType"].is_number()) {
		m_eFileType = static_cast<FILE_TYPE>(json["FileType"].int_value());
	}
	else
	{
		return false;
	}
	
	return true;
}


std::string MsgElemVec(const ChatMsgElemVec& vec)
{
	using namespace json11;
	json11::Json::array jsonArray;
	std::size_t Count = vec.size();
	for (std::size_t index = 0; index < Count; index++)
	{
		auto& item = vec[index];
		switch (item.m_eType)
		{
		case CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE:
		{
			Json clientObj = Json::object(
				{
					{"Index",static_cast<int>(index)},
					{"Type",static_cast<int>(item.m_eType)},
					{"Data",item.m_strContext},
				});
			jsonArray.push_back(clientObj);
		}break;
		case CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE:
		{
			Json clientObj = Json::object(
				{
					{"Index",static_cast<int>(index)},
					{"Type",static_cast<int>(item.m_eType)},
					{"Data",std::to_string(item.m_nFaceId)},
				});
			jsonArray.push_back(clientObj);
		}break;
		case CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE:
		{
			Json clientObj = Json::object(
				{
					{"Index",static_cast<int>(index)},
					{"Type",static_cast<int>(item.m_eType)},
					{"Data",item.m_strImageName},
				});
			jsonArray.push_back(clientObj);
		}break;
		case CHAT_MSG_TYPE::E_CHAT_FILE_TYPE:
		{
			Json clientObj = Json::object(
				{
					{"Index",static_cast<int>(index)},
					{"Type",static_cast<int>(item.m_eType)},
					{"Data",item.m_strImageName},
				});
			jsonArray.push_back(clientObj);
		}break;
        default:
        {

        }break;
		}
	}
	Json resultJson = Json::object({
		{"MsgList",jsonArray},
		});

	return resultJson.dump();
}
ChatMsgElemVec MsgElemVec(const std::string strVec)
{
	ChatMsgElemVec result;
	std::string err;
	using namespace json11;
	auto jsonObj = Json::parse(strVec, err);
	if (!err.empty())
	{
		return result;
	}

	if (jsonObj["MsgList"].is_array())
	{
		auto jsonArray = jsonObj["MsgList"].array_items();
		for (auto jsonItem : jsonArray)
		{
			if (jsonItem["Index"].is_number())
			{
				CHAT_MSG_TYPE eType = static_cast<CHAT_MSG_TYPE>(jsonItem["Type"].int_value());
				switch (eType)
				{
				case CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE:
				{
					ChatMsgElem msg;
					msg.m_eType = CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE;
					if (jsonItem["Data"].is_string())
					{
						msg.m_strContext = jsonItem["Data"].string_value();
						result.push_back(msg);
					}
				}break;
				case CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE:
				{
					ChatMsgElem msg;
					msg.m_eType = CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE;
					if (jsonItem["Data"].is_string())
					{
						msg.m_nFaceId = std::atoi(jsonItem["Data"].string_value().c_str());
						result.push_back(msg);
					}
				}break;
				case CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE:
				{
					ChatMsgElem msg;
					msg.m_eType = CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE;
					if (jsonItem["Data"].is_string())
					{
						msg.m_strImageName = jsonItem["Data"].string_value();
						result.push_back(msg);
					}
				}break;
				case CHAT_MSG_TYPE::E_CHAT_FILE_TYPE:
				{
					ChatMsgElem msg;
					msg.m_eType = CHAT_MSG_TYPE::E_CHAT_FILE_TYPE;
					if (jsonItem["Data"].is_string())
					{
						msg.m_strImageName = jsonItem["Data"].string_value();
						result.push_back(msg);
					}
				}break;
                default:
                {

                }break;
				}
			}
		}
	}
	return result;
}


GetRandomUserReqMsg::GetRandomUserReqMsg()
{
	m_type = E_MsgType::GetRandomUserReq_Type;
}

std::string GetRandomUserReqMsg::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool GetRandomUserReqMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

GetRandomUserRspMsg::GetRandomUserRspMsg()
{
	m_type = E_MsgType::GetRandomUserRsp_Type;
}

std::string GetRandomUserRspMsg::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"Code",static_cast<int>(m_errCode)},
			{"Message",m_errMsg},
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"FriendName",m_strFriendName},
		});

	return clientObj.dump();
}

bool GetRandomUserRspMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}

	if (json["Code"].is_number()) {
		m_errCode = static_cast<ERROR_CODE_TYPE>(json["Code"].int_value());
	}
	else
	{
		return false;
	}

	if (json["Message"].is_string()) {
		m_errMsg = json["Message"].string_value();
	}

	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendName"].is_string()) {
		m_strFriendName = json["FriendName"].string_value();
	}
	else {
		return false;
	}
	return true;
}


UdpP2pStartReqMsg::UdpP2pStartReqMsg()
{
	m_type = E_MsgType::UdpP2PStartReq_Type;
}

std::string UdpP2pStartReqMsg::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"FriendId",m_strFriendId},
		});

	return clientObj.dump();
}

bool UdpP2pStartReqMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string())
	{
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}

UdpP2pStartRspMsg::UdpP2pStartRspMsg()
{
	m_type = E_MsgType::UdpP2PStartRsp_Type;
}

std::string UdpP2pStartRspMsg::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"FriendId",m_strFriendId},
		});

	return clientObj.dump();
}

bool UdpP2pStartRspMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string())
	{
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}


UdpMultiCastReqMsg::UdpMultiCastReqMsg()
{
	m_type = E_MsgType::UdpMultiCastReq_Type;
}

std::string UdpMultiCastReqMsg::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
		});

	return clientObj.dump();
}

bool UdpMultiCastReqMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	return true;
}

UdpMultiCastRspMsg::UdpMultiCastRspMsg()
{
	m_type = E_MsgType::UdpMultiCastRsp_Type;
}

std::string UdpMultiCastRspMsg::ToString() const
{
	using namespace json11;
	Json clientObj = Json::object(
		{
			{"MsgId", m_strMsgId},
			{"UserId", m_strUserId},
			{"FriendId",m_strFriendId},
		});

	return clientObj.dump();
}

bool UdpMultiCastRspMsg::FromString(const std::string &strJson)
{
	std::string err;
	using namespace json11;
	auto json = Json::parse(strJson, err);
	if (!err.empty())
	{
		return false;
	}
	if (json["MsgId"].is_string())
	{
		m_strMsgId = json["MsgId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["UserId"].is_string())
	{
		m_strUserId = json["UserId"].string_value();
	}
	else
	{
		return false;
	}

	if (json["FriendId"].is_string())
	{
		m_strFriendId = json["FriendId"].string_value();
	}
	else
	{
		return false;
	}
	return true;
}