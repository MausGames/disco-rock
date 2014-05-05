/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

#define COMBO_BAR_LENGTH 0.5f


// ****************************************************************
cInterface::cInterface()noexcept
: m_bComboBackState ((Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY) < 1) ? true : false)
, m_Show            (coreTimer(1.0f, 1.0f, 1))
, m_Hide            (coreTimer(1.0f, 1.0f, 1))
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
    m_ComboBar.DefineProgramShare("2d_shader_color");
    m_ComboBar.SetCenter(coreVector2(0.0f,-0.5f));
    m_ComboBar.SetAlignment(coreVector2(0.0f,1.0f));
    m_ComboBar.SetColor3(COLOR_BLUE_F.xyz());

    // create combo background
    m_ComboBack.DefineProgramShare("2d_shader_color");
    m_ComboBack.SetSize(coreVector2(COMBO_BAR_LENGTH + 0.08f,0.12f));
    m_ComboBack.SetCenter(coreVector2(0.0f,-0.5f));
    m_ComboBack.SetAlignment(coreVector2(0.0f,1.0f));
    m_ComboBack.SetColor3(coreVector3(0.0f,0.0f,0.0f));

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // create touch controls
    m_MoveLeft.Construct("data/textures/button_move.png", "data/textures/button_move.png");
    m_MoveLeft.DefineProgramShare("2d_shader"); // override
    m_MoveLeft.SetPosition(coreVector2(0.03f,-0.2f));
    m_MoveLeft.SetSize(coreVector2(0.1f,0.15f));
    m_MoveLeft.SetCenter(coreVector2(-0.5f,0.5f));
    m_MoveLeft.SetAlignment(coreVector2(1.0f,-1.0f));

    m_MoveRight.Construct("data/textures/button_move.png", "data/textures/button_move.png");
    m_MoveRight.DefineProgramShare("2d_shader"); // override
    m_MoveRight.SetPosition(coreVector2(0.15f,-0.2f));
    m_MoveRight.SetSize(coreVector2(0.1f,0.15f));
    m_MoveRight.SetCenter(coreVector2(-0.5f,0.5f));
    m_MoveRight.SetAlignment(coreVector2(1.0f,-1.0f));
    m_MoveRight.SetDirection(coreVector2(0.0f,-1.0f));

    m_Jump.Construct("data/textures/button_jump.png", "data/textures/button_jump.png");
    m_Jump.DefineProgramShare("2d_shader"); // override
    m_Jump.SetPosition(coreVector2(-0.03f,-0.2f));
    m_Jump.SetSize(coreVector2(0.15f,0.15f));
    m_Jump.SetCenter(coreVector2(0.5f,0.5f));
    m_Jump.SetAlignment(coreVector2(-1.0f,-1.0f));

    m_Pause.Construct("data/textures/button_pause.png", "data/textures/button_pause.png");
    m_Pause.DefineProgramShare("2d_shader"); // override
    m_Pause.SetPosition(coreVector2(-0.03f,-0.45f));
    m_Pause.SetSize(coreVector2(0.075f,0.075f));
    m_Pause.SetCenter(coreVector2(0.5f,0.5f));
    m_Pause.SetAlignment(coreVector2(-1.0f,-1.0f));

#endif

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

    if(!g_bPause)
    {
        // render all labels
        m_Score.Render();
        m_ScoreValue.Render();
        m_Time.Render();
        m_TimeValueMil.Render();
        m_TimeValueSec.Render();

        // render combo background
        if(m_bComboBackState && m_ComboBar.GetSize().x > 0.0001f)
            m_ComboBack.Render();

        // render combo labels and bar
        m_Combo.Render();
        m_ComboValue.Render();
        m_ComboBar.Render();
    }

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // render touch controls
    m_MoveLeft.Render();
    m_MoveRight.Render();
    m_Jump.Render();
    m_Pause.Render();

#endif
}


// ****************************************************************
void cInterface::Move()
{
    if(m_Hide.GetCurrent(false) >= 1.0f) return;

    // update both fade timers
    m_Show.Update(1.0f);
    m_Hide.Update(1.0f);

    // assemble all labels (just for fun)
    coreLabel* apLabel[]   = {&m_Score, &m_ScoreValue, &m_Time, &m_TimeValueMil, &m_TimeValueSec, &m_Combo, &m_ComboValue};
    constexpr_var int iNum = ARRAY_SIZE(apLabel);

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

    // update combo bar and background
    m_ComboBar.SetAlpha(fAlpha);
    m_ComboBar.Move();
    if(m_bComboBackState)
    {
        m_ComboBack.SetAlpha(fAlpha*0.6f);
        m_ComboBack.Move();
    }

#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    // display jump status
    m_Jump.SetColor3(g_pGame->GetRock()->GetJumped() ? LERP(COLOR_BLUE_F.xyz(), COLOR_WHITE_F.xyz(), 0.25f) : COLOR_WHITE_F.xyz());

    // update touch controls
    m_MoveLeft.SetAlpha(fAlpha  * (m_MoveLeft.IsFocused()  ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));
    m_MoveRight.SetAlpha(fAlpha * (m_MoveRight.IsFocused() ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));
    m_Jump.SetAlpha(fAlpha      * (m_Jump.IsFocused()      ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));
    m_Pause.SetAlpha(fAlpha     * (m_Pause.IsFocused()     ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1));

    m_MoveLeft.Move();
    m_MoveRight.Move();
    m_Jump.Move();
    m_Pause.Move();

#endif
}


// ****************************************************************
void cInterface::Update(const float& fScore, const float& fTime, const float& fCombo, const float& fDelay)
{
    if(m_Hide.GetCurrent(false) >= 1.0f) return;

    // update score and time value text
    m_ScoreValue.SetText  (coreData::Print("%06.0f",  std::floor(fScore)));
    m_TimeValueSec.SetText(coreData::Print("%03.0f.", std::floor(fTime)));
    m_TimeValueMil.SetText(coreData::Print("%01d",    int(std::floor(fTime * 10.0f)) % 10));

    // update combo value text
    const bool bCombo = (fCombo > 1.0f) ? true : false;
    m_Combo.SetText     (bCombo ? "COMBO"                            : "");
    m_ComboValue.SetText(bCombo ? coreData::Print("x%01.1f", fCombo) : "");

    // update combo value and bar color
    const coreVector3 vComboColor = LERP(COLOR_RED_F.xyz(), COLOR_GREEN_F.xyz(), fDelay);
    m_ComboValue.SetColor3(vComboColor);
    m_ComboBar.SetColor3  (vComboColor);
    m_ComboBar.SetSize(coreVector2(COMBO_BAR_LENGTH * fDelay, 0.01f));
}


// ****************************************************************
void cInterface::InteractControl()
{
#if defined(_CORE_ANDROID_)

    // interact with movement touch buttons
    m_MoveLeft.Interact();
    m_MoveRight.Interact();
    m_Jump.Interact();

#endif
}

void cInterface::InteractPause()
{
#if defined(_CORE_ANDROID_)

    // interact with pause touch button
    m_Pause.Interact();

#endif
}