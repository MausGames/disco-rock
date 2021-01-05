//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_FIRST_H_
#define _DR_GUARD_FIRST_H_

#if defined(_CORE_MOBILE_) || defined(_DR_EMULATE_MOBILE_)


// ****************************************************************
// first-time menu class
class cFirst final : public coreMenu
{
private:
    coreLabel       m_ControlText;   // control header
    coreSwitchBoxU8 m_ControlType;   // control setting selection
    coreButton      m_Start;         // start into main menu

    cInterface m_Interface;          // interface instance to display current control settings


public:
    cFirst()noexcept;

    DISABLE_COPY(cFirst)

    // render and move the first-time menu
    void Render()final;
    void Move  ()final;
};


#else


// just define as empty object
typedef coreObject2D cFirst;


#endif // _CORE_MOBILE_ || _DR_EMULATE_MOBILE_

#endif // _DR_GUARD_FIRST_H_