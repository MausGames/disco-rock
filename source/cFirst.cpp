//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_) || defined(_DR_EMULATE_MOBILE_)


// ****************************************************************
// constructor
cFirst::cFirst()noexcept
: coreMenu (3u, 0u)
{
    // reset configuration settings
    Core::Config->GetInt("Game", "Control", 0);
    m_Interface.ChangeControlType(0u);

    // create control configuration objects
    m_ControlText.Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_ControlText.SetPosition(coreVector2(0.0f,0.27f));
    m_ControlText.SetText    ("CONTROLS");

    m_ControlType.Construct  (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_ControlType.SetPosition(coreVector2(0.0f, m_ControlText.GetPosition().y - 0.08f));
    m_ControlType.SetSize    (coreVector2(0.49f,0.075f));
    m_ControlType.GetCaption()->SetColor3(coreVector3(0.75f,0.75f,0.75f));
    m_ControlType.AddEntry("CLASSIC",    CONTROL_CLASSIC);
    m_ControlType.AddEntry("MOTION",     CONTROL_MOTION);
    m_ControlType.AddEntry("FULLSCREEN", CONTROL_FULLSCREEN);

    for(coreUintW i = 0u; i < 2u; ++i)
    {
        coreButton* pArrow = m_ControlType.GetArrow(i);

        pArrow->Construct       (NULL, NULL, FONT_ROCKS, 45u, OUTLINE_SIZE);
        pArrow->DefineProgram   ("2d_border_program");
        pArrow->SetColor3       (coreVector3(0.05f,0.05f,0.05f));
        pArrow->SetTexSize      (coreVector2(0.62f,0.62f) / m_ControlType.GetSize().y * 0.0165f);
        pArrow->SetFocusModifier(coreVector2(1.2f,1.2f));
        pArrow->GetCaption()->SetText(i ? ">" : "<");
    }

    // create start button
    m_Start.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Start.DefineProgram("2d_border_program"); // override
    m_Start.SetPosition  (coreVector2(0.0f,0.0f));
    m_Start.SetSize      (coreVector2(0.49f,0.1f));
    m_Start.SetColor3    (COLOR_BLUE_F);
    m_Start.GetCaption()->SetText("START GAME");
    m_Start.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_Start)

    constexpr coreVector3 vTouchColor = LERP(COLOR_BLUE_F, COLOR_WHITE_F, 0.75f);

    // overload touch object color
    m_Interface.GetTouchMoveLeft ()->SetColor3(vTouchColor);
    m_Interface.GetTouchMoveRight()->SetColor3(vTouchColor);
    m_Interface.GetTouchJump     ()->SetColor3(vTouchColor);
    m_Interface.GetTouchPause    ()->SetColor3(vTouchColor);
    m_Interface.GetLine(0u)->SetColor3(vTouchColor);
    m_Interface.GetLine(1u)->SetColor3(vTouchColor);

    // add menu objects
    this->BindObject(1u, &m_ControlText);
    this->BindObject(1u, &m_ControlType);
    this->BindObject(1u, &m_Start);
}


// ****************************************************************
// render the first-time menu
void cFirst::Render()
{
    const coreFloat fTouchAlpha = m_ControlText.GetAlpha() * 0.35f * (0.88f + 0.12f * SIN(coreFloat(Core::System->GetTotalTime()) * 12.0f));

    // forward menu alpha value
    m_Interface.GetTouchMoveLeft ()->SetAlpha(fTouchAlpha);
    m_Interface.GetTouchMoveRight()->SetAlpha(fTouchAlpha);
    m_Interface.GetTouchJump     ()->SetAlpha(fTouchAlpha);
    m_Interface.GetTouchPause    ()->SetAlpha(fTouchAlpha);
    m_Interface.GetLine(0u)->SetAlpha(fTouchAlpha);
    m_Interface.GetLine(1u)->SetAlpha(fTouchAlpha);

    // render touch objects
    m_Interface.RenderTouch();

    // render the menu
    this->coreMenu::Render();
}


// ****************************************************************
// move the first-time menu
void cFirst::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = 0;

    // move touch objects
    m_Interface.GetTouchMoveLeft ()->Move();
    m_Interface.GetTouchMoveRight()->Move();
    m_Interface.GetTouchJump     ()->Move();
    m_Interface.GetTouchPause    ()->Move();
    m_Interface.GetLine(0u)->Move();
    m_Interface.GetLine(1u)->Move();

    // set transparency of control buttons
    m_Start.SetAlpha(m_Start.GetAlpha() * MENU_ALPHA_IDLE_2);
    m_ControlType.GetArrow(0u)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 0u) ? 0.5f : 1.0f));
    m_ControlType.GetArrow(1u)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 2u) ? 0.5f : 1.0f));
    m_ControlType.GetCaption()->SetAlpha(m_ControlType.GetAlpha());
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(0u));
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(1u));

    // apply and save control changes
    if(m_ControlType.IsClicked())
    {
        if(Core::Config->GetInt("Game", "Control", 0) != coreInt32(m_ControlType.GetCurIndex()))
        {
            Core::Config->SetInt("Game", "Control", m_ControlType.GetCurIndex());
            m_Interface.ChangeControlType(m_ControlType.GetCurIndex());
        }
        g_pMenu->PlayClickSound();
    }

    // control current surface
    if(!this->GetCurSurface()) {this->ChangeSurface(1u, 1.0f);}
    if(m_Start.IsClicked())    {this->ChangeSurface(2u, 1.0f); g_pMenu->PlayHappySound();}

    // finish the first-time menu
    if(this->GetCurSurface() == 2u && !this->GetTransition().GetStatus())
    {
        g_pMenu->UpdateControl();
        m_iStatus = 1;
    }
}


#endif // _CORE_ANDROID_ || _CORE_DEBUG_ || _DR_EMULATE_MOBILE_