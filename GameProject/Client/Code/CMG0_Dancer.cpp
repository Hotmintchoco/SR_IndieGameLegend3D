#include "pch.h"
#include "CMG0_Dancer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CSoundMgr.h"
#include "CMiniGame.h"
#include "CMG0_Deck.h"
#include "CMG0_Player.h"
#include "CMG0_Dialogue.h"
#include "CMG0_Sunglass.h"
#include "CMG0_DancerHead.h"
#include "CMG0_DancerBody.h"
#include "CMG0_DancerLArm.h"
#include "CMG0_DancerRArm.h"
#include "CMG0_DancerLLeg.h"
#include "CMG0_DancerRLeg.h"

CMG0_Dancer::CMG0_Dancer(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Dancer::~CMG0_Dancer()
{
}

HRESULT CMG0_Dancer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Ready_Part()))
        return E_FAIL;

    Update_Part(0.f);

    return CGameObject::Ready_GameObject();
}

_int CMG0_Dancer::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return iExit;

    Update_State(pScene->Get_Dialogue(), pScene->Get_Deck(), pScene->Get_Player(), fTimeDelta);
    Update_Part(fTimeDelta);
    Update_Sunglass(pScene->Get_Deck(), fTimeDelta);

    if (STATE_END != m_eState)
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CMG0_Dancer::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Dancer::Render_GameObject()
{
    m_pTextureCom->Set_Texture(0);

    for (CMG0_DancerPart* pPart : m_pPart)
        pPart->Render_GameObject();

    if (m_pSunglass)
        m_pSunglass->Render_GameObject();
}

void CMG0_Dancer::Update_State(CMG0_Dialogue* pDialogue, CMG0_Deck* pDeck, CMG0_Player* pPlayer, _float fTimeDelta)
{
    if (m_eState < STATE_BOARD && pDeck->Is_Finished())
    {
        m_eState = STATE_BOARD;
        m_fStateTime = 0.f;

        m_pTransformCom->Set_Scale(MG_PILOT_SCALE, MG_PILOT_SCALE, MG_PILOT_SCALE);
        m_pTransformCom->Set_Rotation_Raw(_vec3(0.f, 0.f, 0.f));
    }

    switch (m_eState)
    {
    case STATE_WAIT:
        if (pDialogue->Is_Finished())
        {
            m_eState = STATE_ENTER;
            m_fStateTime = 0.f;
        }
        break;
    case STATE_ENTER:
    {
        m_fStateTime += fTimeDelta;

        _float fRate = min(1.f, m_fStateTime / MG_DANCER_ENTER_TIME);
        _float fEase = 1.f - powf(1.f - fRate, 3.f);

        m_fHeight = MG_DANCER_ENTER_HEIGHT * (1.f - fEase);

        if (fRate >= 1.f)
            m_eState = STATE_IDLE;
        break;
    }
    case STATE_IDLE:
        m_fHeight = 0.f;

        if (pDeck->Is_Playing())
        {
            m_eState = STATE_DANCE;
            m_fDanceTime = 0.f;
        }
        break;
    case STATE_DANCE:
        m_fDanceTime += fTimeDelta;

        if (!pDeck->Is_Playing())
            m_eState = STATE_IDLE;
        break;
    case STATE_BOARD:
    {
        m_fStateTime += fTimeDelta;

        _float fRate = min(1.f, m_fStateTime / MG_PILOT_BOARD_TIME);
        _float fEase = 1.f - (1.f - fRate) * (1.f - fRate);

        m_vPos = pPlayer->Get_Pos() + MG_PILOT_OFFSET;
        m_fHeight = MG_PILOT_HEIGHT * (1.f - fEase);

        if (fRate >= 1.f)
            m_eState = STATE_END;
        break;
    }
    default:
        break;
    }
}

void CMG0_Dancer::Update_Part(_float fTimeDelta)
{
    m_pTransformCom->Set_Pos(m_vPos + _vec3(0.f, m_fHeight, 0.f));

    const _matrix& matRoot = *m_pTransformCom->Get_World();
    const MINIGAME0::MGDANCEBEAT tBeat = Get_DanceBeat();

    for (CMG0_DancerPart* pPart : m_pPart)
        pPart->Update_Part(tBeat, fTimeDelta, matRoot);
}

void CMG0_Dancer::Update_Sunglass(CMG0_Deck* pDeck, _float fTimeDelta)
{
    if (!m_bSunglass && pDeck->Get_State().iAttack >= MG_WEAPON_DIAMOND)
    {
        m_bSunglass = true;
        m_pSunglass = CMG0_Sunglass::Create(m_pGraphicDev);

        CSoundMgr::GetInstance()->SetSFXChannelVolume(CSoundMgr::GetInstance()->PlaySFX(L"sfxOMG.wav"), MG_SUNGLASS_SFX_VOLUME);
    }

    if (m_pSunglass)
        m_pSunglass->Update_Sunglass(fTimeDelta, *m_pPart[PART_HEAD]->Get_Transform()->Get_World());
}

MINIGAME0::MGDANCEBEAT CMG0_Dancer::Get_DanceBeat()
{
    MINIGAME0::MGDANCEBEAT tBeat;

    tBeat.bSit = (STATE_BOARD == m_eState);

    if (STATE_DANCE != m_eState)
        return tBeat;

    _float fBeat = m_fDanceTime / MG_DANCE_BEAT;
    _int iBeat = (_int)fBeat;
    _float fRate = min(1.f, (fBeat - iBeat) / MG_DANCE_MOVE_RATE);

    tBeat.bDance = true;
    tBeat.iPoseFrom = (iBeat + MG_DANCE_POSE_COUNT - 1) % MG_DANCE_POSE_COUNT;
    tBeat.iPoseTo = iBeat % MG_DANCE_POSE_COUNT;
    tBeat.fRate = fRate * fRate * (3.f - 2.f * fRate);

    return tBeat;
}

HRESULT CMG0_Dancer::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Player_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    m_pTransformCom->Set_Scale(MG_DANCER_SCALE, MG_DANCER_SCALE, MG_DANCER_SCALE);
    m_pTransformCom->Set_Rotation_Raw(_vec3(0.f, 180.f, 0.f));

    return S_OK;
}

HRESULT CMG0_Dancer::Ready_Part()
{
    m_pPart[PART_BODY] = CMG0_DancerBody::Create(m_pGraphicDev);
    m_pPart[PART_HEAD] = CMG0_DancerHead::Create(m_pGraphicDev);
    m_pPart[PART_LARM] = CMG0_DancerLArm::Create(m_pGraphicDev);
    m_pPart[PART_RARM] = CMG0_DancerRArm::Create(m_pGraphicDev);
    m_pPart[PART_LLEG] = CMG0_DancerLLeg::Create(m_pGraphicDev);
    m_pPart[PART_RLEG] = CMG0_DancerRLeg::Create(m_pGraphicDev);

    for (CMG0_DancerPart* pPart : m_pPart)
    {
        if (nullptr == pPart)
            return E_FAIL;
    }

    m_pPart[PART_HEAD]->Set_Parent(m_pPart[PART_BODY]);
    m_pPart[PART_LARM]->Set_Parent(m_pPart[PART_BODY]);
    m_pPart[PART_RARM]->Set_Parent(m_pPart[PART_BODY]);

    return S_OK;
}

CMG0_Dancer* CMG0_Dancer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Dancer* pDancer = new CMG0_Dancer(pGraphicDev);

    if (FAILED(pDancer->Ready_GameObject()))
    {
        Safe_Release(pDancer);
        MSG_BOX("CMG0_Dancer Create Failed");
        return nullptr;
    }

    return pDancer;
}

void CMG0_Dancer::Free()
{
    for (CMG0_DancerPart*& pPart : m_pPart)
        Safe_Release(pPart);

    Safe_Release(m_pSunglass);

    CGameObject::Free();
}
