#pragma once

#include "CLiminalObject.h"

class CLiminalBomb : public CLiminalObject
{
public:

	virtual	_int Update_GameObject(_float fTimeDelta) override;

	float GetScale();
	void SetScale(float fScale);

	void SetDir(const _vec3 vDir) { m_vDir = vDir; }

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	_vec3 m_vDir{ 0.f, 0.f, 0.f };
	float m_fSpeed = 1.f;
};

