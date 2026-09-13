#include "pch.h"
#include "CRoomLoadingMgr.h"

IMPLEMENT_SINGLETON(CRoomLoadingMgr);

CRoomLoadingMgr::CRoomLoadingMgr()
{
}

CRoomLoadingMgr::~CRoomLoadingMgr()
{
	Free();
}

void CRoomLoadingMgr::RegisterRoomData(int iIndex, const TRoomData& tData)
{
	m_mapRoomData.insert({ iIndex, tData });
}

void CRoomLoadingMgr::Free()
{
}