#include"EcontrolHelp.h"
#include<string>
#include"etcp\etcpapi.h"


#define SERVEREX L"my_e_sever_ex"
static INT s_server_cmd[] = { 39 , 40 , 41 , 42 , 43 , 44 ,45 };

HBITMAP g_hbmp_server = NULL;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
#ifndef __E_STATIC_LIB
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		HDC hCDC = CreateCompatibleDC(ps.hdc);
		SelectObject(hCDC, g_hbmp_server);
		RECT rc;
		GetClientRect(hWnd, &rc);
		FillRect(ps.hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(ps.hdc, &rc, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
		BitBlt(ps.hdc, (rc.right - 24) / 2, (rc.bottom - 24) / 2, 24, 24, hCDC, 0, 0, SRCCOPY);
		DeleteDC(hCDC);
		EndPaint(hWnd, &ps);
		break;
	}
#endif
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
static ATOM g_atom_clinet_ex = NULL;
static void Sever_RegWndClass()
{
#ifndef __E_STATIC_LIB
	g_hbmp_server = (HBITMAP)LoadImageW(g_hModule, MAKEINTRESOURCEW(IDB_SERVER_EX), IMAGE_BITMAP, 0, 0, 0);
#endif
	WNDCLASSW wc = { sizeof(WNDCLASSW) };
	wc.lpszClassName = SERVEREX;
	wc.hCursor = LoadCursorW(NULL, (PWSTR)IDC_ARROW);
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbWndExtra = sizeof(void*);
	g_atom_clinet_ex = RegisterClassW(&wc);
}

//struct _ESERVERDATA
//{
//	u_short m_prot;
//	char m_ip[16];
//	_ESERVERDATA() {
//		memset(this, 0, sizeof(this));
//	}
//}ESERVERDATA, * PESERVERDATA;


static void server_callback_incline(eServer* pServer) {

	EVENT_NOTIFY2 event(pServer->m_dwWinFormID, pServer->m_dwUnitID, 1);
	elibkrnln::NotifySys(NRS_EVENT_NOTIFY2, (DWORD) & event, 0);
}
static void server_callback_get_data(eServer* pServer) {

	EVENT_NOTIFY2 event(pServer->m_dwWinFormID, pServer->m_dwUnitID, 0);
	elibkrnln::NotifySys(NRS_EVENT_NOTIFY2, (DWORD) & event, 0);
}
static void server_callback_outcline(eServer* pServer) {
	EVENT_NOTIFY2 event(pServer->m_dwWinFormID, pServer->m_dwUnitID, 2);
	elibkrnln::NotifySys(NRS_EVENT_NOTIFY2, (DWORD) & event, 0);
}
HUNIT WINAPI Create_SeverWindow(
	LPBYTE pAllPropertyData,            //   ָ�򱾴��ڵ�Ԫ��������������, �ɱ����ڵ�Ԫ��ITF_GET_PROPERTY_DATA�ӿڲ���, ���û��������ΪNULL
	INT nAllPropertyDataSize,           //   �ṩpAllPropertyData��ָ�����ݵĳߴ�, ���û����Ϊ0
	DWORD dwStyle,                      //   Ԥ�����õĴ��ڷ��
	HWND hParentWnd,                    //   �����ھ��
	UINT uID,                           //   �ڸ������е�ID
	HMENU hMenu,                        //   δʹ��
	INT x, INT y, INT cx, INT cy,       //   ָ��λ�ü��ߴ�
	DWORD dwWinFormID, DWORD dwUnitID,  //   �����ڵ�Ԫ���ڴ��ڼ�������ID, ����֪ͨ��ϵͳ
	HWND hDesignWnd,                    //   ���blInDesignModeΪ��, ��hDesignWnd�ṩ����ƴ��ڵĴ��ھ��
	BOOL blInDesignMode                 //   ˵���Ƿ�������IDE�����Խ��п��ӻ����, ����ʱΪ��
)
{
	if (!g_atom_clinet_ex)
	{
		Sever_RegWndClass();
	}

	if (!blInDesignMode)
		dwStyle &= (~WS_VISIBLE);
	HWND hWnd = CreateWindowExW(
		0,
		SERVEREX,
		NULL,
		dwStyle | WS_CHILD | WS_CLIPSIBLINGS,
		x,
		y,
		cx,
		cy,
		hParentWnd,
		hMenu,
		NULL,
		NULL
	);
	if (!hWnd)
	{
		return NULL;
	}

	eServer* server = new eServer;
	server->m_clinet_enter = server_callback_incline;
	server->m_clinet_leave = server_callback_outcline;
	server->m_data_get = server_callback_get_data;
	if (pAllPropertyData && nAllPropertyDataSize == sizeof(u_short))
	{
		server->m_port_num = *reinterpret_cast<u_short*>(pAllPropertyData);
	}
	SetWindowLongPtrW(hWnd, GWL_USERDATA, (LONG_PTR)server);
	if (!blInDesignMode)
	{
		server->start();
	}
	server->m_dwUnitID = dwUnitID;
	server->m_dwWinFormID = dwWinFormID;
	return elibkrnln::make_cwnd(hWnd);
}
BOOL WINAPI NotifyPropertyChanged_ServerApp(HUNIT hUnit, INT nPropertyIndex,
	PUNIT_PROPERTY_VALUE pValue, LPTSTR* ppszTipText)    //Ŀǰ��δʹ��
{
	HWND hWnd = elibkrnln::get_hwnd_from_hunit(hUnit);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	switch (nPropertyIndex)
	{
	case 0:
		pServer->m_port_num = pValue->m_int;
		return FALSE;
	case 1:
		//��Ȼ�û������ˣ���������
		pServer->set_ip(pValue->m_szText);
		return FALSE;
	default:break;
	}
	return FALSE;//�������´���
}
HGLOBAL WINAPI GetAllPropertyData_ServerApp(HUNIT hUnit)
{
	HWND hWnd = elibkrnln::get_hwnd_from_hunit(hUnit);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(u_short));
	if (hGlobal)
	{
		void* pGlobal = GlobalLock(hGlobal);
		if (pGlobal)
		{
			memcpy(pGlobal, &pServer->m_port_num, sizeof(u_short));
			GlobalUnlock(hGlobal);
		}
	}
	return hGlobal;
}
BOOL WINAPI GetPropertyData_ServerApp(HUNIT hUnit, INT nPropertyIndex,
	PUNIT_PROPERTY_VALUE pValue)
{
	HWND hWnd = elibkrnln::get_hwnd_from_hunit(hUnit);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	switch (nPropertyIndex)
	{
	case 0:
		pValue->m_int = pServer->m_port_num;
		return FALSE;
	case 1: {
		char ip[16] = { 0 };
		memcpy(ip, pServer->get_ip().c_str(), pServer->get_ip().size());
		//�Ƿ�Ϊ�������죿������ʱ������һ��;
		pValue->m_szText = ip;
		return FALSE;
	}
	default:break;
	}
	return FALSE;
}
extern "C" PFN_INTERFACE WINAPI libkrnln_GetInterface_serverex(INT nInterfaceNO)
{

	return nInterfaceNO == ITF_CREATE_UNIT ? (PFN_INTERFACE)Create_SeverWindow :
		nInterfaceNO == ITF_NOTIFY_PROPERTY_CHANGED ? (PFN_INTERFACE)NotifyPropertyChanged_ServerApp :
		nInterfaceNO == ITF_GET_ALL_PROPERTY_DATA ? (PFN_INTERFACE)GetAllPropertyData_ServerApp :
		nInterfaceNO == ITF_GET_PROPERTY_DATA ? (PFN_INTERFACE)GetPropertyData_ServerApp :
		NULL;
}










static EVENT_INFO2 s_sever_event[] =
{
	/*000*/ {"���ݵ���", "�յ�����ʱִ��", _EVENT_OS(OS_ALL) | EV_IS_VER2, 0, 0, _SDT_NULL},
	/*001*/ {"�ͻ�����", "�пͻ�������ʱִ��", _EVENT_OS(OS_ALL) | EV_IS_VER2, 0, 0, _SDT_NULL},
	/*002*/ {"�ͻ��뿪", "�пͻ��˶Ͽ�ʱִ��", _EVENT_OS(OS_ALL) | EV_IS_VER2, 0, 0, _SDT_NULL},
};


static UNIT_PROPERTY s_server_member[] =
{
	// FIXED_WIN_UNIT_PROPERTY,    // ������ϴ˺�, ����ֱ��չ��, �����չ��
	//1=������, 2=Ӣ��������, 3=���Խ���, 4=���Ե���������UD_,5=���Եı�־, 6=˳���¼���еı�ѡ�ı�UW_(����UD_FILE_NAME), ��һ���մ�����

	/*000*/ {"���", "left", NULL, UD_INT, _PROP_OS(OS_ALL), NULL},
	/*001*/ {"����", "top", NULL, UD_INT, _PROP_OS(OS_ALL), NULL},
	/*002*/ {"����", "width", NULL, UD_INT, _PROP_OS(OS_ALL), NULL},
	/*003*/ {"�߶�", "height", NULL, UD_INT, _PROP_OS(OS_ALL), NULL},
	/*004*/ {"���", "tag", NULL, UD_TEXT, _PROP_OS(OS_ALL), NULL},
	/*005*/ {"����", "visible", NULL, UD_BOOL, _PROP_OS(OS_ALL), NULL},
	/*006*/ {"��ֹ", "disable", NULL, UD_BOOL, _PROP_OS(OS_ALL), NULL},
	/*007*/ {"���ָ��", "MousePointer", NULL, UD_CURSOR, _PROP_OS(OS_ALL), NULL},



	/*000*/ {"�˿ں�", "port", "�����Ǵ��� 0 С�� 32767 ���κ��Զ���ֵ", UD_INT, _PROP_OS(__OS_WIN), NULL},
	/*000*/ {"Ĭ��ip", "ip", "Ĭ��Ϊ����ip", UD_TEXT, UW_ONLY_READ | _PROP_OS(__OS_WIN), NULL},
};
LIB_DATA_TYPE_INFO severex = {
	/*m_szName*/			"������Ex",
	/*m_szEgName*/			"serverex",
	/*m_szExplain*/			"����ETCP�����ӵ��������ݽ����ķ��������������ָ���˿ڼ������Կͻ��������ݡ�",
	/*m_nCmdCount*/			sizeof(s_server_cmd) / sizeof(s_server_cmd[0]),
	/*m_pnCmdsIndex*/		s_server_cmd,
	/*m_dwState*/			_DT_OS(__OS_WIN) | LDT_WIN_UNIT | LDT_IS_FUNCTION_PROVIDER ,
	/*m_dwUnitBmpID*/		IDB_SERVER_EX,
	/*m_nEventCount*/		3,
	/*m_pEventBegin*/		s_sever_event,
	/*m_nPropertyCount*/	sizeof(s_server_member) / sizeof(s_server_member[0]),
	/*m_pPropertyBegin*/	s_server_member,
	/*m_pfnGetInterface*/	 libkrnln_GetInterface_serverex,
	/*m_nElementCount*/		0,
	/*m_pElementBegin*/		NULL,
};






#pragma region servercmd
EXTERN_C void Fn_Server_start(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	pRetData->m_bool = pServer->start();
}

FucInfo Server_start = { {
		/*ccname*/  ("����"),
		/*egname*/  ("start"),
		/*explain*/ ("������������Ĭ��������״̬"),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_BOOL,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/0,
		/*arg lp*/  0,
	} ,Fn_Server_start ,"Fn_Server_start" };
EXTERN_C void Fn_Server_close(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	pRetData->m_bool = pServer->close();
}

FucInfo Server_close = { {
		/*ccname*/  ("�ر�"),
		/*egname*/  ("close"),
		/*explain*/ ("�رշ�����"),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_BOOL,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/0,
		/*arg lp*/  0,
	} ,Fn_Server_close ,"Fn_Server_close" };

EXTERN_C void Fn_Server_is_open(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	pRetData->m_bool = pServer->is_create();
}

FucInfo Server_is_open = { {
		/*ccname*/  ("�Ƿ�������"),
		/*egname*/  ("is_open"),
		/*explain*/ ("�ж��Ƿ��Ѿ�����������"),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_BOOL,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/0,
		/*arg lp*/  0,
	} ,Fn_Server_is_open,"Fn_Server_is_open" };



EXTERN_C void Fn_Server_GetData(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	vector<unsigned char> tempdata = pServer->get_data();
	pRetData->m_pBin = elibkrnln::clone_bin(tempdata.data(), tempdata.size());
}

FucInfo Server_GetData = { {
		/*ccname*/  ("ȡ������"),
		/*egname*/  ("GetData"),
		/*explain*/ (""),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_BIN,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/0,
		/*arg lp*/  0,
	} ,Fn_Server_GetData ,"Fn_Server_GetData" };


EXTERN_C void Fn_Server_GetCline(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	pRetData->m_int = pServer->get_clinet();
}

FucInfo Server_GetCline = { {
		/*ccname*/  ("ȡ�ؿͻ�"),
		/*egname*/  ("GetCline"),
		/*explain*/ ("ȡ�ؿͻ���SOCKET����������ΪΨһ��ʶ��,���ڿ�ͨ���˱�ʶ���ƿͻ�"),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_INT,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/0,
		/*arg lp*/  0,
	} ,Fn_Server_GetCline ,"Fn_Server_GetCline" };

static ARG_INFO Args[] =
{
	{
		/*name*/    "�ͻ�SOCKET",
		/*explain*/ ("ͨ��ȡ�ؿͻ���ȡ��SOKET"),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_INT,
		/*default*/ 0,
		/*state*/   NULL,
	},
	{
		/*name*/    "�����Ͽ�",
		/*explain*/ ("�Ƿ������Ͽ�,Ĭ��Ϊ��"),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_BOOL,
		/*default*/ 0,
		/*state*/   AS_DEFAULT_VALUE_IS_EMPTY,
	}
};

EXTERN_C void Fn_Server_CloseClient(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	auto bnow = elibkrnln::args_to_data<BOOL>(pArgInf, 2);
	pRetData->m_bool = pServer->breakoff(pArgInf[1].m_int, bnow.has_value() ? bnow.value() : FALSE);
}

FucInfo Server_CloseClient = { {
		/*ccname*/  ("�Ͽ��ͻ�"),
		/*egname*/  ("CloseClinet"),
		/*explain*/ ("ȡ�ؿͻ���SOCKET����������ΪΨһ��ʶ��,���ڿ�ͨ���˱�ʶ���ƿͻ�"),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_BOOL,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/2,
		/*arg lp*/  Args,
	} ,Fn_Server_CloseClient ,"Fn_Server_CloseClient" };

static ARG_INFO Args2[] =
{
	{
		/*name*/    "�ͻ�SOCKET",
		/*explain*/ ("ͨ��ȡ�ؿͻ���ȡ��SOKET"),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_INT,
		/*default*/ 0,
		/*state*/   NULL,
	},
	{
		/*name*/    "��������",
		/*explain*/ ("�����͵�����"),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_BIN,
		/*default*/ 0,
		/*state*/   NULL,
	},
	{
		/*name*/    "�Ƿ񷵻�",
		/*explain*/ ("�Ƿ񷵻أ�Ĭ��Ϊ��"),
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*type*/    SDT_BOOL,
		/*default*/ 0,
		/*state*/   AS_DEFAULT_VALUE_IS_EMPTY,
	}
};

EXTERN_C void Fn_Server_Send(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	HWND hWnd = elibkrnln::get_hwnd_from_arg(pArgInf);
	eServer* pServer = (eServer*)GetWindowLongPtrW(hWnd, GWL_USERDATA);
	auto bnow = elibkrnln::args_to_data<BOOL>(pArgInf, 3);
	pRetData->m_bool = pServer->send(pArgInf[1].m_int, ebin2v(pArgInf[2].m_pBin), bnow.has_value() ? bnow.value() : FALSE);
}

FucInfo Server_Send = { {
		/*ccname*/  ("��������"),
		/*egname*/  ("send"),
		/*explain*/ ("��ָ���Ѿ����ӽ����Ŀͻ��������ݡ��ɹ������棬ʧ�ܷ��ؼ١�"),
		/*category*/-1,
		/*state*/   NULL,
		/*ret*/     SDT_BOOL,
		/*reserved*/NULL,
		/*level*/   LVL_HIGH,
		/*bmp inx*/ 0,
		/*bmp num*/ 0,
		/*ArgCount*/3,
		/*arg lp*/  Args2,
	} ,Fn_Server_Send ,"Fn_Server_Send" };


#pragma endregion