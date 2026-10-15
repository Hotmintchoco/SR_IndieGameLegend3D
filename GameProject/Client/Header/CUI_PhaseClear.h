#pragma once
#include "CUI.h"
#include "CEventDelegate.h"

/* 직교 미니게임 페이즈 클리어 표시 : 화면 가운데에서 스프라이트를 한 번 재생하고, 끝나면 스스로 비활성화 */
class CUI_PhaseClear : public CUI
{
protected:
	explicit CUI_PhaseClear(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_PhaseClear();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(_float fTimeDelta) override;
	virtual void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual void Render_GameObject() override;

	/* 첫 프레임부터 한 번 재생 (재생 중에 다시 부르면 처음부터) */
	void Play();
	inline bool IsPlaying() const { return m_bPlaying; }

	/* 재생이 끝나 스스로 꺼지는 순간 알림 */
	CEventDelegate<void> m_OnPlayFinished;

private:
	/* 애니메이션 (현재 프레임은 CUI의 m_fFrame) */
	bool m_bPlaying = false;
	int m_iFrameCount = 0;				/* 텍스처 장수 */
	float m_fFrameInterval = 0.2f;		/* 한 장당 시간 */
	float m_fTimeAfterPlay = 0.f;		/* Play 이후 지난 시간 */

	/* 화면에 보이는 크기 (픽셀). RcTex가 -1~1이라 Set_Size에는 절반을 넘김 */
	static constexpr float s_fWidth = 420.f;
	static constexpr float s_fHeight = 170.f;

public:
	static CUI_PhaseClear* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
