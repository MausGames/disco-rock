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
// engine header files
#include "Core.h"
#include <deque>


// ****************************************************************
// global definitions
#define FONT_ROCKS "gomarice_rocks.ttf"

#define COLOR_WHITE_F  coreVector3(1.000f, 1.000f, 1.000f)
#define COLOR_YELLOW_F coreVector3(1.000f, 0.824f, 0.392f)
#define COLOR_ORANGE_F coreVector3(1.000f, 0.443f, 0.227f)
#define COLOR_RED_F    coreVector3(1.000f, 0.275f, 0.275f)
#define COLOR_PURPLE_F coreVector3(0.710f, 0.333f, 1.000f)
#define COLOR_BLUE_F   coreVector3(0.102f, 0.702f, 1.000f)
#define COLOR_GREEN_F  coreVector3(0.118f, 0.745f, 0.353f)

#define COLOR_BRIGHTNESS (0.83f)

static constexpr_var coreVector3 g_avColor[] = 
{
    COLOR_YELLOW_F,
    COLOR_ORANGE_F,
    COLOR_RED_F,
    COLOR_PURPLE_F,
    COLOR_BLUE_F,
    COLOR_GREEN_F,
};
#define COLOR_NUM ARRAY_SIZE(g_avColor)

#define CONTROL_CLASSIC    (0)
#define CONTROL_MOTION     (1)
#define CONTROL_FULLSCREEN (2)


// ****************************************************************
// game header files
#include "cOnline.h"
#include "cBackground.h"
#include "cMenu.h"
#include "cPlate.h"
#include "cRay.h"
#include "cRock.h"
#include "cBeverage.h"
#include "cTrap.h"
#include "cCombatText.h"
#include "cInterface.h"
#include "cFirst.h"
#include "cGame.h"


// ****************************************************************
// global variables
extern cOnline*            g_pOnline;           // network access object for leaderboards and achievements

extern cBackground*        g_pBackground;       // background with dance floor
extern cMenu*              g_pMenu;             // global menu object
extern cCombatText*        g_pCombatText;       // global "combat" text object
extern cGame*              g_pGame;             // global game object
extern cFirst*             g_pFirst;            // special menu displayed at the first time

extern coreMusicPlayer*    g_pMusicPlayer;      // primary music player
extern coreParticleSystem* g_pParticleSystem;   // primary particle system

extern float               g_fTargetSpeed;      // new target speed
extern float               g_fCurSpeed;         // current speed
extern float               g_fTargetCam;        // new target camera position (sin(Y))
extern float               g_fCurCam;           // current camera position
extern bool                g_bPause;            // pause status

extern float               g_fCamSpeed;         // helper value for camera movement calculations
extern float               g_fCamTime;          // time value fpor interpolation
extern float               g_fOldCam;           // old camera value for interpolation
extern bool                g_bCamMode;          // false = move smooth to target, true = interpolate
extern bool                g_bUpsideDown;       // upside-down camera status

extern int                 g_iNumGames;         // number of started games
extern int                 g_iNumFails;         // number of deaths below 10 seconds


// ****************************************************************
// message container class
struct sMsgList
{
    std::vector<const std::string*> m_apsMsg;   // shuffled messages
    coreUint m_iCur;                            // current message index

    inline sMsgList(const std::string* psMsg, const coreUint& iSize)noexcept
    : m_iCur (0)
    {
        // add messages
        m_apsMsg.reserve(iSize);
        for(coreUint i = 0; i < iSize; ++i) m_apsMsg.push_back(&psMsg[i]);
        
        // shuffle them
        coreRand::Seed();
        std::random_shuffle(m_apsMsg.begin(), m_apsMsg.end(), [](int i) {return coreRand::Rand() % i;});
    }

    inline const char* Get()
    {
        // get next message
        if(++m_iCur >= m_apsMsg.size()) m_iCur = 0;
        return m_apsMsg[m_iCur]->c_str();
    }
};


// ****************************************************************
// message containers
extern sMsgList g_MsgIntro;
extern sMsgList g_MsgFallen;
extern sMsgList g_MsgTrap;
extern sMsgList g_MsgBegin;


#endif // _DR_GUARD_MAIN_H_