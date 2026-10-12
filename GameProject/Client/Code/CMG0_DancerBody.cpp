#include "pch.h"
#include "CMG0_DancerBody.h"

CMG0_DancerBody::CMG0_DancerBody(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_DancerPart(pGraphicDev)
{
}

CMG0_DancerBody::~CMG0_DancerBody()
{
}

HRESULT CMG0_DancerBody::Ready_GameObject()
{
    return CMG0_DancerPart::Ready_GameObject(L"Proto_Player_Body_Vertex");
}

CMG0_DancerBody* CMG0_DancerBody::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_DancerBody* pPart = new CMG0_DancerBody(pGraphicDev);

    if (FAILED(pPart->Ready_GameObject()))
    {
        Safe_Release(pPart);
        MSG_BOX("CMG0_DancerBody Create Failed");
        return nullptr;
    }

    return pPart;
}

void CMG0_DancerBody::Free()
{
    CMG0_DancerPart::Free();
}
