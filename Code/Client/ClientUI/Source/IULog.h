
/**
 * @file IULog.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用来创建日志的类
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __LOG_H__
#define __LOG_H__
#include "asioNet/include/common/Log.h"
std::shared_ptr<spdlog::logger> CreateLogger();
#endif // !__LOG_H__