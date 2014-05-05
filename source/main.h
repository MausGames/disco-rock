/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_MAIN_H_
#define _DR_GUARD_MAIN_H_


// ****************************************************************
#include "Core.h"
#include "gjAPI.h"
#include <deque>

#define FONT_ROCKS "data/fonts/gomarice_rocks.ttf"

#define COLOR_WHITE_F  coreVector4(  1.0f,   1.0f,   1.0f, 1.0f)
#define COLOR_YELLOW_F coreVector4(  1.0f, 0.824f, 0.392f, 1.0f)
#define COLOR_ORANGE_F coreVector4(  1.0f, 0.443f, 0.227f, 1.0f)
#define COLOR_RED_F    coreVector4(  1.0f, 0.275f, 0.275f, 1.0f)
#define COLOR_BLUE_F   coreVector4(0.102f, 0.702f,   1.0f, 1.0f)
#define COLOR_GREEN_F  coreVector4(0.118f, 0.745f, 0.353f, 1.0f)
#define COLOR_PURPLE_F coreVector4( 0.71f, 0.333f,   1.0f, 1.0f)

static constexpr_var coreVector4 g_avColor[] = 
{
    COLOR_YELLOW_F,
    COLOR_ORANGE_F,
    COLOR_RED_F,
    COLOR_BLUE_F,
    COLOR_GREEN_F,
    COLOR_PURPLE_F,
};
#define COLOR_NUM ARRAY_SIZE(g_avColor)


// ****************************************************************
#include "cMenu.h"
#include "cBackground.h"
#include "cPlate.h"
#include "cRay.h"
#include "cRock.h"
#include "cBeverage.h"
#include "cTrap.h"
#include "cCombatText.h"
#include "cInterface.h"
#include "cGame.h"


// ****************************************************************
extern cBackground*        g_pBackground;
extern cMenu*              g_pMenu;
extern cGame*              g_pGame;
extern cCombatText*        g_pCombatText;

extern gjAPI*              g_pGJ;
extern coreMusicPlayer*    g_pMusicPlayer;
extern coreParticleSystem* g_pParticleSystem;

extern float               g_fTargetSpeed;
extern float               g_fCurSpeed;
extern float               g_fTargetCam;
extern float               g_fCurCam;
extern bool                g_bPause;

extern int                 g_iNumFails;


// ****************************************************************
struct sMsgList
{
    std::vector<const std::string*> m_apsMsg;   // shuffled messages
    coreUint m_iCur;                            // current message index

    inline sMsgList(const std::string* psMsg, const coreUint& iSize)noexcept
    : m_iCur (0)
    {
        // add messages and shuffle them
        m_apsMsg.reserve(iSize);
        for(coreUint i = 0; i < iSize; ++i) m_apsMsg.push_back(&psMsg[i]);
        std::random_shuffle(m_apsMsg.begin(), m_apsMsg.end(), [](int i) {return std::rand() % i;});
    }

    inline const char* Get()
    {
        // get next message
        if(++m_iCur >= m_apsMsg.size()) m_iCur = 0;
        return m_apsMsg[m_iCur]->c_str();
    }
};


// ****************************************************************
extern sMsgList g_MsgIntro;
extern sMsgList g_MsgFallen;
extern sMsgList g_MsgTrap;
extern sMsgList g_MsgBegin;


#endif // _DR_GUARD_MAIN_H_