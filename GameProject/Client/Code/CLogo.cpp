#include "pch.h"
#include "CLogo.h"
#include "CGaugeUI.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_pLoading(nullptr)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);

	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update_Scene(_float fTimeDelta)
{
    const float fTarget = m_pLoading->Get_Progress();
    m_fDisplayProgress = min(fTarget, m_fDisplayProgress + fTimeDelta * 0.8f);
    m_pLoadingGauge->Set_Percent(m_fDisplayProgress);

    if (m_pLoading->Get_Finish() && m_fDisplayProgress >= 1.f && !m_bStageFailed)
    {
        m_fCompleteHold += fTimeDelta;
        if (m_fCompleteHold >= 0.2f)
        {
            CScene* pStage = CStage::Create(m_pGraphicDev);
            if (!pStage)
            {
                m_bStageFailed = true;
                return CScene::Update_Scene(fTimeDelta);
            }

            if (FAILED(CManagement::GetInstance()->Change_Scene(0, pStage, true)))
            {
                Safe_Release(pStage);
                return -1;
            }
            return pStage->Update_Scene(fTimeDelta);
        }
    }

    return CScene::Update_Scene(fTimeDelta);
}

void CLogo::LateUpdate_Scene(_float fTimeDelta)
{
	CScene::LateUpdate_Scene(fTimeDelta);
}

void CLogo::Render_Scene()
{
    if (m_pLoading->Get_Failed() || m_bStageFailed)
    {
        _vec2 vPos{ WINCX * 0.5f - 180.f, WINCY * 0.8f - 50.f };
        CFontMgr::GetInstance()->Render_Font(L"Font_Jinji",
            L"Loading failed. Please restart the game.", &vPos,
            D3DXCOLOR(1.f, 0.3f, 0.3f, 1.f));
    }
}

HRESULT CLogo::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (!pLayer)
        return E_FAIL;
    m_mapLayer.insert({ pLayerTag, pLayer });

    CUI* pBackground = CUI::Create(m_pGraphicDev, L"Proto_LogoTexture");
    if (!pBackground)
        return E_FAIL;
    pBackground->Set_Pos(WINCX * 0.5f, WINCY * 0.5f, 0.9f);
    pBackground->Set_Size({ WINCX * 0.5f, WINCY * 0.5f });
    if (FAILED(pLayer->Add_GameObject(L"BackGround", pBackground)))
    {
        Safe_Release(pBackground);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CLogo::Ready_UI_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (!pLayer)
        return E_FAIL;
    m_mapLayer.insert({ pLayerTag, pLayer });

    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);
    const float fX = WINCX * 0.5f;
    const float fY = WINCY * 0.8f;
    CUI* pFrame = CUI::Create(m_pGraphicDev, L"Proto_BossHpBarTexture");
    if (!pFrame)
        return E_FAIL;
    pFrame->Set_Pos(fX, fY, 0.2f);
    pFrame->Set_Size({ 324.f * fScale, 24.f * fScale });
    if (FAILED(pLayer->Add_GameObject(L"LoadingFrame", pFrame)))
    {
        Safe_Release(pFrame);
        return E_FAIL;
    }

    CGaugeUI* pGauge = CGaugeUI::Create(m_pGraphicDev, L"Proto_RedTexture", true);
    if (!pGauge)
        return E_FAIL;
    pGauge->Set_Pos(fX, fY, 0.1f);
    pGauge->Set_Size({ 306.f * fScale, 19.5f * fScale });
    pGauge->Set_Percent(0.f);
    if (FAILED(pLayer->Add_GameObject(L"LoadingGauge", pGauge)))
    {
        Safe_Release(pGauge);
        return E_FAIL;
    }
    m_pLoadingGauge = pGauge;
    return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossHpBarTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/BossHpBar.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RedTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/RedColor.png", 1))))
        return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LogoTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Logo/SkyboxStars.png", 1))))
		return E_FAIL;	

	return S_OK;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pLogo = new CLogo(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pLogo;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);
	
	CScene::Free();
}
