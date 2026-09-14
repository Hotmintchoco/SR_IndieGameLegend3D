#include "CEventDelegate.h"

int CEventDelegate<void>::AddBinding(const std::weak_ptr<void>& pToken, std::function<void()> F)
{
	++m_ID;
	m_mapEvent.insert({ m_ID, TBindingInfo{ pToken, F } });

	return m_ID;
}

void CEventDelegate<void>::DeleteBinding(int ID)
{
	m_mapEvent.erase(ID);
}

void CEventDelegate<void>::Broadcast()
{
	for (auto iter = m_mapEvent.begin(); iter != m_mapEvent.end(); )
	{
		const auto& [pToken, F] = iter->second;

		if (pToken.expired())
		{
			iter = m_mapEvent.erase(iter);
		}
		else
		{
			F();
			++iter;
		}
	}
}