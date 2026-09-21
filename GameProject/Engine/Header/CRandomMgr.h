#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CRandomMgr : public CBase
{
	DECLARE_SINGLETON(CRandomMgr);

private:
	explicit CRandomMgr();
	virtual ~CRandomMgr();

public:
	bool Chance(float fProb);

	template<typename T>
	T AddRandomNoise(const T& Value, const float fRange)
	{
		T Min = (T)(Value * (1.f - fRange));
		T Max = (T)(Value * (1.f + fRange));

		/* Value가 음수인 경우 */
		if (Min > Max) swap(Min, Max);

		if constexpr (is_integral_v<T>)
		{
			uniform_int_distribution<T> dist(Min, Max);
			return dist(m_gen);
		}
		else
		{
			uniform_real_distribution<T> dist(Min, Max);
			return dist(m_gen);
		}
	}

	template<typename T>
	T GetRandomValue(const T& Min, const T& Max)
	{
		if constexpr (is_integral_v<T>)
		{
			uniform_int_distribution<T> dist(Min, Max);
			return dist(m_gen);
		}
		else
		{
			uniform_real_distribution<T> dist(Min, Max);
			return dist(m_gen);
		}
	}

private:
	mt19937 m_gen;

private:
	virtual void Free() override;
};

END