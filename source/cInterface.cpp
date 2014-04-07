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
cInterface::cInterface()noexcept
: m_Score        (FONT_ROCKS, 53, 0)
, m_ScoreValue   (FONT_ROCKS, 67, 16)
, m_Time         (FONT_ROCKS, 53, 0)
, m_TimeValueSec (FONT_ROCKS, 67, 4)
, m_TimeValueMil (FONT_ROCKS, 67, 8)
, m_Combo        (FONT_ROCKS, 53, 8)
, m_ComboValue   (FONT_ROCKS, 67, 8)
, m_Show         (coreTimer(1.0f, 1.0f, 1))
, m_Hide         (coreTimer(1.0f, 1.0f, 1))
{
    constexpr_var float fScale = 4.0f/3.0f;

    // create score labels
    m_ScoreValue.SetCenter(coreVector2(-0.5f,0.5f));
    m_ScoreValue.SetAlignment(coreVector2(1.0f,-1.0f));
    m_ScoreValue.SetPosition(coreVector2(0.01f,-0.01f)*fScale);
    m_ScoreValue.SetText("000000"); 

    m_Score.SetCenter(m_ScoreValue.GetCenter());
    m_Score.SetAlignment(m_ScoreValue.GetAlignment());
    m_Score.SetPosition(m_ScoreValue.GetPosition() + coreVector2(0.0f,-0.06f)*fScale);
    m_Score.SetText("SCORE");
    m_Score.SetColor3(coreVector3(0.75f,0.75f,0.75));
   
    // create time labels
    m_TimeValueSec.SetCenter(coreVector2(0.5f,0.5f));
    m_TimeValueSec.SetAlignment(coreVector2(-1.0f,-1.0f));
    m_TimeValueSec.SetPosition(coreVector2(-0.01f,-0.01f)*fScale);
    m_TimeValueSec.SetText("0");

    m_TimeValueMil.SetCenter(coreVector2(0.5f,0.5f));
    m_TimeValueMil.SetAlignment(coreVector2(-1.0f,-1.0f));
    m_TimeValueMil.SetPosition(coreVector2(-0.048f,-0.01f)*fScale);
    m_TimeValueMil.SetText("000."); 

    m_Time.SetCenter(m_TimeValueSec.GetCenter());
    m_Time.SetAlignment(m_TimeValueSec.GetAlignment());
    m_Time.SetPosition(m_TimeValueSec.GetPosition() + coreVector2(0.0f,-0.06f)*fScale);
    m_Time.SetText("TIME");
    m_Time.SetColor3(coreVector3(0.75f,0.75f,0.75));

    // create combo labels
    m_ComboValue.SetCenter(coreVector2(0.0f,-0.5f));
    m_ComboValue.SetAlignment(coreVector2(-1.0f,1.0f));
    m_ComboValue.SetPosition(coreVector2(0.18f,0.01f)*fScale);
    m_ComboValue.SetText("");

    m_Combo.SetCenter(coreVector2(0.0f,-0.5f));
    m_Combo.SetAlignment(coreVector2(1.0f,1.0f));
    m_Combo.SetPosition(coreVector2(-0.18f,0.01f)*fScale);
    m_Combo.SetText(""); 
    m_Combo.SetColor3(coreVector3(0.75f,0.75f,0.75));

    // fade in the interface
    m_Show.Play(true);
}


// ****************************************************************
cInterface::~cInterface()
{
}


// ****************************************************************
void cInterface::Render()
{
    if(m_Hide.GetCurrent(false) >= 1.0f) return;

    // render all labels
    m_Score.Render();
    m_ScoreValue.Render();
    m_Time.Render();
    m_TimeValueSec.Render();
    m_TimeValueMil.Render();
    m_Combo.Render();
    m_ComboValue.Render();
}


// ****************************************************************
void cInterface::Move()
{
    if(m_Hide.GetCurrent(false) >= 1.0f) return;

    // update both fade timers
    m_Show.Update(1.0f);
    m_Hide.Update(1.0f);

    // assemble all labels
    coreLabel* apLabel[]   = {&m_Score, &m_ScoreValue, &m_Time, &m_TimeValueSec, &m_TimeValueMil, &m_Combo, &m_ComboValue};
    constexpr_var int iNum = sizeof(apLabel) / sizeof(apLabel[0]);

    // calculate alpha and scale value
    const float fAlpha = m_Show.GetCurrent(false) - m_Hide.GetCurrent(false);
    const float fScale = g_pBackground->GetFlash(0.3f);

    for(int i = 0; i < iNum; ++i)
    {
        // update all labels
        apLabel[i]->SetAlpha(fAlpha);
        apLabel[i]->SetScale(fScale);
        apLabel[i]->Move();
    }
}


// ****************************************************************
void cInterface::Update(const float& fScore, const float& fTime, const float& fCombo)
{
    if(m_Hide.GetCurrent(false) >= 1.0f) return;

    // update score and time value text
    m_ScoreValue  .SetText(coreData::Print("%06.0f",  std::floor(fScore)));
    m_TimeValueSec.SetText(coreData::Print("%01d",    int(std::floor(fTime * 10.0f)) % 10));
    m_TimeValueMil.SetText(coreData::Print("%03.0f.", std::floor(fTime)));

    // update combo value text
    const bool bCombo = (fCombo > 1.0f) ? true : false;
    m_Combo     .SetText(bCombo ? "COMBO"                            : "");
    m_ComboValue.SetText(bCombo ? coreData::Print("x%01.1f", fCombo) : "");
}