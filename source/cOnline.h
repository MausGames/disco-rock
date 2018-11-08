//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_ONLINE_H_
#define _DR_GUARD_ONLINE_H_


// ****************************************************************
// online definitions
#if defined(_CORE_ANDROID_)
    #define _API_GOOGLE_PLAY_   // undefine to use Game Jolt on Android (but removes control configuration)
#endif
#if !defined(_API_GOOGLE_PLAY_)
    #define _API_GAME_JOLT_
#endif

#define SCORE_TABLES   (2u)    // number of different score tables (leaderboards)
#define SCORE_ENTRIES  (8u)    // number of score entries to display at once
#define SCORE_PAGES    (5u)    // number of score pages

#define TROPHY_ITEMS   (15u)   // number of implemented trophies
#define TROPHY_SECRETS (4u)    // number of secret trophies


// ****************************************************************
// Game Jolt relevant definitions
#include "gjAPI.h"
static_assert(GJ_API_OFFCACHE_TROPHY == true, "Don't forget the Offline Cache!");

#define GJ_KEY ""

#define GJ_TROPHY_01 (0)
#define GJ_TROPHY_02 (1)
#define GJ_TROPHY_03 (2)
#define GJ_TROPHY_04 (3)
#define GJ_TROPHY_05 (4)
#define GJ_TROPHY_06 (5)
#define GJ_TROPHY_07 (6)
#define GJ_TROPHY_08 (7)
#define GJ_TROPHY_09 (8)
#define GJ_TROPHY_10 (9)
#define GJ_TROPHY_11 (10)
#define GJ_TROPHY_12 (11)
#define GJ_TROPHY_13 (12)
#define GJ_TROPHY_14 (13)
#define GJ_TROPHY_15 (14)

#define GJ_LEADERBOARD_01 (0)
#define GJ_LEADERBOARD_02 (1)


// ****************************************************************
// Google Play Games relevant definitions
#if defined(_API_GOOGLE_PLAY_)

    #include <jni.h>
    #include "gpg/gpg.h"

    extern JavaVM* g_pJNIJavaVM;     // Java Virtual Machine object
    extern jobject g_pJNIActivity;   // reference to the singleton activity

#endif

// control ad banner transparency
extern void SetBannerAlpha(coreFloat fAlpha);

#define GPG_TROPHY_01 ""
#define GPG_TROPHY_02 ""
#define GPG_TROPHY_03 ""
#define GPG_TROPHY_04 ""
#define GPG_TROPHY_05 ""
#define GPG_TROPHY_06 ""
#define GPG_TROPHY_07 ""
#define GPG_TROPHY_08 ""
#define GPG_TROPHY_09 ""
#define GPG_TROPHY_10 ""
#define GPG_TROPHY_11 ""
#define GPG_TROPHY_12 ""
#define GPG_TROPHY_13 ""
#define GPG_TROPHY_14 ""
#define GPG_TROPHY_15 ""

#define GPG_LEADERBOARD_01 ""
#define GPG_LEADERBOARD_02 ""


// ****************************************************************
// online class
class cOnline final
{
private:
    gjAPI* m_pGameJolt;   // Game Jolt API access (still used with Google Play to read trophy data from file)

#if defined(_API_GOOGLE_PLAY_)

    std::unique_ptr<gpg::GameServices> m_pGooglePlay;   // Google Play Games API access

    std::string m_sUserName;                            // current user name
    coreBool  m_bAuthorized;                            // current authorization status
    coreInt32 m_iNumConnections;                        // current number of active connections

    gjScoreTable* m_apScoreTable[SCORE_TABLES];         // dummy score tables for the score list
    gjScoreList m_aapScoreList[SCORE_TABLES];           // managed list with score objects

#endif


public:
    cOnline()noexcept;
    ~cOnline();

    DISABLE_COPY(cOnline)

    // update the network object
    void Update();

    // get status data
    inline const coreChar* GetUserName      ()const;
    inline const coreChar* GetUserToken     ()const;
    inline coreInt32       GetNumConnections()const;
    inline const coreBool& IsUserConnected  ()const;

#if defined(_API_GOOGLE_PLAY_)

    // open Google Play screens
    inline void OpenTrophy() {m_pGooglePlay->Achievements().ShowAllUI([](const gpg::UIStatus &iStatus){});}
    inline void OpenScore () {m_pGooglePlay->Leaderboards().ShowAllUI([](const gpg::UIStatus &iStatus){});}

#endif

    // layered request functions to support both APIs
    template <typename T> coreInt32 AchieveTrophy    (gjTrophyPtr pTrophy, GJ_NETWORK_OUTPUT(gjTrophyPtr));
    template <typename T> coreInt32 FetchTrophies    (GJ_NETWORK_OUTPUT(gjTrophyList));
    template <typename T> coreInt32 SubmitScore      (const coreUintW iTableID, const std::string& sScore, const coreUintW iSort, const std::string& sExtraData, const std::string& sGuestName, GJ_NETWORK_OUTPUT(gjScorePtr));
    template <typename T> coreInt32 FetchLeaderboards(GJ_NETWORK_OUTPUT(gjScoreTableMap));
    template <typename T> coreInt32 FetchScores      (const coreUintW iTableID, const coreBool bOnlyUser, const coreUintW iLimit, GJ_NETWORK_OUTPUT(gjScoreList));
    template <typename T> coreInt32 Login            (const coreChar* pcName, const coreChar* pcToken, GJ_NETWORK_OUTPUT(coreInt32));
    inline void Logout();

    // access Game Jolt directly
    inline gjAPI* GameJolt()const {return m_pGameJolt;}


private:
    // forward error message to menu
    void __SetErrorMessage(const coreVector3& vColor, const coreChar* pcMessage1, const coreChar* pcMessage2, const coreChar* pcMessage3);

    // forward score table update to menu
    void __InvokeScoreUpdate(const coreUintW iTableNum);
};


// ****************************************************************
// get user name
inline const coreChar* cOnline::GetUserName()const
{
#if defined(_API_GOOGLE_PLAY_)

    // return Google Play Games user name
    return m_sUserName.c_str();

#else

    // return Game Jolt user name
    return m_pGameJolt->GetUserName().c_str();

#endif
}


// ****************************************************************
// get user token
inline const coreChar* cOnline::GetUserToken()const
{
#if defined(_API_GOOGLE_PLAY_)

    // return nothing
    return "";

#else

    // return Game Jolt user token
    return m_pGameJolt->GetUserToken().c_str();

#endif
}


// ****************************************************************
// get current network load
inline coreInt32 cOnline::GetNumConnections()const
{
#if defined(_API_GOOGLE_PLAY_)

    // return Google Play Games connections
    return m_iNumConnections;

#else

    // return Game Jolt connections
    return coreInt32(m_pGameJolt->AccessNetwork()->GetNumSessions());

#endif
}


// ****************************************************************
// check for connected user
inline const coreBool& cOnline::IsUserConnected()const
{
#if defined(_API_GOOGLE_PLAY_)

    // return Google Play Games status
    return m_bAuthorized;

#else

    // return Game Jolt status
    return m_pGameJolt->IsUserConnected();

#endif
}


// ****************************************************************
// achieve trophy
template <typename T> coreInt32 cOnline::AchieveTrophy(gjTrophyPtr pTrophy, GJ_NETWORK_OUTPUT(gjTrophyPtr))
{
#if defined(_API_GOOGLE_PLAY_)

    // get trophy number
    const coreUintW iTrophyID = P_TO_I(pOutputData);
    std::string sTrophyID = "";

    // map trophy number to achievement ID
    switch(iTrophyID)
    {
    case  0u: sTrophyID = GPG_TROPHY_01; break;
    case  1u: sTrophyID = GPG_TROPHY_02; break;
    case  2u: sTrophyID = GPG_TROPHY_03; break;
    case  3u: sTrophyID = GPG_TROPHY_04; break;
    case  4u: sTrophyID = GPG_TROPHY_05; break;
    case  5u: sTrophyID = GPG_TROPHY_06; break;
    case  6u: sTrophyID = GPG_TROPHY_07; break;
    case  7u: sTrophyID = GPG_TROPHY_08; break;
    case  8u: sTrophyID = GPG_TROPHY_09; break;
    case  9u: sTrophyID = GPG_TROPHY_10; break;
    case 10u: sTrophyID = GPG_TROPHY_11; break;
    case 11u: sTrophyID = GPG_TROPHY_12; break;
    case 12u: sTrophyID = GPG_TROPHY_13; break;
    case 13u: sTrophyID = GPG_TROPHY_14; break;
    case 14u: sTrophyID = GPG_TROPHY_15; break;
    default: Core::Log->Warning("Trophy (%d) not found", iTrophyID); return GJ_REQUEST_FAILED;
    }

    // achieve trophy with Google Play Games
    m_pGooglePlay->Achievements().Unlock(sTrophyID);

    // call callback
    (pOutputObj->*(OutputCallback))(pTrophy, pOutputData);
    return GJ_OK;

#else

    // achieve trophy with Game Jolt
    return pTrophy->AchieveCall(GJ_NETWORK_OUTPUT_FW);

#endif
}


// ****************************************************************
// fetch trophies
template <typename T> coreInt32 cOnline::FetchTrophies(GJ_NETWORK_OUTPUT(gjTrophyList))
{
#if defined(_API_GOOGLE_PLAY_)

    // fetch trophy data
    ++this->m_iNumConnections;
    m_pGooglePlay->Achievements().FetchAll([=](const gpg::AchievementManager::FetchAllResponse& Response)
    {
        --this->m_iNumConnections;

        gjTrophyList apTrophy;
        apTrophy.reserve(TROPHY_ITEMS);

        // check for valid response
        if(Response.status == gpg::ResponseStatus::VALID)
        {
            // reload cache file to reset trophies
            // a logout/disconnect may show wrong achieved trophies until the application is restarted
            this->m_pGameJolt->InterTrophy()->FetchTrophiesNow(GJ_TROPHY_ALL, &apTrophy);

            // loop through all returned trophies
            FOR_EACH(it, Response.data)
            {
                // check for valid and achieved trophy
                if(it->Valid() && it->State() == gpg::AchievementState::UNLOCKED)
                {
                    FOR_EACH(gj, apTrophy)
                    {
                        // map Google Play Games achievement to Game Jolt trophy
                        if((*gj)->GetTitle() == it->Name())
                        {
                            // achieve
                            (*gj)->AchieveNow();
                            break;
                        }
                    }
                }
            }
        }

        // call callback (trophy list is empty on error)
        (pOutputObj->*(OutputCallback))(apTrophy, pOutputData);
    });

    return GJ_OK;

#else

    // fetch trophy with Game Jolt
    return m_pGameJolt->InterTrophy()->FetchTrophiesCall(GJ_TROPHY_ALL, GJ_NETWORK_OUTPUT_FW);

#endif
}


// ****************************************************************
// submit score
template <typename T> coreInt32 cOnline::SubmitScore(const coreUintW iTableID, const std::string& sScore, const coreUintW iSort, const std::string& sExtraData, const std::string& sGuestName, GJ_NETWORK_OUTPUT(gjScorePtr))
{
#if defined(_API_GOOGLE_PLAY_)

    std::string sLeaderboardID = "";

    // map score table number to leaderboard ID
    switch(iTableID)
    {
    case GJ_LEADERBOARD_01: sLeaderboardID = GPG_LEADERBOARD_01; break;
    case GJ_LEADERBOARD_02: sLeaderboardID = GPG_LEADERBOARD_02; break;
    default: Core::Log->Warning("Leaderboard (%d) not found", iTableID); return GJ_REQUEST_FAILED;
    }

    // submit score with Google Play Games (no guest supported)
    m_pGooglePlay->Leaderboards().SubmitScore(sLeaderboardID, iSort);

    // create dummy pointers
    gjData asEmpty;
    asEmpty["sort"] = "0";

    gjScore Score(asEmpty, m_apScoreTable[(iTableID == GJ_LEADERBOARD_01) ? 0u : 1u], m_pGameJolt);

     // call callback
    (pOutputObj->*(OutputCallback))(&Score, pOutputData);
    return GJ_OK;

#else

    // submit score with Game Jolt
    return m_pGameJolt->InterScore()->GetScoreTable(iTableID)->AddGuestScoreCall(sScore, iSort, sExtraData, sGuestName, GJ_NETWORK_OUTPUT_FW);

#endif
}


// ****************************************************************
// fetch leaderboards
template <typename T> coreInt32 cOnline::FetchLeaderboards(GJ_NETWORK_OUTPUT(gjScoreTableMap))
{
#if defined(_API_GOOGLE_PLAY_)

    // assemble leaderboard map
    gjScoreTableMap apMap;
    apMap[GJ_LEADERBOARD_01] = m_apScoreTable[0];
    apMap[GJ_LEADERBOARD_02] = m_apScoreTable[1];

    // call callback
    (pOutputObj->*(OutputCallback))(apMap, pOutputData);
    return GJ_OK;

#else

    // fetch leaderboards with Game Jolt
    return m_pGameJolt->InterScore()->FetchScoreTablesCall(GJ_NETWORK_OUTPUT_FW);

#endif
}


// ****************************************************************
// fetch scores
template <typename T> coreInt32 cOnline::FetchScores(const coreUintW iTableID, const coreBool bOnlyUser, const coreUintW iLimit, GJ_NETWORK_OUTPUT(gjScoreList))
{
#if defined(_API_GOOGLE_PLAY_)

    std::string sLeaderboardID = "";
    const coreUintW iTableNum = (iTableID == GJ_LEADERBOARD_01) ? 0u : 1u;

    // map score table number to leaderboard ID
    switch(iTableID)
    {
    case GJ_LEADERBOARD_01: sLeaderboardID = GPG_LEADERBOARD_01; break;
    case GJ_LEADERBOARD_02: sLeaderboardID = GPG_LEADERBOARD_02; break;
    default: Core::Log->Warning("Leaderboard (%d) not found", iTableID); return GJ_REQUEST_FAILED;
    }

    // get fetch type (0 = all, 1 = user only)
    const coreUintW iType = P_TO_I(pOutputData);

    if(iType == 0u)
    {
        // get score page token
        gpg::ScorePage::ScorePageToken oToken = m_pGooglePlay->Leaderboards().ScorePageToken(sLeaderboardID, gpg::LeaderboardStart::TOP_SCORES,
                                                                                             gpg::LeaderboardTimeSpan::ALL_TIME, gpg::LeaderboardCollection::PUBLIC);

        // fetch global best score data
        ++this->m_iNumConnections;
        m_pGooglePlay->Leaderboards().FetchScorePage(oToken, iLimit, [=](const gpg::LeaderboardManager::FetchScorePageResponse& Response)
        {
            --this->m_iNumConnections;

            gjScoreList apScore;
            apScore.reserve(iLimit);

            // check for valid response
            if(Response.status == gpg::ResponseStatus::VALID && Response.data.Valid())
            {
                // clear score list
                FOR_EACH(it, this->m_aapScoreList[iTableNum])
                    SAFE_DELETE(*it)
                this->m_aapScoreList[iTableNum].clear();

                // loop through all returned score entries
                FOR_EACH(it, Response.data.Entries())
                {
                    if(it->Valid())
                    {
                        // create persistent score pointer
                        gjData asScoreData;
                        asScoreData["user"] = "-";   // placeholder
                        asScoreData["sort"] = m_pGameJolt->UtilIntToString(coreInt32(it->Score().Value()));

                        gjScore* pScore = new gjScore(asScoreData, m_apScoreTable[iTableNum], this->m_pGameJolt);

                        // invoke asynchronous user-name replace (this is possible (though the score list clear) because the Google library calls everything consecutively)
                        std::string* psName = const_cast<std::string*>(&pScore->GetUserName());
                        ++this->m_iNumConnections;
                        this->m_pGooglePlay->Players().Fetch(it->PlayerId(), [=](const gpg::PlayerManager::FetchResponse& Response)
                        {
                            --this->m_iNumConnections;

                            // check for valid response
                            if(Response.status == gpg::ResponseStatus::VALID && Response.data.Valid())
                            {
                                // replace player ID with player name and invoke visual update
                                *psName = Response.data.Name();
                                this->__InvokeScoreUpdate(iTableNum);
                            }
                        });

                        // add global score to list
                        this->m_aapScoreList[iTableNum].push_back(pScore);
                        apScore.push_back(pScore);
                    }
                }
            }

            // call callback (score list is empty on error)
            (pOutputObj->*(OutputCallback))(apScore, pOutputData);
        });
    }
    else
    {
        // fetch user score data
        ++this->m_iNumConnections;
        m_pGooglePlay->Leaderboards().FetchScoreSummary(sLeaderboardID, gpg::LeaderboardTimeSpan::ALL_TIME,
                                                        gpg::LeaderboardCollection::PUBLIC, [=](const gpg::LeaderboardManager::FetchScoreSummaryResponse&Response)
        {
            --this->m_iNumConnections;

            gjScoreList apScore;
            apScore.reserve(iLimit);
            gjScore* pUserOnlyScore = NULL;

            // check for valid response
            if(Response.status == gpg::ResponseStatus::VALID && Response.data.Valid())
            {
                const gpg::Score& Score = Response.data.CurrentPlayerScore();
                if(Score.Valid())
                {
                    // create dummy score pointer
                    gjData asScoreData;
                    asScoreData["user"]       = this->m_sUserName;
                    asScoreData["sort"]       = m_pGameJolt->UtilIntToString(coreInt32(Score.Value()));
                    asScoreData["extra_data"] = m_pGameJolt->UtilIntToString(coreInt32(Score.Rank())) + "."; // also get rank

                    pUserOnlyScore = new gjScore(asScoreData, m_apScoreTable[(iTableID == GJ_LEADERBOARD_01) ? 0u : 1u], this->m_pGameJolt);

                    // add user score to list
                    apScore.push_back(pUserOnlyScore);
                }
            }

            // call callback (score list is empty on error)
            (pOutputObj->*(OutputCallback))(apScore, pOutputData);
            SAFE_DELETE(pUserOnlyScore)
        });
    }

    return GJ_OK;

#else

    // fetch scores with Game Jolt
    return m_pGameJolt->InterScore()->GetScoreTable(iTableID)->FetchScoresCall(bOnlyUser, iLimit, GJ_NETWORK_OUTPUT_FW);

#endif
}


// ****************************************************************
// login
template <typename T> coreInt32 cOnline::Login(const coreChar* pcName, const coreChar* pcToken, GJ_NETWORK_OUTPUT(coreInt32))
{
    // get login type (0 = QuickPlay or Google Play Games setup)
    const coreUintW iLoginType = P_TO_I(pOutputData);

#if defined(_API_GOOGLE_PLAY_)

    if(iLoginType == 0u)
    {
        // init Google Play Games with Java Virtual Machine object
        gpg::AndroidInitialization::JNI_OnLoad(g_pJNIJavaVM);

        // create Android configuration struct and forward main Java-activity
        gpg::AndroidPlatformConfiguration AndroidConfig;
        AndroidConfig.SetActivity(g_pJNIActivity);

        // create Google Play Games API access (game service object)
        m_pGooglePlay = gpg::GameServices::Builder()

        // just log everything
        .SetDefaultOnLog(gpg::LogLevel::VERBOSE)

        // set authorization callback
        .SetOnAuthActionStarted ([&](gpg::AuthOperation op) {})
        .SetOnAuthActionFinished([=](gpg::AuthOperation op, gpg::AuthStatus iStatus)
        {
            // check status
            this->m_bAuthorized = false;
            switch(iStatus)
            {
            // successful
            case gpg::AuthStatus::VALID:
                this->__SetErrorMessage(coreVector3(), NULL, NULL, NULL);
                this->m_bAuthorized = true;
                break;

            // failed
            case gpg::AuthStatus::ERROR_INTERNAL:                this->__SetErrorMessage(COLOR_RED_F, "", "GOOGLE PLAY GAMES", "INTERNAL PROBLEM");       break;
            case gpg::AuthStatus::ERROR_TIMEOUT:                 this->__SetErrorMessage(COLOR_ORANGE_F,  "GOOGLE PLAY GAMES", "CONNECTION TIMEOUT", ""); break;
            case gpg::AuthStatus::ERROR_NOT_AUTHORIZED:          this->__SetErrorMessage(COLOR_ORANGE_F,  "GOOGLE PLAY GAMES", "NOT YET ENABLED",    ""); break;
            case gpg::AuthStatus::ERROR_VERSION_UPDATE_REQUIRED: this->__SetErrorMessage(COLOR_RED_F, "", "GOOGLE PLAY GAMES", "UPDATE REQUIRED");        break;
            }

            if(this->m_bAuthorized)
            {
                // fetch player data
                ++this->m_iNumConnections;
                this->m_pGooglePlay->Players().FetchSelf([=](const gpg::PlayerManager::FetchSelfResponse& Response)
                {
                    --this->m_iNumConnections;

                    // check for valid response
                    if(Response.status == gpg::ResponseStatus::VALID && Response.data.Valid())
                    {
                        // save player name
                        this->m_sUserName = Response.data.Name();
                        (pOutputObj->*(OutputCallback))(GJ_OK, pOutputData);
                    }
                    else (pOutputObj->*(OutputCallback))(GJ_REQUEST_FAILED, pOutputData);
                });
            }
            else (pOutputObj->*(OutputCallback))(GJ_REQUEST_FAILED, pOutputData);
        })

        // start creation
        .Create(AndroidConfig);
    }
    else
    {
        // start Google Play Games authorization
        m_pGooglePlay->StartAuthorizationUI();
    }

    return GJ_OK;

#else

    // login with Game Jolt
    if(iLoginType == 0u && std::strlen(pcName) == 0u && std::strlen(pcToken) == 0u)
         return m_pGameJolt->LoginCall(true, "../" GJ_API_CRED, GJ_NETWORK_OUTPUT_FW);
    else return m_pGameJolt->LoginCall(true, pcName, pcToken,   GJ_NETWORK_OUTPUT_FW);

#endif
}


// ****************************************************************
// logout
inline void cOnline::Logout()
{
#if defined(_API_GOOGLE_PLAY_)

    // logout with Google Play Games
    m_pGooglePlay->SignOut();

#else

    // logout with Game Jolt
    m_pGameJolt->Logout();

#endif
}


#endif // _DR_GUARD_ONLINE_H_