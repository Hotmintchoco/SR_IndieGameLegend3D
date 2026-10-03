#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform();
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	void		Move_Pos(const _vec3* pDir, const _float& fSpeed, const _float& fTimeDelta)
	{
		m_vInfo[INFO_POS] += *pDir * fSpeed * fTimeDelta;	
	}
	void		Move_Pos(const _vec3& vVelocity, const _float& fTimeDelta)
	{
		m_vInfo[INFO_POS] += vVelocity * fTimeDelta;
	}

	void		Rotation(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vAngle) + eType) += fAngle;
		UpdateMatrix();
	}
	void		Set_Rotation_Raw(const _vec3& vDegreeAngle)
	{
		m_vAngle = vDegreeAngle;
		UpdateMatrix();
	}
	/* Degree임 */
	inline _vec3 Get_Rotation() { return m_vAngle; }

	_matrix* Get_World() { return &m_matWorld; }

	void	 Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
	}

	_vec3 Get_Info_Value(INFO eType)
	{
		return _vec3{ m_matWorld.m[eType][0], m_matWorld.m[eType][1], m_matWorld.m[eType][2] };
	}

	void		Set_Pos(_float fX, _float fY, _float fZ)
	{
		m_vInfo[INFO_POS] = { fX, fY, fZ };
		UpdateMatrix();
	}
	inline void Set_Pos(const _vec3& vPos)
	{
		m_vInfo[INFO_POS] = vPos;
		UpdateMatrix();
	}

	void		Set_World(_matrix* pWorld)
	{
		m_matWorld = *pWorld;
	}

	void		Set_Scale(_float fX, _float fY, _float fZ)
	{
		m_vScale = { fX, fY, fZ };
		UpdateMatrix();
	}

	inline void		Set_Scale(const _vec3& vScale)
	{
		m_vScale = vScale;
		UpdateMatrix();
	}

	inline _vec3 Get_Scale() { return m_vScale; }

	inline void UpdateMatrix()
	{
		if (m_bUseLocal)
		{
			UpdateLocalMatrix();
		}
		else
		{
			UpdateWorldMatrix();
		}
	}

	void UpdateWorldMatrix();
	
	void UpdateLocalMatrix(); // 부모 객체 트랜스폼이 있는 경우 자신의 로컬 위치 캐싱용
	void WorldMatrixPropagation(const _matrix& matParentWorld); // 부모 월드 행렬을 자식에게 전파
	inline void SetUseLocal(bool bFlag) { m_bUseLocal = bFlag; }

	/* 부모 오브젝트 없는 오브젝트에 대해서, Set World 이후에 영구적으로 transform을 저장하고 싶을 때 사용하시오 */
	void WorldMatrixDecompose();

public:
	HRESULT			Ready_Transform();
	virtual _int	Update_Component(_float fTimeDelta);
	virtual void	LateUpdate_Component();

public:
	void		Chase_Target(const _vec3* pPos, const _vec3* pAngle, const _float& fSpeed, const _float& fTimeDelta);
	void		Chase_Target2(const _vec3* pPos, const _vec3* pAngle, const _float& fSpeed, const _float& fTimeDelta);
	void LookAt_Player(const _vec3* pPos, const _vec3* pLook);
	_matrix* Compute_LookAtTarget(const _vec3* pPos, const _vec3* pLook);

public:
	_vec3		m_vInfo[INFO_END];

	_vec3		m_vScale;
	_vec3		m_vAngle;

	_matrix		m_matWorld;
	_matrix m_matLocal; // 부모 객체 트랜스폼이 있는 경우 자신의 로컬 위치 캐싱용
	bool m_bUseLocal = false; // 마찬가지의 이유로 위치 변경 시 local matrix 업데이트 용

	_float m_fAccumulatedTime;


public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void		Free();

};

END

