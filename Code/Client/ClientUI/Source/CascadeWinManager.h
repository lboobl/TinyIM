
/**
 * @file CascadeWinManager.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 层叠窗口管理器头文件
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_C_CASCADE_WIN_MANAGER_H_
#define _TINY_IM_C_CASCADE_WIN_MANAGER_H_
#include <vector>

struct CASCADE_WIN_INFO			// 层叠窗口信息
{
	HWND hWnd;					// 窗口句柄
	POINT pt;					// 窗口左上角坐标
};

class CCascadeWinManager		// 层叠窗口管理器
{
public:
	CCascadeWinManager(void);
	~CCascadeWinManager(void);

public:
	BOOL Add(HWND hWnd, int cx, int cy);				// 添加窗口，并层叠显示
	void Del(HWND hWnd);								// 删除窗口
	BOOL SetPos(HWND hWnd, int x, int y);				// 设置窗口位置
	void Clear();										// 清除所有窗口

private:
	std::vector<CASCADE_WIN_INFO*> m_arrWinInfo;		// 窗口信息数组
};
#endif