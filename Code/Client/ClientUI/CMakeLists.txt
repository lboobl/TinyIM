cmake_minimum_required(VERSION 3.0)
project(TinyIM)
include_directories(./Source/wtl9.0/)
include_directories(./Source/)
include_directories(./Source/Proto/)
include_directories(./Source/asioNet/include/thirdparty/)
include_directories(./Source/asioNet/include/thirdparty/asio/)
include_directories(./Source/asioNet/include/thirdparty/spdlog/)
include_directories(./Source/asioNet/include/thirdparty/json11/)
include_directories(./Source/asioNet/include/thirdparty/fmt/include/)
include_directories(./Source/asioNet/include/common/)
include_directories(./Source/net/)
include_directories(${PROJECT_SOURCE_DIR}/../../msgStruct/)
ADD_DEFINITIONS(-D_AFXDLL)


if(APPLE)
    include_directories(${PROJECT_SOURCE_DIR} "/usr/local/include" "/usr/local/ssl/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    link_directories(/usr/local/lib)
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX")
	add_compile_options(/wd4838)#warning C4838: 从“int”转换到“UINT”需要收缩转换
	add_compile_options(/wd4302)#warning C4302: “类型强制转换”: 从“LPCTSTR”到“WORD”截断
	add_compile_options(/wd4091)#warning C4091: “typedef ”: 没有声明变量时忽略“<unnamed-enum-hdBase>”的左侧
else(Linux)
    include_directories(${PROJECT_SOURCE_DIR} "/usr/local/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")
# 在共享库中使用MFC
SET(CMAKE_MFC_FLAG 2)
add_link_options("/SUBSYSTEM:WINDOWS")
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
link_directories(./Source/lib/)
SET(FLAMING_GO_CLIENT_SOURCE 
./Source/AboutDlg.h ./Source/AboutDlg.cpp
./Source/AddFriendConfirmDlg.h ./Source/AddFriendConfirmDlg.cpp
./Source/BuddyChatDlg.h ./Source/BuddyChatDlg.cpp
./Source/BuddyInfoDlg.h ./Source/BuddyInfoDlg.cpp
./Source/BuddyListCtrl.h ./Source/BuddyListCtrl.cpp
./Source/UI_Core_Convert.h ./Source/UI_Core_Convert.cpp
./Source/CascadeWinManager.h ./Source/CascadeWinManager.cpp
./Source/ChangePicHead.h ./Source/ChangePicHead.cpp
./Source/ChangePicHeadDlg.h ./Source/ChangePicHeadDlg.cpp
./Source/ChatDlgCommon.h ./Source/ChatDlgCommon.cpp
./Source/ChatTabMgr.h ./Source/ChatTabMgr.cpp
./Source/ClosePromptDlg.h ./Source/ClosePromptDlg.cpp
./Source/MessageBoxDlg.h ./Source/MessageBoxDlg.cpp
./Source/Crash.cpp
./Source/UICommonDef.h
./Source/UIDefaultValue.h
./Source/UICommonStruct.cpp
./Source/CreateNewGroupDlg.h ./Source/CreateNewGroupDlg.cpp
./Source/CustomMsgDef.h
./Source/DstVersion.h

./Source/ChangePicHeadDlg.h ./Source/ChangePicHeadDlg.cpp
./Source/FaceCtrl.h ./Source/FaceCtrl.cpp
./Source/FaceList.h ./Source/FaceList.cpp
./Source/FaceSelDlg.h ./Source/FaceSelDlg.cpp
./Source/FileTool.h ./Source/FileTool.cpp
./Source/FileTransferCtrl.h ./Source/FileTransferCtrl.cpp
./Source/FindFriendDlg.h ./Source/FindFriendDlg.cpp
./Source/FontSelDlg.h ./Source/FontSelDlg.cpp
./Source/GDIFactory.h ./Source/GDIFactory.cpp
./Source/GifImage.h ./Source/GifImage.cpp
./Source/GroupChatDlg.h ./Source/GroupChatDlg.cpp
./Source/GroupInfoDlg.h ./Source/GroupInfoDlg.cpp
./../../msgStruct/CommonDef.cpp
./../../msgStruct/CommonMsg.cpp
./Source/IImageOle.h
./Source/ImageEx.h ./Source/ImageEx.cpp
./Source/IniFile.h ./Source/IniFile.cpp
./Source/IULog.h ./Source/IULog.cpp
./Source/LoginAccountList.h ./Source/LoginAccountList.cpp 
./Source/LoginDlg.h ./Source/LoginDlg.cpp 
./Source/LoginSettingsDlg.h ./Source/LoginSettingsDlg.cpp 
./Source/LogonUserInfoDlg.h ./Source/LogonUserInfoDlg.cpp 
./Source/MainDlg.h ./Source/MainDlg.cpp
./Source/MessageLogger.h ./Source/MessageLogger.cpp
./Source/MiniBuffer.h ./Source/MinuBuffer.cpp 
./Source/ModifyMarkNameDlg.h ./Source/ModifyMarkNameDlg.cpp
./Source/ModifyPasswordDlg.h ./Source/ModifyPasswordDlg.cpp
./Source/Path.h ./Source/Path.cpp  
./Source/PerformanceCounter.h ./Source/PerformanceCounter.cpp  
./Source/PicBarDlg.h ./Source/PicBarDlg.cpp  
./Source/RecentListCtrl.h ./Source/RecentListCtrl.cpp   
./Source/RegisterDialog.h ./Source/RegisterDialog.cpp
./Source/resource.h ./Source/Result.h 
./Source/RichEditUtil.h ./Source/RichEditUtil.cpp  
./Source/SplitterCtrl.h  ./Source/SplitterCtrl.cpp   
./Source/MsgTipDlg.h  ./Source/MsgTipDlg.cpp 
./Source/SrcVersion.h  ./Source/   
./Source/UICommonStruct.h 
./Source/UIWndMsgDef.h
./Source/stdafx.h  ./Source/stdafx.cpp   
./Source/SystemSettingDlg.h  ./Source/SystemSettingDlg.cpp  
./Source/TeamDlg.h  ./Source/TeamDlg.cpp 
./Source/Thread.h  ./Source/Thread.cpp 
./Source/ThreadPool.h  ./Source/ThreadPool.cpp 
./Source/UtilTime.h  ./Source/UtilTime.cpp  
./Source/TrayIcon.h  ./Source/TrayIcon.cpp  
./Source/UIText.h    
./Source/UpdateDlg.h  ./Source/UpdateDlg.cpp  
./Source/Updater.h  ./Source/Updater.cpp 
./Source/UserConfig.h  ./Source/UserConfig.cpp
./Source/UserMgr.h  ./Source/UserMgr.cpp
./Source/UICommonType.h
./Source/UserSnapInfoDlg.h  ./Source/UserSnapInfoDlg.cpp
./Source/Utils.h  ./Source/Utils.cpp
./Source/XmlDocument.h  ./Source/XmlDocument.cpp
./Source/ZlibUtil.h  ./Source/ZlibUtil.cpp
./Source/flamingo.rc)

SET(ENCODE_UTIL_SOURCE_FILE ./Source/EncodingUtil.h ./Source/EncodingUtil.cpp)

SET(UI_NET_SOURCE_FILE ./Source/UI_USER_INFO.h  ./Source/UI_USER_INFO.cpp)

SET(NET_SOURCE_FILE ./Source/net/Msg.h ./Source/net/FileMsg.h
./Source/net/IUProtocolData.h ./Source/net/IUProtocolData.cpp
./Source/net/protocolstream.h ./Source/net/protocolstream.cpp
./Source/Proto/Proto.h  ./Source/Proto/Proto.cpp
./Source/asioNet/include/thirdparty/json11/json11.cpp
./Source/asioNet/CClientSess.cpp
./Source/asioNet/CSessManager.cpp
)



SET(REMOTE_DESKTOP_SOURCE_FILE ./Source/RemoteDesktop/CursorInfo.h 
./Source/RemoteDesktop/ScreenManager.h 
./Source/RemoteDesktop/ScreenManager.cpp
./Source/RemoteDesktop/ScreenSpy.h
./Source/RemoteDesktop/ScreenSpy.cpp
./Source/RemoteDesktop/until.h
./Source/RemoteDesktop/until.cpp)

add_subdirectory(./Source/SkinLib)
add_subdirectory(./Source/zlib1.2.11)
link_libraries(SkinLib zlib RichEd20)
if(CMAKE_CL_64)
    #设置64位的程序目标(可执行程序)输出路径
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug64/)     
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release64/) 
else()
    #设置32位的程序目标(可执行程序)输出路径
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Bin/Debug/)     
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Bin/Release/) 
endif()

add_executable(TinyIM ./Source/Startup.h  ./Source/Startup.cpp    ${FLAMING_GO_CLIENT_SOURCE} ${NET_SOURCE_FILE} ${UI_NET_SOURCE_FILE} ${ENCODE_UTIL_SOURCE_FILE})

add_subdirectory(./CatchScreen)
#add_subdirectory(./iAutoUpdate)