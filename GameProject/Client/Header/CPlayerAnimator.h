#pragma once

#include "CComponent.h"
#include "Client_Enum.h"

namespace Engine
{
	class CTransform;
}

class CPlayerPartTex;

struct TPlayerBuffer
{
	CPlayerPartTex* pBuffer = nullptr;
	CTransform* pTransform = nullptr;

	_vec3 vInitPos{};
	_vec3 vInitRot{};
};

enum class EPlayerLocomotionState
{
	NONE,

	IDLE,
	WALK,
	SPRINT,
	JUMP,

	MAX,
};

enum class EPlayerActionState
{
	NONE,

	GUN_SHOOT,
	BOW_SHOOT,
	DIE,

	MAX,
};

class CPlayerAnimator : public CComponent
{
protected:
	explicit CPlayerAnimator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerAnimator();

public:
	virtual _int Update_Component(_float fTimeDelta) override;
	virtual void LateUpdate_Component() override;
	virtual CComponent* Clone() { assert(0); return nullptr; };

	void TransformPropagation(const _matrix& matRootWorld);

	void SetBuffer(const array<TPlayerBuffer, PP_END>& tBuffer);
	void SetInitialTransform();

	constexpr static int PP_ROOT = -1;

	void RenderDebugTransform();

private:
	// Root
	// ├─ vBody
	// │  ├─ vHead
	// │  ├─ vLArm
	// │  └─ vRArm
	// ├─ vLLeg
	// └─ vRLeg

	const static array<int, PP_END> s_arrParent;
	const static array<PLAYERPART, PP_END> s_arrUpdateOrder;
	static const char* s_szPartName[PP_END];
	array<TPlayerBuffer, PP_END> m_arrBuffer = {};

	/* 애니메이션 디버그용 */
	bool m_bAnimPause = false;

public:
	static CPlayerAnimator* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

