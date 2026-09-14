#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CLayer;
}

class CWorm : public CMonster
{
public:
	enum WORMDIR
	{
		FRONT,
		SIDE,
		TOP,
		SIDE45,
		CONNECTOR
	};
protected:
	explicit CWorm(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWorm(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex);
	virtual ~CWorm();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	_float Angle_To_Worm();
private:
	HRESULT			Add_Component();
	_uint m_iWormIndex;
	void Set_Front_Part(CWorm* pFrontPart) { m_pFrontPart = pFrontPart; }
	CMonster* m_pFrontPart;
	map<const _tchar*, CLayer*>* m_pmapLayer;
public:
	void Set_LayerPointer(map<const _tchar*, CLayer*>* pmapLayer) { m_pmapLayer = pmapLayer; }
public:
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev, map<const _tchar*, CLayer*>* pmap);
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex);
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex, map<const _tchar*, CLayer*>* pmap);

protected:
	virtual void		Free();
};
