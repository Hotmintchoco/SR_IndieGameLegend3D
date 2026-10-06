#pragma once

#include "CBase.h"
#include "Engine_Define.h"
#include <atomic>

struct TRoomData;

class CLoading : public CBase
{
public:
	enum LOADINGID { LOADING_STAGE, LOADING_BOSS, LOADING_END };

private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	LOADINGID	  Get_LoadingID()		{ return m_eLoadingID; }
    _bool Get_Finish() const { return m_bFinish.load(); }
    _bool Get_Failed() const { return m_bFailed.load(); }
    _float Get_Progress() const { return m_fProgress.load(); }
	CRITICAL_SECTION* Get_Crt()			{ return &m_Crt; }

public:
	HRESULT		Ready_Loading(LOADINGID eID);
	_uint		Loading_Stage();

public:
	static unsigned int CALLBACK Thread_Main(void* pArg);

private:
	HRESULT ParseRoomData();
	HRESULT ParseSingleRoom(int iRoomIdx);
	HRESULT ParseDefaultRoom(int iRoomIdx);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	
	HANDLE				m_hThread = nullptr;
	LOADINGID			m_eLoadingID;

	CRITICAL_SECTION	m_Crt;
    std::atomic_bool m_bFinish{ false };
    std::atomic_bool m_bFailed{ false };
    std::atomic<float> m_fProgress{ 0.f };


public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);

private:
	virtual void Free();

	
};

