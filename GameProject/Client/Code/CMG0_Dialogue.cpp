#include "pch.h"
#include "CMG0_Dialogue.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CSoundMgr.h"
#include "CTexture.h"
#include "CMiniGame.h"

CMG0_Dialogue::CMG0_Dialogue(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Dialogue::~CMG0_Dialogue()
{
}

HRESULT CMG0_Dialogue::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    Ready_Page(0);

    return CGameObject::Ready_GameObject();
}

_int CMG0_Dialogue::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (m_bFinished)
        return iExit;

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return iExit;

    _float fSceneTime = pScene->Get_SceneTime();
    if (fSceneTime < MG_DIALOGUE_START_TIME)
        return iExit;

    if (fSceneTime >= MG_DIALOGUE_END_TIME)
    {
        m_bFinished = true;
        return iExit;
    }

    const _float fPageLimit = (MG_DIALOGUE_END_TIME - MG_DIALOGUE_START_TIME) / _countof(MINIGAME0::MG_DIALOGUE_TEXT);

    m_fPageTime += fTimeDelta;

    if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
    {
        if (Is_Speaking())
            Show_All();
        else
            Next_Page();
    }
    else if (m_fPageTime >= fPageLimit)
        Next_Page();

    if (m_bFinished)
        return iExit;

    Update_Text(fTimeDelta);

    m_fPortraitTime += fTimeDelta;
    m_iPortraitFrame = Is_Speaking() ? (_uint)(m_fPortraitTime / MG_DIALOGUE_PORTRAIT_TIME) % 2 : 1;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CMG0_Dialogue::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Dialogue::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwZEnable, dwAlphaTest, dwTexFactor, dwMinFilter, dwMagFilter;
    DWORD dwColorOp, dwColorArg1, dwColorArg2, dwAlphaOp, dwAlphaArg1, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    Render_Quad(m_pWindowTex, 0, WINCX * 0.5f, MG_DIALOGUE_WINDOW_Y, MG_DIALOGUE_WINDOW_HALF_X, MG_DIALOGUE_WINDOW_HALF_Y);
    Render_Quad(m_pWindowTex, 0, WINCX * 0.5f, MG_DIALOGUE_GUIDE_Y, MG_DIALOGUE_GUIDE_HALF_X, MG_DIALOGUE_GUIDE_HALF_Y);
    Render_Quad(m_pPortraitTex, m_iPortraitFrame, MG_DIALOGUE_PORTRAIT_X, MG_DIALOGUE_WINDOW_Y, MG_DIALOGUE_PORTRAIT_HALF, MG_DIALOGUE_PORTRAIT_HALF);

    Render_Text();

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

    for (size_t i = 0; i < m_iVisibleGlyph; ++i)
        Render_Mark(m_vecGlyph[i]);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

void CMG0_Dialogue::Ready_Page(size_t iPage)
{
    const wstring strText = MINIGAME0::MG_DIALOGUE_TEXT[iPage];

    m_iPage = iPage;
    m_iVisibleWord = 0;
    m_iVisibleGlyph = 0;
    m_fTextTime = 0.f;
    m_fPageTime = 0.f;

    m_vecGlyph.clear();
    m_vecWordEnd.clear();

    _float fLineX = MG_DIALOGUE_TEXT_X;
    _float fLineY = 0.f;

    for (_tchar cChar : strText)
    {
        if (L'\n' == cChar)
        {
            fLineX = MG_DIALOGUE_TEXT_X;
            fLineY += MG_DIALOGUE_FONT_SIZE;
            continue;
        }

        if (Is_Mark(cChar))
        {
            if (!m_vecGlyph.empty() && m_vecGlyph.back().vPos.y == fLineY)
                m_vecGlyph.back().cMark = cChar;
            continue;
        }

        GLYPH tGlyph;
        tGlyph.cChar = cChar;
        tGlyph.fWidth = Get_Width(cChar);
        tGlyph.vPos = _vec2(fLineX, fLineY);

        m_vecGlyph.push_back(tGlyph);

        fLineX += tGlyph.fWidth;
    }

    const _float fTop = MG_DIALOGUE_WINDOW_Y - (fLineY + MG_DIALOGUE_FONT_SIZE) * 0.5f;

    for (GLYPH& tGlyph : m_vecGlyph)
        tGlyph.vPos.y += fTop;

    for (size_t i = 0; i < m_vecGlyph.size(); ++i)
    {
        if (Is_Space(m_vecGlyph[i]))
            continue;

        if (i + 1 == m_vecGlyph.size() || Is_Space(m_vecGlyph[i + 1]) || m_vecGlyph[i + 1].vPos.y != m_vecGlyph[i].vPos.y)
            m_vecWordEnd.push_back(i + 1);
    }
}

void CMG0_Dialogue::Next_Page()
{
    if (m_iPage + 1 >= _countof(MINIGAME0::MG_DIALOGUE_TEXT))
    {
        m_bFinished = true;
        return;
    }

    Ready_Page(m_iPage + 1);
}

void CMG0_Dialogue::Show_All()
{
    m_iVisibleWord = m_vecWordEnd.size();
    m_iVisibleGlyph = m_vecGlyph.size();
}

void CMG0_Dialogue::Update_Text(_float fTimeDelta)
{
    if (!Is_Speaking())
        return;

    m_fTextTime += fTimeDelta;

    _bool bRevealed = false;

    while (m_fTextTime >= MG_DIALOGUE_WORD_TIME && Is_Speaking())
    {
        m_fTextTime -= MG_DIALOGUE_WORD_TIME;

        m_iVisibleGlyph = m_vecWordEnd[m_iVisibleWord];
        ++m_iVisibleWord;

        bRevealed = true;
    }

    if (bRevealed)
        CSoundMgr::GetInstance()->PlaySFX(L"sfxEnergy.wav");
}

void CMG0_Dialogue::Render_Quad(Engine::CTexture* pTexture, _uint iFrame, _float fX, _float fY, _float fHalfX, _float fHalfY)
{
    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, fHalfX, fHalfY, 1.f);
    D3DXMatrixTranslation(&matTrans, fX - WINCX * 0.5f, WINCY * 0.5f - fY, 0.5f);
    _matrix matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    pTexture->Set_Texture(iFrame);
    m_pBufferCom->Render_Buffer();
}

void CMG0_Dialogue::Render_Text()
{
    const D3DXCOLOR Color(1.f, 1.f, 1.f, 1.f);

    for (size_t i = 0; i < m_iVisibleGlyph; ++i)
    {
        const GLYPH& tGlyph = m_vecGlyph[i];
        if (Is_Space(tGlyph))
            continue;

        const _tchar szChar[2] = { tGlyph.cChar, L'\0' };

        CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", szChar, &tGlyph.vPos, Color);
    }

    const wstring strGuide = MG_DIALOGUE_GUIDE;
    const _vec2 vGuide(WINCX * 0.5f - strGuide.size() * MG_DIALOGUE_FONT_SIZE * 0.25f, MG_DIALOGUE_GUIDE_Y - MG_DIALOGUE_FONT_SIZE * 0.5f);

    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", strGuide.c_str(), &vGuide, Color);
}

void CMG0_Dialogue::Render_Mark(const GLYPH& tGlyph)
{
    const _float fX = tGlyph.vPos.x + tGlyph.fWidth * 0.5f;
    const _float fY = tGlyph.vPos.y + MG_DIALOGUE_FONT_SIZE * 0.5f;
    const _float fHalf = tGlyph.fWidth * 0.5f;

    switch (tGlyph.cMark)
    {
    case MARK_TILDE:
        Render_Line(fX - fHalf * 0.5f, fY - 2.f, fHalf * 0.5f + 1.f, 1.f, 0.f);
        Render_Line(fX, fY, 1.f, 3.f, 0.f);
        Render_Line(fX + fHalf * 0.5f, fY + 2.f, fHalf * 0.5f + 1.f, 1.f, 0.f);
        break;
    case MARK_STROKE_SHORT:
        Render_Line(fX, fY, fHalf * 0.6f, 1.f, 0.f);
        break;
    case MARK_STROKE_LONG:
        Render_Line(fX, fY, fHalf + 2.f, 1.f, 0.f);
        break;
    case MARK_SLASH_SHORT:
        Render_Line(fX, fY, 7.f, 1.f, 60.f);
        break;
    case MARK_SLASH_LONG:
        Render_Line(fX, fY, 14.f, 1.f, 60.f);
        break;
    default:
        break;
    }
}

void CMG0_Dialogue::Render_Line(_float fX, _float fY, _float fHalfX, _float fHalfY, _float fAngle)
{
    _matrix matScale, matRot, matTrans;
    D3DXMatrixScaling(&matScale, fHalfX, fHalfY, 1.f);
    D3DXMatrixRotationZ(&matRot, D3DXToRadian(fAngle));
    D3DXMatrixTranslation(&matTrans, fX - WINCX * 0.5f, WINCY * 0.5f - fY, 0.5f);
    _matrix matWorld = matScale * matRot * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pBufferCom->Render_Buffer();
}

HRESULT CMG0_Dialogue::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pWindowTex = CTexture::Create(m_pGraphicDev, TEX_NORMAL, MG_DIALOGUE_WINDOW_PATH);

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_WindowTexture", pComponent });

    pComponent = m_pPortraitTex = CTexture::Create(m_pGraphicDev, TEX_NORMAL, MG_DIALOGUE_PORTRAIT_PATH, 2);

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_PortraitTexture", pComponent });

    return S_OK;
}

CMG0_Dialogue* CMG0_Dialogue::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Dialogue* pDialogue = new CMG0_Dialogue(pGraphicDev);

    if (FAILED(pDialogue->Ready_GameObject()))
    {
        Safe_Release(pDialogue);
        MSG_BOX("CMG0_Dialogue Create Failed");
        return nullptr;
    }

    return pDialogue;
}

void CMG0_Dialogue::Free()
{
    CGameObject::Free();
}
