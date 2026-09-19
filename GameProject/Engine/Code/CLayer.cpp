#include "CLayer.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent* CLayer::Get_Component(COMPONENTID eID, wstring pObjTag, const _tchar* pComponentTag)
{
	auto iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second->Get_Component(eID, pComponentTag);
}

CGameObject* CLayer::Get_GameObject(const wstring& pObjTag)
{
	auto iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second;
}

HRESULT CLayer::Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject->SetOwner(this);

	m_mapObject.insert({ pObjTag, pGameObject });

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{
	_int iResult(0);

	for (auto iter = m_mapObject.begin(); iter != m_mapObject.end(); )
	{
		CGameObject* pObj = iter->second;

		if (pObj->Is_Dead())
		{
			Safe_Release(pObj);
			iter = m_mapObject.erase(iter);
			continue;
		}

		iResult = pObj->Update_GameObject(fTimeDelta);
		if (iResult & 0x80000000)
			return iResult;

		++iter;
	}

	return iResult;
}

void CLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	for (auto& pObj : m_mapObject)
		pObj.second->LateUpdate_GameObject(fTimeDelta);
}

CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}
