#pragma once

#include "CBase.h"
#include "Engine_Define.h"

struct TMapData;

class CLoading : public CBase
{
public:
	enum LOADINGID { LOADING_STAGE, LOADING_BOSS, LOADING_END };

private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	const _tchar* Get_String()			{ return m_szLoading; }
	LOADINGID	  Get_LoadingID()		{ return m_eLoadingID; }
	_bool		  Get_Finish()			{ return m_bFinish; }
	CRITICAL_SECTION* Get_Crt()			{ return &m_Crt; }

public:
	HRESULT		Ready_Loading(LOADINGID eID);
	_uint		Loading_Stage();

public:
	static unsigned int CALLBACK Thread_Main(void* pArg);
	static void ParseMapData(TMapData* pOut);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_tchar				m_szLoading[128];
	
	HANDLE				m_hThread;
	LOADINGID			m_eLoadingID;

	CRITICAL_SECTION	m_Crt;
	_bool				m_bFinish;


public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);

private:
	virtual void Free();

	
};

