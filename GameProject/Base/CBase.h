#pragma once

#include <memory>

#pragma warning(push)
#pragma warning(disable: 4251)

class _declspec(dllexport) CBase
{
protected:
	inline explicit CBase();
	inline CBase(const CBase& rhs);
	inline virtual ~CBase();
	CBase& operator=(const CBase&) = delete;

public:
	inline unsigned long		AddRef();
	inline unsigned long		Release();

	/* 이걸로 얻은 weak ptr은 expired() 로만 사용하기. lock() 쓰면 안됨 */
	inline std::weak_ptr<void> GetToken() const { return m_pToken; }

private:
	unsigned long			m_dwRefCnt;
	
	std::shared_ptr<void> m_pToken = std::make_shared<char>();

private:
	inline virtual void		Free() = 0;
	
};

#pragma warning(pop)

#include "CBase.inl"