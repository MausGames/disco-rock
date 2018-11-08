//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_INTERFACE_H_
#define _DR_GUARD_INTERFACE_H_


// ****************************************************************
// interface definitions
#define COMBO_BAR_LENGTH (0.5f)   // combo bar length


// ****************************************************************
// interface class
class cInterface final
{
private:
    coreLabel m_Score;            // score name text
    coreLabel m_ScoreValue;       // score value text

    coreLabel m_Time;             // time name text
    coreLabel m_TimeValueSec;     // time value text (separated to remove wobbling)
    coreLabel m_TimeValueMil;     // time value text

    coreLabel    m_Combo;         // combo name text
    coreLabel    m_ComboValue;    // combo value text
    coreObject2D m_ComboBar;      // simple bar to display the current combo delay

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_) || defined(_DR_EMULATE_MOBILE_)

    coreButton m_MoveLeft;        // touch move left button
    coreButton m_MoveRight;       // touch move right button
    coreButton m_Jump;            // touch jump button
    coreButton m_Pause;           // touch pause button

    coreObject2D m_aLine   [2];   // line objects separating the screen into three parts
    coreObject2D m_aOverlay[2];   // invisible button-overlays to increase touch-range (0 = left, 1 = right)

    coreUint8 m_iControlType;     // cached control type value
    coreFloat m_fFadeOut;         // fade out fullscreen touch elements

#endif

    coreTimer m_Show;             // timer to fade in (on game-start)
    coreTimer m_Hide;             // timer to fade out (on game-end)


public:
    cInterface()noexcept;

    DISABLE_COPY(cInterface)

    // render and move the interface
    void Render();
    void Move();

    // update all interface values
    void Update(const coreFloat fScore, const coreFloat fTime, const coreFloat fCombo, const coreFloat fDelay);

    // smoothly fade out the interface
    inline void Hide() {if(m_Hide.GetValue(CORE_TIMER_GET_NORMAL) == 0.0f) m_Hide.Play(CORE_TIMER_PLAY_CURRENT);}

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_) || defined(_DR_EMULATE_MOBILE_)

    // render touch objects separately
    void RenderTouch();

    // access touch objects
    inline coreObject2D* GetTouchMoveLeft ()                       {return &m_MoveLeft;}
    inline coreObject2D* GetTouchMoveRight()                       {return &m_MoveRight;}
    inline coreObject2D* GetTouchJump     ()                       {return &m_Jump;}
    inline coreObject2D* GetTouchPause    ()                       {return &m_Pause;}
    inline coreObject2D* GetLine          (const coreUintW iIndex) {return &m_aLine[iIndex];}
    inline coreObject2D* GetOverlayLeft   ()                       {return &m_aOverlay[0];}
    inline coreObject2D* GetOverlayRight  ()                       {return &m_aOverlay[1];}

    // control control type !
    void ChangeControlType(const coreUint8 iControlType);
    inline const coreUint8& GetControlType()const {return m_iControlType;}

    // interact with touch objects
    void InteractControl();
    void InteractPause();

#else

    // empty interaction functions
    inline void InteractControl()const {}
    inline void InteractPause  ()const {}

#endif
};


#endif // _DR_GUARD_INTERFACE_H_