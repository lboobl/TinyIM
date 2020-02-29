
/**
 * @file GDIFactory.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief GDI工厂类，暂时不知道用途
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_GDI_FACTORY_H_
#define _TINY_IM_GDI_FACTORY_H_
#include <map>

class CGDIFactory
{
public:
	static HFONT GetFont(long nSize);
	static HFONT GetBoldFont(long nSize);

	static void Uninit();

private:
	static std::map<long, HFONT> m_mapFontCache;
	static std::map<long, HFONT> m_mapBoldFontCache;
};

#endif