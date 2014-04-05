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

    coreTimer m_Show;           // timer to fade in (on game-start)
    coreTimer m_Hide;           // timer to fade out (on game-end)


public:
    cInterface()noexcept;
    ~cInterface();

    // render and move the interface
    void Render();
    void Move();

    // update all interface values
    void Update(const float& fScore, const float& fTime, const float& fCombo);

    // smoothly fade out the interface
    inline void Hide() {if(m_Hide.GetCurrent(false) == 0.0f) m_Hide.Play(false);}
};


#endif // _DR_GUARD_INTERFACE_H_