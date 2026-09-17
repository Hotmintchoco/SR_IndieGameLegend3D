#include "CRandomMgr.h"

IMPLEMENT_SINGLETON(CRandomMgr)

CRandomMgr::CRandomMgr() : m_gen(random_device{}())
{
}

CRandomMgr::~CRandomMgr()
{
}

bool CRandomMgr::Chance(float fProb)
{
	uniform_real_distribution<float> dist(0.f, 1.f);
	float fRes = dist(m_gen);
	return fRes <= fProb;
}

long long CRandomMgr::AddRandomNoise(long long llValue, float fRange)
{
	long long llMin = (long long)(llValue * (1.f - fRange));
	long long llMax = (long long)(llValue * (1.f + fRange));

	uniform_int_distribution<long long> dist(llMin, llMax);
	return dist(m_gen);
}

float CRandomMgr::GetRandomFloat(float fMin, float fMax)
{
	uniform_real_distribution<float> dist(fMin, fMax);
	return dist(m_gen);
}

int CRandomMgr::GetRandomInt(int iMin, int iMax)
{
	uniform_int_distribution<int> dist(iMin, iMax);
	return dist(m_gen);
}

long long CRandomMgr::GetRandomLonglong(long long llMin, long long llMax)
{
	uniform_int_distribution<long long> dist(llMin, llMax);
	return dist(m_gen);
}

void CRandomMgr::Free()
{
}
