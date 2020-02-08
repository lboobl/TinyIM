cmake_minimum_required(VERSION 3.0)
project(MediumServer)
add_definitions(-DUSE_STANDALONE_ASIO)
add_definitions(-DASIO_STANDALONE)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
include_directories(../../../msgStruct/)
include_directories(../../../CommonFunction/)
include_directories(../../../msgStruct/json11/)
include_directories(../include/thirdparty/)
include_directories(../include/thirdparty/asio/)
include_directories(../include/thirdparty/spdlog/)
include_directories(../include/thirdparty/fmt/include/)
include_directories(../include/thirdparty/httpServer/)
include_directories(../include/thirdparty/SnowFlake/)
include_directories(../include/thirdparty/SQLiteCpp/include/)
include_directories(../include/thirdparty/SQLiteCpp/sqlite3/)
include_directories(../include/common/)
include_directories(../include/DaemonApp/)
link_libraries(zlib)
if(APPLE)
    include_directories(${PROJECT_SOURCE_DIR} "/usr/local/include" "/usr/local/ssl/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    link_directories(/usr/local/lib)
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX")
else()
    include_directories(${PROJECT_SOURCE_DIR} "/usr/local/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g")
endif()

if(WIN32)
else()
link_libraries(pthread dl)
endif()
set(SERVER_FILES 
		CServerSess.h CServerSess.cpp
        CMediumServer.h CMediumServer.cpp 
        ../../../msgStruct/json11/json11.cpp
        CClientSess.h CClientSess.cpp
		../include/common/IUProtocolData.cpp
        ../include/DaemonApp/DaemonSvcApp.h 
        ../include/DaemonApp/DaemonSvcApp.cpp
		./CHttpServer.h 
        ./CHttpServer.cpp
		CMsgPersistentUtil.h 
        CMsgPersistentUtil.cpp
		./CUdpMultiCastSender.h
        ./CUdpMultiCastSender.cpp		
		./CUdpMultiCastReciver.h 
		./CUdpMultiCastReciver.cpp
		../../../CommonFunction/CFileUtil.h  
		../../../CommonFunction/CFileUtil.cpp
		../../../CommonFunction/md5.h  
		../../../CommonFunction/md5.cpp
		./CUdpClient.h ./CUdpClient.cpp
		../../../msgStruct/CommonDef.cpp
		../../../msgStruct/CommonMsg.cpp
		../include/common/CommonFunction.cpp
		./MainFunc.h
		./MainFunc.cpp
		./CNetUtil.h 
		./CNetUtil.cpp
        )

SET(SQLITE_CPP_SOURCE ../include/thirdparty/SQLiteCpp/sqlite3/sqlite3.c
../include/thirdparty/SQLiteCpp/src/Backup.cpp
../include/thirdparty/SQLiteCpp/src/Column.cpp
../include/thirdparty/SQLiteCpp/src/Database.cpp
../include/thirdparty/SQLiteCpp/src/Exception.cpp
../include/thirdparty/SQLiteCpp/src/Statement.cpp
../include/thirdparty/SQLiteCpp/src/Transaction.cpp)

add_executable(ClientCore MediumServer.cpp ${SERVER_FILES}  ${SQLITE_CPP_SOURCE})
add_executable(Sender ./sender.cpp ../../../msgStruct/CommonMsg.cpp ../../../msgStruct/json11/json11.cpp ../../../msgStruct/CommonDef.cpp)
add_executable(Reciver ./reciver.cpp ../../../msgStruct/CommonMsg.cpp ../../../msgStruct/json11/json11.cpp ../../../msgStruct/CommonDef.cpp)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)  