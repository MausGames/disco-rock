/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

#if defined(_API_GOOGLE_PLAY_) && 0   // # disabled

    static jmethodID iSetBannerAlpha = NULL;   // function ID to set ad banner visibility
    static pthread_key_t iThreadKey  = 0u;     // thread key

    static coreFloat fCurAlpha = 0.0f;         // current banner transparency
    static coreDouble dCurTime = 0.0;          // current time to reduce the update frequency


    // ****************************************************************
    void ThreadDestroyed(void* pValue)
    {
        if(pValue)
        {
            // detach current thread from the Java Virtual Machine
            g_pJNIJavaVM->DetachCurrentThread();
            pthread_setspecific(iThreadKey, NULL);
        }
    }


    // ****************************************************************
    JNIEnv* GetEnv(void)
    {
        JNIEnv* pEnv;

        // attach current thread to Java Virtual Machine
        if(g_pJNIJavaVM->AttachCurrentThread(&pEnv, NULL) < 0) return NULL;
        pthread_setspecific(iThreadKey, (void*)pEnv);

        return pEnv;
    }


    // ****************************************************************
    void InitBanner()
    {
        // create thread key
        pthread_key_create(&iThreadKey, ThreadDestroyed);

        // fetch function ID to set ad banner visibility
        JNIEnv* pEnv = GetEnv();
        if(pEnv) iSetBannerAlpha = pEnv->GetMethodID(pEnv->FindClass("at/mausgames/dr/DiscoRock"), "SetBannerAlpha", "(F)V");
    }


    // ****************************************************************
    void SetBannerAlpha(coreFloat fAlpha)
    {
        // check current time value
        if(dCurTime + 0.033333 <= Core::System->GetTotalTime())
        {
            dCurTime = Core::System->GetTotalTime();

            // check current alpha value
            fAlpha = CLAMP(fAlpha, 0.0f, 1.0f);
            if(fCurAlpha != fAlpha)
            {
                fCurAlpha = fAlpha;

                // forward alpha value to Java method
                JNIEnv* pEnv = GetEnv();
                if(pEnv) pEnv->CallVoidMethod(g_pJNIActivity, iSetBannerAlpha, fCurAlpha);
            }
        }
    }

#else
    void InitBanner()                     {}
    void SetBannerAlpha(coreFloat fAlpha) {}
#endif


// ****************************************************************
// constructor
cOnline::cOnline()noexcept
#if defined(_API_GOOGLE_PLAY_)
: m_sUserName       ("")
, m_bAuthorized     (false)
, m_iNumConnections (0u)
#endif
{
    // create Game Jolt API access
    m_pGameJolt = new gjAPI(18996, GJ_KEY);

    // sort and hide trophies
    const coreInt32 aiTrophySort[] = {GJ_TROPHY_01, GJ_TROPHY_02, GJ_TROPHY_03, GJ_TROPHY_04, GJ_TROPHY_05, GJ_TROPHY_06, GJ_TROPHY_07, GJ_TROPHY_08, GJ_TROPHY_09, GJ_TROPHY_10, GJ_TROPHY_11, GJ_TROPHY_12, GJ_TROPHY_13, GJ_TROPHY_14, GJ_TROPHY_15};
    m_pGameJolt->InterTrophy()->SetSort(aiTrophySort, ARRAY_SIZE(aiTrophySort));

    const coreInt32 aiTrophySecret[] = {GJ_TROPHY_01, GJ_TROPHY_02, GJ_TROPHY_03, GJ_TROPHY_04};
    m_pGameJolt->InterTrophy()->SetSecret(aiTrophySecret, ARRAY_SIZE(aiTrophySecret));

    // Google Play Games access is created through QuickPlay routine

#if defined(_API_GOOGLE_PLAY_)

    // create dummy score tables
    gjData asTable1, asTable2;
    asTable1["id"] = m_pGameJolt->UtilIntToString(GJ_LEADERBOARD_01);
    asTable2["id"] = m_pGameJolt->UtilIntToString(GJ_LEADERBOARD_02);

    m_apScoreTable[0] = new gjScoreTable(asTable1, m_pGameJolt);
    m_apScoreTable[1] = new gjScoreTable(asTable2, m_pGameJolt);

    // reserve memory
    m_aapScoreList[0].reserve(SCORE_ENTRIES * SCORE_PAGES);
    m_aapScoreList[1].reserve(SCORE_ENTRIES * SCORE_PAGES);

    // load banner visibility function
    InitBanner();

#endif
}


// ****************************************************************
// destructor
cOnline::~cOnline()
{
#if defined(_API_GOOGLE_PLAY_)

    // delete Google Play Games API access
    m_pGooglePlay.reset(NULL);

    // delete dummy score tables
    SAFE_DELETE(m_apScoreTable[0])
    SAFE_DELETE(m_apScoreTable[1])

    // clear score list
    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
    {
        FOR_EACH(it, m_aapScoreList[i])
            SAFE_DELETE(*it)
        m_aapScoreList[i].clear();
    }

#endif

    // delete Game Jolt API access
    SAFE_DELETE(m_pGameJolt)
}


// ****************************************************************
// update network transfers
void cOnline::Update()
{
    // update Game Jolt API access
    m_pGameJolt->SetSessionActive(g_pGame ? true : false);
    m_pGameJolt->Update();
}


// ****************************************************************
// forward error message to menu
void cOnline::__SetErrorMessage(const coreVector3& vColor, const coreChar* pcMessage1, const coreChar* pcMessage2, const coreChar* pcMessage3)
{
    g_pMenu->SetErrorMessage(vColor, pcMessage1, pcMessage2, pcMessage3);
}


// ****************************************************************
// forward score table update to menu
void cOnline::__InvokeScoreUpdate(const coreUintW iTableNum)
{
    g_pMenu->InvokeScoreUpdate(iTableNum);
}