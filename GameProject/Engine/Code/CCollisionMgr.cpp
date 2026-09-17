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
	m_ColList[iGroup].push_back(pCollider);
}

CCollisionMgr::COLLIDER_TOKEN_PAIR CCollisionMgr::Make_ColliderTokenPair(CCollider* pA, CCollider* pB) const
{
    const std::shared_ptr<void> spA = pA->GetToken();
    const std::shared_ptr<void> spB = pB->GetToken();

    std::owner_less<std::shared_ptr<void>> lessShared;
    if (lessShared(spA, spB))
        return { COLLIDER_TOKEN(spA), COLLIDER_TOKEN(spB) };

    return { COLLIDER_TOKEN(spB), COLLIDER_TOKEN(spA) };
}

void CCollisionMgr::Update_Collision()
{
    std::set<COLLIDER_TOKEN_PAIR, CColliderTokenPairLess> setCurCollisionPairs;
    std::set<CCollider*> setCurColliders;
    std::map<std::shared_ptr<void>, CCollider*, std::owner_less<std::shared_ptr<void>>> mapLiveColliders;

    for (_uint i = 0; i < COLL_END; ++i)
    {
        for (_uint j = i; j < COLL_END; ++j)
        {
            if (!m_bCheckMatrix[i][j])
                continue;

            auto& LeftList = m_ColList[i];
            auto& RightList = m_ColList[j];

            for (auto& pColLeft : LeftList)
            {
                mapLiveColliders[pColLeft->GetToken()] = pColLeft;

                for (auto& pColRight : RightList)
                {
                    mapLiveColliders[pColRight->GetToken()] = pColRight;

					// ������ �ݶ��̴��� ���ϴ� ���� ����
                    if (pColLeft == pColRight)
                        continue;

					// ������ �׷� ������ �� ��, �ߺ� �񱳸� �����ϱ� ���� ������ ���
                    if (i == j && pColRight < pColLeft)
                        continue;

					// Ȱ��ȭ ���� Ȯ��
                    if (!pColLeft->Get_IsActive() || !pColRight->Get_IsActive())
                        continue;

					// �浹 ���� Ȯ��
                    if (!pColLeft->Intersect(pColRight))
                        continue;

                    const COLLIDER_TOKEN_PAIR tPair = Make_ColliderTokenPair(pColLeft, pColRight);

                    if (!setCurCollisionPairs.insert(tPair).second)
                        continue;

                    setCurColliders.insert(pColLeft);
                    setCurColliders.insert(pColRight);

                    if (m_setPrevCollisionPairs.find(tPair) == m_setPrevCollisionPairs.end())
                    {
                        pColLeft->OnCollisionEnter(pColRight);
                        pColRight->OnCollisionEnter(pColLeft);
                    }
                    else
                    {
                        pColLeft->OnCollisionStay(pColRight);
                        pColRight->OnCollisionStay(pColLeft);
                    }
                }
            }
        }
    }

    for (const auto& tPrevPair : m_setPrevCollisionPairs)
    {
        if (setCurCollisionPairs.find(tPrevPair) != setCurCollisionPairs.end())
            continue;

        const std::shared_ptr<void> spLeft = tPrevPair.first.lock();
        const std::shared_ptr<void> spRight = tPrevPair.second.lock();

        if (!spLeft || !spRight)
            continue; // �̹� �ı��� �ݶ��̴��� ���� ����

        auto itLeft = mapLiveColliders.find(spLeft);
        auto itRight = mapLiveColliders.find(spRight);

        if (itLeft == mapLiveColliders.end() || itRight == mapLiveColliders.end())
            continue;

        itLeft->second->OnCollisionExit(itRight->second);
        itRight->second->OnCollisionExit(itLeft->second);
    }

    m_setPrevCollisionPairs = std::move(setCurCollisionPairs);

    for (_uint i = 0; i < COLL_END; ++i)
    {
        for (auto& pCol : m_ColList[i])
        {
            const bool bCollided = (setCurColliders.find(pCol) != setCurColliders.end());
            pCol->Set_IsCollided(bCollided);
        }
    }
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

	m_setPrevCollisionPairs.clear();
}
