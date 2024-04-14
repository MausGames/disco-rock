//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cCombatText::sData::sData(const coreChar* pcText, const coreVector2 vPosition, const coreVector4 vColor)noexcept
: sText     (pcText)
, vPosition (vPosition)
, vColor    (vColor)
{
}

cCombatText::sData::sData(sData&& m)noexcept
: sText     (std::move(m.sText))
, vPosition (m.vPosition)
, vColor    (m.vColor)
{
}


// ****************************************************************
// constructor
cCombatText::cCombatText()noexcept
: m_iCurText    (0u)
, m_Delay       (coreTimer(0.01f, 1.0f, 1))
, m_afAlpha     {}
, m_TrophyTimer (coreTimer(1.5f,  1.0f, 1))
{
    for(coreUintW i = 0u; i < COMBAT_TEXT_NUM; ++i)
    {
        // create labels and reset all missing properties
        m_aText [i].Construct (FONT_ROCKS, 37u, OUTLINE_SIZE);
        m_aText [i].SetRectify(false);
        m_aFloat[i].Set(1.0f, 1.0f, 1);
    }

    // create trophy symbol
    m_Trophy.DefineTexture(1u, "icon_trophy.webp");
    m_Trophy.DefineProgram(m_aText[0].GetProgram());
    m_Trophy.SetSize      (coreVector2(0.12f,0.12f));
    m_Trophy.SetColor3    (COLOR_YELLOW_F);

    // create trophy label
    m_TrophyLabel.Construct(FONT_ROCKS, 37u, OUTLINE_SIZE);
    m_TrophyLabel.SetRectify(false);
}


// ****************************************************************
// destructor
cCombatText::~cCombatText()
{
    // reset all active texts
    this->Reset();
}


// ****************************************************************
// render the combat text
void cCombatText::Render()
{
    // render trophy symbol
    if(m_TrophyTimer.GetStatus())
    {
        m_Trophy.Render();
        m_TrophyLabel.Render();
    }

    // render active labels
    for(coreUintW i = 0u; i < COMBAT_TEXT_NUM; ++i)
    {
        if(m_aFloat[i].GetStatus())
            m_aText[i].Render();
    }
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    // update delay between two texts
    m_Delay.Update(1.0f);
    if(!m_Delay.GetStatus())
    {
        // check for available text data
        if(!m_aData.empty())
        {
            if(++m_iCurText >= COMBAT_TEXT_NUM) m_iCurText = 0u;

            // get next text data
            const sData& oData = m_aData.front();

            // init label object
            coreLabel* pText = &m_aText[m_iCurText];
            pText->SetText     (oData.sText.c_str());
            pText->SetDirection(coreVector2::Direction(Core::Rand->Float(-0.3f, 0.3f)));
            pText->SetCenter   (oData.vPosition);
            pText->SetColor4   (oData.vColor);

            // save alpha value
            m_afAlpha[m_iCurText] = oData.vColor.w;

            // start timers
            m_aFloat[m_iCurText].Play(CORE_TIMER_PLAY_RESET);
            m_Delay.Play(CORE_TIMER_PLAY_RESET);

            m_aData.pop_front();
        }
    }

    const coreFloat fFlash = g_pBackground->GetFlash(0.3f);

    for(coreUintW i = 0u; i < COMBAT_TEXT_NUM; ++i)
    {
        if(m_aFloat[i].GetStatus())
        {
            // update the float timer
            if(m_aFloat[i].Update(1.0f)) m_aText[i].SetAlpha(0.0f);
            else
            {
                // update the combat text
                m_aText[i].SetPosition(coreVector2(m_aText[i].GetCenter().x*1.5f, -1.0f) * -0.13333f * m_aFloat[i].GetValue(CORE_TIMER_GET_NORMAL));
                m_aText[i].SetScale   (coreVector2(1.0f,1.0f) * fFlash);
                m_aText[i].SetAlpha   (m_aFloat[i].GetValue(CORE_TIMER_GET_REVERSED) * m_afAlpha[i]);
                m_aText[i].Move();
            }
        }
    }

    if(m_TrophyTimer.GetStatus())
    {
        // update the trophy timer
        if(m_TrophyTimer.Update(1.0f)) m_Trophy.SetAlpha(0.0f);
        else
        {
            // update the trophy symbol
            m_Trophy.SetPosition (coreVector2(0.0f, 0.08f + 0.35f * m_TrophyTimer.GetValue(CORE_TIMER_GET_NORMAL)));
            m_Trophy.SetDirection(coreVector2::Direction(m_TrophyTimer.GetValue(CORE_TIMER_GET_NORMAL) * 2.0f * PI));
            m_Trophy.SetAlpha    (m_TrophyTimer.GetValue(CORE_TIMER_GET_REVERSED) * 1.5f);
            m_Trophy.Move();

            // update the trophy label
            m_TrophyLabel.SetPosition(coreVector2(0.0f, 0.04f + 0.2f * m_TrophyTimer.GetValue(CORE_TIMER_GET_NORMAL)));
            m_TrophyLabel.SetScale   (coreVector2(1.0f,1.0f) * fFlash);
            m_TrophyLabel.SetAlpha   (m_TrophyTimer.GetValue(CORE_TIMER_GET_REVERSED) * 1.5f);
            m_TrophyLabel.Move();
        }
    }
}


// ****************************************************************
// reset all combat text entries
void cCombatText::Reset()
{
    // stop all timers
    m_Delay.Stop();
    for(coreUintW i = 0u; i < COMBAT_TEXT_NUM; ++i)
        m_aFloat[i].Stop();

    // clear memory
    m_aData.clear();
}


// ****************************************************************
// add new transformed combat text entry
void cCombatText::AddTextTransformed(const coreChar* pcText, const coreVector3 vPosition, const coreVector4 vColor)
{
    if(vColor.w <= 0.0f) return;

    // calculate screen position and add text
    const coreVector2 vScreenPos = (coreVector4(vPosition, 1.0f) * Core::Graphics->GetCamera() * Core::Graphics->GetPerspective()).xy() / coreVector2(180.0f,135.0f) + coreVector2::Rand(-0.05f,0.05f);
    this->AddText(pcText, vScreenPos, vColor);
}


// ****************************************************************
// show trophy animation
void cCombatText::ShowTrophy(const coreChar* pcText, const coreVector3 vPosition)
{
    // calculate screen position
    const coreVector2 vScreenPos = (coreVector4(vPosition, 1.0f) * Core::Graphics->GetCamera() * Core::Graphics->GetPerspective()).xy() / coreVector2(180.0f,135.0f);

    // set trophy and trophy text
    m_Trophy.SetCenter(vScreenPos);
    m_TrophyLabel.SetText  (pcText);
    m_TrophyLabel.SetCenter(vScreenPos);

    // start trophy
    m_TrophyTimer.Play(CORE_TIMER_PLAY_RESET);
}