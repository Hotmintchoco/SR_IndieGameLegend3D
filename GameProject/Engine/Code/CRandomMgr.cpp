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

void CRandomMgr::Free()
{
}
