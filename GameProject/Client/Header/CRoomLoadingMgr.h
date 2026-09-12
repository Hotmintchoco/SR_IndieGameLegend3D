#pragma once

#include "Engine_Define.h"
#include "Define.h"
#include <unordered_map>

class CAbstractFactory;

class CRoomLoadingMgr
{
	DECLARE_SINGLETON(CRoomLoadingMgr);

private:
	explicit CRoomLoadingMgr();
	virtual ~CRoomLoadingMgr();

public:
	void RegisterRoomData(int iIndex, const TRoomData& tData);
	HRESULT LoadRoomData(int iIndex);

	inline const _vec3 GetCenterRoomPosition() { return m_vCenterRoomPosition; }
	inline const _vec3 GetInnerRoomSize() { return m_vInnerRoomSize; }
	inline const _vec3 GetOuterRoomSize() { return m_vOuterRoomSize; }
	inline const int GetRoomTotalCount() { return m_iRoomRowCount * m_iRoomColCount; }
	inline const int GetRoomRowCount() { return m_iRoomRowCount; }
	inline const int GetRoomColCount() { return m_iRoomColCount; }

private:
	CAbstractFactory* m_pFactory = nullptr;

	unordered_map<int, TRoomData> m_mapRoomData;
	const _vec3 m_vCenterRoomPosition = _vec3{ 60.f, 0.f, 60.f };
	const _vec3 m_vInnerRoomSize = _vec3{ 13.f, 0.f, 11.f };
	const _vec3 m_vOuterRoomSize = _vec3{ 15.f, 0.f, 13.f };
	const int m_iRoomRowCount = 5;
	const int m_iRoomColCount = 5;

private:
	virtual void Free();

};

