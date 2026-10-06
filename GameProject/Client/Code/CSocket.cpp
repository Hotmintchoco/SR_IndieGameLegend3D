#include "pch.h"
#include "CSocket.h"
#include "CTransform.h"

CSocket::CSocket(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pParentTransform)
    : CComponent(pGraphicDev), m_pParentTransform(pParentTransform)
{
    D3DXMatrixIdentity(&m_matOffset);
}

CSocket::~CSocket()
{
}

_int CSocket::Update_Component(_float fTimeDelta)
{
    return S_OK;
}

void CSocket::LateUpdate_Component()
{
}

void CSocket::UpdateSocket()
{
    bool bValid = m_pTargetTransform && m_pParentTransform && m_pTargetTransform->IsLocal();
    if (!bValid) return;

    _matrix mat;
    D3DXMatrixIdentity(&mat);
    mat = m_matOffset * (*m_pParentTransform->Get_World());

    m_pTargetTransform->WorldMatrixPropagation(mat);
}

CSocket* CSocket::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pParentTransform)
{
    return new CSocket(pGraphicDev, pParentTransform);
}

void CSocket::Free()
{
    CComponent::Free();
}
