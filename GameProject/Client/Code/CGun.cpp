#include "pch.h"
#include "CGun.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include <ctime>
#include "CCameraMgr.h"
#include "CBullet.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CImGuiTool.h"
#include "CGraphicDev.h"
#include "CRoomLoadingMgr.h"
#include "CGameStatusMgr.h"

CGun::CGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev),
    m_fLastShotTime(0.f),
    m_fShootRate(0.4f),
    m_fRunningTime(0.f),
    m_fUltimateGauge(0.f),
    m_fSpecialGauge(0.f),
    m_fUltimateTimer(0.f),
    m_fUltimateDuration(10.f),
    m_fSpecialTimer(0.f),
    m_fSpecialDuration(10.f),
    m_bUltimateActivated(false),
    m_bSpecialActivated(false),
    m_iCurBullet(BULLET_DEFAULT),
    m_iDmg(2)
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

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

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

    _vec3	vPos_Gun = vPos_Player + (vRight * 0.2f) + (vForword * 0.1f) + (vUp * -0.6f); // 총Mesh 월드좌표 계산

#pragma region 스킬 사용 및 총알 스위칭

    if (m_bUltimateActivated == true)
    {
        m_fUltimateTimer += fTimeDelta;
        if (m_fUltimateTimer > m_fUltimateDuration)
        {
            m_fUltimateTimer = 0.f;
            m_bUltimateActivated = false;
        }
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_C))
    {
        if ((m_bUltimateActivated == false) && (m_fUltimateGauge >= 1.f))
        {
            m_bUltimateActivated = true;
            m_fUltimateGauge = 0.f;
        }
    }

    if (m_bSpecialActivated == true)
    {
        m_fSpecialTimer += fTimeDelta;
        if (m_fSpecialTimer > m_fSpecialDuration)
        {
            m_fSpecialTimer = 0.f;
            m_bSpecialActivated = false;
        }
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_F))
    {
        if ((m_bSpecialActivated == false) && (m_fSpecialGauge >= 1.f))
        {
            m_bSpecialActivated = true;
            m_fSpecialGauge = 0.f;
        }
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_Q))
    {
        switch (m_iCurBullet)
        {
        case BULLET_DEFAULT:
            m_iCurBullet = BULLET_SMALL;
            break;
        case BULLET_SMALL:
            m_iCurBullet = BULLET_DEFAULT;
            break;
        }
    }

    switch (m_iCurBullet)
    {
    case BULLET_DEFAULT:
        m_fShootRate = 0.3f;
        m_iDmg = 2;
        break;
    case BULLET_SMALL:
        m_fShootRate = 0.15f;
        m_iDmg = 1;
        break;
    }

    _int iDmgMultiplier = 2;
    _float fAtkSpeedMultiplier = 2.f;
    if (m_bUltimateActivated == true) m_iDmg *= iDmgMultiplier;
    if (m_bSpecialActivated == true) m_fShootRate /= fAtkSpeedMultiplier;


#pragma endregion

#pragma region 총알 발사

    m_fLastShotTime += fTimeDelta;

    if ((CDInputMgr::GetInstance()->Mouse_Press(DIM_LB)) && (m_fLastShotTime >= m_fShootRate))
    {
        m_fLastShotTime = 0.f;

        _vec3	vBullet_From = vPos_Gun + (vRight * 0.0f) + (vForword * 0.45f) + (vUp * 0.25f); // 총구 위치
        _vec3	vBullet_To; // 크로스헤어 도달점

#pragma region RayCast를 이용한 조준좌표 계산
        
        pair<_vec3, _vec3> pairMouseRay = Get_MouseRay();
        vector<wstring> vMapKey;
        _float fToDistance = 0.f;

        vMapKey.push_back(L"Environment_Layer");
        // vMapKey.push_back(L"GameLogic_Layer");
        int iRoomCnt = CRoomLoadingMgr::GetInstance()->GetRoomTotalCount();
        for (int i = 0; i < iRoomCnt; ++i)
        {
            wstring wstrLayerTag = L"Room_" + to_wstring(i) + L"_Layer";
            vMapKey.push_back(wstrLayerTag);
        }

        multimap<wstring, CGameObject*> mapCurLayer;
        CLayer* pLayer;
        
        for (auto& iter : vMapKey)
        {
            pLayer = CManagement::GetInstance()->Get_Layer(iter.c_str());
            if (pLayer != nullptr)
            {
                mapCurLayer = pLayer->Get_ObjMap();
                for (auto& pair : mapCurLayer)
                {
                    if (pair.first != L"Player" &&
                        pair.first != L"Gun" &&
                        pair.first != L"Bullet")
                    {
                        CVIBuffer* pTextureCom = dynamic_cast<CVIBuffer*>(pair.second->Get_Component(ID_STATIC, L"Com_Buffer"));
                        if (pTextureCom != nullptr)
                        {
                            CTransform* pTransCom = dynamic_cast<CTransform*>(pair.second->Get_Component(ID_DYNAMIC, L"Com_Transform"));
                            if (pTransCom != nullptr)
                            {
                                LPDIRECT3DVERTEXBUFFER9 pVB;
                                LPDIRECT3DINDEXBUFFER9  pIB;
                                D3DFORMAT               idxFmt;
                                VTXSTRUCTTYPE			vtxStructType;
                                _ulong                  dwVtxCnt;
                                _ulong                  dwTriCnt;
                                pTextureCom->Get_VIInfo(pVB, pIB, vtxStructType, idxFmt, dwVtxCnt, dwTriCnt);
                                _matrix matWorld = *pTransCom->Get_World();
                                _vec3 vCamPos = vPos_Player - (vForword * 0.5f);

                                switch (vtxStructType)
                                {
                                case VTXSTRUCT_COL:
                                {
                                    /// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
                                    VTXCOL* pVertex = NULL;
                                    switch (idxFmt)
                                    {
                                    case D3DFMT_INDEX16:
                                    {
                                        INDEX16* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    case D3DFMT_INDEX32:
                                    {
                                        INDEX32* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    }
                                    break;
                                }
                                case VTXSTRUCT_TEX:
                                {
                                    VTXTEX* pVertex = NULL;
                                    switch (idxFmt)
                                    {
                                    case D3DFMT_INDEX16:
                                    {
                                        INDEX16* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    case D3DFMT_INDEX32:
                                    {
                                        INDEX32* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    }
                                    break;
                                }

                                case VTXSTRUCT_TEX_NORMAL_NONE : 
                                {
                                    VTXTEX_NORMAL_NONE* pVertex = NULL;
                                    switch (idxFmt)
                                    {
                                    case D3DFMT_INDEX16:
                                    {
                                        INDEX16* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    case D3DFMT_INDEX32:
                                    {
                                        INDEX32* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    }
                                    break;
                                }
                                case VTXSTRUCT_CUBE:
                                {
                                    VTXCUBE* pVertex = NULL;
                                    switch (idxFmt)
                                    {
                                    case D3DFMT_INDEX16:
                                    {
                                        INDEX16* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    case D3DFMT_INDEX32:
                                    {
                                        INDEX32* pIndex = NULL;
                                        CheckVtxIntersect(pVB, pIB, pVertex, pIndex, dwVtxCnt, dwTriCnt, fToDistance, vBullet_To, matWorld, vCamPos, vForword);
                                        break;
                                    }
                                    }
                                    break;
                                }
                                }
                                
                            }
                        }
                    }
                }
            }
        }
        
#pragma endregion

        _vec3	vBullet_Look = vBullet_To - vBullet_From;
        D3DXVec3Normalize(&vBullet_Look, &vBullet_Look);

        pGameObject = CBullet::Create(m_pGraphicDev, &vBullet_From, &vBullet_Look);
        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"Bullet", pGameObject);
    }

#pragma endregion

    _matrix matWorld;
    D3DXMatrixIdentity(&matWorld);
    
    _matrix matScale;

    D3DXMatrixScaling(&matScale, 0.6f, 0.6f, 0.6f);

    D3DXVec3TransformCoord(&vUp, &vUp, &matScale);
    D3DXVec3TransformCoord(&vRight, &vRight, &matScale);
    D3DXVec3TransformCoord(&vForword, &vForword, &matScale);


#pragma region 총알 발사 시 반동

    _matrix matAxis;

    if (m_iCurBullet == BULLET_DEFAULT)
    {
        if (m_fLastShotTime < 0.05f)
        {
            D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(-m_fLastShotTime * 800.f));
        }
        else if (m_fLastShotTime < 0.2f)
        {
            D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(-60 + m_fLastShotTime * 300.f));
        }
        else
        {
            D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(0.f));
        }
    }
    else if (m_iCurBullet == BULLET_SMALL)
    {
        if (m_fLastShotTime < 0.025f)
        {
            D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(-m_fLastShotTime * 800.f));
        }
        else if (m_fLastShotTime < 0.1f)
        {
            D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(-30 + m_fLastShotTime * 300.f));
        }
        else
        {
            D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(0.f));
        }
    }

    D3DXVec3TransformCoord(&vUp, &vUp, &matAxis); //
    D3DXVec3TransformCoord(&vForword, &vForword, &matAxis); //

    _vec3	vBullet_From = vPos_Gun + (vRight * 0.0f) + (vForword * 0.8f) + (vUp * 0.4f);
    _vec3   vRotAxis = vPos_Gun - vBullet_From;
    _matrix matRotAxis;

    D3DXVec3Normalize(&vRotAxis, &vRotAxis);

#pragma endregion

#pragma region 이동 시 좌우 총 떨림
    if (CDInputMgr::GetInstance()->Key_Press(DIK_W) ||
        CDInputMgr::GetInstance()->Key_Press(DIK_A) ||
        CDInputMgr::GetInstance()->Key_Press(DIK_S) ||
        CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        m_fRunningTime += fTimeDelta;
    }

    if (m_fLastShotTime > m_fShootRate)
    {
        if (CDInputMgr::GetInstance()->Key_Press(DIK_W) ||
            CDInputMgr::GetInstance()->Key_Press(DIK_A) ||
            CDInputMgr::GetInstance()->Key_Press(DIK_S) ||
            CDInputMgr::GetInstance()->Key_Press(DIK_D))
        {
            _float fAnimationDelta = fmod(m_fRunningTime, 0.8f);
            _float fAnimationSpeed = 25.0f;
            if (CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT))
            {
                fAnimationSpeed = 50.f;
            }

            if (fAnimationDelta > 0.6f)
            {
                D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis, D3DXToRadian((0.8f - fAnimationDelta) * fAnimationSpeed));
            }
            else if (fAnimationDelta > 0.2f)
            {
                D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis, D3DXToRadian((fAnimationDelta - 0.4f) * fAnimationSpeed));
            }
            else
            {
                D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis, D3DXToRadian(-fAnimationDelta * fAnimationSpeed));
            }
            D3DXVec3TransformCoord(&vUp, &vUp, &matRotAxis); //
            D3DXVec3TransformCoord(&vForword, &vForword, &matRotAxis); //
            D3DXVec3TransformCoord(&vRight, &vRight, &matRotAxis); //
        }
        else
        {
            m_fRunningTime = 0.f;
        }
    }

#pragma endregion

    memcpy(&matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_UP][0], &vUp, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_LOOK][0], &vForword, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_POS][0], &vPos_Gun, sizeof(_vec3));

    m_pTransformCom->Set_World(&matWorld);

}

void CGun::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
#ifdef _DEBUG
    //RenderImGui();
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

void CGun::UpdateUltimateGauge(_float fAmount)
{
    if (fAmount > 0)
    {
        if (m_fUltimateGauge + fAmount > 1.f)
        {
            m_fUltimateGauge = 1.f;
        }
        else
        {
            m_fUltimateGauge += fAmount;
        }
    }
    else if (fAmount < 0)
    {
        if (m_fUltimateGauge + fAmount <= 0)
        {
            m_fUltimateGauge = 0.f;
        }
        else
        {
            m_fUltimateGauge += fAmount;
        }
    }
    CGameStatusMgr::GetInstance()->SetUltimateGauge(m_fUltimateGauge);
}

void CGun::UpdateSpecialGauge(_float fAmount)
{
    if (fAmount > 0)
    {
        if (m_fSpecialGauge + fAmount > 1.f)
        {
            m_fSpecialGauge = 1.f;
        }
        else
        {
            m_fSpecialGauge += fAmount;
        }
    }
    else if (fAmount < 0)
    {
        if (m_fSpecialGauge + fAmount <= 0)
        {
            m_fSpecialGauge = 0.f;
        }
        else
        {
            m_fSpecialGauge += fAmount;
        }
    }
    CGameStatusMgr::GetInstance()->SetSpecialAttackGauge(m_fSpecialGauge);
}

void CGun::Free()
{
    CGameObject::Free();
}

void CGun::RenderImGui()
{
    /* ImGui */
    ImGui::Begin("Gun Debug Information");

    ImGui::Text("LAST FIRE TIME :  %.2f SECOND AGO", m_fLastShotTime);
    const char* cCurBullet = NULL;
    switch (m_iCurBullet)
    {
    case BULLET_DEFAULT:
        cCurBullet = "BULLET_DEFAULT";
        break;
    case BULLET_SMALL:
        cCurBullet = "BULLET_SMALL";
        break;
    }
    ImGui::Text("CURRENT BULLET : %s", cCurBullet);
    ImGui::Text("CURRENT BULLET DMG : %i", m_iDmg);
    ImGui::Text("CURRENT BULLET RATE : %.2f SEC / FIRE", m_fShootRate);
    ImGui::Text("RUNNING TIME : %.2f SEC", m_fRunningTime);
    ImGui::End();
}



pair<_vec3, _vec3> CGun::Get_MouseRay() // 마우스 월드변환
{
    POINT		ptMouse{};
    _vec3	    vMousePos;

    LPDIRECT3DDEVICE9 pGraphicDev = CGraphicDev::GetInstance()->Get_GraphicDev();

    //GetCursorPos(&ptMouse);
    //ScreenToClient(g_hWnd, &ptMouse);        현재 조준점 고정중이므로 검사 제외
    
    ptMouse.x = WINCX / 2;
    ptMouse.y = WINCY / 2;


    D3DVIEWPORT9	ViewPort;
    ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
    pGraphicDev->GetViewport(&ViewPort);

    // 뷰 포트 영역 -> 투영 영역
    vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
    vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;

    // 투영 영역 - > 뷰 스페이스 영역
    D3DXMATRIX	matProj;
    pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixInverse(&matProj, 0, &matProj);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

    // 뷰 스페이스 -> 월드 영역

    D3DXMATRIX	matView;
    pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, 0, &matView);

    _vec3	vRayPos{ 0.f, 0.f, 0.f };
    _vec3	vRayDir = vMousePos - vRayPos;

    D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
    D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

    pair<_vec3, _vec3> pairMouseRay;
    pairMouseRay.first = vRayPos;
    pairMouseRay.second = vRayDir;

    return pairMouseRay;
}