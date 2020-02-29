#ifndef _DENNIS_THINK_C_REGISTER_SKIN_HYPER_LINK_H_
#define _DENNIS_THINK_C_REGISTER_SKIN_HYPER_LINK_H_
#include "..\UTalkClient\RegisterTask.h"
#include "..\SkinLib\SkinHyperLink.h"

//注锟结按钮锟斤拷
class CRegisterSkinHyperLink : public CSkinHyperLink
{
public:
	CRegisterSkinHyperLink();
	virtual ~CRegisterSkinHyperLink();

	BEGIN_MSG_MAP_EX(CRegisterSkinHyperLink)
		MSG_WM_LBUTTONUP(OnLButtonUp)
	END_MSG_MAP()

protected:
	void OnLButtonUp(UINT nFlags, CPoint point);

protected:
	
};
#endif