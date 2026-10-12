#include "pch.h"
#include "CAnamorphicObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CCamera.h"
#include "CClientCameraMgr.h"
#include "IAnamorphicTarget.h"
#include "CLiminalRoomLayer.h"
#include "CManagement.h"
#include "CStage.h"

CAnamorphicObject::CAnamorphicObject(LPDIRECT3DDEVICE9 pGraphicDev)
    :CLiminalObject(pGraphicDev)
{
    D3DXMatrixIdentity(&m_matVPCaptured);
}

CAnamorphicObject::~CAnamorphicObject()
{
}

HRESULT CAnamorphicObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Ready_RenderTarget()))
        return E_FAIL;

    if (FAILED(CLiminalObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CAnamorphicObject::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CLiminalObject::Update_GameObject(fTimeDelta);

    if (m_b3D)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    }
    else
    {
        /* 단지 벽, 버튼 등의 그려짐 대상보다 뒤에 그려짐을 보장하기 위함 */
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    }
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    return iExit;
}

void CAnamorphicObject::LateUpdate_GameObject(_float fTimeDelta)
{
    CLiminalObject::LateUpdate_GameObject(fTimeDelta);
}

void CAnamorphicObject::Render_GameObject()
{
}

void CAnamorphicObject::Capture()
{
    m_b3D = false;
    m_bCapturePending = true;
}

void CAnamorphicObject::ManualCapture(const _vec3& vEye, const _vec3& vAt)
{
    _vec3 vWorldUp{ 0.f, 1.f, 0.f };
    D3DXMatrixLookAtLH(&m_matViewCaptured, &vEye, &vAt, &vWorldUp);

    /* 1인칭 시점 투영 하드코딩 */
    D3DXMatrixPerspectiveFovLH(&m_matProjCaptured,
        D3DXToRadian(60.f),
        static_cast<_float>(m_iRTWidth) / m_iRTHeight,
        0.1f, 1000.f);

    m_matVPCaptured = m_matViewCaptured * m_matProjCaptured;

    m_b3D = false;
    m_bCapturePending = true;
    m_bManualCapture = true;
}

void CAnamorphicObject::ReverseCapture()
{
    m_b3D = true;
}


HRESULT CAnamorphicObject::Add_Component()
{
    return S_OK;
}

HRESULT CAnamorphicObject::Ready_RenderTarget()
{
    /* 백버퍼 크기 정보 얻기 */
    D3DVIEWPORT9 vp;
    m_pGraphicDev->GetViewport(&vp);
    m_iRTWidth = vp.Width;
    m_iRTHeight = vp.Height;

    /* 백버퍼와 같은 크기의 텍스쳐, 깊이 버퍼 얻기. 상태 블록 얻기 */
    if (FAILED(m_pGraphicDev->CreateTexture(
        m_iRTWidth, m_iRTHeight,
        1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
        &m_pRT, nullptr)))
        return E_FAIL;

    if (FAILED(m_pGraphicDev->CreateDepthStencilSurface(
        m_iRTWidth, m_iRTHeight,
        D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0,
        TRUE, &m_pRTDepth, nullptr)))
        return E_FAIL;

    if (FAILED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock)))
        return E_FAIL;

    return S_OK;
}

void CAnamorphicObject::Render_Capture()
{
    _matrix matOldView, matOldProj;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

    if (!m_bManualCapture)
    {
        m_matViewCaptured = matOldView;
        m_matProjCaptured = matOldProj;
        m_matVPCaptured = m_matViewCaptured * m_matProjCaptured;
    }

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matViewCaptured);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProjCaptured);

    /* 그래픽 디바이스는 상태 기계이다. 따라서 현재 연결되어 있는 백버퍼 등은 백업하여 가지고 있기 */
    LPDIRECT3DSURFACE9 pOldRT = nullptr, pOldDS = nullptr, pRTSurf = nullptr;
    D3DVIEWPORT9 oldVP;
    m_pGraphicDev->GetRenderTarget(0, &pOldRT);
    m_pGraphicDev->GetDepthStencilSurface(&pOldDS);
    m_pGraphicDev->GetViewport(&oldVP);

    /* 멤버 텍스쳐가 가지고 있는 렌더 타겟을 초기화하기 */
    m_pRT->GetSurfaceLevel(0, &pRTSurf);
    m_pGraphicDev->SetRenderTarget(0, pRTSurf);
    m_pGraphicDev->SetDepthStencilSurface(m_pRTDepth);
    m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.f, 0);

    /* 렌더 타겟에 물체 단 하나만 그리기 */
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    /* 이전 값으로 복구하기 */
    m_pGraphicDev->SetRenderTarget(0, pOldRT);
    m_pGraphicDev->SetDepthStencilSurface(pOldDS);
    m_pGraphicDev->SetViewport(&oldVP);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);

    Safe_Release(pRTSurf);
    Safe_Release(pOldRT);
    Safe_Release(pOldDS);
    m_bManualCapture = false;
}

void CAnamorphicObject::Render_Paint()
{
    m_pStateBlock->Capture();

    /* 텍스쳐 행렬 구성하기 : 현재 카메라 뷰 좌표로부터 캡쳐 시점 뷰포트 좌표까지 매핑되어야 한다
    현재 시점의 카메라 View 역행렬 * 월드 행렬 * VP 행렬 * 뷰포트 행렬 
    Q) 카메라 View 역행렬을 곱하는 이유는 dx9 고정 파이프라인이 제공하는 자동 생성 좌표가 메인 카메라 공간 좌표이기 때문 */

    _matrix matView, matInvView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matInvView, nullptr, &matView);
    
    float fOffX = 0.5f + 0.5f / m_iRTWidth;    // 하프텍셀 보정
    float fOffY = 0.5f + 0.5f / m_iRTHeight;
    _matrix matScaleBias(
        0.5f, 0.f, 0.f, 0.f,
        0.f, -0.5f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        fOffX, fOffY, 0.f, 1.f);

    /* 고정 파이프라인에서 이미 월드 * 카메라 View가 곱해진 상태로 제공 */
    _matrix matTex = matInvView * m_matVPCaptured * matScaleBias;
    m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &matTex);

    /* UV는 카메라 View 공간 기준으로 제공된다 (그래서 행렬을 만들 때 다시 롤백함) */
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
    /* D3DTTFF_COUNT4 : 텍스쳐 행렬의 4성분 (x, y, z, w)를 모두 쓰고, D3DTTFF_PROJECTED : w로 나눈다 */
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED);

    /* 색과 알파의 계산 방식 설정 : ARG들의 OP 연산으로 결정된다
    색   = COLOROP(COLORARG1, COLORARG2)
    알파 = ALPHAOP(ALPHAARG1, ALPHAARG2)
    여기서는 둘다 ARG1만 받는 걸로 하고 있다
    */
    m_pGraphicDev->SetTexture(0, m_pRT);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    /* 뷰포트가 텍스쳐의 UV [0, 1]에 대응된다. 결국 UV 커팅을 하는 것과 Frustum 바깥 부분을 쳐내는 것과 같은 동작임 여기선 */
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    /* 알파테스트로 백버퍼에 그려진 부분만 그리기 (없어도 되긴 할듯)*/
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    /* 카메라의 트랜스폼을 통해 클립 평면을 정의한다 */
    /* 기존에 하드웨어가 자동적으로 처리해주던 view frustum culling은 텍스쳐 경로에서는 적용되지 않는다
    * 따라서 카메라 뒤쪽에 위치한 정점들은 음수 w로 클리핑 되지 않고 반전된 UV로 그려지게 된다.
    * 그래서 그 부분을 평면을 직접 정의하여 잘라내는 것
    */
    _matrix matInvViewCaptured;
    D3DXMatrixInverse(&matInvViewCaptured, nullptr, &m_matViewCaptured);
    _vec3 vCapturePos, vCaptureLook;
    memcpy(&vCapturePos, &matInvViewCaptured.m[3][0], sizeof(_vec3));
    memcpy(&vCaptureLook, &matInvViewCaptured.m[2][0], sizeof(_vec3));
    D3DXPLANE plane;
    D3DXPlaneFromPointNormal(&plane, &vCapturePos, &vCaptureLook);
    m_pGraphicDev->SetClipPlane(0, plane);
    m_pGraphicDev->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage)
    {
        CLiminalRoomLayer* pLayer = dynamic_cast<CLiminalRoomLayer*>(pStage->GetCurrentRoomLayer());
        if (pLayer)
        {
            const vector<IAnamorphicTarget*> vecDrawTarget = pLayer->GetAnamorphicTargetList();
            for (IAnamorphicTarget* pTarget : vecDrawTarget)
            {
                CVIBuffer* pBuffer = pTarget->GetBuffer();
                CTransform* pTransform = pTarget->GetTransform();

                m_pGraphicDev->SetTransform(D3DTS_WORLD, pTransform->Get_World());
                pBuffer->Render_Buffer();
            }
        }
    }

    m_pStateBlock->Apply();
}

CAnamorphicObject* CAnamorphicObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CAnamorphicObject* pObject = new CAnamorphicObject(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CAnamorphicObject Create Failed");
        return nullptr;
    }

    return pObject;
}

void CAnamorphicObject::Free()
{
    Safe_Release(m_pStateBlock);
    Safe_Release(m_pRTDepth);
    Safe_Release(m_pRT);
    CGameObject::Free();
}
