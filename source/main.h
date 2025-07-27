//////////////////////////////////////////////////////////////////////////////////
//*----------------------------------------------------------------------------*//
//| Disco Rock v1.5.0 (https://www.maus-games.at)                              |//
//*----------------------------------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics                                        |//
//|                                                                            |//
//| This software is provided 'as-is', without any express or implied          |//
//| warranty. In no event will the authors be held liable for any damages      |//
//| arising from the use of this software.                                     |//
//|                                                                            |//
//| Permission is granted to anyone to use this software for any purpose,      |//
//| including commercial applications, and to alter it and redistribute it     |//
//| freely, subject to the following restrictions:                             |//
//|                                                                            |//
//| 1. The origin of this software must not be misrepresented; you must not    |//
//|    claim that you wrote the original software. If you use this software    |//
//|    in a product, an acknowledgment in the product documentation would be   |//
//|    appreciated but is not required.                                        |//
//|                                                                            |//
//| 2. Altered source versions must be plainly marked as such, and must not be |//
//|    misrepresented as being the original software.                          |//
//|                                                                            |//
//| 3. This notice may not be removed or altered from any source distribution. |//
//*----------------------------------------------------------------------------*//
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_MAIN_H_
#define _DR_GUARD_MAIN_H_


// ****************************************************************
// engine headers
#include "Core.h"

#if defined(_CORE_DEBUG_)
    //#define _DR_EMULATE_MOBILE_
#endif
#if !defined(_CORE_MOBILE_) && !defined(_DR_EMULATE_MOBILE_)
    #define _DR_API_GAMEJOLT_
#endif

#if defined(_CORE_MSVC_)
    #pragma warning(disable : 4127)   // constant conditional expression
#endif


// ****************************************************************
// general definitions
#define FONT_ROCKS       "gomarice_rocks.ttf"
#define OUTLINE_SIZE     (3u)

#define COLOR_WHITE_F    coreVector3(1.000f, 1.000f, 1.000f)
#define COLOR_YELLOW_F   coreVector3(1.000f, 0.824f, 0.392f)
#define COLOR_ORANGE_F   coreVector3(1.000f, 0.443f, 0.227f)
#define COLOR_RED_F      coreVector3(1.000f, 0.275f, 0.275f)
#define COLOR_PURPLE_F   coreVector3(0.710f, 0.333f, 1.000f)
#define COLOR_BLUE_F     coreVector3(0.102f, 0.702f, 1.000f)
#define COLOR_GREEN_F    coreVector3(0.118f, 0.745f, 0.353f)

#define COLOR_BRIGHTNESS (0.83f)
#define SOUND_PROPERTIES (1.0f), (5.0f), (1.0f)

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

enum eControl : coreUint8
{
    CONTROL_CLASSIC = 0u,
    CONTROL_MOTION,
    CONTROL_FULLSCREEN
};

enum eStage : coreUint8
{
    STAGE_DIAMOND = 0u,
    STAGE_QUAD,
    STAGE_ZIGZAG,
    STAGE_CONCAVE,
    STAGE_NET,
    STAGE_JUMP,
    STAGE_CHOICE,
    STAGE_SHADING,
    STAGE_SIDE,
    STAGE_TRAP,
    STAGE_PREVENT,
    STAGE_BONUS,
    STAGE_MIDDLE,
    STAGE_LINES,
    STAGE_GATEWAY,
    STAGE_CHEESE,
    STAGE_CANYON,
    STAGE_DOUBLE,
    STAGE_RAILS,
    STAGE_SLALOM,
    STAGE_TOTAL_NUM   // total number of game stages
};


// ****************************************************************
// game headers
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
extern cBackground*        const g_pBackground;       // background with dance floor
extern cMenu*              const g_pMenu;             // global menu object
extern cCombatText*        const g_pCombatText;       // global "combat" text object
extern cGame*              const g_pGame;             // global game object
extern cFirst*             const g_pFirst;            // special menu displayed at the first time

extern coreMusicPlayer*    const g_pMusicPlayer;      // primary music player
extern coreParticleSystem* const g_pParticleSystem;   // primary particle system

extern cOnline*            const g_pOnline;           // network access object for leaderboards and achievements

extern coreFloat           g_fTargetSpeed;            // new target speed
extern coreFloat           g_fCurSpeed;               // current speed
extern coreFloat           g_fMusicSpeed;             // dedicated music speed
extern coreBool            g_bPause;                  // pause status

extern coreUint16          g_iNumGames;               // number of started games
extern coreUint16          g_iNumFails;               // number of deaths below 10 seconds
extern coreBool            g_bCamUpsideDown;          // upside-down camera status


// ****************************************************************
// message container structure
struct sMsgList final
{
    coreList<const coreChar*> m_apcMsg;   // shuffled messages
    coreUintW m_iCur;                     // current message index

    inline sMsgList(const coreChar* const* ppcMsg, const coreUintW iSize)noexcept
    : m_iCur (0u)
    {
        // add messages
        m_apcMsg.reserve(iSize);
        for(coreUintW i = 0u; i < iSize; ++i) m_apcMsg.push_back(ppcMsg[i]);

        // shuffle them
        coreData::RangeShuffle(m_apcMsg.begin(), m_apcMsg.end());
    }

    inline const coreChar* Get()
    {
        // get next message
        if(++m_iCur >= m_apcMsg.size()) m_iCur = 0u;
        return m_apcMsg[m_iCur];
    }
};


// ****************************************************************
// message containers
extern sMsgList g_MsgIntro;
extern sMsgList g_MsgFallen;
extern sMsgList g_MsgTrap;
extern sMsgList g_MsgBegin;


#endif // _DR_GUARD_MAIN_H_