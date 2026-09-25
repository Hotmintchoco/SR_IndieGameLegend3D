#include "pch.h"
#include "CRapidGun.h"
#include "CImGuiTool.h"
#include "CProtoMgr.h"
#include "CDefaultBullet.h"
#include "CSoundMgr.h"
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"

CRapidGun::CRapidGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CWeapon(pGraphicDev)
{
}

CRapidGun::~CRapidGun()
{
}

HRESULT CRapidGun::Ready_GameObject()
{
    if (FAILED(CWeapon::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);

    return S_OK;
}

_int CRapidGun::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    return iExit;
}

void CRapidGun::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CWeapon::LateUpdate_GameObject(fTimeDelta);
}

void CRapidGun::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_bSpecialAttackSwitchOn)
    {
        m_pTextureCom->Set_Texture(1);
    }
    else
    {
        m_pTextureCom->Set_Texture(0);
    }

    m_pBufferCom->Render_Buffer();

    // RenderEditorPanel();
}

void CRapidGun::RenderEditorPanel()
{
    ImGui::Begin("Gun");

    ImGui::SeparatorText("Transform");
    ImGui::DragFloat3("Scale", &m_vScaleLocal.x, 0.01f, 0.001f, 100.f);
    ImGui::DragFloat3("Position", &m_vPositionLocal.x, 0.01f);
    ImGui::DragFloat3("Rotation", &m_vRotationLocal.x, 0.5f, -360.f, 360.f);

    ImGui::SeparatorText("Animation");
    ImGui::DragFloat("Move Cycle", &m_fMoveAnimationFrequency, 0.01f, 0.05f, 5.f, "%.2f s");
    ImGui::DragFloat("Horizontal Move", &m_fHorizontalMove, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Quadratic A", &m_fQuadraticA, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Max Recoil Angle", &m_fMaxRecoilAngle, 0.5f, -90.f, 0.f);
    ImGui::DragFloat("Recoil Damping", &m_fRecoilDamping, 0.05f, 0.f, 20.f);

    ImGui::End();

    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);
}

void CRapidGun::SpecialAttack()
{
    _vec3 vDir = m_vBulletTo - m_vBulletFrom;
    D3DXVec3Normalize(&vDir, &vDir);

    CProjectile* pProjectile = CDefaultBullet::Create(m_pGraphicDev, m_vBulletFrom, vDir);
    CGameStatusMgr::GetInstance()->GetCurrentRoomLayer()->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");

    m_bIsCoolTime = true;
    m_fCoolTimeLeft = m_fSpecialAtkInterval;
    StartShotAnimation();
}

void CRapidGun::UltimateAttack()
{
}

HRESULT CRapidGun::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CRapidGun* CRapidGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRapidGun* pGun = new CRapidGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CRapidGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CRapidGun::Free()
{
    CWeapon::Free();
}
