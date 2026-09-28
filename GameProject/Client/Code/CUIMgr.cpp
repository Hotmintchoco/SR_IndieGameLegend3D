#include "pch.h"
#include "CUIMgr.h"
#include "CUI.h" // UI 베이스 클래스 (Set_Active 등 포함)

IMPLEMENT_SINGLETON(CUIMgr)

CUIMgr::CUIMgr()
{
}

CUIMgr::~CUIMgr()
{
    Free();
}


void CUIMgr::Free()
{
    
}