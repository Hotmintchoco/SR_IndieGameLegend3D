#pragma once

#include "CAnamorphicObject.h"
#include "ITerrain.h"
#include "Client_Enum.h"

class CLiminalFrame : public CAnamorphicObject, public ITerrain
{
protected:
	explicit CLiminalFrame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalFrame();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	/* IAnamorphicTarget */
	HRESULT SetBuffer(const wstring& wstrTag);
	HRESULT SetTexture(const wstring& wstrTag);

	/* ITerrain */
	virtual float SampleTerrainHeight(const _vec3& vRayStart) override;

	inline void SetFrameColor(EColorType eType) { m_eType = eType; }
	inline EColorType GetFrameColor() { return m_eType; }

private:
	HRESULT Add_Component();
	void Remove_Component(COMPONENTID eID, CComponent* pComponent);

	EColorType m_eType = EColorType::WHITE;

public:
	static CLiminalFrame* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
