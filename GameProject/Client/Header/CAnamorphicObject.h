#pragma once

#include "CLiminalObject.h"

class IAnamorphicTarget;

class CAnamorphicObject : public CLiminalObject
{
protected:
	explicit CAnamorphicObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAnamorphicObject();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	void Capture();
	void ManualCapture(const _vec3& vEye, const _vec3& vAt);
	void ReverseCapture();

protected:
	HRESULT Add_Component();

	bool m_b3D = true;
	_matrix m_matVPCaptured;
	bool m_bManualCapture = false; /* 캡쳐 시점 view, 판정용 */

	/* 렌더 타겟 준비 */
	HRESULT Ready_RenderTarget();
	/* RT에 그림 그리기 */
	void Render_Capture();
	/* 실제 그리기 대상에 그림 그리기 */
	void Render_Paint();
	/* 백버퍼와 같은 크기를 위한 저장용 변수 */
	int m_iRTWidth = -1;
	int m_iRTHeight = -1;

	/* 리소스 인터페이스들 */
	/* Note : Texture와 Surface의 차이는 샘플링 여부이다. Texture는 Surface 묶음이라고도 볼 수 있음 */
	LPDIRECT3DTEXTURE9 m_pRT = nullptr; /* 텍스쳐 */
	LPDIRECT3DSURFACE9 m_pRTDepth = nullptr; /* 깊이 버퍼는... */
	LPDIRECT3DSTATEBLOCK9 m_pStateBlock = nullptr; /* 상태 블록은 SetTextureStageState, SetSamplerState, SetRenderState 등의 작업을 간단하게 해줌 */
	
	/* 뷰 공간 클립용 */
	_matrix m_matViewCaptured;

	/* 위치 지정 캡쳐 시 proj 저장용 */
	_matrix m_matProjCaptured;

	/* 캡쳐 행렬은 처음 딱 한번만 */
	bool m_bCapturePending = false;

public:
	static CAnamorphicObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

