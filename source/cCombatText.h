/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_COMBATTEXT_H_
#define _DR_GUARD_COMBATTEXT_H_


// ****************************************************************
// combat text definitions
#define COMBAT_TEXT_NUM (8u)   // number of label objects


// ****************************************************************
// combat text class
class cCombatText final   // actually there is no "combat" in the game
{
private:
    // text data structure
    struct sData final
    {
        std::string sText;       // actual text to display
        coreVector2 vPosition;   // screen position
        coreVector4 vColor;      // RGBA color-value

        sData(const coreChar* pcText, const coreVector2& vPosition, const coreVector4& vColor)noexcept;
        sData(sData&& m)noexcept;
    };


private:
    coreLabel m_aText[COMBAT_TEXT_NUM];     // label objects to display combat text
    coreUintW m_iCurText;                   // current label object

    coreTimer m_Delay;                      // delay between two created texts to increase visibility
    coreTimer m_aFloat [COMBAT_TEXT_NUM];   // timer for the float-animation
    coreFloat m_afAlpha[COMBAT_TEXT_NUM];   // saved initial alpha values for more dynamic fading

    coreObject2D m_Trophy;                  // flying trophy symbol
    coreLabel    m_TrophyLabel;             // dedicated trophy label
    coreTimer    m_TrophyTimer;             // timer for the flying trophy symbol

    std::deque<sData> m_aData;              // text data deque


public:
    cCombatText()noexcept;
    ~cCombatText();

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // reset all active labels
    void Reset();

    // add new combat text data
    inline void AddText           (const coreChar* pcText, const coreVector2& vPosition, const coreVector4& vColor) {m_aData.emplace_back(pcText, vPosition, vColor);}
    void        AddTextTransformed(const coreChar* pcText, const coreVector3& vPosition, const coreVector4& vColor);

    // show trophy animation
    void ShowTrophy(const coreChar* pcText, const coreVector3& vPosition);
};


#endif // _DR_GUARD_COMBATTEXT_H_