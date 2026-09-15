#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <set>
#include <utility>
#include <memory>
#include <map>

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
    void Add_Collider(_int iGroup, class CCollider* pCollider);
    void Update_Collision();
    void Clear_ColliderList();

private:
    using COLLIDER_TOKEN = std::weak_ptr<void>;
    using COLLIDER_TOKEN_PAIR = std::pair<COLLIDER_TOKEN, COLLIDER_TOKEN>;

    struct CColliderTokenPairLess
    {
        bool operator()(const COLLIDER_TOKEN_PAIR& lhs, const COLLIDER_TOKEN_PAIR& rhs) const
        {
            std::owner_less<COLLIDER_TOKEN> lessToken;
            if (lessToken(lhs.first, rhs.first))
                return true;
            if (lessToken(rhs.first, lhs.first))
                return false;
            return lessToken(lhs.second, rhs.second);
        }
    };

    COLLIDER_TOKEN_PAIR Make_ColliderTokenPair(CCollider* pA, CCollider* pB) const;

private:
    list<CCollider*>    m_ColList[COLL_END];
    bool                m_bCheckMatrix[COLL_END][COLL_END];
    std::set<COLLIDER_TOKEN_PAIR, CColliderTokenPairLess> m_setPrevCollisionPairs;

public:
    virtual void Free() override;
};

END

