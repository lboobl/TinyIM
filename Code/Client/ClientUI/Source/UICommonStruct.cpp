#include "UICommonStruct.h"
#include "EncodingUtil.h"
#include "json11.hpp"
#include <map>

/**
 * @brief 将UI编辑框的消息数据,转为可以传递的JSON字符串
 * 
 * @param msgList UI编辑框的消息数据
 * @return std::string 
 */
std::string RichEditMsg(const RichEditMsgList& msgList)
{
	using namespace json11;
	json11::Json::array jsonArray;
	std::size_t index = 0;
	std::size_t Count = msgList.size();
	for (std::size_t index = 0; index < Count; index++)
	{
		auto& item = msgList[index];
		switch (item.m_eType)
		{
			case E_RichEditType::TEXT:
			{
				Json clientObj = Json::object(
					{
						{"Index",static_cast<int>(index)},
						{"Type",static_cast<int>(msgList[index].m_eType)},
						{"Data",EncodeUtil::UnicodeToAnsi(item.m_strContext)},
					});
				jsonArray.push_back(clientObj);
			}break;
			case E_RichEditType::FACE:
			{
				Json clientObj = Json::object(
					{
						{"Index",static_cast<int>(index)},
						{"Type",static_cast<int>(msgList[index].m_eType)},
						{"Data",std::to_string(item.m_nFaceId)},
					});
				jsonArray.push_back(clientObj);
			}break;
			case E_RichEditType::IMAGE:
			{
				Json clientObj = Json::object(
					{
						{"Index",static_cast<int>(index)},
						{"Type",static_cast<int>(msgList[index].m_eType)},
						{"Data",EncodeUtil::UnicodeToAnsi(item.m_strImageName)},
					});
				jsonArray.push_back(clientObj);
			}break;
		}
	}
	Json resultJson = Json::object({
		{"MsgList",jsonArray},
	});

	return resultJson.dump();
}


/**
 * @brief 将JOSN格式的消息数据,转为UI消息的数组
 * 
 * @param strMsg 
 * @return RichEditMsgList 
 */
RichEditMsgList RichEditMsg(const std::string& strMsg)
{
	RichEditMsgList result;
	std::map<int, RichEditMsg_st> msgMap;
	using namespace json11;
	std::string err;
	using namespace json11;
	auto jsonObj = Json::parse(strMsg, err);
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
				E_RichEditType eType = static_cast<E_RichEditType>(jsonItem["Type"].int_value());
				switch (eType)
				{
				case E_RichEditType::TEXT:
				{
					RichEditMsg_st msg;
					msg.m_eType = E_RichEditType::TEXT;
					if (jsonItem["Data"].is_string())
					{
						msg.m_strContext = EncodeUtil::AnsiToUnicode(jsonItem["Data"].string_value());
						result.push_back(msg);
					}
				}break;
				case E_RichEditType::FACE:
				{
					RichEditMsg_st msg;
					msg.m_eType = E_RichEditType::FACE;
					if (jsonItem["Data"].is_string())
					{
						msg.m_nFaceId = std::atoi(jsonItem["Data"].string_value().c_str());
						result.push_back(msg);
					}
				}break;
				case E_RichEditType::IMAGE:
				{
					RichEditMsg_st msg;
					msg.m_eType = E_RichEditType::IMAGE;
					if (jsonItem["Data"].is_string())
					{
						msg.m_strImageName = EncodeUtil::AnsiToUnicode(jsonItem["Data"].string_value());
						result.push_back(msg);
					}
				}break;
				}
			}
		}
	}

	return result;
}