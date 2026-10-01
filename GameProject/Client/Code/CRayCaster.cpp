#include "pch.h"
#include "CRayCaster.h"
#include "CVIBuffer.h"

CRayCaster::CRayCaster(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CRayCaster::~CRayCaster()
{
}

HRESULT CRayCaster::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CRayCaster::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CRayCaster::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRayCaster::Render_GameObject()
{

}

HRESULT CRayCaster::Add_Component()
{
    return S_OK;
}

void CRayCaster::RayTest(THitInfo& tHitInfo, const _vec3& vRayStart, const _vec3& vRayDir, CVIBuffer* pBuffer, _matrix* pMatWorld)
{
	TVIBufferInfo tBufferInfo = pBuffer->GetInfo();
	const vector<TTriInfo>& vecTriInfo = pBuffer->GetTri();

	for (int i = 0; i < (int)vecTriInfo.size(); ++i)
	{
		/* 세 정점으로 얻은 삼각형에 대해 처리 */
		_vec3 vPos0 = vecTriInfo[i].vTriPos[0];
		_vec3 vPos1 = vecTriInfo[i].vTriPos[1];
		_vec3 vPos2 = vecTriInfo[i].vTriPos[2];

		D3DXVec3TransformCoord(&vPos0, &vPos0, pMatWorld);
		D3DXVec3TransformCoord(&vPos1, &vPos1, pMatWorld);
		D3DXVec3TransformCoord(&vPos2, &vPos2, pMatWorld);

		float fU, fV, fDist;
		bool bHit = D3DXIntersectTri(
			&vPos0, &vPos1, &vPos2,
			&vRayStart, &vRayDir,
			&fU, &fV, &fDist
		);

		if (bHit && fDist < tHitInfo.fDist)
		{
			tHitInfo.bHit = true;
			tHitInfo.fDist = fDist;
			tHitInfo.fHitPoint = vPos0 + fU * (vPos1 - vPos0) + fV * (vPos2 - vPos0);
			_vec3 v1, v2, vNorm;
			v1 = vPos1 - vPos0;
			v2 = vPos2 - vPos0;
			D3DXVec3Cross(&vNorm, &v1, &v2);
			D3DXVec3Normalize(&tHitInfo.fTriNormal, &vNorm);
			tHitInfo.vTriVtx = { vPos0, vPos1, vPos2 };
		}
	}
}


CRayCaster* CRayCaster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRayCaster* pObject = new CRayCaster(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CRayCaster Create Failed");
        return nullptr;
    }

    return pObject;
}

void CRayCaster::Free()
{
    CGameObject::Free();
}
