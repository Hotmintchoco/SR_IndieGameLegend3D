#pragma once
#include "CUI.h"

/* 직교 미니게임 좌상단 페이즈 표시 : "PHASE 현재 / 전체"
   - 값은 방 레이어가 세터로 넣어줌 (1부터 세는 표시용 번호)
   - "PHASE" 글자는 CUI의 기본 텍스처(m_pTextureCom), 숫자/슬래시는 별도 텍스처 컴포넌트
   - 숫자 텍스처는 장 번호 = 숫자, 나머지는 0번 고정
   - 미니게임 동안만 Show, 끝나면 Hide */
class CUI_PhaseNumber : public CUI
{
protected:
	explicit CUI_PhaseNumber(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_PhaseNumber();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(_float fTimeDelta) override;
	virtual void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual void Render_GameObject() override;

	inline void SetCurrentPhase(int iPhase) { m_iCurrentPhase = max(0, iPhase); }
	inline void SetTotalPhase(int iPhase) { m_iTotalPhase = max(0, iPhase); }

	void Show();
	void Hide();

private:
	HRESULT Add_NumberComponent();
	HRESULT Add_SlashComponent();
	/* 화면 좌표(좌상단 원점, 픽셀) 중심/절반 크기로 사각형 하나 그림 */
	void RenderQuad(Engine::CTexture* pTexture, _uint iIndex, const _vec2& vCenter, const _vec2& vHalfSize);
	/* 숫자를 자릿수대로 왼쪽부터 그림 (vCenter는 전체 숫자 묶음의 가운데) */
	void RenderNumber(int iNumber, const _vec2& vCenter);
	_uint GetDigitTextureIdx(int iDigit) const;

	Engine::CTexture* m_pNumberTextureCom = nullptr;
	Engine::CTexture* m_pSlashTextureCom = nullptr;

	int m_iCurrentPhase = 1;
	int m_iTotalPhase = 1;

	/* 배치 (픽셀, 화면 좌상단 원점, 원본 크기의 1.5배 : PHASE 90x32 -> 135x48, 숫자/슬래시 32x32 -> 48x48) */
	inline static const _vec2 s_vTextCenter{ 97.5f, 54.f };
	inline static const _vec2 s_vTextHalfSize{ 67.5f, 24.f };
	inline static const _vec2 s_vCurrentCenter{ 199.f, 54.f };
	inline static const _vec2 s_vSlashCenter{ 239.f, 54.f };
	inline static const _vec2 s_vTotalCenter{ 279.f, 54.f };
	inline static const _vec2 s_vDigitHalfSize{ 24.f, 24.f };
	static constexpr float s_fDigitAdvance = 36.f;	/* 두 자리 이상일 때 자리 간격 */

public:
	static CUI_PhaseNumber* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
