/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
cCombatText::sData::sData()noexcept
: sData ("", coreVector2(0.0f,0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f))
{
}

cCombatText::sData::sData(const char* pcText, const coreVector2& vPosition, const coreVector4& vColor)noexcept
: sText     (pcText)
, vPosition (vPosition)
, vColor    (vColor)
{
}


// ****************************************************************
cCombatText::sData::sData(sData&& m)noexcept
: sText     (std::move(m.sText))
, vPosition (m.vPosition)
, vColor    (m.vColor)
{
}


// ****************************************************************
cCombatText::cCombatText()noexcept
: m_iCurText    (0)
, m_Delay       (coreTimer(0.01f, 1.0f, 1))
, m_TrophyTimer (coreTimer(1.5f,  1.0f, 1))
{
    for(int i = 0; i < COMBAT_TEXT_NUM; ++i)
    {
        // create labels and reset all missing properties
        m_aText  [i].Construct(FONT_ROCKS, 37, 32);
        m_aFloat [i].Set(1.0f, 1.0f, 1);
        m_afAlpha[i] = 0.0f;
    }

    // create trophy symbol
    m_Trophy.DefineTextureFile(0, "data/textures/icon_trophy.png");
    m_Trophy.DefineProgram(m_aText[0].GetProgram());
    m_Trophy.SetSize(coreVector2(0.12f,0.12f));
    m_Trophy.SetColor3(COLOR_YELLOW_F);

    // creatr trophy label
    m_TrophyLabel.Construct(FONT_ROCKS, 37, 32);
}


// ****************************************************************
cCombatText::~cCombatText()
{
    // reset all active texts
    this->Reset();
}


// ****************************************************************
void cCombatText::Render()
{
    // render trophy symbol
    if(m_TrophyTimer.GetStatus())
    {
        m_Trophy.Render();
        m_TrophyLabel.Render();
    }

    // render active labels
    for(int i = 0; i < COMBAT_TEXT_NUM; ++i)
    {
        if(m_aFloat[i].GetStatus())
            m_aText[i].Render();
    }
}


// ****************************************************************
void cCombatText::Move()
{
    // update delay between two texts
    m_Delay.Update(1.0f);
    if(!m_Delay.GetStatus())
    {
        // check for available text data
        if(!m_aData.empty())
        {
            if(++m_iCurText >= COMBAT_TEXT_NUM) m_iCurText = 0;

            // get next text data
            const sData& Data = m_aData.front();
            
            // init label object
            coreLabel* pText = &m_aText[m_iCurText];
            pText->SetText(Data.sText.c_str());
            pText->SetDirection(coreVector2::Direction(Core::Rand->Float(-0.3f, 0.3f)));
            pText->SetCenter(Data.vPosition);
            pText->SetColor4(Data.vColor);

            // save alpha value
            m_afAlpha[m_iCurText] = Data.vColor.a;

            // start timers
            m_aFloat[m_iCurText].Play(true);
            m_Delay.Play(true);

            m_aData.pop_front();
        }
    }

    const float fFlash = g_pBackground->GetFlash(0.3f);

    for(int i = 0; i < COMBAT_TEXT_NUM; ++i)
    {
        if(m_aFloat[i].GetStatus())
        {
            // update the float timer
            if(m_aFloat[i].Update(1.0f)) m_aText[i].SetAlpha(0.0f);
            else
            {
                // update the combat text
                m_aText[i].SetPosition(coreVector2(m_aText[i].GetCenter().x*1.5f, -1.0f) * -0.13333f * m_aFloat[i].GetCurrent(false));
                m_aText[i].SetScale(fFlash);
                m_aText[i].SetAlpha(m_aFloat[i].GetCurrent(true) * m_afAlpha[i]);
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
            m_Trophy.SetPosition(coreVector2(0.0f, 0.08f + 0.35f * m_TrophyTimer.GetCurrent(false)));
            m_Trophy.SetDirection(coreVector2::Direction(m_TrophyTimer.GetCurrent(false) * 2.0f * PI));
            m_Trophy.SetAlpha(m_TrophyTimer.GetCurrent(true) * 1.5f);
            m_Trophy.Move();

            // update the trophy label
            m_TrophyLabel.SetPosition(coreVector2(0.0f, 0.04f + 0.2f * m_TrophyTimer.GetCurrent(false)));
            m_TrophyLabel.SetScale(fFlash);
            m_TrophyLabel.SetAlpha(m_TrophyTimer.GetCurrent(true) * 1.5f);
            m_TrophyLabel.Move();
        }
    }
}


// ****************************************************************
void cCombatText::Reset()
{
    // stop all timers
    m_Delay.Stop();
    for(int i = 0; i < COMBAT_TEXT_NUM; ++i) 
        m_aFloat[i].Stop();

    // clear memory
    m_aData.clear();
}


// ****************************************************************
void cCombatText::AddTextTransformed(const char* pcText, const coreVector3& vPosition, const coreVector4& vColor)
{
    if(vColor.a <= 0.0f) return;

    // calculate screen position and add text
    const coreVector2 vScreenPos = (vPosition * Core::Graphics->GetCamera() * Core::Graphics->GetPerspective()).xy() / coreVector2(180.0f,135.0f) + coreVector2::Rand(-0.05f,0.05f);
    this->AddText(pcText, vScreenPos, vColor);
}


// ****************************************************************
void cCombatText::ShowTrophy(const char* pcText, const coreVector3& vPosition)
{
    // calculate screen position
    const coreVector2 vScreenPos = (vPosition * Core::Graphics->GetCamera() * Core::Graphics->GetPerspective()).xy() / coreVector2(180.0f,135.0f);
    
    // set trophy and trophy text
    m_Trophy.SetCenter(vScreenPos);
    m_TrophyLabel.SetText(pcText);
    m_TrophyLabel.SetCenter(vScreenPos);

    // start trophy
    m_TrophyTimer.Play(true);
}