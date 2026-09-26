#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <unordered_map>
#include <memory>

BEGIN(Engine)

class CCollider;

class ENGINE_DLL CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr();

public:
	void Check_Group(_int iLeft, _int iRight);
	void Add_Collider(_int iGroup, CCollider* pCollider);
	void Update_Collision();
	void Clear_ColliderList();

private:
	struct COLLIDER_PAIR_INFO
	{
		CCollider* pLeft;
		CCollider* pRight;
		std::weak_ptr<void> wLeftToken;
		std::weak_ptr<void> wRightToken;
	};

	ULONGLONG Make_ColliderPairKey(CCollider* pLeft, CCollider* pRight) const;

private:
	list<CCollider*>	m_ColList[COLL_END];
	bool				m_bCheckMatrix[COLL_END][COLL_END];

	std::unordered_map<ULONGLONG, COLLIDER_PAIR_INFO> m_mapPrevCollisionPairs;

public:
	virtual void Free() override;
};

END

