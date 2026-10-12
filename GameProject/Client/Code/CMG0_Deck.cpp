#include "pch.h"
#include "CMG0_Deck.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CTexture.h"
#include "CMiniGame.h"
#include "CMG0_Card.h"
#include "CMG0_Player.h"

CMG0_Deck::CMG0_Deck(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Deck::~CMG0_Deck()
{
}

HRESULT CMG0_Deck::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_tState.iStatus[MINIGAME0::CARD_HP] = MG_CARD_HP;

    Ready_Deck();

    return CGameObject::Ready_GameObject();
}

HRESULT CMG0_Deck::Add_Component()
{
    for (_int i = 0; i < MINIGAME0::CARD_END; ++i)
    {
        m_pCardTex[i] = CTexture::Create(m_pGraphicDev, TEX_NORMAL, MINIGAME0::MG_CARD_IMG_PATH[i]);

        if (nullptr == m_pCardTex[i])
            return E_FAIL;

        m_mapComponent[ID_STATIC].insert({ L"Com_CardTexture_" + to_wstring(i), m_pCardTex[i] });
    }

    return S_OK;
}

_int CMG0_Deck::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (m_fNoticeTime > 0.f)
        m_fNoticeTime -= fTimeDelta;

    CMiniGame* pScene = Get_Scene();
    if (nullptr == pScene)
        return iExit;

    if (m_bFinished)
        return iExit;

    _float fSceneTime = pScene->Get_SceneTime();

    m_bCardTime = fSceneTime >= MG_CARD_START_TIME - MG_CARD_READY_TIME;
    m_fTimeRate = max(0.f, min(1.f, (MG_CARD_END_TIME - fSceneTime) / (MG_CARD_END_TIME - MG_CARD_START_TIME)));

    if (m_bComplete)
    {
        if (fSceneTime >= MG_CARD_END_TIME + MG_CARD_COMPLETE_TIME)
            Finish();

        return iExit;
    }

    if (fSceneTime >= MG_CARD_END_TIME)
    {
        Complete();
        return iExit;
    }

    if (!m_bPlaying)
    {
        if (fSceneTime >= MG_CARD_START_TIME)
            Start_Turn();

        return iExit;
    }

    Key_Input();
    Update_Hand();

    return iExit;
}

void CMG0_Deck::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Deck::Update_Hand()
{
    for (_int i = 0; i < MG_CARD_HAND_COUNT; ++i)
    {
        if (nullptr == m_pHand[i])
            continue;

        m_pHand[i]->Set_Usable(Can_Use(m_pHand[i]->Get_Card()));
        m_pHand[i]->Set_Selected(i == m_iSelect);
    }
}

void CMG0_Deck::Start_Turn()
{
    ++m_iTurn;
    m_bPlaying = true;
    m_bActionUsed = false;
    m_iSelect = MG_CARD_SELECT_NONE;
    m_tState.iExtraAction = 0;

    Update_Status();

    Draw_Hand();
}

void CMG0_Deck::Next_Turn()
{
    Discard_Hand();

    if (m_iTurn >= MG_CARD_MAX_TURN)
        Complete();
    else
        Start_Turn();
}

void CMG0_Deck::Complete()
{
    Discard_Hand();

    m_iSelect = MG_CARD_SELECT_NONE;
    m_bPlaying = false;
    m_bComplete = true;
}

void CMG0_Deck::Finish()
{
    m_bComplete = false;
    m_bCardTime = false;
    m_bFinished = true;
}

void CMG0_Deck::Key_Input()
{
    CDInputMgr* pInput = CDInputMgr::GetInstance();

    if (pInput->Key_Down(DIK_Z))
        Select(0);
    if (pInput->Key_Down(DIK_X))
        Select(1);
    if (pInput->Key_Down(DIK_C))
        Select(2);
    if (pInput->Key_Down(DIK_V))
        m_iSelect = MG_CARD_SELECT_REST;

    if (pInput->Key_Down(DIK_RETURN))
        Confirm();
}

void CMG0_Deck::Select(_int iSlot)
{
    if (nullptr == m_pHand[iSlot] || !Can_Use(m_pHand[iSlot]->Get_Card()))
        return;

    m_iSelect = iSlot;
}

void CMG0_Deck::Confirm()
{
    if (MG_CARD_SELECT_NONE == m_iSelect)
        return;

    if (MG_CARD_SELECT_REST == m_iSelect)
    {
        Rest();
        Next_Turn();
        return;
    }

    if (!Use_Card(m_iSelect))
        return;

    m_iSelect = MG_CARD_SELECT_NONE;

    if (m_tState.iExtraAction <= 0 || Is_HandEmpty())
        Next_Turn();
}

_bool CMG0_Deck::Is_HandEmpty()
{
    for (CMG0_Card* pCard : m_pHand)
    {
        if (pCard)
            return false;
    }

    return true;
}

void CMG0_Deck::Rest()
{
    _int iRecover = Recover_CardHp(MG_CARD_SKIP_HP);

    Set_Notice(L"휴식  체력 +" + to_wstring(iRecover));
}

_bool CMG0_Deck::Use_Card(_int iSlot)
{
    CMG0_Card* pCard = m_pHand[iSlot];
    if (nullptr == pCard)
        return false;

    MINIGAME0::MGCARD eCard = pCard->Get_Card();
    if (!Can_Use(eCard))
        return false;

    Activate_Card(eCard);
    Sync_Attack();

    m_bActionUsed = true;

    if (MINIGAME0::REUSABLE == MINIGAME0::MG_CARD_DATA[eCard].eType)
        m_vecDiscard.push_back(eCard);

    pCard->Set_Dead(true);
    m_pHand[iSlot] = nullptr;

    return true;
}

void CMG0_Deck::Activate_Card(MINIGAME0::MGCARD eCard)
{
    if (m_bActionUsed)
        Consume_ExtraAction(1);

    Pay_Cost(Get_Cost(eCard));

    _int iActivate = 1 + m_tState.iExtraActivate;
    m_tState.iExtraActivate = 0;

    for (_int i = 0; i < iActivate; ++i)
        Apply_Effect(eCard);

    if (Is_ActiveCard(eCard))
        Add_ActiveParameter();
}

void CMG0_Deck::Sync_Attack()
{
    CMiniGame* pScene = Get_Scene();
    if (nullptr == pScene)
        return;

    pScene->Get_Player()->Set_Attack(m_tState.iAttack);
}

MINIGAME0::MGCARDSTATE CMG0_Deck::Get_Preview()
{
    MINIGAME0::MGCARDSTATE tState = m_tState;

    if (!m_bPlaying || MG_CARD_SELECT_NONE == m_iSelect)
        return tState;

    if (MG_CARD_SELECT_REST == m_iSelect)
        Recover_CardHp(MG_CARD_SKIP_HP);
    else if (m_pHand[m_iSelect])
        Activate_Card(m_pHand[m_iSelect]->Get_Card());

    MINIGAME0::MGCARDSTATE tPreview = m_tState;
    m_tState = tState;

    return tPreview;
}

_bool CMG0_Deck::Can_Use(MINIGAME0::MGCARD eCard)
{
    MINIGAME0::MGCARDCOST tCost = Get_Cost(eCard);

    if (m_tState.iStatus[MINIGAME0::CARD_HP] + m_tState.iStatus[MINIGAME0::STAMINA] < tCost.iCardHp)
        return false;
    if (m_tState.iStatus[MINIGAME0::MOOD_GOOD] < tCost.iMoodGood)
        return false;
    if (m_tState.iStatus[MINIGAME0::ATTENTION] < tCost.iAttention)
        return false;

    return true;
}

_bool CMG0_Deck::Is_ActiveCard(MINIGAME0::MGCARD eCard)
{
    switch (eCard)
    {
    case MINIGAME0::CARD_APPEAL:
    case MINIGAME0::CARD_HIGHTOUCH:
    case MINIGAME0::CARD_EXCITE:
    case MINIGAME0::CARD_CALLRESPONSE:
    case MINIGAME0::CARD_ACHIEVE:
    case MINIGAME0::CARD_SPRECHCHOR:
        return true;
    default:
        return false;
    }
}

MINIGAME0::MGCARDCOST CMG0_Deck::Get_Cost(MINIGAME0::MGCARD eCard)
{
    MINIGAME0::MGCARDCOST tCost;

    switch (eCard)
    {
    case MINIGAME0::CARD_APPEAL:
        tCost.iCardHp = 3;
        break;
    case MINIGAME0::CARD_TVAPPEAR:
        tCost.iCardHp = 1;
        break;
    case MINIGAME0::CARD_HIGHTOUCH:
        tCost.iCardHp = 4;
        break;
    case MINIGAME0::CARD_BREATH:
        tCost.iCardHp = 7;
        break;
    case MINIGAME0::CARD_EXCITE:
        tCost.iCardHp = 4;
        break;
    case MINIGAME0::CARD_SPEAKER:
        tCost.iCardHp = 3;
        break;
    case MINIGAME0::CARD_CALLRESPONSE:
        tCost.iCardHp = 6;
        break;
    case MINIGAME0::CARD_NATIONALIDOL:
        tCost.iMoodGood = 1;
        break;
    case MINIGAME0::CARD_ACHIEVE:
        tCost.iCardHp = 10;
        break;
    case MINIGAME0::CARD_ENTERTAINMENT:
        tCost.iAttention = 2;
        break;
    case MINIGAME0::CARD_PRESENCE:
        tCost.iMoodGood = 1;
        break;
    case MINIGAME0::CARD_SPRECHCHOR:
        tCost.iAttention = 2;
        break;
    default:
        break;
    }

    tCost.iCardHp = Apply_CostHalf(tCost.iCardHp);

    return tCost;
}

_float CMG0_Deck::Get_MoodRate()
{
    if (m_tState.iStatus[MINIGAME0::MOOD_GOOD] <= 0)
        return 1.f;

    _float fRate = MG_MOOD_GOOD_RATE;

    if (m_tState.iStatus[MINIGAME0::MOOD_PERFECT] > 0)
        fRate += MG_MOOD_PERFECT_RATE * m_tState.iStatus[MINIGAME0::MOOD_GOOD];

    return fRate;
}

CMiniGame* CMG0_Deck::Get_Scene()
{
    return dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
}

void CMG0_Deck::Set_Notice(const wstring& strNotice)
{
    m_strNotice = strNotice;
    m_fNoticeTime = MG_CARD_NOTICE_TIME;
}

void CMG0_Deck::Ready_Deck()
{
    for (MINIGAME0::MGCARD eCard : MINIGAME0::MG_DECK)
        m_vecDiscard.push_back(eCard);

    Shuffle_Deck();
}

void CMG0_Deck::Shuffle_Deck()
{
    m_vecDeck.insert(m_vecDeck.end(), m_vecDiscard.begin(), m_vecDiscard.end());
    m_vecDiscard.clear();

    for (size_t i = m_vecDeck.size(); i > 1; --i)
        swap(m_vecDeck[i - 1], m_vecDeck[rand() % i]);
}

MINIGAME0::MGCARD CMG0_Deck::Draw_Card()
{
    if (m_vecDeck.empty())
        Shuffle_Deck();

    if (m_vecDeck.empty())
        return MINIGAME0::CARD_END;

    MINIGAME0::MGCARD eCard = m_vecDeck.back();
    m_vecDeck.pop_back();

    return eCard;
}

void CMG0_Deck::Draw_Hand()
{
    CMiniGame* pScene = Get_Scene();
    if (nullptr == pScene)
        return;

    for (_int i = 0; i < MG_CARD_HAND_COUNT; ++i)
    {
        MINIGAME0::MGCARD eCard = Draw_Card();
        if (MINIGAME0::CARD_END == eCard)
            break;

        m_pHand[i] = CMG0_Card::Create(m_pGraphicDev, eCard, i, m_pCardTex[eCard]);
        if (FAILED(pScene->Add_GameObject(L"Card", m_pHand[i])))
        {
            Safe_Release(m_pHand[i]);
            m_pHand[i] = nullptr;
            m_vecDiscard.push_back(eCard);
        }
    }
}

void CMG0_Deck::Discard_Hand()
{
    for (CMG0_Card*& pCard : m_pHand)
    {
        if (nullptr == pCard)
            continue;

        m_vecDiscard.push_back(pCard->Get_Card());
        pCard->Set_Dead(true);
        pCard = nullptr;
    }
}

void CMG0_Deck::Pay_Cost(const MINIGAME0::MGCARDCOST& tCost)
{
    Consume_CardHp(tCost.iCardHp);
    Consume_MoodGood(tCost.iMoodGood);
    Consume_Attention(tCost.iAttention);
}

void CMG0_Deck::Apply_Effect(MINIGAME0::MGCARD eCard)
{
    switch (eCard)
    {
    case MINIGAME0::CARD_APPEAL:
        Add_Parameter(14);
        break;
    case MINIGAME0::CARD_BULB:
        Add_Stamina(7);
        break;
    case MINIGAME0::CARD_TVAPPEAR:
        Add_Stamina(5);
        Add_CostHalf(5);
        break;
    case MINIGAME0::CARD_HIGHTOUCH:
        Add_Parameter(23, 2.f);
        break;
    case MINIGAME0::CARD_BREATH:
        Add_MoodGood(4);
        Add_Attention(5);
        break;
    case MINIGAME0::CARD_EXCITE:
        Add_Parameter(10);
        Add_MoodPerfect(4);
        break;
    case MINIGAME0::CARD_SPEAKER:
        Add_MoodGood(7);
        break;
    case MINIGAME0::CARD_CALLRESPONSE:
        Add_Parameter(15);
        if (m_tState.iStatus[MINIGAME0::ATTENTION] >= 3)
            Add_Parameter(34, 1.5f);
        break;
    case MINIGAME0::CARD_NATIONALIDOL:
        Add_Stamina(8);
        m_tState.iExtraActivate += 1;
        Add_ExtraAction(1);
        break;
    case MINIGAME0::CARD_ACHIEVE:
        Add_MoodGood(7);
        Add_Parameter(40);
        break;
    case MINIGAME0::CARD_ENTERTAINMENT:
        m_tState.iActiveParameter += 5;
        break;
    case MINIGAME0::CARD_PRESENCE:
        Add_Attention(6);
        Add_ExtraAction(1);
        break;
    case MINIGAME0::CARD_SPRECHCHOR:
        Add_Parameter(6);
        Add_MoodGood(3);
        Add_ExtraAction(1);
        break;
    default:
        break;
    }
}

void CMG0_Deck::Add_Parameter(_int iParameter, _float fAttentionRate)
{
    _float fParameter = iParameter + m_tState.iStatus[MINIGAME0::ATTENTION] * fAttentionRate;

    m_tState.iAttack += (_int)ceilf(fParameter * Get_MoodRate() - 0.001f);
}

void CMG0_Deck::Add_ActiveParameter()
{
    if (m_tState.iActiveParameter > 0)
        Add_Parameter(m_tState.iActiveParameter);
}

void CMG0_Deck::Add_MoodGood(_int iTurn)
{
    Add_Turn(MINIGAME0::MOOD_GOOD, iTurn);
}

void CMG0_Deck::Add_MoodPerfect(_int iTurn)
{
    Add_Turn(MINIGAME0::MOOD_PERFECT, iTurn);
}

void CMG0_Deck::Add_CostHalf(_int iTurn)
{
    Add_Turn(MINIGAME0::COST_HALF, iTurn);
}

void CMG0_Deck::Add_Attention(_int iValue)
{
    m_tState.iStatus[MINIGAME0::ATTENTION] += iValue;
}

void CMG0_Deck::Add_Stamina(_int iValue)
{
    m_tState.iStatus[MINIGAME0::STAMINA] += iValue;
}

void CMG0_Deck::Add_ExtraAction(_int iCount)
{
    m_tState.iExtraAction += iCount;
}

void CMG0_Deck::Consume_ExtraAction(_int iCount)
{
    m_tState.iExtraAction = max(0, m_tState.iExtraAction - iCount);
}

_int CMG0_Deck::Recover_CardHp(_int iValue)
{
    _int iRecover = min(iValue, MG_CARD_HP - m_tState.iStatus[MINIGAME0::CARD_HP]);

    m_tState.iStatus[MINIGAME0::CARD_HP] += iRecover;

    return iRecover;
}

void CMG0_Deck::Consume_CardHp(_int iCost)
{
    _int iStamina = min(m_tState.iStatus[MINIGAME0::STAMINA], iCost);

    m_tState.iStatus[MINIGAME0::STAMINA] -= iStamina;
    m_tState.iStatus[MINIGAME0::CARD_HP] -= iCost - iStamina;
}

void CMG0_Deck::Consume_MoodGood(_int iTurn)
{
    m_tState.iStatus[MINIGAME0::MOOD_GOOD] -= iTurn;
}

void CMG0_Deck::Consume_Attention(_int iValue)
{
    m_tState.iStatus[MINIGAME0::ATTENTION] -= iValue;
}

_int CMG0_Deck::Apply_CostHalf(_int iCost)
{
    if (m_tState.iStatus[MINIGAME0::COST_HALF] <= 0)
        return iCost;

    return (iCost + 1) / 2;
}

void CMG0_Deck::Add_Turn(MINIGAME0::MGSTATUS eStatus, _int iTurn)
{
    if (m_tState.iStatus[eStatus] <= 0)
        m_tState.bFreshTurn[eStatus] = true;

    m_tState.iStatus[eStatus] += iTurn;
}

void CMG0_Deck::Update_Turn(MINIGAME0::MGSTATUS eStatus)
{
    if (m_tState.bFreshTurn[eStatus])
        m_tState.bFreshTurn[eStatus] = false;
    else if (m_tState.iStatus[eStatus] > 0)
        --m_tState.iStatus[eStatus];
}

void CMG0_Deck::Update_Status()
{
    Update_Turn(MINIGAME0::MOOD_GOOD);
    Update_Turn(MINIGAME0::MOOD_PERFECT);
    Update_Turn(MINIGAME0::COST_HALF);
}

CMG0_Deck* CMG0_Deck::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Deck* pDeck = new CMG0_Deck(pGraphicDev);

    if (FAILED(pDeck->Ready_GameObject()))
    {
        Safe_Release(pDeck);
        MSG_BOX("CMG0_Deck Create Failed");
        return nullptr;
    }

    return pDeck;
}

void CMG0_Deck::Free()
{
    CGameObject::Free();
}
