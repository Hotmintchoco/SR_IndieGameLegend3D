#pragma once
#include "CMG0_DancerPart.h"

class CMG0_DancerRArm : public CMG0_DancerPart
{
protected:
	explicit CMG0_DancerRArm(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_DancerRArm();

public:
	virtual			HRESULT		Ready_GameObject();

	virtual _float	Get_PoseAngle(_int iPose) override;
	virtual _float	Get_SitAngle() override;

public:
	static CMG0_DancerRArm* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
