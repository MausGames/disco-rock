/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)


// ****************************************************************
cFirst::cFirst()noexcept
: coreMenu (3, 0)
{
    // reset configuration settings
    Core::Config->GetInt("Game", "Control", 0);
    m_Interface.ChangeControlType(0);

    // create control configuration objects
    m_ControlText.Construct(FONT_ROCKS, 45, 0);
    m_ControlText.SetPosition(coreVector2(0.0f,0.27f));
    m_ControlText.SetText("CONTROL PATTERN");

    m_ControlType.Construct(FONT_ROCKS, 29, 16, 3);
    m_ControlType.SetPosition(coreVector2(0.0f, m_ControlText.GetPosition().y - 0.08f));
    m_ControlType.SetSize(coreVector2(0.49f,0.075f));
    m_ControlType.GetCaption()->SetColor3(coreVector3(0.75f,0.75f,0.75f));
    m_ControlType.AddEntry("CLASSIC",    CONTROL_CLASSIC);
    m_ControlType.AddEntry("MOTION",     CONTROL_MOTION);
    m_ControlType.AddEntry("FULLSCREEN", CONTROL_FULLSCREEN);

    for(int i = 0; i < 2; ++i)
    {
        coreButton* pArrow = m_ControlType.GetArrow(i);

        pArrow->Construct(NULL, NULL, FONT_ROCKS, 45, 2);
        pArrow->DefineProgramShare("2d_shader_border");
        pArrow->SetColor3(coreVector3(0.05f,0.05f,0.05f));
        pArrow->SetTexSize(coreVector2(0.62f,0.62f) / m_ControlType.GetSize().y * 0.0165f);
        pArrow->SetFocusRange(1.2f);
        pArrow->GetCaption()->SetText(i ? ">" : "<");
    }

    // create start button
    m_Start.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Start.DefineProgramShare("2d_shader_border"); // override
    m_Start.SetPosition(coreVector2(0.0f,0.0f));
    m_Start.SetSize(coreVector2(0.49f,0.1f));
    m_Start.SetColor3(COLOR_BLUE_F.xyz());
    m_Start.GetCaption()->SetText("START GAME");
    m_Start.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_Start)

    constexpr_var coreVector3 vTouchColor = LERP(COLOR_BLUE_F.xyz(), COLOR_WHITE_F.xyz(), 0.75f);

    // overload trouch object color
    m_Interface.GetTouchMoveLeft()->SetColor3(vTouchColor);
    m_Interface.GetTouchMoveRight()->SetColor3(vTouchColor);
    m_Interface.GetTouchJump()->SetColor3(vTouchColor);
    m_Interface.GetTouchPause()->SetColor3(vTouchColor);
    m_Interface.GetLine(0)->SetColor3(vTouchColor);
    m_Interface.GetLine(1)->SetColor3(vTouchColor);

    // add menu objects
    this->AddObject(1, &m_ControlText);
    this->AddObject(1, &m_ControlType);
    this->AddObject(1, &m_Start);
}


// ****************************************************************
cFirst::~cFirst()
{
}


// ****************************************************************
void cFirst::Render()
{
    const float fTouchAlpha = m_ControlText.GetAlpha() * 0.5f;

    // forward menu alpha value
    m_Interface.GetTouchMoveLeft()->SetAlpha(fTouchAlpha);
    m_Interface.GetTouchMoveRight()->SetAlpha(fTouchAlpha);
    m_Interface.GetTouchJump()->SetAlpha(fTouchAlpha);
    m_Interface.GetTouchPause()->SetAlpha(fTouchAlpha);
    m_Interface.GetLine(0)->SetAlpha(fTouchAlpha);
    m_Interface.GetLine(1)->SetAlpha(fTouchAlpha);

    // render touch objects
    m_Interface.RenderTouch();

    // render the menu
    coreMenu::Render();
}


// ****************************************************************
void cFirst::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

    // move touch objects
    m_Interface.GetTouchMoveLeft()->Move();
    m_Interface.GetTouchMoveRight()->Move();
    m_Interface.GetTouchJump()->Move();
    m_Interface.GetTouchPause()->Move();
    m_Interface.GetLine(0)->Move();
    m_Interface.GetLine(1)->Move();

    // set transparency of control buttons
    m_Start.SetAlpha(m_Start.GetAlpha() * MENU_ALPHA_IDLE_2); 
    m_ControlType.GetArrow(0)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 0) ? 0.5f : 1.0f));
    m_ControlType.GetArrow(1)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 2) ? 0.5f : 1.0f));
    m_ControlType.GetCaption()->SetAlpha(m_ControlType.GetAlpha());
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(0));
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(1));

    // apply and save control changes
    if(m_ControlType.IsClicked())
    {
        if(Core::Config->GetInt("Game", "Control", 0) != m_ControlType.GetCurIndex())
        {
            Core::Config->SetInt("Game", "Control", 0, m_ControlType.GetCurIndex());
            m_Interface.ChangeControlType(m_ControlType.GetCurIndex());
        }
    }

    // control current surface
    if(!this->GetCurSurface()) this->ChangeSurface(1, 1.0f);
    if(m_Start.IsClicked())    this->ChangeSurface(2, 1.0f);

    // finish the first-time menu
    if(this->GetCurSurface() == 2 && !this->GetTransition().GetStatus())
    {
        g_pMenu->UpdateControl();
        m_iStatus = 1;
    }
}


#endif // _CORE_ANDROID_ || _CORE_DEBUG_