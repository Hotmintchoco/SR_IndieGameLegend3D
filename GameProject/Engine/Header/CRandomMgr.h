#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CRandomMgr
{
	DECLARE_SINGLETON(CRandomMgr);

private:
	explicit CRandomMgr();
	virtual ~CRandomMgr();

public:
	bool Chance(float fProb);
	long long AddRandomNoise(long long llValue, float fRange);
	float GetRandomFloat(float fMin, float fMax);
	int GetRandomInt(int iMin, int iMax);
	long long GetRandomLonglong(long long iMin, long long iMax);

private:
	mt19937 m_gen;
};

END