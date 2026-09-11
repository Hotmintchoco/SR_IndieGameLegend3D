#include "pch.h"
#include "CLogo.h"
#include "CBackGround.h"
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

	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);

	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	if (m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			CScene* pStage = CStage::Create(m_pGraphicDev);

			if (nullptr == pStage)
				return E_FAIL;

			if (FAILED(CManagement::GetInstance()->Set_Scene(pStage)))
			{
				Safe_Release(pStage);
				MSG_BOX("Stage Create Failed");
				return -1;
			}
			else
			{
				/* 이 시점에 매니저 클래스의 씬 변수가 보장됨 */
				if (FAILED(static_cast<CStage*>(pStage)->PostInitialize()))
				{
					MSG_BOX("Stage PostInitialize Failed");
					return -1;
				}
			}
		}
	}

	return iExit;
}

void CLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
	CScene::LateUpdate_Scene(fTimeDelta);
}

void CLogo::Render_Scene()
{	

	_vec2	vPos{ 100.f, 100.f };
	CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", m_pLoading->Get_String(), &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	// 오브젝트 추가
	CGameObject* pGameObject = nullptr;

	// BackGround

	pGameObject = CBackGround::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"BackGround", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag ,pLayer });

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LogoTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Logo/sana.jpg", 1))))
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
