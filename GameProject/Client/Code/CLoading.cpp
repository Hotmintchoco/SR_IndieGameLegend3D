#include "pch.h"
#include "CLoading.h"
#include "CProtoMgr.h"
#include "JsonAdapter.h"
#include "CRoomLoadingMgr.h"
#include "Utils.h"

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

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcColCustom", Engine::CRcColCustom::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
        return E_FAIL;

    lstrcpy(m_szLoading, L"Monster Texture Loading............................");
    //skull
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_skull3Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/skull/skull3.png", 1))))return E_FAIL;
    //worm
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_worm_drillTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/worm/worm_drill_%d.png", 12)))) return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_worm_bobyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/worm/worm_body_%d.png", 5)))) return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_worm_tailTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/worm/worm_tail_%d.png", 3)))) return E_FAIL;
    //Boss1
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_boss1Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/boss1/boss1_%d.png", 4)))) return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_boss1_angryTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/boss1/boss%d.png", 2)))) return E_FAIL;
    //Speyeder
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_speyederTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/speyeder/speyeder_%d.png", 7)))) return E_FAIL;
    //magmamouth
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_magmamouthTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/magmamouth/magmamouth_%d.png", 7)))) return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_fireballTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/fireball/fireBall_%d.png", 4)))) return E_FAIL;
    
    lstrcpy(m_szLoading, L"Effect Texture Loading............................");
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_smallexplodeTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/smallexplode/smallExplode_%d.png", 4)))) return E_FAIL;


    lstrcpy(m_szLoading, L"Texture Loading............................");
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/Invisible.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture2", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Terrain0.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyBoxTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/SkyboxStars.dds", 1))))
        return E_FAIL;

    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90))))
    //    return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/bigbullet_0.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture_Small", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/sprBullet_0.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTrailTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/BulletTrail.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletParticleTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/Particle_%d.png", 3))))
        return E_FAIL;


    // UI Texture
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HpUITexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/HPBar.png", 1))))
		return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CrosshairTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Reticle_%d.png", 4))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_NumberTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Num%d.png", 10))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DirectionUITexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/PlayerDir.png", 10))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HudMapTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/hud_map.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HudAttackInfoTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/hud_AttackInfo.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_AmmoTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/hud_ammo.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkillTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Skill%d.png", 9))))
        return E_FAIL;

    lstrcpy(m_szLoading, L"Etc Loading............................");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
        return E_FAIL;

    
    /* 충돌 처리 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SphereCollider", Engine::CSphereCollider::Create(m_pGraphicDev))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BoxCollider", Engine::CBoxCollider::Create(m_pGraphicDev))))
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

    /* 맵 오브젝트 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrayFrustum_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/GrayFrustum.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BrownFrustum_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/BrownFrustum.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ExplosiveFrustum_Bottom_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/ExplosiveFrustum2_Bottom.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ExplosiveFrustum_Top_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/ExplosiveFrustum2_Top.ply"))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrayFrustum_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/GrayFrustum_Diffuse.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BrownFrustum_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/BrownFrustum_Diffuse.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ExplosiveFrustum_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Mesh/ExplosiveFrustum2_Diffuse.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BombLight_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/RoomProp/bomblight2_%d.png", 2))))
        return E_FAIL;

    lstrcpy(m_szLoading, L"Room Data Loading............................");
    
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

    /* 타일 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlaneTex", Engine::CPlaneTex::Create(m_pGraphicDev))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Tile_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Tile/StaticTile/StaticTile_%d.png", 57))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileLava_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Tile/AnimationTile/FloorLava_%d.png", 4))))
        return E_FAIL;

    /* 안개 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Fog_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/RoomProp/fog.png", 1))))
        return E_FAIL;

    /* 문 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Door_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/RoomProp/door_%d.png", 5))))
        return E_FAIL;

    /* 가짜 어둠 */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Sphere_Vertex", Engine::CPlyTex::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/Sphere.ply"))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Black_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Black/%d.png", 101))))
        return E_FAIL;

    /* 맵 배치 데이터 */
    if (FAILED(ParseRoomData()))
    {
        return E_FAIL;
    }

    /* Item Loading */
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Item_Energy_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Item/Energy_%d.png", 8))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Item_Heart_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Item/hearts_0.png", 1))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Item_Gem_Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Item/sprCoin_strip6_%d.png", 6))))
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

HRESULT CLoading::ParseRoomData()
{
    for (int i = 0; i < 25; ++i)
    {
        if (FAILED(ParseSingleRoom(i)))
        {
            return E_FAIL;
        }
    }
    return S_OK;
}

HRESULT CLoading::ParseSingleRoom(int iRoomIdx)
{
    using json = nlohmann::json;

    int iRoomRow = iRoomIdx / 5;
    int iRoomCol = iRoomIdx % 5;

    wstring wstrFilePath = L"../Bin/Resource/Map/Room_" + to_wstring(iRoomRow) + L"_" + to_wstring(iRoomCol) + L".json";
    ifstream f(wstrFilePath);
    if (!f.is_open()) {
        if (FAILED(ParseDefaultRoom(iRoomIdx)))
        {
            return E_FAIL;
        }
        return S_OK;
    }

    TRoomData t;
    try {
        json data = json::parse(f);
        string str = data.at("roomName").get<string>();
        wstring wstr = Utils::Utf8ToWide(str);
        t.wstrRoomName = wstr;
        data.at("defaultTile").get_to(t.iDefaultTileIdx);
        data.at("tileList").get_to(t.vecTile);
        data.at("resistContaminationList").get_to(t.vecResistContamination);
        data.at("objectTilingList").get_to(t.vecObjectTilingInfo);
        data.at("objectList").get_to(t.vecObjectInfo);
        data.at("door").get_to(t.vecDoorInfo);
        data.at("doorTile").get_to(t.vecDoorTile);
        vector<string> vecClearRaw;
        data.at("clear").get_to(vecClearRaw);
        t.vecClearCondition.reserve(vecClearRaw.size());
        for (const auto& str : vecClearRaw)
        {
            t.vecClearCondition.push_back(Utils::Utf8ToWide(str));
        }

        // 매니저 클래스에 데이터 등록
        CRoomLoadingMgr::GetInstance()->RegisterRoomData(iRoomIdx, t);
    }
    catch (const json::exception&) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoading::ParseDefaultRoom(int iRoomIdx)
{
    using json = nlohmann::json;

    wstring wstrFilePath = L"../Bin/Resource/Map/DefaultRoom.json";
    ifstream f(wstrFilePath);
    if (!f.is_open()) {
        MSG_BOX("[CLoading] 맵 Json 데이터 파일 열기 실패");
        return E_FAIL;
    }

    TRoomData t;
    try {
        json data = json::parse(f);
        string str = data.at("roomName").get<string>();
        wstring wstr = Utils::Utf8ToWide(str);
        t.wstrRoomName = wstr;
        data.at("defaultTile").get_to(t.iDefaultTileIdx);
        data.at("tileList").get_to(t.vecTile);
        data.at("resistContaminationList").get_to(t.vecResistContamination);
        data.at("objectTilingList").get_to(t.vecObjectTilingInfo);
        data.at("objectList").get_to(t.vecObjectInfo);
        data.at("door").get_to(t.vecDoorInfo);
        data.at("doorTile").get_to(t.vecDoorTile);       
        vector<string> vecClearRaw;
        data.at("clear").get_to(vecClearRaw);
        t.vecClearCondition.reserve(vecClearRaw.size());
        for (const auto& str : vecClearRaw)
        {
            t.vecClearCondition.push_back(Utils::Utf8ToWide(str));
        }

        // 매니저 클래스에 데이터 등록
        CRoomLoadingMgr::GetInstance()->RegisterRoomData(iRoomIdx, t);
    }
    catch (const json::exception&) {
        return E_FAIL;
    }

    return S_OK;
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
