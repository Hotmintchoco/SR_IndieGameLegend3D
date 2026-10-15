#pragma once

#include "CSpriteTile.h"

/* 직교 미니게임 전용 미니 타일
   - 방 전체를 덮는 배경 텍스처 한 장을 미니 격자 칸 수로 나눠, 자기 칸(행/열)에 해당하는 조각만 그림
   - 버퍼 UV는 그대로 두고 고정 파이프라인 텍스처 트랜스폼으로 잘라 씀
   - 칸 구분선 : 테두리 텍스처(가장자리만 회색)를 스테이지 1에서 원래 UV로 곱함
   - 배경은 모든 칸/페이즈 공용이고 스프라이트 애니메이션이라 장 수만큼 루프
     장 번호는 방 레이어가 들고 있는 공용 시간으로 계산 (꺼져 있던 타일이 다시 켜져도 다른 칸과 같은 장)
   - 미니게임에서는 오염을 쓰지 않으므로 항상 오염 저항으로 생성 (오염 렌더는 처리하지 않음)
   - 타입은 SPRITE 그대로라 기존 CSpriteTile 캐스팅과 호환 */
class COrthoSpriteTile : public CSpriteTile
{
protected:
	explicit COrthoSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, const TTileIdx& tIdx, int iGridX, int iGridZ, const float* pSharedTime);
	virtual ~COrthoSpriteTile();

public:
	virtual			HRESULT		Ready_GameObject() override;
	virtual			void		Render_GameObject() override;

private:
	HRESULT			Add_BackgroundComponent();
	/* 공용 시간으로 지금 그릴 배경 장 번호 */
	int				GetBackgroundIdx() const;

	Engine::CTexture* m_pBackgroundTextureCom = nullptr;
	/* 칸 구분선 (흰 바탕, 가장자리 1픽셀 회색) */
	Engine::CTexture* m_pBorderTextureCom = nullptr;

	/* 배경 애니메이션 : 전체 장 수 (Ready에서 텍스처에서 받음), 공용 시간 (방 레이어 소유, 꺼져 있어도 흐름) */
	int m_iBackgroundFrameCount = 1;
	const float* m_pSharedTime = nullptr;
	float m_fBackgroundFrameInterval = 0.1f;	/* 한 장 표시 시간 (10fps) */

	/* 배경을 나누는 칸 수 (미니 격자 크기) */
	int m_iGridX = 1;
	int m_iGridZ = 1;

public:
	/* tIdx : 미니 격자 기준 행/열, iGridX/iGridZ : 미니 격자 칸 수, pSharedTime : 배경 애니메이션 공용 시간 (타일보다 오래 살아야 함) */
	static COrthoSpriteTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, const TTileIdx& tIdx, int iGridX, int iGridZ, const float* pSharedTime);

protected:
	virtual void		Free() override;
};
