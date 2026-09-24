#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
}

class CGun : public CGameObject
{
protected:
	explicit CGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGun();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


private:
	HRESULT				Add_Component();
	void				RenderImGui();
	pair<_vec3, _vec3>	Get_MouseRay();
	template <typename T1, typename T2>
    _bool               CheckVtxIntersect(LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DINDEXBUFFER9 pIB, T1 pVertex, T2 pIndex, _ulong dwVtxCnt, _ulong dwTriCnt, _float& fToDistance, _vec3& vResult, _matrix matWorld, _vec3 vRayPos, _vec3 vRayDir);

private:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
    void    UpdateUltimateGauge(_float fAmount);
    void    UpdateSpecialGauge(_float iAmount);
public:
	BULLETID	m_iCurBullet;
	_int		m_iDmg;

private:
	_float			m_fLastShotTime;
	_float			m_fShootRate;
	_float			m_fRunningTime;
    _float          m_fUltimateGauge;
    _float          m_fSpecialGauge;
	_float			m_fUltimateTimer;
    _float          m_fUltimateDuration;
    _float			m_fSpecialTimer;
    _float          m_fSpecialDuration;
	_bool			m_bUltimateActivated;
    _bool           m_bSpecialActivated;

private:
	virtual void		Free();
};


/* RayCast용 템플릿함수 */

template <typename T1, typename T2>
_bool CGun::CheckVtxIntersect(LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DINDEXBUFFER9 pIB, T1 pVertex, T2 pIndex, _ulong dwVtxCnt, _ulong dwTriCnt, _float& fToDistance, _vec3& vResult, _matrix matWorld, _vec3 vRayPos, _vec3 vRayDir)
{
    _bool bIsHit = false;
    pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_READONLY);
    pIB->Lock(0, 0, (void**)&pIndex, 0);


    vector<_vec3> vecVtx;

    for (_ulong dwCnt = 0; dwCnt < dwTriCnt; dwCnt++)
    {
        /* VtxBuffer 추출*/

        _vec3 vVtxInfo[3] =
            { pVertex[pIndex[dwCnt]._0].vPosition,
            pVertex[pIndex[dwCnt]._1].vPosition,
            pVertex[pIndex[dwCnt]._2].vPosition};
        
        /* IdxBuffer 추출 */

        for (_int i = 0; i < 3; i++)
        {
            D3DXVec3TransformCoord(&vVtxInfo[i], &vVtxInfo[i], &matWorld);
        }

        /* RayCast를 이용한 맞은 위치 특정*/

        _vec3 vCurHit;
        _float fU, fV, fCurToDistance;
        if (D3DXIntersectTri(&vVtxInfo[0], &vVtxInfo[1], &vVtxInfo[2], &vRayPos, &vRayDir, &fU, &fV, &fCurToDistance))
        {
            bIsHit = true;
            if (fabs(fCurToDistance) < fToDistance || fToDistance == 0.f) // 가장 가까운 거리의 맞은 위치만 저장
            {
                fToDistance = fabs(fCurToDistance);
                vResult = vVtxInfo[0] + fU * (vVtxInfo[1] - vVtxInfo[0]) + fV * (vVtxInfo[2] - vVtxInfo[0]);
            }
        }

    }

    pVB->Unlock();
    pIB->Unlock();

    return bIsHit;
}
