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
	Render_DebugTriangle(pGraphicDev);

	Render_UI(pGraphicDev);

	Render_DebugScreen(pGraphicDev);

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

	m_RenderGroup[RENDER_UI].sort([](IRenderable* pDst, IRenderable* pSrc)
		{
			return pDst->Get_Z() > pSrc->Get_Z();
		});

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

void CRenderer::Render_DebugTriangle(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (!CDebugMgr::GetInstance()->GetShowDebugTriangle()) return;

	if (m_vecDebugTri.empty())
		return;

	// 상태 백업
	DWORD dwLighting, dwCull;
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
	_matrix matOldWorld, matIdentity;
	pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);
	D3DXMatrixIdentity(&matIdentity);

	pGraphicDev->SetTransform(D3DTS_WORLD, &matIdentity);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pGraphicDev->SetTexture(0, nullptr);
	pGraphicDev->SetFVF(FVF_COL);

	// 요청된 삼각형들을 한 번에 모아서 드로우콜 1회
	std::vector<VTXCOL> vecVtx;
	vecVtx.reserve(m_vecDebugTri.size() * 3);
	for (const auto& t : m_vecDebugTri)
	{
		_vec3 vN;
		D3DXVec3Normalize(&vN, &t.vNormal);
		for (int i = 0; i < 3; ++i)
			vecVtx.push_back({ t.vTri[i] + vN * 0.002f, t.dwColor });
	}
	pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
		(UINT)m_vecDebugTri.size(), vecVtx.data(), sizeof(VTXCOL));

	// 복구
	pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);

	m_vecDebugTri.clear();   // 매 프레임 새로 요청받는 방식
}

void CRenderer::Render_DebugScreen(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (m_vecDebugScreenLine.empty())
		return;

	// 상태 백업
	DWORD dwZ, dwLighting, dwAlphaTest, dwFog;
	pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
	pGraphicDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);

	pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pGraphicDev->SetVertexShader(nullptr);
	pGraphicDev->SetPixelShader(nullptr);
	pGraphicDev->SetTexture(0, nullptr);
	pGraphicDev->SetFVF(FVF_SCREEN);

	pGraphicDev->DrawPrimitiveUP(D3DPT_LINELIST,
		(UINT)(m_vecDebugScreenLine.size() / 2),
		m_vecDebugScreenLine.data(), sizeof(VTXSCREEN));

	// 복구
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZ);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
	pGraphicDev->SetRenderState(D3DRS_FOGENABLE, dwFog);

	m_vecDebugScreenLine.clear();
}

void CRenderer::Add_DebugTriangle(const std::array<_vec3, 3>& vTri, const _vec3& vNormal, D3DCOLOR dwColor)
{
	m_vecDebugTri.push_back({ vTri, vNormal, dwColor });
}

void CRenderer::Add_DebugScreenLine(const _vec2& vStart, const _vec2& vEnd, D3DCOLOR dwColor)
{
	m_vecDebugScreenLine.push_back({ _vec4(vStart.x, vStart.y, 0.f, 1.f), dwColor });
	m_vecDebugScreenLine.push_back({ _vec4(vEnd.x,   vEnd.y,   0.f, 1.f), dwColor });
}

void CRenderer::Add_DebugScreenCross(const _vec2& vCenter, _float fSize, D3DCOLOR dwColor)
{
	Add_DebugScreenLine({ vCenter.x - fSize, vCenter.y }, { vCenter.x + fSize, vCenter.y }, dwColor);
	Add_DebugScreenLine({ vCenter.x, vCenter.y - fSize }, { vCenter.x, vCenter.y + fSize }, dwColor);
}

void CRenderer::Add_DebugScreenRect(const _vec2& vCenter, _float fHalf, D3DCOLOR dwColor)
{
	_vec2 lt{ vCenter.x - fHalf, vCenter.y - fHalf }, rt{ vCenter.x + fHalf, vCenter.y - fHalf };
	_vec2 lb{ vCenter.x - fHalf, vCenter.y + fHalf }, rb{ vCenter.x + fHalf, vCenter.y + fHalf };
	Add_DebugScreenLine(lt, rt, dwColor);
	Add_DebugScreenLine(rt, rb, dwColor);
	Add_DebugScreenLine(rb, lb, dwColor);
	Add_DebugScreenLine(lb, lt, dwColor);
}

void CRenderer::Add_DebugWorldMarker(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vWorldPos, _float fSize, D3DCOLOR dwColor)
{
	_matrix matView, matProj;
	pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 vp;
	pGraphicDev->GetViewport(&vp);

	_vec4 vClip;
	_matrix matVP = matView * matProj;
	_vec4 vXYZW(vWorldPos, 1.f);
	D3DXVec4Transform(&vClip, &vXYZW, &matVP);
	if (vClip.w <= 0.f) return;   // 카메라 뒤

	_float fX = (vClip.x / vClip.w + 1.f) * 0.5f * vp.Width + vp.X;
	_float fY = (1.f - vClip.y / vClip.w) * 0.5f * vp.Height + vp.Y;

	Add_DebugScreenCross({ fX, fY }, fSize, dwColor);
}

void CRenderer::Free()
{
    Clear_RenderGroup();
}
