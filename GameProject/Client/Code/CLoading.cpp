#include "pch.h"
#include "CLoading.h"
#include "CProtoMgr.h"
#include "Define.h"
#include "JsonAdapter.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_bFinish(false), m_eLoadingID(LOADING_END)
{
    ZeroMemory(m_szLoading, sizeof(m_szLoading));
    m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
    InitializeCriticalSection(&m_Crt);

    m_eLoadingID = eID;

    // 쓰레드 생성
    m_hThread = (HANDLE)_beginthreadex(NULL, // 보안 속성(핸들의 상속 여부, NULL인 경우 상속에서 제외)
                                        0,   // 디폴트 스택 사이즈(1 바이트)
                                        Thread_Main, // 구동할 쓰레드 함수
                                        this,       // 쓰레드 함수로 전달할 데이터 주소
                                        0,          // 쓰레드 생성 및 실행을 조정하기 위한 옵션
                                        NULL);      // 쓰레드 ID



    return S_OK;
}

_uint CLoading::Loading_Stage()
{
    lstrcpy(m_szLoading, L"Buffer Loading............................");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TriCol", Engine::CTriCol::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcCol", Engine::CRcCol::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
        return E_FAIL;

    lstrcpy(m_szLoading, L"Texture Loading............................");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/Ma.jpg", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MonsterTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/skull3.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture2", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Terrain0.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyBoxTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/SkyboxStars.dds", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90))))
        return E_FAIL;

    lstrcpy(m_szLoading, L"Etc Loading............................");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
        return E_FAIL;


    /*  메쉬 사용 안내
    *   CPlyTex
            - 버텍스 정보를 담은 CVIBuffer 하위 클래스
            - 생성 시 외부 파일에서 버텍스 정보를 받아오기 때문에 Create 함수에 파일 경로가 포함되어야 함
    *   텍스쳐는 수업 내용과 동일
    *   예시는 총
    */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Gun_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/Gun.ply"))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Gun_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/Gun_Diffuse.png", 1))))
        return E_FAIL;


    /* 맵 출력용 에셋 */
    
    /* 벽 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_EW_NoDoor_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/Wall_EW_NoDoor.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_NS_NoDoor_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/Wall_NS_NoDoor.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_EW_Door_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/Wall_EW_Door.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_NS_Door_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/Wall_NS_Door.ply"))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_EW_NoDoor_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/WallShort.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_NS_NoDoor_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/WallLong.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_EW_Door_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/WallShortDoor.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Wall_NS_Door_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/WallLongDoor.png", 1))))
        return E_FAIL;

    /* 맵 오브젝트 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrayFrustum_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/GrayFrustum.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BrownFrustum_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/BrownFrustum.ply"))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider", Engine::CSphereCollider::Create(m_pGraphicDev))))
		return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrayFrustum_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/GrayFrustum_Diffuse.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BrownFrustum_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/BrownFrustum_Diffuse.png", 1))))
        return E_FAIL;

    /* 타일 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlaneTex", Engine::CPlaneTex::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Tile_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture2D/StaticTile/StaticTile_%d.png", 54))))
        return E_FAIL;



    lstrcpy(m_szLoading, L"Loading Complete!!!");

    m_bFinish = true;
    
    return 0;
}

unsigned int CLoading::Thread_Main(void* pArg)
{
    CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);

    int iFlag(0);

    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_LoadingID())
    {
    case LOADING_STAGE:
        iFlag = pLoading->Loading_Stage();
        break;

    case LOADING_BOSS:
        break;
    }

    LeaveCriticalSection(pLoading->Get_Crt());

    // _endthreadex(0);

    return iFlag;   // 0 리턴 시, _endthreadex 함수가 자동 호출
}

void CLoading::ParseMapData(TMapData* pOut)
{
    using json = nlohmann::json;

    ifstream f("../Bin/Resource/Map/testmap.json");
    if (!f.is_open()) {
        MSG_BOX("파일 열기 실패");
        return;
    }

    try {
        json data = json::parse(f);
        data.at("tileList").get_to(pOut->vecTile);
        data.at("objectList").get_to(pOut->vecObjectInfo);
        data.at("monsterList").get_to(pOut->vecMonsterInfo);
        data.at("door").get_to(pOut->vecDoorInfo);
    }
    catch (const json::exception& e) {
        return;
    }
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
    CLoading* pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);
        MSG_BOX("Loading Create Failed");
        return nullptr;
    }
    return pLoading;
}

void CLoading::Free()
{
    WaitForSingleObject(m_hThread, INFINITE);

    CloseHandle(m_hThread);

    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_pGraphicDev);
}
