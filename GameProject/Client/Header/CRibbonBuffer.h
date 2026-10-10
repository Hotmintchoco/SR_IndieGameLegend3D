#pragma once

#include "CVIBuffer.h"

class CRibbonBuffer : public CVIBuffer
{
protected:
	explicit CRibbonBuffer();
	explicit CRibbonBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	CRibbonBuffer(const CRibbonBuffer& rhs) = delete;
	CRibbonBuffer& operator =(const CRibbonBuffer& rhs) = delete;
	virtual ~CRibbonBuffer();

public:
	virtual HRESULT	Ready_Buffer();
	virtual void Render_Buffer();

	void AddSegment(const _vec3& vLeft, const _vec3& vRight, const float fTraveled);
	void ReplaceHead(const _vec3& vLeft, const _vec3& vRight, const float fTraveled);

private:
	/* section : Left, Right 단면 한 쌍 */
	DWORD m_dwMaxSection = 1024;
	DWORD m_dwCurSection = 0;

public:
	static CRibbonBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone() { assert(0); return nullptr; };

private:
	virtual void Free() override;
};

