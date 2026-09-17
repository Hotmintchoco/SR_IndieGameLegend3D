#pragma once

#include "Engine_Define.h"
#include <functional>
#include <unordered_map>
#include <memory>

BEGIN(Engine)

// T is the event context to deliver
template<typename T>
class CEventDelegate
{
	struct TBindingInfo
	{
		std::weak_ptr<void> pToken;
		std::function<void(const T& Ctx)> F;
	};

public:
	int AddBinding(const std::weak_ptr<void>& pToken, std::function<void(const T& Ctx)> F);
	void DeleteBinding(int ID);

	void Broadcast(const T& Ctx);

private:
	int m_ID = 0;
	std::unordered_map<int, TBindingInfo> m_mapEvent;
};

// 컴파일러가 템플릿을 인스턴스화하기 위해 템플릿 클래스는 구현부를 헤더에 적어준다
template<typename T>
int CEventDelegate<T>::AddBinding(const std::weak_ptr<void>& pToken, std::function<void(const T&)> F)
{
	++m_ID;
	m_mapEvent.insert({ m_ID, TBindingInfo{ pToken, F } });

	return m_ID;
}

template<typename T>
void CEventDelegate<T>::DeleteBinding(int ID)
{
	m_mapEvent.erase(ID);
}

template<typename T>
void CEventDelegate<T>::Broadcast(const T& Ctx)
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
			F(Ctx);
			++iter;
		}
	}
}

// 완전 특수화 템플릿 -> 구현부를 분리할 수 있다
template<>
class ENGINE_DLL CEventDelegate<void>
{
	struct TBindingInfo
	{
		std::weak_ptr<void> pToken;
		std::function<void()> F;
	};

public:
	int AddBinding(const std::weak_ptr<void>& pToken, std::function<void()> F);
	void DeleteBinding(int ID);

	void Broadcast();

private:
	int m_ID = 0;
	std::unordered_map<int, TBindingInfo> m_mapEvent;
};

END