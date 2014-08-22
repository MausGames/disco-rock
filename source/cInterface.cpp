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
// constructor
cInterface::cInterface()noexcept
: m_Show (coreTimer(1.0f, 1.0f, 1))
, m_Hide (coreTimer(1.0f, 1.0f, 1))
{
    // create score labels
    m_ScoreValue.Construct(FONT_ROCKS, 67, 16);
    m_ScoreValue.SetCenter(coreVector2(-0.5f,0.5f));
    m_ScoreValue.SetAlignment(coreVector2(1.0f,-1.0f));
    m_ScoreValue.SetPosition(coreVector2(0.01333f,-0.01333f));
    m_ScoreValue.SetText("000000"); 

    m_Score.Construct(FONT_ROCKS, 53, 0);
    m_Score.SetCenter(m_ScoreValue.GetCenter());
    m_Score.SetAlignment(m_ScoreValue.GetAlignment());
    m_Score.SetPosition(m_ScoreValue.GetPosition() + coreVector2(0.0f,-0.08f));
    m_Score.SetText("SCORE");
    m_Score.SetColor3(coreVector3(0.75f,0.75f,0.75));
   
    // create time labels
    m_TimeValueMil.Construct(FONT_ROCKS, 67, 4);
    m_TimeValueMil.SetCenter(coreVector2(0.5f,0.5f));
    m_TimeValueMil.SetAlignment(coreVector2(-1.0f,-1.0f));
    m_TimeValueMil.SetPosition(coreVector2(-0.01333f,-0.01333f));
    m_TimeValueMil.SetText("0");

    m_TimeValueSec.Construct(FONT_ROCKS, 67, 8);
    m_TimeValueSec.SetCenter(coreVector2(0.5f,0.5f));
    m_TimeValueSec.SetAlignment(coreVector2(-1.0f,-1.0f));
    m_TimeValueSec.SetPosition(coreVector2(-0.064f,-0.01333f));
    m_TimeValueSec.SetText("000."); 

    m_Time.Construct(FONT_ROCKS, 53, 0);
    m_Time.SetCenter(m_TimeValueMil.GetCenter());
    m_Time.SetAlignment(m_TimeValueMil.GetAlignment());
    m_Time.SetPosition(m_TimeValueMil.GetPosition() + coreVector2(0.0f,-0.08f));
    m_Time.SetText("TIME");
    m_Time.SetColor3(coreVector3(0.75f,0.75f,0.75));

    // create combo labels
    m_ComboValue.Construct(FONT_ROCKS, 67, 8);
    m_ComboValue.SetCenter(coreVector2(0.0f,-0.5f));
    m_ComboValue.SetAlignment(coreVector2(-1.0f,1.0f));
    m_ComboValue.SetPosition(coreVector2(0.24f,0.01333f));
    m_ComboValue.SetText("");

    m_Combo.Construct(FONT_ROCKS, 53, 8);
    m_Combo.SetCenter(coreVector2(0.0f,-0.5f));
    m_Combo.SetAlignment(coreVector2(1.0f,1.0f));
    m_Combo.SetPosition(coreVector2(-0.24f,0.01333f));
    m_Combo.SetText(""); 
    m_Combo.SetColor3(coreVector3(0.75f,0.75f,0.75));

    // create combo bar
    m_ComboBar.DefineProgram("2d_program_color");
    m_ComboBar.SetCenter(coreVector2(0.0f,-0.5f));
    m_ComboBar.SetAlignment(coreVector2(0.0f,1.0f));
    m_ComboBar.SetColor3(COLOR_BLUE_F);

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // create touch controls
    m_MoveLeft.Construct("button_move.png", "button_move.png");
    m_MoveLeft.DefineProgram("2d_program"); // override
    m_MoveLeft.SetSize(coreVector2(0.1f,0.15f));
    m_MoveLeft.SetFocusRange(1.2f);

    m_MoveRight.Construct("button_move.png", "button_move.png");
    m_MoveRight.DefineProgram("2d_program"); // override
    m_MoveRight.SetSize(coreVector2(0.1f,0.15f));
    m_MoveRight.SetDirection(coreVector2(0.0f,-1.0f));
    m_MoveRight.SetFocusRange(1.2f);

    m_Jump.Construct("button_jump.png", "button_jump.png");
    m_Jump.DefineProgram("2d_program"); // override
    m_Jump.SetSize(coreVector2(0.15f,0.15f));
    m_Jump.SetFocusRange(1.8f);

    m_Pause.Construct("button_pause.png", "button_pause.png");
    m_Pause.DefineProgram("2d_program"); // override
    m_Pause.SetSize(coreVector2(0.075f,0.075f));

    // create separating lines
    for(int i = 0; i < 2; ++i)
    {
        m_apLine[i].DefineProgram("2d_program_color");
        m_apLine[i].SetPosition(coreVector2((i ? 0.1667f : -0.1667f) * Core::System->GetResolution().AspectRatio(), 0.0f));
        m_apLine[i].SetSize(coreVector2(0.01f,1.01f));
        m_apLine[i].SetColor3(coreVector3(1.0f,1.0f,1.0f));
    }

    // init control type
    this->ChangeControlType(CLAMP(Core::Config->GetInt("Game", "Control", 0), 0, 2));

#endif

    // fade in the interface
    m_Show.Play(CORE_TIMER_PLAY_RESET);
}


// ****************************************************************
// destructor
cInterface::~cInterface()
{
}


// ****************************************************************
// render the interface
void cInterface::Render()
{
    if(m_Hide.GetValue(CORE_TIMER_GET_NORMAL) >= 1.0f) return;

    if(!g_bPause)
    {
        // render all labels
        m_Score.Render();
        m_ScoreValue.Render();
        m_Time.Render();
        m_TimeValueMil.Render();
        m_TimeValueSec.Render();

        // render combo labels and bar
        m_Combo.Render();
        m_ComboValue.Render();
        m_ComboBar.Render();
    }

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // render touch objects separately
    this->RenderTouch();

#endif
}


// ****************************************************************
// move the interface
void cInterface::Move()
{
    if(m_Hide.GetValue(CORE_TIMER_GET_NORMAL) >= 1.0f) return;

    // update both fade timers
    m_Show.Update(1.0f);
    m_Hide.Update(1.0f);

    // assemble all labels (just for fun)
    coreLabel* apLabel[]   = {&m_Score, &m_ScoreValue, &m_Time, &m_TimeValueMil, &m_TimeValueSec, &m_Combo, &m_ComboValue};
    constexpr_var int iNum = ARRAY_SIZE(apLabel);

    // calculate alpha and scale value
    const float fAlpha = m_Show.GetValue(CORE_TIMER_GET_NORMAL) - m_Hide.GetValue(CORE_TIMER_GET_NORMAL);
    const float fScale = g_pBackground->GetFlash(0.3f);

    for(int i = 0; i < iNum; ++i)
    {
        // update all labels
        apLabel[i]->SetAlpha(fAlpha);
        apLabel[i]->SetScale(fScale);
        apLabel[i]->Move();
    }

    // update combo bar and background
    m_ComboBar.SetAlpha(fAlpha);
    m_ComboBar.Move();

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // display jump status
    m_Jump.SetColor3(g_pGame->GetRock()->GetJumped() ? LERP(COLOR_BLUE_F, COLOR_WHITE_F, 0.25f) : COLOR_WHITE_F);

    // update touch controls
    if(m_iControlType == CONTROL_FULLSCREEN)
    {
        // fade out fullscreen objects
        if(m_fFadeOut) m_fFadeOut = MAX(m_fFadeOut - Core::System->GetTime() * ((g_pGame->GetTime() > 10.0f) ? 1.0f : 0.25f), 0.0f);
        const float fFadeAlpha = 0.1f + 0.9f*m_fFadeOut;

        m_MoveLeft.SetAlpha (fAlpha * fFadeAlpha);
        m_MoveRight.SetAlpha(fAlpha * fFadeAlpha);
        m_Jump.SetAlpha     (fAlpha * fFadeAlpha);

        // also update separating lines
        for(int i = 0; i < 2; ++i)
        {
            m_apLine[i].SetAlpha(fAlpha * fFadeAlpha);
            m_apLine[i].Move();
        }
    }
    else
    {
        m_MoveLeft.SetAlpha (fAlpha * (m_MoveLeft.IsFocused()  ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));
        m_MoveRight.SetAlpha(fAlpha * (m_MoveRight.IsFocused() ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));
        m_Jump.SetAlpha     (fAlpha * (m_Jump.IsFocused()      ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));
    }
    m_Pause.SetAlpha(fAlpha * (m_Pause.IsFocused() ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));

    m_MoveLeft.Move();
    m_MoveRight.Move();
    m_Jump.Move();
    m_Pause.Move();

#endif
}


// ****************************************************************
// update all interface values
void cInterface::Update(const float& fScore, const float& fTime, const float& fCombo, const float& fDelay)
{
    if(m_Hide.GetValue(CORE_TIMER_GET_NORMAL) >= 1.0f) return;

    // update score and time value text
    m_ScoreValue.SetText  (PRINT("%06.0f",  FLOOR(fScore)));
    m_TimeValueSec.SetText(PRINT("%03.0f.", FLOOR(fTime)));
    m_TimeValueMil.SetText(PRINT("%01d",    int(FLOOR(fTime * 10.0f)) % 10));

    // update combo value text
    const bool bCombo = (fCombo > 1.0f) ? true : false;
    m_Combo.SetText     (bCombo ? "COMBO"                  : "");
    m_ComboValue.SetText(bCombo ? PRINT("x%01.1f", fCombo) : "");

    // update combo value and bar color
    const coreVector3 vComboColor = LERP(COLOR_RED_F, LERP(COLOR_YELLOW_F, COLOR_GREEN_F, MAX((fDelay-0.5f) * 2.0f, 0.0f)), MIN(fDelay * 2.0f, 1.0f));
    m_ComboValue.SetColor3(vComboColor);
    m_ComboBar.SetColor3  (vComboColor);
    m_ComboBar.SetSize(coreVector2(COMBO_BAR_LENGTH * fDelay, 0.01f));
}


#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)


// ****************************************************************
// render touch objects separately
void cInterface::RenderTouch()
{
    // render touch controls
    if(m_iControlType != CONTROL_MOTION)
    {
        m_MoveLeft.Render();
        m_MoveRight.Render();
        m_Jump.Render();
    }
    m_Pause.Render();

    // render separating lines
    if(m_iControlType == CONTROL_FULLSCREEN)
    {
        for(int i = 0; i < 2; ++i)
            m_apLine[i].Render();
    }
}


// ****************************************************************
// change current control type
void cInterface::ChangeControlType(const int& iControlType)
{
    // save control type value
    m_iControlType = CLAMP(iControlType, 0, 2);
    m_fFadeOut = 1.0f;

    // change interface objects according to current control type
    if(m_iControlType == CONTROL_CLASSIC)
    {
        m_MoveLeft.SetPosition(coreVector2(0.03f,-0.2f));
        m_MoveLeft.SetCenter(coreVector2(-0.5f,0.5f));
        m_MoveLeft.SetAlignment(coreVector2(1.0f,-1.0f));

        m_MoveRight.SetPosition(coreVector2(0.15f,-0.2f));
        m_MoveRight.SetCenter(coreVector2(-0.5f,0.5f));
        m_MoveRight.SetAlignment(coreVector2(1.0f,-1.0f));

        m_Jump.SetPosition(coreVector2(-0.03f,-0.2f));
        m_Jump.SetCenter(coreVector2(0.5f,0.5f));
        m_Jump.SetAlignment(coreVector2(-1.0f,-1.0f));

        m_Pause.SetPosition(coreVector2(-0.03f,-0.45f));
        m_Pause.SetCenter(coreVector2(0.5f,0.5f));
        m_Pause.SetAlignment(coreVector2(-1.0f,-1.0f));
        m_Pause.SetFocusRange(1.4f);
    }
    else if(m_iControlType == CONTROL_MOTION)
    {
        // most objects are not visible

        m_Pause.SetPosition(coreVector2(-0.03f,0.03f));
        m_Pause.SetCenter(coreVector2(0.5f,-0.5f));
        m_Pause.SetAlignment(coreVector2(-1.0f,1.0f));
        m_Pause.SetFocusRange(1.8f);
    }
    else // == CONTROL_FULLSCREEN
    {
        m_MoveLeft.SetPosition(coreVector2(-0.3333f * Core::System->GetResolution().AspectRatio(),-0.3f));
        m_MoveLeft.SetCenter(coreVector2(0.0f,0.0f));
        m_MoveLeft.SetAlignment(coreVector2(0.0f,0.0f));

        m_MoveRight.SetPosition(coreVector2(0.0f,-0.3f));
        m_MoveRight.SetCenter(coreVector2(0.0f,0.0f));
        m_MoveRight.SetAlignment(coreVector2(0.0f,0.0f));

        m_Jump.SetPosition(coreVector2(0.3333f * Core::System->GetResolution().AspectRatio(),-0.3f));
        m_Jump.SetCenter(coreVector2(0.0f,0.0f));
        m_Jump.SetAlignment(coreVector2(0.0f,0.0f));

        m_Pause.SetPosition(coreVector2(-0.03f,0.03f));
        m_Pause.SetCenter(coreVector2(0.5f,-0.5f));
        m_Pause.SetAlignment(coreVector2(-1.0f,1.0f));
        m_Pause.SetFocusRange(1.8f);
    }

    // override alpha
    const float fAlpha = (m_Show.GetValue(CORE_TIMER_GET_NORMAL) - m_Hide.GetValue(CORE_TIMER_GET_NORMAL)) * MENU_ALPHA_IDLE_1;
    m_MoveLeft.SetAlpha(fAlpha);
    m_MoveRight.SetAlpha(fAlpha);
    m_Jump.SetAlpha(fAlpha);
    m_Pause.SetAlpha(fAlpha);

    // immediately move objects
    m_MoveLeft.Move();
    m_MoveRight.Move();
    m_Jump.Move();
    m_Pause.Move();
}


// ****************************************************************
// interact with control objects
void cInterface::InteractControl()
{
    // interact with movement touch buttons
    m_MoveLeft.Interact();
    m_MoveRight.Interact();
    m_Jump.Interact();
}


// ****************************************************************
// interact with pause objects
void cInterface::InteractPause()
{
    // interact with pause touch button
    m_Pause.Interact();
}


#endif