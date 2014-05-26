/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_INTERFACE_H_
#define _DR_GUARD_INTERFACE_H_


// ****************************************************************
class cInterface final
{
private:
    coreLabel m_Score;          // score name text
    coreLabel m_ScoreValue;     // score value text
                                
    coreLabel m_Time;           // time name text
    coreLabel m_TimeValueSec;   // time value text (seperated to remove wobbling)
    coreLabel m_TimeValueMil;   // time value text
                                  
    coreLabel m_Combo;          // combo name text
    coreLabel m_ComboValue;     // combo value text
    coreObject2D m_ComboBar;    // simple bar to display the current combo delay

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    coreButton m_MoveLeft;      // touch move left button
    coreButton m_MoveRight;     // touch move right button
    coreButton m_Jump;          // touch jump button
    coreButton m_Pause;         // touch pause button
                                
#endif                          
                                
    coreTimer m_Show;           // timer to fade in (on game-start)
    coreTimer m_Hide;           // timer to fade out (on game-end)


public:
    cInterface()noexcept;
    ~cInterface();

    // render and move the interface
    void Render();
    void Move();

    // update all interface values
    void Update(const float& fScore, const float& fTime, const float& fCombo, const float& fDelay);

    // smoothly fade out the interface
    inline void Hide() {if(m_Hide.GetCurrent(false) == 0.0f) m_Hide.Play(false);}

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // access touch objects
    inline coreObject2D* GetTouchMoveLeft()  {return &m_MoveLeft;}
    inline coreObject2D* GetTouchMoveRight() {return &m_MoveRight;}
    inline coreObject2D* GetTouchJump()      {return &m_Jump;}
    inline coreObject2D* GetTouchPause()     {return &m_Pause;}

#endif

    // interact with touch objects
    void InteractControl();
    void InteractPause();
};


#endif // _DR_GUARD_INTERFACE_H_