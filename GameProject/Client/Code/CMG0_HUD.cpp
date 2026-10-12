#include "pch.h"
#include "CMG0_HUD.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"
#include "CMG0_Deck.h"
#include "CMG0_Card.h"
#include "CMG0_Herobrine.h"

static const D3DXCOLOR Color_White(1.f, 1.f, 1.f, 1.f);
static const D3DXCOLOR Color_Yellow(1.f, 0.9f, 0.3f, 1.f);
static const D3DXCOLOR Color_Gray(0.55f, 0.55f, 0.55f, 1.f);
static const D3DXCOLOR Color_Dark(0.16f, 0.18f, 0.27f, 1.f);
static const D3DXCOLOR Color_Red(1.f, 0.35f, 0.35f, 1.f);
static const D3DXCOLOR Color_Blue(0.4f, 0.7f, 1.f, 1.f);
static const D3DXCOLOR Color_Purple(0.62f, 0.5f, 0.92f, 1.f);

static const DWORD Rect_Dark = D3DCOLOR_ARGB(180, 0, 0, 0);
static const DWORD Rect_Panel = D3DCOLOR_ARGB(235, 16, 24, 48);
static const DWORD Rect_Border = D3DCOLOR_ARGB(255, 200, 210, 230);
static const DWORD Rect_Select = D3DCOLOR_ARGB(255, 255, 230, 77);
static const DWORD Rect_Popup = D3DCOLOR_ARGB(250, 255, 255, 255);
static const DWORD Rect_PopupBorder = D3DCOLOR_ARGB(255, 190, 196, 214);
static const DWORD Rect_Warning = D3DCOLOR_ARGB(255, 255, 50, 50);
static const DWORD Rect_WarningBack = D3DCOLOR_ARGB(235, 30, 0, 0);
static const DWORD Rect_BossBack = D3DCOLOR_ARGB(255, 40, 10, 10);
static const DWORD Rect_BossHp = D3DCOLOR_ARGB(255, 220, 40, 40);

CMG0_HUD::CMG0_HUD(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_HUD::~CMG0_HUD()
{
}

HRESULT CMG0_HUD::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Ready_Font()))
        return E_FAIL;

    return CGameObject::Ready_GameObject();
}

_int CMG0_HUD::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (pScene)
    {
        CMG0_Player* pPlayer = pScene->Get_Player();

        _float fFlash = pPlayer->Get_Invincible() - (MG_HIT_INVINCIBLE - MG_HIT_FLASH_TIME);
        if (pPlayer->Is_GameOver())
            fFlash = 0.f;

        m_iLife = pPlayer->Get_Life();
        m_iHitAlpha = max(0, min(255, (_int)(fFlash / MG_HIT_FLASH_TIME * 255.f)));
        m_bCleared = pScene->Is_Cleared();
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CMG0_HUD::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    CMG0_Deck* pDeck = pScene->Get_Deck();

    MINIGAME0::MGCARDSTATE tState = pDeck->Get_State();
    MINIGAME0::MGCARDSTATE tPreview = pDeck->Get_Preview();

    Update_View(m_tAttack, tState.iAttack, tPreview.iAttack, fTimeDelta);
    Update_View(m_tExtraActivate, tState.iExtraActivate, tPreview.iExtraActivate, fTimeDelta);
    Update_View(m_tExtraAction, tState.iExtraAction, tPreview.iExtraAction, fTimeDelta);
    Update_View(m_tActiveParameter, tState.iActiveParameter, tPreview.iActiveParameter, fTimeDelta);

    for (_int i = 0; i < MINIGAME0::STATUS_END; ++i)
        Update_View(m_tStatus[i], tState.iStatus[i], tPreview.iStatus[i], fTimeDelta);

    m_bCardTime = pDeck->Is_CardTime();
    m_bPlaying = pDeck->Is_Playing();
    m_iTurn = pDeck->Get_Turn();
    m_iSelect = pDeck->Get_Select();
    m_fTurnRate = pDeck->Get_TimeRate();
    m_bComplete = pDeck->Is_Complete();

    for (_int i = 0; i < MG_CARD_HAND_COUNT; ++i)
    {
        HANDSLOT& tSlot = m_tHand[i];
        CMG0_Card* pCard = pDeck->Get_Hand(i);

        tSlot.eCard = pCard ? pCard->Get_Card() : MINIGAME0::CARD_END;
        if (nullptr == pCard)
            continue;

        tSlot.tCost = pDeck->Get_Cost(tSlot.eCard);
        tSlot.bUsable = pDeck->Can_Use(tSlot.eCard);
    }

    m_strNotice = pDeck->Is_Notice() ? pDeck->Get_Notice() : L"";

    _float fSceneTime = pScene->Get_SceneTime();
    _float fWarningTime = fSceneTime - MG_BOSS_WARNING_START;

    m_bWarning = fWarningTime >= 0.f && fWarningTime < MG_BOSS_WARNING_TIME && 0 == (_int)(fWarningTime / MG_BOSS_WARNING_BLINK) % 2;
    m_bBossHp = fSceneTime >= MG_BOSS_HPBAR_START && !pScene->Get_Boss()->Is_Dead();
    m_bTimeOver = pScene->Is_TimeOver();
    m_iWhiteAlpha = (_int)(255.f * pScene->Get_WhiteRate());

    if (m_bBossHp)
    {
        CMG0_Herobrine* pBoss = pScene->Get_Boss();
        _float fRate = (_float)pBoss->Get_Hp() / pBoss->Get_MaxHp();

        m_fBossHpRate += (fRate - m_fBossHpRate) * min(1.f, 3.f * fTimeDelta);
    }
}

void CMG0_HUD::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwZEnable, dwAlphaTest, dwTexFactor;
    DWORD dwColorOp, dwColorArg1, dwColorArg2, dwAlphaOp, dwAlphaArg1, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &m_dwMagFilter);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &m_dwMinFilter);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    Render_Life();
    Render_Attack();

    if (m_bCardTime)
    {
        Render_Vital();
        Render_Buff();
    }

    if (m_bPlaying)
    {
        Render_Turn();
        Render_Hand();
        Render_Rest();
        Render_Popup();
    }

    if (m_bComplete)
        Render_Complete();

    Render_Notice();

    if (m_bBossHp)
        Render_BossHp();

    if (m_bWarning)
        Render_Warning();

    if (m_iWhiteAlpha > 0)
        Render_Rect(WINCX * 0.5f, WINCY * 0.5f, WINCX * 0.5f, WINCY * 0.5f, D3DCOLOR_ARGB(m_iWhiteAlpha, 255, 255, 255));

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);

    Flush_Text();

    if (m_bWarning)
        Render_WarningText();

    if (m_bTimeOver && m_iWhiteAlpha >= 255)
        Render_GameOverText();

    if (m_bCleared)
        Render_ClearText();
}

void CMG0_HUD::Render_Life()
{
    if (m_iHitAlpha > 0)
        Render_Quad(m_pHitTex, 0, 0.f, 0.f, WINCX * 0.5f, WINCY * 0.5f, D3DCOLOR_ARGB(m_iHitAlpha, 255, 255, 255));

    for (_int i = 0; i < MG_MAX_LIFE; ++i)
    {
        _float fX = -WINCX * 0.5f + 34.f + i * 40.f;
        _float fY = WINCY * 0.5f - 34.f;
        Render_Quad(m_pHeartTex, i < m_iLife ? MG_HEART_FULL : 0, fX, fY, 16.f, 16.f, 0xFFFFFFFF);
    }
}

void CMG0_HUD::Render_Attack()
{
    const wstring strLabel = L"공격력 ";
    const _vec2 vPos(WINCX - 192.f, 18.f);

    Render_Text(strLabel, vPos, Color_Yellow);
    Render_Value(m_tAttack, vPos.x + Measure_Text(strLabel), vPos.y, Color_Yellow);

    MINIGAME0::MGICON eWeapon = MINIGAME0::ICON_WEAPON_WOOD;
    if (m_tAttack.iActual >= MG_WEAPON_DIAMOND)
        eWeapon = MINIGAME0::ICON_WEAPON_DIAMOND;
    else if (m_tAttack.iActual >= MG_WEAPON_IRON)
        eWeapon = MINIGAME0::ICON_WEAPON_IRON;
    else if (m_tAttack.iActual >= MG_WEAPON_STONE)
        eWeapon = MINIGAME0::ICON_WEAPON_STONE;

    const _float fHalf = MG_UI_WEAPON_SIZE * 0.5f;
    const _float fX = vPos.x + fHalf;
    const _float fY = vPos.y + MG_UI_LINE_HEIGHT + 4.f + fHalf;

    Render_Quad(m_pIconTex[eWeapon], 0, fX - WINCX * 0.5f, WINCY * 0.5f - fY, fHalf, fHalf, 0xFFFFFFFF);
}

void CMG0_HUD::Render_Complete()
{
    const _float fX = WINCX * 0.5f;
    const _float fY = WINCY * 0.5f;
    const _float fScale = 2.f;

    Render_Rect(fX, fY, WINCX * 0.5f, MG_UI_TEXT_HEIGHT * fScale * 0.5f + 12.f, Rect_Dark);
    Render_Text(L"무기 업그레이드 완료", _vec2(fX, fY - MG_UI_TEXT_HEIGHT * fScale * 0.5f), Color_Yellow, true, true, fScale);
}

void CMG0_HUD::Render_Vital()
{
    const _float fLeft = WINCX - 192.f;
    const _float fBarY = 18.f + MG_UI_LINE_HEIGHT + 4.f + MG_UI_WEAPON_SIZE + 16.f;
    const _float fBarHalfX = 86.f;
    const _float fBarHalfY = 6.f;
    const _float fBarX = fLeft + fBarHalfX;
    const _float fRowStep = MG_UI_ICON_SIZE + 2.f;

    const STATVIEW& tCardHp = m_tStatus[MINIGAME0::CARD_HP];
    const STATVIEW& tStamina = m_tStatus[MINIGAME0::STAMINA];
    _float fRate = max(0.f, min(1.f, tCardHp.fShown / (_float)MG_CARD_HP));

    Render_Rect(fBarX, fBarY, fBarHalfX + 2.f, fBarHalfY + 2.f, Rect_Dark);
    Render_Rect(fBarX - fBarHalfX * (1.f - fRate), fBarY, fBarHalfX * fRate, fBarHalfY, D3DCOLOR_ARGB(255, 60, 230, 160));

    _float fY = fBarY + 30.f;

    Render_Icon(MINIGAME0::ICON_STAMINA, fLeft + 14.f, fY, MG_UI_ICON_SIZE);
    Render_Value(tStamina, fLeft + 34.f, fY - 11.f, (tStamina.iActual > 0 || tStamina.iTarget > 0) ? Color_White : Color_Gray);

    fY += fRowStep;

    Render_Icon(MINIGAME0::ICON_HP, fLeft + 14.f, fY, MG_UI_ICON_SIZE - 2.f);
    Render_Value(tCardHp, fLeft + 34.f, fY - 11.f, Color_White);
}

void CMG0_HUD::Render_Buff()
{
    const pair<MINIGAME0::MGICON, const STATVIEW*> tBuff[] =
    {
        { MINIGAME0::ICON_MOOD_GOOD, &m_tStatus[MINIGAME0::MOOD_GOOD] },
        { MINIGAME0::ICON_MOOD_PERFECT, &m_tStatus[MINIGAME0::MOOD_PERFECT] },
        { MINIGAME0::ICON_ATTENTION, &m_tStatus[MINIGAME0::ATTENTION] },
        { MINIGAME0::ICON_SKILL_DOUBLE, &m_tExtraActivate },
        { MINIGAME0::ICON_EXTRA_ACTION, &m_tExtraAction },
        { MINIGAME0::ICON_COST_HALF, &m_tStatus[MINIGAME0::COST_HALF] },
    };

    _float fY = 84.f;

    for (const auto& Pair : tBuff)
    {
        const STATVIEW& tView = *Pair.second;
        if (tView.iActual <= 0 && tView.iTarget <= 0 && Get_Shown(tView) <= 0)
            continue;

        Render_Icon(Pair.first, 34.f, fY, MG_UI_ICON_SIZE);
        Render_Value(tView, 56.f, fY - 11.f, Color_White);

        fY += MG_UI_ICON_SIZE + 6.f;
    }

    if (m_tActiveParameter.iActual > 0 || m_tActiveParameter.iTarget > 0)
    {
        const wstring strLabel = L"액티브 +";

        Render_Text(strLabel, _vec2(20.f, fY - 11.f), Color_White);
        Render_Value(m_tActiveParameter, 20.f + Measure_Text(strLabel), fY - 11.f, Color_White);
    }
}

void CMG0_HUD::Update_View(STATVIEW& tView, _int iActual, _int iTarget, _float fTimeDelta)
{
    tView.iActual = iActual;
    tView.iTarget = iTarget;

    if (!tView.bReady)
    {
        tView.fShown = (_float)iTarget;
        tView.bReady = true;
        return;
    }

    _float fDiff = iTarget - tView.fShown;
    _float fStep = max(MG_UI_COUNT_SPEED, fabsf(fDiff) * 8.f) * fTimeDelta;

    if (fabsf(fDiff) <= fStep)
        tView.fShown = (_float)iTarget;
    else
        tView.fShown += (fDiff > 0.f) ? fStep : -fStep;
}

_int CMG0_HUD::Get_Shown(const STATVIEW& tView)
{
    if (tView.fShown > tView.iTarget)
        return (_int)ceilf(tView.fShown);

    return (_int)floorf(tView.fShown);
}

void CMG0_HUD::Render_Value(const STATVIEW& tView, _float fX, _float fY, D3DXCOLOR Color)
{
    const wstring strValue = to_wstring(Get_Shown(tView));

    Render_Text(strValue, _vec2(fX, fY), Color);

    _int iDelta = tView.iTarget - tView.iActual;
    if (0 == iDelta)
        return;

    const wstring strDelta = (iDelta > 0 ? L"+" : L"") + to_wstring(iDelta);

    Render_Text(strDelta, _vec2(fX + Measure_Text(strValue) + 6.f, fY), iDelta > 0 ? Color_Blue : Color_Red);
}

_float CMG0_HUD::Measure_Text(const wstring& strText)
{
    _float fWidth = 0.f;
    for (const TEXTRUN& tRun : Split_Text(strText))
        fWidth += tRun.fWidth;

    return fWidth;
}

void CMG0_HUD::Render_Turn()
{
    const _float fCenterX = WINCX * 0.5f;
    const _float fBarY = 52.f;
    const _float fBarHalfX = 100.f;
    const _float fBarHalfY = 4.f;

    Render_Text(L"턴 " + to_wstring(m_iTurn) + L" / " + to_wstring(MG_CARD_MAX_TURN), _vec2(fCenterX, 18.f), Color_White, true);

    Render_Rect(fCenterX, fBarY, fBarHalfX + 2.f, fBarHalfY + 2.f, Rect_Dark);
    Render_Rect(fCenterX - fBarHalfX * (1.f - m_fTurnRate), fBarY, fBarHalfX * m_fTurnRate, fBarHalfY,
        m_fTurnRate > 0.3f ? Rect_Select : D3DCOLOR_ARGB(255, 255, 90, 90));
}

void CMG0_HUD::Render_Hand()
{
    const _tchar* pKeyName[MG_CARD_HAND_COUNT] = { L"Z", L"X", L"C" };

    for (_int i = 0; i < MG_CARD_HAND_COUNT; ++i)
    {
        const HANDSLOT& tSlot = m_tHand[i];
        if (MINIGAME0::CARD_END == tSlot.eCard)
            continue;

        _bool bSelected = (i == m_iSelect);
        D3DXCOLOR Color = !tSlot.bUsable ? Color_Gray : (bSelected ? Color_Yellow : Color_White);

        _float fX = WINCX * 0.5f + (i - (MG_CARD_HAND_COUNT - 1) * 0.5f) * MG_CARD_UI_GAP_X;
        _float fCenterY = MG_CARD_UI_CENTER_Y - (bSelected ? MG_CARD_UI_SELECT_RAISE : 0.f);
        _float fTop = fCenterY - MG_CARD_UI_HALF_Y;
        _float fBottom = fCenterY + MG_CARD_UI_HALF_Y;

        Render_Text(pKeyName[i], _vec2(fX, fTop - MG_UI_LINE_HEIGHT - 2.f), Color, true);

        if (bSelected)
            Render_Icon(MINIGAME0::ICON_SELECT, fX, fBottom + 10.f, MG_UI_SELECT_SIZE_X, MG_UI_SELECT_SIZE_Y);
    }
}

void CMG0_HUD::Render_Rest()
{
    const _float fX = WINCX * 0.5f + MG_CARD_UI_GAP_X * 2.f;
    const _float fY = MG_CARD_UI_CENTER_Y;
    const _float fHalfX = 56.f;
    const _float fHalfY = 26.f;

    _bool bSelected = (MG_CARD_SELECT_REST == m_iSelect);
    D3DXCOLOR Color = bSelected ? Color_Yellow : Color_White;

    Render_Rect(fX, fY, fHalfX + 3.f, fHalfY + 3.f, bSelected ? Rect_Select : Rect_Border);
    Render_Rect(fX, fY, fHalfX, fHalfY, Rect_Panel);

    Render_Text(L"V", _vec2(fX - fHalfX + 10.f, fY - 11.f), Color);
    Render_Icon(MINIGAME0::ICON_HP, fX - 4.f, fY, 24.f);
    Render_Text(L"+" + to_wstring(MG_CARD_SKIP_HP), _vec2(fX + 12.f, fY - 11.f), Color);

    if (bSelected)
        Render_Icon(MINIGAME0::ICON_SELECT, fX, fY + fHalfY + 10.f, MG_UI_SELECT_SIZE_X, MG_UI_SELECT_SIZE_Y);
}

void CMG0_HUD::Render_Popup()
{
    const _float fX = WINCX * 0.5f;
    const _float fY = 320.f;
    const _float fHalfX = 200.f;
    const _float fHalfY = 72.f;

    if (MG_CARD_SELECT_NONE == m_iSelect)
        return;
    if (MG_CARD_SELECT_REST != m_iSelect && MINIGAME0::CARD_END == m_tHand[m_iSelect].eCard)
        return;

    const _float fTop = fY - fHalfY;
    const _float fLineX = fX - fHalfX + 44.f;
    const _float fLineY = fTop + 44.f;
    const _float fLineStep = MG_UI_LINE_HEIGHT;

    Render_Rect(fX, fY, fHalfX + 2.f, fHalfY + 2.f, Rect_PopupBorder);
    Render_Rect(fX, fY, fHalfX, fHalfY, Rect_Popup);
    Render_Rect(fX, fTop + 36.f, fHalfX - 16.f, 1.f, Rect_PopupBorder);

    if (MG_CARD_SELECT_REST == m_iSelect)
    {
        Render_Text(L"휴식", _vec2(fX, fTop + 8.f), Color_Dark, true, false);
        Render_Icon(MINIGAME0::ICON_HP, fLineX, fLineY + 11.f, 24.f);
        Render_Text(L"체력 +" + to_wstring(MG_CARD_SKIP_HP), _vec2(fLineX + 20.f, fLineY), Color_Dark, false, false);
        return;
    }

    const HANDSLOT& tSlot = m_tHand[m_iSelect];
    const MINIGAME0::MGCARDDATA& tData = MINIGAME0::MG_CARD_DATA[tSlot.eCard];

    Render_Text(tData.pName, _vec2(fX, fTop + 8.f), Color_Dark, true, false);
    Render_Cost(tSlot.tCost, fX + fHalfX - 60.f, fTop + 19.f, Color_Dark, false);

    _float fTextY = fLineY;

    for (const MINIGAME0::MGCARDLINE& tLine : tData.tLine)
    {
        if (L'\0' == tLine.pText[0])
            continue;

        if (MINIGAME0::ICON_END != tLine.eIcon)
            Render_Icon(tLine.eIcon, fLineX, fTextY + 11.f, 24.f);

        Render_Text(tLine.pText, _vec2(fLineX + 20.f, fTextY), Color_Dark, false, false);

        fTextY += fLineStep;
    }

    if (MINIGAME0::ONCE_PER_LESSON == tData.eType)
        Render_Text(L"레슨 중 1회", _vec2(fLineX + 20.f, fTextY), Color_Purple, false, false);
}

void CMG0_HUD::Render_Notice()
{
    if (m_strNotice.empty())
        return;

    Render_Text(m_strNotice, _vec2(WINCX * 0.5f, 66.f), Color_Yellow, true);
}

void CMG0_HUD::Render_Cost(const MINIGAME0::MGCARDCOST& tCost, _float fX, _float fY, D3DXCOLOR Color, _bool bOutline)
{
    MINIGAME0::MGICON eIcon = MINIGAME0::ICON_END;
    _int iCost = 0;

    if (tCost.iCardHp > 0)
    {
        eIcon = MINIGAME0::ICON_HP;
        iCost = tCost.iCardHp;
    }
    else if (tCost.iMoodGood > 0)
    {
        eIcon = MINIGAME0::ICON_MOOD_GOOD;
        iCost = tCost.iMoodGood;
    }
    else if (tCost.iAttention > 0)
    {
        eIcon = MINIGAME0::ICON_ATTENTION;
        iCost = tCost.iAttention;
    }

    if (MINIGAME0::ICON_END == eIcon)
        return;

    Render_Icon(eIcon, fX, fY, 24.f);
    Render_Text(L"-" + to_wstring(iCost), _vec2(fX + 14.f, fY - 11.f), Color, false, bOutline);
}

void CMG0_HUD::Render_Quad(Engine::CTexture* pTexture, _uint iFrame, _float fX, _float fY, _float fSizeX, _float fSizeY, DWORD dwTint)
{
    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, fSizeX, fSizeY, 1.f);
    D3DXMatrixTranslation(&matTrans, fX, fY, 0.5f);
    _matrix matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTint);
    pTexture->Set_Texture(iFrame);
    m_pBufferCom->Render_Buffer();
}

void CMG0_HUD::Render_Rect(_float fX, _float fY, _float fHalfX, _float fHalfY, DWORD dwColor)
{
    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, fHalfX, fHalfY, 1.f);
    D3DXMatrixTranslation(&matTrans, fX - WINCX * 0.5f, WINCY * 0.5f - fY, 0.5f);
    _matrix matWorld = matScale * matTrans;

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

void CMG0_HUD::Render_Icon(MINIGAME0::MGICON eIcon, _float fX, _float fY, _float fSize, DWORD dwTint)
{
    Render_Icon(eIcon, fX, fY, fSize, fSize, dwTint);
}

void CMG0_HUD::Render_Icon(MINIGAME0::MGICON eIcon, _float fX, _float fY, _float fSizeX, _float fSizeY, DWORD dwTint)
{
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

    Render_Quad(m_pIconTex[eIcon], 0, fX - WINCX * 0.5f, WINCY * 0.5f - fY, fSizeX * 0.5f, fSizeY * 0.5f, dwTint);

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, m_dwMagFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, m_dwMinFilter);
}

void CMG0_HUD::Render_Text(const wstring& strText, _vec2 vPos, D3DXCOLOR Color, _bool bCenter, _bool bOutline, _float fScale)
{
    if (strText.empty())
        return;

    TEXTCMD tCmd;
    tCmd.strText = strText;
    tCmd.vPos = vPos;
    tCmd.Color = Color;
    tCmd.bCenter = bCenter;
    tCmd.bOutline = bOutline;
    tCmd.fScale = fScale;

    m_vecText.push_back(tCmd);
}

void CMG0_HUD::Flush_Text()
{
    m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    for (const TEXTCMD& tCmd : m_vecText)
    {
        _matrix matScale;
        D3DXMatrixScaling(&matScale, tCmd.fScale / MG_UI_FONT_SCALE, tCmd.fScale / MG_UI_FONT_SCALE, 1.f);
        m_pSprite->SetTransform(&matScale);

        vector<TEXTRUN> vecRun = Split_Text(tCmd.strText);
        _vec2 vPos = tCmd.vPos / tCmd.fScale;

        if (tCmd.bCenter)
        {
            _float fWidth = 0.f;
            for (const TEXTRUN& tRun : vecRun)
                fWidth += tRun.fWidth;

            vPos.x -= fWidth * 0.5f;
        }

        if (tCmd.bOutline)
        {
            const D3DXCOLOR OutlineColor(0.f, 0.f, 0.f, tCmd.Color.a);

            for (_int iOffsetY = -1; iOffsetY <= 1; ++iOffsetY)
            {
                for (_int iOffsetX = -1; iOffsetX <= 1; ++iOffsetX)
                {
                    if (0 == iOffsetX && 0 == iOffsetY)
                        continue;

                    Draw_Run(vecRun, vPos + _vec2((_float)iOffsetX, (_float)iOffsetY), OutlineColor);
                }
            }
        }

        Draw_Run(vecRun, vPos, tCmd.Color);
    }

    m_pSprite->End();

    m_vecText.clear();
}

void CMG0_HUD::Draw_Run(const vector<TEXTRUN>& vecRun, _vec2 vPos, D3DXCOLOR Color)
{
    for (const TEXTRUN& tRun : vecRun)
    {
        LPD3DXFONT pFont = tRun.bNumber ? m_pNumFont : m_pTextFont;
        _float fY = tRun.bNumber ? vPos.y + m_fNumOffsetY : vPos.y;

        RECT rcText{ (LONG)vPos.x * MG_UI_FONT_SCALE, (LONG)fY * MG_UI_FONT_SCALE, 0, 0 };

        pFont->DrawTextW(m_pSprite, tRun.strText.c_str(), -1, &rcText, DT_SINGLELINE | DT_NOCLIP, Color);

        vPos.x += tRun.fWidth;
    }
}

_float CMG0_HUD::Get_TextWidth(LPD3DXFONT pFont, const wstring& strText, const _tchar* pGuard)
{
    wstring strMeasure = strText + pGuard;

    RECT rc{ 0, 0, 0, 0 };
    pFont->DrawTextW(nullptr, strMeasure.c_str(), -1, &rc, DT_CALCRECT | DT_SINGLELINE | DT_NOCLIP, 0);

    return (_float)(rc.right - rc.left) / MG_UI_FONT_SCALE;
}

vector<CMG0_HUD::TEXTRUN> CMG0_HUD::Split_Text(const wstring& strText)
{
    vector<TEXTRUN> vecRun;

    for (_tchar cChar : strText)
    {
        _bool bNumber = (cChar >= L'0' && cChar <= L'9');

        if (vecRun.empty() || vecRun.back().bNumber != bNumber)
        {
            TEXTRUN tRun;
            tRun.bNumber = bNumber;
            vecRun.push_back(tRun);
        }

        vecRun.back().strText += cChar;
    }

    for (TEXTRUN& tRun : vecRun)
    {
        if (tRun.bNumber)
            tRun.fWidth = Get_TextWidth(m_pNumFont, tRun.strText, L"0") - m_fNumGuard;
        else
            tRun.fWidth = Get_TextWidth(m_pTextFont, tRun.strText, L".") - m_fTextGuard;
    }

    return vecRun;
}

void CMG0_HUD::Render_BossHp()
{
    const _float fY = 36.f;
    const _float fFaceX = 254.f;
    const _float fFaceHalf = 22.f;
    const _float fBarLeft = 284.f;
    const _float fBarHalfX = 150.f;
    const _float fBarHalfY = 9.f;
    const _float fBarX = fBarLeft + fBarHalfX;

    Render_Rect(fFaceX, fY, fFaceHalf + 2.f, fFaceHalf + 2.f, Rect_Border);

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

    Render_Quad(m_pBossFaceTex, 0, fFaceX - WINCX * 0.5f, WINCY * 0.5f - fY, fFaceHalf, fFaceHalf, 0xFFFFFFFF);

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, m_dwMagFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, m_dwMinFilter);

    Render_Rect(fBarX, fY, fBarHalfX + 2.f, fBarHalfY + 2.f, Rect_Border);
    Render_Rect(fBarX, fY, fBarHalfX, fBarHalfY, Rect_BossBack);
    Render_Rect(fBarLeft + fBarHalfX * m_fBossHpRate, fY, fBarHalfX * m_fBossHpRate, fBarHalfY, Rect_BossHp);
}

void CMG0_HUD::Render_Warning()
{
    const _float fX = WINCX * 0.5f;
    const _float fY = WINCY * 0.5f;

    Render_Rect(fX, fY, 150.f, 34.f, Rect_Warning);
    Render_Rect(fX, fY, 146.f, 30.f, Rect_WarningBack);
}

void CMG0_HUD::Render_WarningText()
{
    const wstring strText = L"WARNING";
    const _vec2 vPos(WINCX * 0.5f - strText.size() * 8.f, WINCY * 0.5f - 16.f);

    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", strText.c_str(), &vPos, D3DXCOLOR(1.f, 0.2f, 0.2f, 1.f));
}

void CMG0_HUD::Render_GameOverText()
{
    const wstring strText = L"GAME OVER";
    const _vec2 vPos(WINCX * 0.5f - strText.size() * 8.f, WINCY * 0.5f - 16.f);

    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", strText.c_str(), &vPos, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
}

void CMG0_HUD::Render_ClearText()
{
    _vec2 vPos(150.f, 100.f);
    _vec2 vShadow = vPos + _vec2(2.f, 2.f);

    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", L"CLEARED!", &vShadow, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", L"CLEARED!", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CMG0_HUD::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pHeartTex = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HpUITexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_HeartTexture", pComponent });

    pComponent = m_pHitTex = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HitScreenTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_HitTexture", pComponent });

    for (_int i = 0; i < MINIGAME0::ICON_END; ++i)
    {
        pComponent = m_pIconTex[i] = CTexture::Create(m_pGraphicDev, TEX_NORMAL, MINIGAME0::MG_ICON_PATH[i]);

        if (nullptr == pComponent)
            return E_FAIL;

        m_mapComponent[ID_STATIC].insert({ L"Com_IconTexture_" + to_wstring(i), pComponent });
    }

    pComponent = m_pBossFaceTex = CTexture::Create(m_pGraphicDev, TEX_NORMAL, MG_BOSS_FACE_PATH);

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_BossFaceTexture", pComponent });

    return S_OK;
}

HRESULT CMG0_HUD::Ready_Font()
{
    if (FAILED(D3DXCreateFontW(m_pGraphicDev, MG_UI_TEXT_HEIGHT * MG_UI_FONT_SCALE, 0, FW_SEMIBOLD, 1, FALSE, HANGUL_CHARSET,
        OUT_TT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"IBM Plex Sans KR SemiBold", &m_pTextFont)))
        return E_FAIL;

    if (FAILED(D3DXCreateFontW(m_pGraphicDev, MG_UI_NUM_HEIGHT * MG_UI_FONT_SCALE, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"GakNum", &m_pNumFont)))
        return E_FAIL;

    if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
        return E_FAIL;

    TEXTMETRICW tTextMetric, tNumMetric;
    m_pTextFont->GetTextMetricsW(&tTextMetric);
    m_pNumFont->GetTextMetricsW(&tNumMetric);

    m_fNumOffsetY = (_float)(tTextMetric.tmAscent - tNumMetric.tmAscent) / MG_UI_FONT_SCALE;
    m_fTextGuard = Get_TextWidth(m_pTextFont, L"", L".");
    m_fNumGuard = Get_TextWidth(m_pNumFont, L"", L"0");

    return S_OK;
}

CMG0_HUD* CMG0_HUD::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_HUD* pHUD = new CMG0_HUD(pGraphicDev);

    if (FAILED(pHUD->Ready_GameObject()))
    {
        Safe_Release(pHUD);
        MSG_BOX("CMG0_HUD Create Failed");
        return nullptr;
    }

    return pHUD;
}

void CMG0_HUD::Free()
{
    Safe_Release(m_pSprite);
    Safe_Release(m_pNumFont);
    Safe_Release(m_pTextFont);

    CGameObject::Free();
}
