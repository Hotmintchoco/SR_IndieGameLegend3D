#include "CCollider.h"
#include "CGameObject.h"
#include "CCollisionMgr.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
	ZeroMemory(&m_bCheckMatrix, sizeof(bool) * COLL_END * COLL_END);
}

CCollisionMgr::~CCollisionMgr()
{
	Free();
}

void CCollisionMgr::Check_Group(_int iLeft, _int iRight)
{
	m_bCheckMatrix[iLeft][iRight] = true;
	m_bCheckMatrix[iRight][iLeft] = true;
}

void CCollisionMgr::Add_Collider(_int iGroup, CCollider* pCollider)
{
	if (nullptr == pCollider)
		return;

	pCollider->Set_CollisionID(iGroup);
	m_ColList[iGroup].push_back(pCollider);
}

ULONGLONG CCollisionMgr::Make_ColliderPairKey(CCollider* pLeft, CCollider* pRight) const
{
	_uint iLeftId = pLeft->GetID();
	_uint iRightId = pRight->GetID();

	if (iLeftId > iRightId)
	{
		const _uint iTemp = iLeftId;
		iLeftId = iRightId;
		iRightId = iTemp;
	}

	return (static_cast<ULONGLONG>(iLeftId) << 32) | iRightId;
}

void CCollisionMgr::Update_Collision()
{
	unordered_map<ULONGLONG, COLLIDER_PAIR_INFO> mapCurCollisionPairs;

	for (_uint i = 0; i < COLL_END; ++i)
	{
		for (CCollider* pCollider : m_ColList[i])
		{
			if (nullptr != pCollider)
				pCollider->Set_IsCollided(false);
		}
	}

	for (_uint i = 0; i < COLL_END; ++i)
	{
		for (_uint j = i; j < COLL_END; ++j)
		{
			if (!m_bCheckMatrix[i][j])
				continue;

			auto& leftList = m_ColList[i];
			auto& rightList = m_ColList[j];

			for (auto iterLeft = leftList.begin(); iterLeft != leftList.end(); ++iterLeft)
			{
				CCollider* pLeft = *iterLeft;
				if (nullptr == pLeft || !pLeft->Get_IsActive())
					continue;

				auto iterRight = rightList.begin();

				// 같은 그룹은 자기 자신과 중복 Pair를 제외한다.
				if (i == j)
				{
					iterRight = iterLeft;
					++iterRight;
				}

				for (; iterRight != rightList.end(); ++iterRight)
				{
					CCollider* pRight = *iterRight;
					if (nullptr == pRight || !pRight->Get_IsActive())
						continue;

					if (!pLeft->Intersect(pRight))
						continue;

					pLeft->Set_IsCollided(true);
					pRight->Set_IsCollided(true);

					const ULONGLONG ullPairKey = Make_ColliderPairKey(pLeft, pRight);

					COLLIDER_PAIR_INFO pairInfo;
					pairInfo.pLeft = pLeft;
					pairInfo.pRight = pRight;
					pairInfo.wLeftToken = pLeft->GetToken();
					pairInfo.wRightToken = pRight->GetToken();

					const bool bFirstPairThisFrame = mapCurCollisionPairs.emplace(ullPairKey, pairInfo).second;
					if (!bFirstPairThisFrame)
						continue;

					if (m_mapPrevCollisionPairs.find(ullPairKey) == m_mapPrevCollisionPairs.end())
					{
						pLeft->OnCollisionEnter(pRight);
						pRight->OnCollisionEnter(pLeft);
					}
					else
					{
						pLeft->OnCollisionStay(pRight);
						pRight->OnCollisionStay(pLeft);
					}
				}
			}
		}
	}

	for (const auto& prevPair : m_mapPrevCollisionPairs)
	{
		if (mapCurCollisionPairs.find(prevPair.first) != mapCurCollisionPairs.end())
			continue;

		const COLLIDER_PAIR_INFO& pairInfo = prevPair.second;

		// 이미 파괴된 Collider에는 Exit를 호출하지 않는다.
		if (pairInfo.wLeftToken.expired() || pairInfo.wRightToken.expired())
			continue;

		pairInfo.pLeft->OnCollisionExit(pairInfo.pRight);
		pairInfo.pRight->OnCollisionExit(pairInfo.pLeft);
	}

	m_mapPrevCollisionPairs.swap(mapCurCollisionPairs);
}

void CCollisionMgr::Clear_ColliderList()
{
	for (_uint i = 0; i < COLL_END; ++i)
		m_ColList[i].clear();
}

void CCollisionMgr::Free()
{
	for (_uint i = 0; i < COLL_END; ++i)
		m_ColList[i].clear();

	m_mapPrevCollisionPairs.clear();
}
