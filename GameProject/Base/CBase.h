#pragma once

#include <memory>

#pragma warning(push)
#pragma warning(disable: 4251)

class _declspec(dllexport) CBase
{
protected:
	inline explicit CBase();
	inline virtual ~CBase();

public:
	inline unsigned long		AddRef();
	inline unsigned long		Release();

	inline const std::shared_ptr<void>& GetToken() const { return m_pToken; }

private:
	unsigned long			m_dwRefCnt;
	
	std::shared_ptr<void> m_pToken = std::make_shared<char>();

private:
	inline virtual void		Free() = 0;
	
};

#pragma warning(pop)

#include "CBase.inl"