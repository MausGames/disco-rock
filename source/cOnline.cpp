//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cOnline::cOnline()noexcept
{
    // init Game Jolt API object
    m_GameJolt.Init(18996, GJ_KEY);

    // sort trophies
    const coreInt32 aiTrophySort[] = {GJ_TROPHY_01, GJ_TROPHY_02, GJ_TROPHY_03, GJ_TROPHY_04, GJ_TROPHY_05, GJ_TROPHY_06, GJ_TROPHY_07, GJ_TROPHY_08, GJ_TROPHY_09, GJ_TROPHY_10, GJ_TROPHY_11, GJ_TROPHY_12, GJ_TROPHY_13, GJ_TROPHY_14, GJ_TROPHY_15};
    m_GameJolt.InterTrophy()->SetSort(aiTrophySort, ARRAY_SIZE(aiTrophySort));

    // hide trophies
    const coreInt32 aiTrophySecret[] = {GJ_TROPHY_01, GJ_TROPHY_02, GJ_TROPHY_03, GJ_TROPHY_04};
    m_GameJolt.InterTrophy()->SetSecret(aiTrophySecret, ARRAY_SIZE(aiTrophySecret));
}


// ****************************************************************
// update network transfers
void cOnline::Update()
{
    // update Game Jolt API object
    m_GameJolt.SetSessionActive(g_pGame != NULL);
    m_GameJolt.Update();
}