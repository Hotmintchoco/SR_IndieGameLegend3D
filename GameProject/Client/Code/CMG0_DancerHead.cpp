#include "pch.h"
#include "CMG0_DancerHead.h"

CMG0_DancerHead::CMG0_DancerHead(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_DancerPart(pGraphicDev)
{
}

CMG0_DancerHead::~CMG0_DancerHead()
{
}

HRESULT CMG0_DancerHead::Ready_GameObject()
{
    return CMG0_DancerPart::Ready_GameObject(L"Proto_Player_Head_Vertex");
}

CMG0_DancerHead* CMG0_DancerHead::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_DancerHead* pPart = new CMG0_DancerHead(pGraphicDev);

    if (FAILED(pPart->Ready_GameObject()))
    {
        Safe_Release(pPart);
        MSG_BOX("CMG0_DancerHead Create Failed");
        return nullptr;
    }

    return pPart;
}

void CMG0_DancerHead::Free()
{
    CMG0_DancerPart::Free();
}
