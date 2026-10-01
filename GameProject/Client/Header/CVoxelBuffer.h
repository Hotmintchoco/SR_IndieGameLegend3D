#pragma once
#include "CVIBuffer.h"

struct TDelta2D
{
	int dx;
	int dy;
};

struct TSideQuadInfo
{
	VTXTEX vertex[4];
	INDEX16 index[2];
};

enum class EQuadDirection { NONE, TOP, BOTTOM, RIGHT, LEFT };

class CVoxelBuffer : public CVIBuffer
{
protected:
	explicit CVoxelBuffer(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTexturePath);
	explicit CVoxelBuffer(const CVoxelBuffer& rhs);
	virtual ~CVoxelBuffer();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CVoxelBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTexturePath);
	virtual CComponent* Clone();

private:
	TSideQuadInfo GetSideQuadInfo(int x, int y, EQuadDirection eDir, const D3DXIMAGE_INFO& tInfo);
	wstring m_wstrTexturePath;

private:
	virtual void Free() override;
};