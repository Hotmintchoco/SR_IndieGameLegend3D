#include "CCollisionMgr.h"
#include "CCollider.h"

IMPLEMENT_SINGLETON(CCollisionMgr)


CCollisionMgr::CCollisionMgr()
{
	ZeroMemory(&m_bCheckMatrix, sizeof(bool) * COLL_END * COLL_END);
}

CCollisionMgr::~CCollisionMgr()
{
	Free();
}

void CCollisionMgr::Check_Group(COLLISIONID eLeft, COLLISIONID eRight)
{
	m_bCheckMatrix[eLeft][eRight] = true;
	m_bCheckMatrix[eRight][eLeft] = true;
}

void CCollisionMgr::Add_Collider(COLLISIONID eGroup, CCollider* pCollider)
{
	m_ColList[eGroup].push_back(pCollider);
}

void CCollisionMgr::Update_Collision()
{
	for (_uint i = 0; i < COLL_END; ++i)
	{
		for (_uint j = i; j < COLL_END; ++j)
		{
            // 두 그룹이 충돌 검사 대상이 아니라면 패스
            if (!m_bCheckMatrix[i][j])
                continue;

            // 검사 대상이라면 두 리스트의 콜라이더들을 1:1로 비교
            auto& LeftList = m_ColList[i];
            auto& RightList = m_ColList[j];

            for (auto& pColLeft : LeftList)
            {
                for (auto& pColRight : RightList)
                {
                    // 자기 자신과의 충돌은 제외
                    if (pColLeft == pColRight)
                        continue;

                    // 생성후 위치가 갱신되지 않은 콜라이더는 패스
                    if (!pColLeft->Get_IsPos() || !pColRight->Get_IsPos())
						continue;

                    // 실제 교차(충돌) 검사
                    if (pColLeft->Intersect(pColRight))
                    {
                        // 충돌(또는 트리거) 발생 시 로직 처리
                        CGameObject* pLeftObj = pColLeft->Get_Owner();
                        CGameObject* pRightObj = pColRight->Get_Owner();

                        // TODO: pLeftObj->OnCollisionEnter(pRightObj) 등 호출
                    }
                }
            }
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


}
