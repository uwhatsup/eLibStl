#include"ElibHelp.h"


static ARG_INFO Args[] =
{
	{
		/*name*/    "��ʾ��Ϣ",
		/*explain*/ (""),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_BIN,
		/*default*/ 0,
		/*state*/   0,
	},
	{
		/*name*/    "��ť",
		/*explain*/ (""),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_INT,
		/*default*/ 0,
		/*state*/   AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
		/*name*/    "���ڱ���",
		/*explain*/ (""),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_BIN,
		/*default*/ 0,
		/*state*/   AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
		/*name*/    "������",
		/*explain*/ (""),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_INT,
		/*default*/ 0,
		/*state*/   AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
		/*name*/    "����ʱ��",
		/*explain*/ ("ָ��ʱ�����Զ��رգ�Ĭ��Ϊ���Զ��ر�"),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_INT,
		/*default*/ 0,
		/*state*/   AS_DEFAULT_VALUE_IS_EMPTY,
	}
};



EXTERN_C int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

EXTERN_C void Fn_MessageBoxW(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	const std::wstring_view& text = elibkrnln::args_to_wsdata(pArgInf, 0);
	auto button = elibkrnln::args_to_data<INT>(pArgInf, 1);
	const std::wstring_view& title = elibkrnln::args_to_wsdata(pArgInf, 2);
	auto hpwnd = elibkrnln::args_to_data<INT>(pArgInf, 3);
	auto time = elibkrnln::args_to_data<INT>(pArgInf, 4);
	const HWND hOldFocusWnd = ::GetFocus();
	INT nResult;
	if (time.has_value() && time.value() >= 0) {
		nResult = ::MessageBoxTimeoutW(reinterpret_cast<HWND>(hpwnd.has_value() ? hpwnd.value() : 0), std::wstring(text).c_str(), std::wstring(title).c_str(), button.has_value() ? button.value() : 0, 0, time.value());
	}
	else
	{
		nResult = ::MessageBoxW(reinterpret_cast<HWND>(hpwnd.has_value() ? hpwnd.value() : 0), std::wstring(text).c_str(), std::wstring(title).c_str(), button.has_value() ? button.value() : 0);
	}

	if (hOldFocusWnd != NULL && ::IsWindow(hOldFocusWnd)) {
		::SetFocus(hOldFocusWnd);
	}
	pRetData->m_int = nResult - 1;
}

FucInfo message_box_w = { {
		/*ccname*/  ("��Ϣ��W"),
		/*egname*/  (""),
		/*explain*/ (""),
		/*category*/3,
		/*state*/   NULL,
		/*ret*/     SDT_INT,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/5,
		/*arg lp*/  &Args[0],
	} ,Fn_MessageBoxW ,"Fn_MessageBoxW" };