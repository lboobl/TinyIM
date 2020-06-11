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

SET(MSG_STRUCT_FILES ../../../msgStruct/json11/json11.cpp
../../../msgStruct/CommonDef.cpp
../../../msgStruct/CommonMsg.cpp
CMsgPersistentUtil.h 
CMsgPersistentUtil.cpp
CServerSess.h 
CServerSess.cpp)

SET(DAEMON_FILES ../include/DaemonApp/DaemonSvcApp.h 
				 ../include/DaemonApp/DaemonSvcApp.cpp)

SET(MULTI_CAST_SESS_FILES ./CUdpMultiCastSender.h
./CUdpMultiCastSender.cpp		
./CUdpMultiCastReciver.h 
./CUdpMultiCastReciver.cpp
./CNetUtil.h 
./CNetUtil.cpp)

SET(HTTP_FILES 		./CHttpServer.h 
					./CHttpServer.cpp)

SET(COMMON_FUNCTION_FILES 		../../../CommonFunction/CFileUtil.h  
../../../CommonFunction/CFileUtil.cpp
../../../CommonFunction/md5.h  
../../../CommonFunction/md5.cpp
../include/common/CommonFunction.cpp
)

SET(TCP_MAIN_SERVER  
CMediumServer.h 
CMediumServer.cpp		
./MainFunc.cpp)

SET(UDP_MAIN_FILES 
./CMediumServerMultiCast.h 
./CMediumServerMultiCast.cpp
./MainFuncMultiCast.cpp)

SET(TCP_SESS_SERVER_FILES   

CClientSess.h 

CClientSess.cpp)

set(TCP_CORE_FILES 
      
		../include/common/IUProtocolData.cpp
		./CUdpClient.h ./CUdpClient.cpp

		./MainFunc.h
)
SET(FILE_SPEED_FILE ./CFileTransSpeedUtil.h ./CFileTransSpeedUtil.cpp)

SET(SQLITE_CPP_SOURCE ../include/thirdparty/SQLiteCpp/sqlite3/sqlite3.c
../include/thirdparty/SQLiteCpp/src/Backup.cpp
../include/thirdparty/SQLiteCpp/src/Column.cpp
../include/thirdparty/SQLiteCpp/src/Database.cpp
../include/thirdparty/SQLiteCpp/src/Exception.cpp
../include/thirdparty/SQLiteCpp/src/Statement.cpp
../include/thirdparty/SQLiteCpp/src/Transaction.cpp)

SET(TCP_SERVER_FILES ${TCP_CORE_FILES} 
					 ${COMMON_FUNCTION_FILES} 
					 ${HTTP_FILES} 
					 ${DAEMON_FILES}
					 ${MSG_STRUCT_FILES}
					 ${TCP_MAIN_SERVER} ${TCP_SESS_SERVER_FILES})

SET(UDP_SERVER_FILES ${TCP_CORE_FILES} 
${COMMON_FUNCTION_FILES} 
${HTTP_FILES} 
${DAEMON_FILES}
${MSG_STRUCT_FILES}
${MULTI_CAST_SESS_FILES}
${UDP_MAIN_FILES}
${FILE_SPEED_FILE})

add_executable(ClientCore MediumServer.cpp ${TCP_SERVER_FILES}  ${SQLITE_CPP_SOURCE})
add_executable(MultiCastCore MediumServer.cpp ${UDP_SERVER_FILES}  ${SQLITE_CPP_SOURCE})
#add_executable(Sender ./sender.cpp ../../../msgStruct/CommonMsg.cpp ../../../msgStruct/json11/json11.cpp ../../../msgStruct/CommonDef.cpp)
#add_executable(Reciver ./reciver.cpp ../../../msgStruct/CommonMsg.cpp ../../../msgStruct/json11/json11.cpp ../../../msgStruct/CommonDef.cpp)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)  