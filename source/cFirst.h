/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_FIRST_H_
#define _DR_GUARD_FIRST_H_

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_) || defined(_DR_EMULATE_MOBILE_)


// ****************************************************************
// first-time menu class
class cFirst final : public coreMenu
{
private:
    coreLabel          m_ControlText;   // control header
    coreSwitchBox<int> m_ControlType;   // control setting selection
    coreButton         m_Start;         // start into main menu

    cInterface m_Interface;             // interface instance to display current control settings


public:
    cFirst()noexcept;
    ~cFirst();

    DISABLE_COPY(cFirst)

    // render and move the first-time menu
    void Render()override;
    void Move  ()override;
};


#else


// just define as empty object
typedef coreObject2D cFirst;


#endif // _CORE_ANDROID_ || _CORE_DEBUG_

#endif // _DR_GUARD_FIRST_H_