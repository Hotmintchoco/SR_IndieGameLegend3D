#pragma once
#include "CUI.h"
#include "CEventDelegate.h"

/* 직교 미니게임 입장 인트로 : 화면 가운데에서 인트로 구간(0~45)을 한 번 재생한 뒤 루프 구간(46~117)을 반복
   루프 구간에 들어온 뒤 아무 키나 누르면 끝나고 스스로 비활성화 */
class CUI_OrthoIntro : public CUI
{
protected:
	explicit CUI_OrthoIntro(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_OrthoIntro();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(_float fTimeDelta) override;
	virtual void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual void Render_GameObject() override;

	/* 첫 프레임부터 재생 (재생 중에 다시 부르면 처음부터) */
	void Play();
	inline bool IsPlaying() const { return m_bPlaying; }

	/* 아무 키로 끝나 스스로 꺼지는 순간 알림 */
	CEventDelegate<void> m_OnPlayFinished;

private:
	/* 이번 프레임에 눌린 키가 하나라도 있는지 */
	static bool IsAnyKeyDown();

	/* 애니메이션 (현재 프레임은 CUI의 m_fFrame) */
	bool m_bPlaying = false;
	int m_iFrameCount = 0;							/* 텍스처 장수 */
	int m_iLoopEnd = 0;								/* 실제 루프 끝 (텍스처 장수에 맞춰 보정) */
	float m_fTimeAfterPlay = 0.f;					/* Play 이후 지난 시간 */

	static constexpr float s_fFrameInterval = 1.f / 12.f;	/* 12 fps */
	static constexpr int s_iLoopStart = 46;
	static constexpr int s_iLoopEnd = 117;

	/* 화면에 보이는 크기 (픽셀, 원본 320x180의 2배). RcTex가 -1~1이라 Set_Size에는 절반을 넘김 */
	static constexpr float s_fWidth = 640.f * (10.f / 6.f);
	static constexpr float s_fHeight = 360.f * (10.f / 6.f);

public:
	static CUI_OrthoIntro* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
