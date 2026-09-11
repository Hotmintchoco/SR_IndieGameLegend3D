#include "CPipeline.h"

void CPipeline::MakeTransformMatrix(_matrix* pOut, const _vec3* pRight, const _vec3* pUp, const _vec3* pLook, const _vec3* pPos)
{
	memcpy(&pOut->m[0][0], pRight, sizeof(_vec3));
	memcpy(&pOut->m[1][0], pUp, sizeof(_vec3));
	memcpy(&pOut->m[2][0], pLook, sizeof(_vec3));
	memcpy(&pOut->m[3][0], pPos, sizeof(_vec3));
}

void CPipeline::MakeLookAtLH(_matrix* pOut, 
							const _vec3* pEye,
							const _vec3* pAt, 
							const _vec3* pUp)
{
	_vec3	vRight	= { 1.f, 0.f, 0.f };
	_vec3	vUp		= { 0.f, 1.f, 0.f };
	_vec3	vLook	= { 0.f, 0.f, 1.f };
	_vec3	vPos	= { 0.f, 0.f, 0.f };

	// look
	vLook = *pAt - *pEye;
	D3DXVec3Normalize(&vLook, &vLook);

	// right
	D3DXVec3Cross(&vRight, pUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	// up
	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	// pos
	vPos = *pEye;

	MakeTransformMatrix(pOut, &vRight, &vUp, &vLook, &vPos);

	D3DXMatrixInverse(pOut, 0, pOut);
}

void CPipeline::MakeProjMatrix(_matrix* pOut, 
								const _float& fFov,
								const _float& fAspect,
								const _float& fNear, 
								const _float& fFar)
{
	D3DXMatrixIdentity(pOut);

	pOut->_11 = (1.f / tanf(fFov / 2.f)) / fAspect;
	pOut->_22 = 1.f / tanf(fFov / 2.f);
	pOut->_33 = fFar / (fFar - fNear);
	pOut->_44 = 0.f;

	pOut->_34 = 1.f;
	pOut->_43 = (-fNear * fFar) / (fFar - fNear);
  
}
