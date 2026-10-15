#include "pch.h"
#include "CUI_PhaseNumber.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CProtoMgr.h"

CUI_PhaseNumber::CUI_PhaseNumber(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}

CUI_PhaseNumber::~CUI_PhaseNumber()
{
}

HRESULT CUI_PhaseNumber::Ready_GameObject()
{
    /* "PHASE" 글자 */
    m_wstrTextureTag = L"Proto_PhaseText_Texture";

    if (FAILED(__super::Add_Component()))
        return E_FAIL;

    if (FAILED(Add_NumberComponent()))
        return E_FAIL;

    if (FAILED(Add_SlashComponent()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    Set_Pos(s_vTextCenter);
    Set_Size(s_vTextHalfSize);
    Set_Texture(0);

    /* 미니게임 시작 전까지는 꺼둠 */
    Set_IsActive(false);

    return S_OK;
}

_int CUI_PhaseNumber::Update_GameObject(_float fTimeDelta)
{
    /* CUI::Update_GameObject는 스위치 값으로 활성 상태를 다시 켜므로 쓰지 않음 */
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CUI_PhaseNumber::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUI_PhaseNumber::Render_GameObject()
{
    /* "PHASE" : CUI 기본 렌더 (텍스처 팩터/스테이지 설정과 원복 포함) */
    CUI::Render_GameObject();

    /* 현재 / 전체 : 같은 버퍼를 위치/크기만 바꿔서 그림 */
    RenderNumber(m_iCurrentPhase, s_vCurrentCenter);
    RenderQuad(m_pSlashTextureCom, 0, s_vSlashCenter, s_vDigitHalfSize);
    RenderNumber(m_iTotalPhase, s_vTotalCenter);
}

void CUI_PhaseNumber::Show()
{
    Set_IsActive(true);
}

void CUI_PhaseNumber::Hide()
{
    Set_IsActive(false);
}

void CUI_PhaseNumber::RenderQuad(CTexture* pTexture, _uint iIndex, const _vec2& vCenter, const _vec2& vHalfSize)
{
    /* CUI::Set_Pos/Set_Size와 같은 변환 : RcTex가 -1~1이라 절반 크기로 스케일, 화면 좌표 -> 가운데 원점 */
    _matrix matScale, matTrans, matWorld;
    D3DXMatrixScaling(&matScale, vHalfSize.x, vHalfSize.y, 1.f);
    D3DXMatrixTranslation(&matTrans, vCenter.x - WINCX * 0.5f, -vCenter.y + WINCY * 0.5f, 0.f);
    matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    pTexture->Set_Texture(iIndex);
    m_pBufferCom->Render_Buffer();
}

void CUI_PhaseNumber::RenderNumber(int iNumber, const _vec2& vCenter)
{
    const string strNumber = to_string(iNumber);

    /* 자릿수만큼 가운데 정렬 */
    float fX = vCenter.x - s_fDigitAdvance * ((float)strNumber.size() - 1.f) * 0.5f;
    for (char ch : strNumber)
    {
        RenderQuad(m_pNumberTextureCom, GetDigitTextureIdx(ch - '0'), _vec2{ fX, vCenter.y }, s_vDigitHalfSize);
        fX += s_fDigitAdvance;
    }
}

_uint CUI_PhaseNumber::GetDigitTextureIdx(int iDigit) const
{
    /* 장 번호 = 숫자 */
    const int iCount = (int)m_pNumberTextureCom->GetCount();
    return (_uint)max(0, min(iDigit, iCount - 1));
}

HRESULT CUI_PhaseNumber::Add_NumberComponent()
{
    CComponent* pComponent = nullptr;

    // Number Texture (0~9)
    pComponent = m_pNumberTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NumberTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_NumberTexture", pComponent });

    return S_OK;
}

HRESULT CUI_PhaseNumber::Add_SlashComponent()
{
    CComponent* pComponent = nullptr;

    // Slash Texture
    pComponent = m_pSlashTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SlashTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_SlashTexture", pComponent });

    return S_OK;
}

CUI_PhaseNumber* CUI_PhaseNumber::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUI_PhaseNumber* pUI = new CUI_PhaseNumber(pGraphicDev);

    if (FAILED(pUI->Ready_GameObject()))
    {
        Safe_Release(pUI);
        MSG_BOX("CUI_PhaseNumber Create Failed");
        return nullptr;
    }

    return pUI;
}

void CUI_PhaseNumber::Free()
{
    CUI::Free();
}
