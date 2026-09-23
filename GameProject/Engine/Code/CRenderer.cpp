#include "CRenderer.h"
#include "CDebugMgr.h"
#include "IRenderable.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eID, IRenderable* pRenderable)
{
	if (eID >= RENDER_END || nullptr == pRenderable)
		return;

	m_RenderGroup[eID].push_back(pRenderable);
	/* 성철 : 기존 GameObject에서 Component 까지 확장하다 보니 CBase* 기반으로 변경 */
	// dynamic_cast<CBase*>(pRenderable)->AddRef();
	pRenderable->GetBase()->AddRef();
}

void CRenderer::Render(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_AlphaTest(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_Collider(pGraphicDev);

	Render_UI(pGraphicDev);

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		/* 성철 : 결론적으로 그대로 둬도 되는거였는데, 어쩌다 보니 바꾸게 됨 */
		for (auto iter = m_RenderGroup[i].begin(); iter != m_RenderGroup[i].end(); ++iter)
		{
			//CBase* p = dynamic_cast<CBase*>(*iter);
			CBase* p = (*iter)->GetBase();
			Safe_Release<CBase*>(p);
		}
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Begin_WireFrame(LPDIRECT3DDEVICE9& pGraphicDev, PIPELINESTATE& tOld, D3DCOLOR dwColor)
{
	pGraphicDev->GetRenderState(D3DRS_FILLMODE, &tOld.dwFill);
	pGraphicDev->GetRenderState(D3DRS_CULLMODE, &tOld.dwCull);
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, &tOld.dwLighting);
	pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &tOld.dwTexFactor);
	pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &tOld.dwColorOp);
	pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &tOld.dwColorArg1);
	pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &tOld.dwAlphaOp);
	pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &tOld.dwAlphaArg1);
	pGraphicDev->GetTexture(0, &tOld.pTexture);   // AddRef 되므로 End에서 Release

	pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* 텍스쳐를 참조하지 않도록 컬러/알파 경로를 모두 TFACTOR로 고정 */
	pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
}

void CRenderer::End_WireFrame(LPDIRECT3DDEVICE9& pGraphicDev, PIPELINESTATE& tOld)
{
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, tOld.dwAlphaArg1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, tOld.dwAlphaOp);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, tOld.dwColorArg1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, tOld.dwColorOp);
	pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, tOld.dwTexFactor);

	pGraphicDev->SetTexture(0, tOld.pTexture);
	if (nullptr != tOld.pTexture)
	{
		tOld.pTexture->Release();
		tOld.pTexture = nullptr;
	}

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, tOld.dwLighting);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, tOld.dwCull);
	pGraphicDev->SetRenderState(D3DRS_FILLMODE, tOld.dwFill);
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_PRIORITY])
		pObj->Render(pGraphicDev);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_NONE) return;

	_bool bWire = (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_WIREFRAME);

	PIPELINESTATE tOld;
	if (bWire)
		Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(200, 200, 200));

	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA])
		pObj->Render(pGraphicDev);

	if (bWire)
		End_WireFrame(pGraphicDev, tOld);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_NONE) return;

	_bool bWire = (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_WIREFRAME);

	m_RenderGroup[RENDER_ALPHA].sort([](IRenderable* pDst, IRenderable* pSrc)->bool
		{
			return pDst->Get_ViewZ() > pSrc->Get_ViewZ();
		});

	PIPELINESTATE tOld;

	if (bWire)
	{
		Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(200, 200, 200));
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	for (auto& pObj : m_RenderGroup[RENDER_ALPHA])
		pObj->Render(pGraphicDev);

	if (bWire)
	{
		End_WireFrame(pGraphicDev, tOld);
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void CRenderer::Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_NONE) return;

	_bool bWire = (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_WIREFRAME);

	PIPELINESTATE tOld;

	if (bWire)
	{
		Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(200, 200, 200));
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x80);

	}

	for (auto& pObj : m_RenderGroup[RENDER_ALPHATEST])
		pObj->Render(pGraphicDev);

	if (bWire)
	{
		End_WireFrame(pGraphicDev, tOld);
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	}
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	_matrix matOldView, matOldProj, matView, matProj;
	pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj, (float)WINCX, (float)WINCY, 0.f, 1.f);

	pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& pObj : m_RenderGroup[RENDER_UI])
		pObj->Render(pGraphicDev);

	pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
}

void CRenderer::Render_Collider(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (!CDebugMgr::GetInstance()->GetShowCollider()) return;

	auto& rGroup = m_RenderGroup[RENDER_DEBUG_COLLIDER];
	if (rGroup.empty())
		return;

	_matrix matOldWorld;
	pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);

	PIPELINESTATE tOld;
	Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(0, 255, 0));

	for (auto& pObj : rGroup)
		pObj->Render(pGraphicDev);

	End_WireFrame(pGraphicDev, tOld);

	pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
