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
#define SCORE_TABLES   (2u)    // number of different score tables (leaderboards)
#define SCORE_ENTRIES  (8u)    // number of score entries to display at once
#define SCORE_PAGES    (5u)    // number of score pages

#define TROPHY_ITEMS   (15u)   // number of implemented trophies
#define TROPHY_SECRETS (4u)    // number of secret trophies


// ****************************************************************
// Game Jolt API definitions
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
// online class
class cOnline final
{
private:
    gjAPI m_GameJolt;   // Game Jolt API object


public:
    cOnline()noexcept;

    DISABLE_COPY(cOnline)

    // update network transfers
    void Update();

    // get status data
    inline const coreChar* GetUserName      ()const;
    inline const coreChar* GetUserToken     ()const;
    inline coreInt32       GetNumConnections()const;
    inline const coreBool& IsUserConnected  ()const;

    // layered request functions to support multiple APIs
    template <typename T> coreInt32 AchieveTrophy    (gjTrophyPtr pTrophy, GJ_NETWORK_OUTPUT(gjTrophyPtr));
    template <typename T> coreInt32 FetchTrophies    (GJ_NETWORK_OUTPUT(gjTrophyList));
    template <typename T> coreInt32 SubmitScore      (const coreUintW iTableID, const coreString& sScore, const coreUintW iSort, const coreString& sExtraData, const std::string& sGuestName, GJ_NETWORK_OUTPUT(gjScorePtr));
    template <typename T> coreInt32 FetchLeaderboards(GJ_NETWORK_OUTPUT(gjScoreTableMap));
    template <typename T> coreInt32 FetchScores      (const coreUintW iTableID, const coreBool bOnlyUser, const coreUintW iLimit, GJ_NETWORK_OUTPUT(gjScoreList));
    template <typename T> coreInt32 Login            (const coreChar* pcName, const coreChar* pcToken, GJ_NETWORK_OUTPUT(coreInt32));
    inline void Logout();

    // access Game Jolt API directly
    inline gjAPI* GameJolt() {return &m_GameJolt;}
};


// ****************************************************************
// get user name
inline const coreChar* cOnline::GetUserName()const
{
    return m_GameJolt.GetUserName().c_str();
}


// ****************************************************************
// get user token
inline const coreChar* cOnline::GetUserToken()const
{
    return m_GameJolt.GetUserToken().c_str();
}


// ****************************************************************
// get current network load
inline coreInt32 cOnline::GetNumConnections()const
{
    return coreInt32(m_GameJolt.AccessNetwork()->GetNumSessions());
}


// ****************************************************************
// check for connected user
inline const coreBool& cOnline::IsUserConnected()const
{
    return m_GameJolt.IsUserConnected();
}


// ****************************************************************
// achieve trophy
template <typename T> coreInt32 cOnline::AchieveTrophy(gjTrophyPtr pTrophy, GJ_NETWORK_OUTPUT(gjTrophyPtr))
{
    return pTrophy->AchieveCall(GJ_NETWORK_OUTPUT_FW);
}


// ****************************************************************
// fetch trophies
template <typename T> coreInt32 cOnline::FetchTrophies(GJ_NETWORK_OUTPUT(gjTrophyList))
{
    return m_GameJolt.InterTrophy()->FetchTrophiesCall(GJ_TROPHY_ALL, GJ_NETWORK_OUTPUT_FW);
}


// ****************************************************************
// submit score
template <typename T> coreInt32 cOnline::SubmitScore(const coreUintW iTableID, const coreString& sScore, const coreUintW iSort, const scoreString& sExtraData, const std::string& sGuestName, GJ_NETWORK_OUTPUT(gjScorePtr))
{
    return m_GameJolt.InterScore()->GetScoreTable(iTableID)->AddGuestScoreCall(sScore, iSort, sExtraData, sGuestName, GJ_NETWORK_OUTPUT_FW);
}


// ****************************************************************
// fetch leaderboards
template <typename T> coreInt32 cOnline::FetchLeaderboards(GJ_NETWORK_OUTPUT(gjScoreTableMap))
{
    return m_GameJolt.InterScore()->FetchScoreTablesCall(GJ_NETWORK_OUTPUT_FW);
}


// ****************************************************************
// fetch scores
template <typename T> coreInt32 cOnline::FetchScores(const coreUintW iTableID, const coreBool bOnlyUser, const coreUintW iLimit, GJ_NETWORK_OUTPUT(gjScoreList))
{
    return m_GameJolt.InterScore()->GetScoreTable(iTableID)->FetchScoresCall(bOnlyUser, iLimit, GJ_NETWORK_OUTPUT_FW);
}


// ****************************************************************
// login
template <typename T> coreInt32 cOnline::Login(const coreChar* pcName, const coreChar* pcToken, GJ_NETWORK_OUTPUT(coreInt32))
{
    if((P_TO_I(pOutputData) == 0u) && (pcName[0] == '\0') && (pcToken[0] == '\0'))
         return m_GameJolt.LoginCall(true, "../" GJ_API_CRED, GJ_NETWORK_OUTPUT_FW);   // quickplay
    else return m_GameJolt.LoginCall(true, pcName, pcToken,   GJ_NETWORK_OUTPUT_FW);
}


// ****************************************************************
// logout
inline void cOnline::Logout()
{
    m_GameJolt.Logout();
}


#endif // _DR_GUARD_ONLINE_H_