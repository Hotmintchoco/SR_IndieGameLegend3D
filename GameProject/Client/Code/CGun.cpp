#include "pch.h"
#include "CGun.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include <ctime>
#include "CCameraMgr.h"
#include "CBUllet.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CImGuiTool.h"

CGun::CGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_fLastShotTime(0.f), m_fReloadTime(0.f), m_iAmmo(99999)
{
}


CGun::~CGun()
{
}

HRESULT CGun::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CGun::Update_GameObject(const _float& fTimeDelta)
{
#pragma region 수정
    /*
    CGameObject* pGameObject = nullptr;
    _vec3	vPos_Player;
    _vec3	vRight;
    _vec3	vForword;
    _vec3	vUp;
    static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_POS, &vPos_Player);
    static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_RIGHT, &vRight);
    CCameraMgr::GetInstance()->Get_CamLook(&vForword);
    D3DXVec3Cross(&vUp, &vForword, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    _vec3	vPos_Gun = vPos_Player + (vRight * 0.3f) + (vForword * 0.5f) + (vUp * -0.7f);


    m_pTransformCom->m_vInfo[INFO_UP] = vUp;
    m_pTransformCom->m_vInfo[INFO_LOOK] = vForword;
    m_pTransformCom->m_vInfo[INFO_POS] = vPos_Gun;
    m_pTransformCom->m_vInfo[INFO_RIGHT] = vRight;


    */

    /*
    //[DEBUG] Start 확인용 움직임
    m_pTransformCom->Rotation(ROT_Y, 30.f * fTimeDelta);
    _vec3 tmp{ 0.f, 1.f, 0.f };
    float t = (float)clock() / CLOCKS_PER_SEC;
    m_pTransformCom->Move_Pos(&tmp, 0.2 * sinf(t), fTimeDelta);
    // [DEBUG] End 확인용 움직임
    */
#pragma endregion

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

    return iExit;
}

void CGun::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    CTransform* pPlayerTrans = static_cast<CTransform*>(CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    CGameObject* pGameObject = nullptr;
    _vec3	vPos_Player;
    _vec3	vRight;
    _vec3	vForword;
    _vec3	vUp;
    static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_POS, &vPos_Player);
    static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_RIGHT, &vRight);
    CCameraMgr::GetInstance()->Get_CamLook(&vForword);
    D3DXVec3Cross(&vUp, &vForword, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    _vec3	vPos_Gun = vPos_Player + (vRight * 0.3f) + (vForword * 0.5f) + (vUp * -0.7f);

    _matrix matWorld;
    D3DXMatrixIdentity(&matWorld);
    memcpy(&matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_UP][0], &vUp, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_LOOK][0], &vForword, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_POS][0], &vPos_Gun, sizeof(_vec3));
    m_pTransformCom->Set_World(&matWorld);

    _int iAmmoMax = 13;
    _float fShootCoolTime = 0.4f;
    _float fReloadCoolTime = 1.f;
    m_fLastShotTime += fTimeDelta;

    if (m_fReloadTime > fReloadCoolTime)
    {
        m_fReloadTime = 0.f;
        m_fLastShotTime = fShootCoolTime;
        m_iAmmo = iAmmoMax;
    }
    else if (m_fReloadTime > 0.f)
    {
        m_fReloadTime += fTimeDelta;
    }

    if ((CDInputMgr::GetInstance()->Mouse_Press(DIM_LB)) && (m_fLastShotTime >= fShootCoolTime) && (m_iAmmo > 0) && (m_fReloadTime == 0))
    {
        m_fLastShotTime = 0.f;
        m_iAmmo--;

        _vec3	vBullet_From = vPos_Gun + (vRight * 0.0f) + (vForword * 0.0f) + (vUp * 0.3f);
        _vec3	vBullet_To = vPos_Player + (vForword * 10.f); // 크로스헤어 도달점

        _vec3	vBullet_Look = vBullet_To - vBullet_From;
        D3DXVec3Normalize(&vBullet_Look, &vBullet_Look);

        pGameObject = CBullet::Create(m_pGraphicDev, &vBullet_From, &vBullet_Look);
        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"Bullet", pGameObject);
    }
    else if ((m_iAmmo == 0))
    {
        if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
        {
            m_fReloadTime += fTimeDelta;
        }
    }

}

void CGun::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
#ifdef _DEBUG
    // RenderImGui();
#endif
}

HRESULT CGun::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}

CGun* CGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGun* pGun = new CGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CGun::Free()
{
    CGameObject::Free();
}

void CGun::RenderImGui()
{
    /* ImGui */
    ImGui::Begin("Gun Debug Information");

    ImGui::Text("AMMO :  %.2i / 13", m_iAmmo);
    if (m_iAmmo == 0)
    {
    	if (m_fReloadTime == 0.f)
    	{
    		if ((int)(m_fLastShotTime * 5) % 2 == 0)
    		{
                ImGui::Text("OUT OF AMMO. PRESS R TO RELOAD.");
    		}
    	}
    	else
    	{
            ImGui::Text("Reloading...");
        }
    }
    ImGui::End();
}






//_vec2	vPos_DebugUI_Ammo{ 100.f, 120.f };
//_vec2	vPos_DebugUI_Reload{ 100.f, 140.f };

//wstring wAmmoInfo = L"AMMO : " + to_wstring(m_iAmmo) + L" / 13";
//CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wAmmoInfo.c_str(), &vPos_DebugUI_Ammo, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

//if (m_iAmmo == 0)
//{
//	if (m_fReloadTime == 0.f)
//	{
//		if ((int)(m_fLastShotTime * 5) % 2 == 0)
//		{
//			CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", L"OUT OF AMMO. PRESS R TO RELOAD.", &vPos_DebugUI_Reload, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
//		}
//	}
//	else
//	{
//		CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", L"Reloading...", &vPos_DebugUI_Reload, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
//	}
//}

