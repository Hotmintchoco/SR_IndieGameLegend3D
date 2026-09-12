#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionMgr : public CBase
{
    DECLARE_SINGLETON(CCollisionMgr)

private:
    explicit CCollisionMgr();
    virtual ~CCollisionMgr();

public:
    // 서로 충돌 검사를 하도록 설정
    void Check_Group(COLLISIONID eLeft, COLLISIONID eRight);

    // 매 프레임 객체들이 자신의 콜라이더를 매니저에 등록하는 함수
    void Add_Collider(COLLISIONID eGroup, class CCollider* pCollider);

    // 매 프레임 실제 충돌 검사를 수행하는 핵심 함수
    void Update_Collision();

    // 매 프레임 끝날 때 리스트를 비워줌
    void Clear_ColliderList();

private:
    list<CCollider*>    m_ColList[COLL_END];

    // 두 그룹 간의 충돌 검사 여부를 체크하는 2차원 매트릭스
    bool                m_bCheckMatrix[COLL_END][COLL_END];

public:
    virtual void Free() override;
};

END

