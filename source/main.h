/////////////////////////////////////////////////////////////////////////////////////
//*-------------------------------------------------------------------------------*//
//|         _____ _____  _____  _____ ____     _____   ____   _____ _   __        |//
//|        |  __ \_   _|/ ____|/ ____/ __ \   |  __ \ / __ \ / ____| | / /        |//
//|        | |  | || | | (___ | |   | |  | |  | |__) | |  | | |    | |/ /         |//
//|        | |  | || |  \___ \| |   | |  | |  |  _  /| |  | | |    |   |          |//
//|        | |__| || |_ ____) | |___| |__| |  | | \ \| |__| | |____| |\ \         |//
//|        |_____/_____|_____/ \_____\____/   |_|  \_\\____/ \_____|_| \_\        |//
//|                                                                               |//
//*-------------------------------------------------------------------------------*//
/////////////////////////////////////////////////////////////////////////////////////
//*-------------------------------------------------------------------------------*//
//| Disco Rock v1.3 (http://www.maus-games.at)                                    |//
//*-------------------------------------------------------------------------------*//
//| Copyright (c) 2013-2015 Martin Mauersics                                      |//
//|                                                                               |//
//| This software is provided 'as-is', without any express or implied             |//
//| warranty. In no event will the authors be held liable for any damages         |//
//| arising from the use of this software.                                        |//
//|                                                                               |//
//| Permission is granted to anyone to use this software for any purpose,         |//
//| including commercial applications, and to alter it and redistribute it        |//
//| freely, subject to the following restrictions:                                |//
//|                                                                               |//
//|   1. The origin of this software must not be misrepresented; you must not     |//
//|   claim that you wrote the original software. If you use this software        |//
//|   in a product, an acknowledgment in the product documentation would be       |//
//|   appreciated but is not required.                                            |//
//|                                                                               |//
//|   2. Altered source versions must be plainly marked as such, and must not be  |//
//|   misrepresented as being the original software.                              |//
//|                                                                               |//
//|   3. This notice may not be removed or altered from any source                |//
//|   distribution.                                                               |//
//*-------------------------------------------------------------------------------*//
/////////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_MAIN_H_
#define _DR_GUARD_MAIN_H_


// ****************************************************************
// engine header files
#include "Core.h"
#include <deque>

#if !defined(_CORE_ANDROID_)
     //#define _DR_EMULATE_MOBILE_ (1)
#endif


// ****************************************************************
// global definitions
#define FONT_ROCKS       "gomarice_rocks.ttf"
#define OUTLINE_SIZE     (1u)

#define COLOR_WHITE_F    coreVector3(1.000f, 1.000f, 1.000f)
#define COLOR_YELLOW_F   coreVector3(1.000f, 0.824f, 0.392f)
#define COLOR_ORANGE_F   coreVector3(1.000f, 0.443f, 0.227f)
#define COLOR_RED_F      coreVector3(1.000f, 0.275f, 0.275f)
#define COLOR_PURPLE_F   coreVector3(0.710f, 0.333f, 1.000f)
#define COLOR_BLUE_F     coreVector3(0.102f, 0.702f, 1.000f)
#define COLOR_GREEN_F    coreVector3(0.118f, 0.745f, 0.353f)

#define COLOR_BRIGHTNESS (0.83f)

static constexpr coreVector3 g_avColor[] =
{
    COLOR_YELLOW_F,
    COLOR_ORANGE_F,
    COLOR_RED_F,
    COLOR_PURPLE_F,
    COLOR_BLUE_F,
    COLOR_GREEN_F,
};
#define COLOR_NUM ARRAY_SIZE(g_avColor)

#define CONTROL_CLASSIC    (0u)
#define CONTROL_MOTION     (1u)
#define CONTROL_FULLSCREEN (2u)

#define STAGE_DIAMOND      (0u)
#define STAGE_QUAD         (1u)
#define STAGE_ZIGZAG       (2u)
#define STAGE_CONCAVE      (3u)
#define STAGE_NET          (4u)
#define STAGE_JUMP         (5u)
#define STAGE_CHOICE       (6u)
#define STAGE_SHADING      (7u)
#define STAGE_SIDE         (8u)
#define STAGE_TRAP         (9u)
#define STAGE_PREVENT      (10u)
#define STAGE_BONUS        (11u)
#define STAGE_MIDDLE       (12u)
#define STAGE_LINES        (13u)
#define STAGE_GATEWAY      (14u)
#define STAGE_CHEESE       (15u)
#define STAGE_CANYON       (16u)
#define STAGE_DOUBLE       (17u)
#define STAGE_RAILS        (18u)
#define STAGE_SLALOM       (19u)
#define STAGE_TOTAL_NUM    (20u)   // total number of game stages


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
extern cBackground*        g_pBackground;       // background with dance floor
extern cMenu*              g_pMenu;             // global menu object
extern cCombatText*        g_pCombatText;       // global "combat" text object
extern cGame*              g_pGame;             // global game object
extern cFirst*             g_pFirst;            // special menu displayed at the first time

extern coreMusicPlayer*    g_pMusicPlayer;      // primary music player
extern coreParticleSystem* g_pParticleSystem;   // primary particle system

extern cOnline*            g_pOnline;           // network access object for leaderboards and achievements

extern coreFloat           g_fTargetSpeed;      // new target speed
extern coreFloat           g_fCurSpeed;         // current speed
extern coreFloat           g_fMusicSpeed;       // dedicated music speed
extern coreBool            g_bPause;            // pause status

extern coreUint16          g_iNumGames;         // number of started games
extern coreUint16          g_iNumFails;         // number of deaths below 10 seconds
extern coreBool            g_bCamUpsideDown;    // upside-down camera status


// ****************************************************************
// message container structure
struct sMsgList final
{
    std::vector<const std::string*> m_apsMsg;   // shuffled messages
    coreUintW m_iCur;                           // current message index

    inline sMsgList(const std::string* psMsg, const coreUintW iSize)noexcept
    : m_iCur (0u)
    {
        // add messages
        m_apsMsg.reserve(iSize);
        for(coreUintW i = 0u; i < iSize; ++i) m_apsMsg.push_back(&psMsg[i]);

        // shuffle them
        std::shuffle(m_apsMsg.begin(), m_apsMsg.end(), std::default_random_engine(CORE_RAND_TIME));
    }

    inline const coreChar* Get()
    {
        // get next message
        if(++m_iCur >= m_apsMsg.size()) m_iCur = 0u;
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