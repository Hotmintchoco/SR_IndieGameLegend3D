#pragma once

#include "CPlyTex.h"

class CPlayerPartTex : public CPlyTex
{
protected:
	explicit CPlayerPartTex();
	explicit CPlayerPartTex(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPivot);
	explicit CPlayerPartTex(const CPlayerPartTex& rhs);
	virtual ~CPlayerPartTex();

public:
	virtual HRESULT	Ready_Buffer(const _tchar * szFilePath) override;
	inline _vec3 GetPivot() { return m_vPivot; }
	inline void SetPivot(const _vec3& vPivot) { m_vPivot = vPivot; }

public:
	static CPlayerPartTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * szFilePath, const _vec3& vPivot);
	virtual CComponent* Clone() override;

	_vec3 m_vPivot{ 0.f, 0.f, 0.f };

private:
	virtual void Free() override;
};

