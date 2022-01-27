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
cMenu::cMenu()noexcept
: coreMenu         (20u, 0u)
, m_ScoreMenu      (8u,  0u)
, m_LoginMenu      (4u,  0u)
, m_Intro          (coreTimer(10.0f, 1.0f, 1u))
, m_afSubmitValue  {}
, m_bSubmitted     (true)
, m_bInLeaderboard (false)
, m_iCurPage       (0u)
, m_iTableUpdate   (0u)
, m_iTrophyStatus  (0u)
, m_iTrophyCurrent (-1)
, m_bFromGuest     (false)
{
    STATIC_ASSERT(sizeof(m_iTrophyStatus)*8u >= TROPHY_ITEMS)

    // create intro objects
    m_MausLogo.DefineTexture(0u, "maus_logo.png");
    m_MausLogo.DefineProgram("2d_simple_program");
    m_MausLogo.SetPosition  (coreVector2(0.0f,0.0f));
    m_MausLogo.SetSize      (coreVector2(0.512f,0.256f) * 0.95f);

    m_BigLogo.DefineTexture(0u, "game_logo.png");
    m_BigLogo.DefineProgram("2d_simple_program");
    m_BigLogo.SetPosition  (coreVector2(0.0f,0.05f));

    // create background objects
    m_Black.DefineProgram("2d_color_program");
    m_Black.SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_White.DefineProgram("2d_color_program");

    m_BackgroundLeft.DefineProgram("2d_border_program");
    m_BackgroundLeft.SetPosition  (coreVector2(0.15f,0.0f));
    m_BackgroundLeft.SetSize      (coreVector2(0.62f,0.62f));
    m_BackgroundLeft.SetColor3    (coreVector3(0.05f,0.05f,0.05f));
    ADJUST_LEFT  (m_BackgroundLeft)
    ADJUST_BORDER(m_BackgroundLeft)

    m_BackgroundRight.DefineProgram("2d_border_program");
    m_BackgroundRight.SetPosition  (coreVector2(-0.03f,0.0f));
    m_BackgroundRight.SetSize      (coreVector2(MIN(Core::System->GetResolution().AspectRatio() - 0.62f - 0.15f - 0.06f, 0.62f + 0.12f), 0.62f));
    m_BackgroundRight.SetColor3    (coreVector3(0.05f,0.05f,0.05f));
    ADJUST_RIGHT (m_BackgroundRight)
    ADJUST_BORDER(m_BackgroundRight)

    const coreVector2 vRightCenter = m_BackgroundRight.GetPosition() - 0.5f*m_BackgroundRight.GetSize();

    // create header objects
    m_Logo.DefineTexture(0u, "game_logo.png");
    m_Logo.DefineProgram("2d_simple_program");
    m_Logo.SetPosition  (coreVector2(vRightCenter.x, 0.129f));
    m_Logo.SetCenter    (coreVector2(0.5f,0.0f));

    m_Pause.Construct  (FONT_ROCKS, 80u, OUTLINE_SIZE);
    m_Pause.SetPosition(m_Logo.GetPosition());
    m_Pause.SetCenter  (coreVector2(0.5f,0.0f));
    m_Pause.SetText    ("PAUSE");

    // create labeled buttons
    m_Start.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Start.DefineProgram("2d_border_program"); // override
    m_Start.SetPosition  (coreVector2(-0.06f,-0.092f));
    m_Start.SetSize      (coreVector2(m_BackgroundRight.GetSize().x - 0.06f,0.1f));
    m_Start.SetColor3    (COLOR_BLUE_F);
    m_Start.GetCaption()->SetText ("START GAME");
    m_Start.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT (m_Start)
    ADJUST_BORDER(m_Start)

    m_Exit.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Exit.DefineProgram("2d_border_program"); // override
    m_Exit.SetPosition  (coreVector2(-0.06f,-0.222f));
    m_Exit.SetSize      (m_Start.GetSize());
    m_Exit.SetColor3    (COLOR_RED_F);
    m_Exit.GetCaption()->SetText ("EXIT GAME");
    m_Exit.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT (m_Exit)
    ADJUST_BORDER(m_Exit)

    const coreBool bTooSmall = (Core::System->GetResolution().AspectRatio() < 1.3f);

    m_Resume.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Resume.DefineProgram("2d_border_program"); // override
    m_Resume.SetPosition  (m_Start.GetPosition());
    m_Resume.SetSize      (m_Start.GetSize());
    m_Resume.SetColor3    (COLOR_BLUE_F);
    m_Resume.GetCaption()->SetText (bTooSmall ? "RESUME" : "RESUME GAME");
    m_Resume.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT (m_Resume)
    ADJUST_BORDER(m_Resume)

    m_Abort.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Abort.DefineProgram("2d_border_program"); // override
    m_Abort.SetPosition  (m_Exit.GetPosition());
    m_Abort.SetSize      (m_Start.GetSize());
    m_Abort.SetColor3    (COLOR_RED_F);
    m_Abort.GetCaption()->SetText (bTooSmall ? "ABORT" : "ABORT GAME");
    m_Abort.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT (m_Abort)
    ADJUST_BORDER(m_Abort)

    m_Submit.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Submit.DefineProgram("2d_border_program"); // override
    m_Submit.SetPosition  (m_Start.GetPosition());
    m_Submit.SetSize      (m_Start.GetSize());
    m_Submit.SetColor3    (COLOR_ORANGE_F);
    m_Submit.GetCaption()->SetText (bTooSmall ? "SUBMIT" : "SUBMIT ONLINE");
    m_Submit.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT (m_Submit)
    ADJUST_BORDER(m_Submit)

    m_Finish.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Finish.DefineProgram("2d_border_program"); // override
    m_Finish.SetPosition  (m_Exit.GetPosition());
    m_Finish.SetSize      (m_Start.GetSize());
    m_Finish.SetColor3    (COLOR_PURPLE_F);
    m_Finish.GetCaption()->SetText ("RETURN");
    m_Finish.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT (m_Finish)
    ADJUST_BORDER(m_Finish)

    // create graphical buttons
    m_Short.Construct       ("button_play.png", "button_play.png");
    m_Short.SetPosition     (coreVector2(-0.06f,0.2425f) * (DEFINED(_CORE_MOBILE_) || DEFINED(_DR_EMULATE_MOBILE_) ? 1.0f : 1000.0f));
    m_Short.SetSize         (coreVector2(0.075f,0.075f));
    m_Short.SetFocusModifier(coreVector2(1.2f,1.2f));
    ADJUST_RIGHT(m_Short)

    m_ButtonScore.Construct  ("button_score.png", "button_score.png");
    m_ButtonScore.SetPosition(coreVector2(0.03f,0.24f));
    m_ButtonScore.SetSize    (coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonScore)

    m_ButtonTime.Construct  ("button_time.png", "button_time.png");
    m_ButtonTime.SetPosition(coreVector2(0.03f,0.12f));
    m_ButtonTime.SetSize    (coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonTime)

    m_ButtonTrophy.Construct  ("button_trophy.png", "button_trophy.png");
    m_ButtonTrophy.SetPosition(coreVector2(0.03f,-0.12f));
    m_ButtonTrophy.SetSize    (coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonTrophy)

    m_ButtonConfig.Construct  ("button_config.png", "button_config.png");
    m_ButtonConfig.SetPosition(coreVector2(0.03f,-0.24f));
    m_ButtonConfig.SetSize    (coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonConfig)

    // create objects on the upper screen
    m_TopUpdating.Construct   (FONT_ROCKS, 21u, OUTLINE_SIZE);
    m_TopUpdating.SetPosition (coreVector2(0.0f,-0.015f));
    m_TopUpdating.SetCenter   (coreVector2(0.0f,0.5f));
    m_TopUpdating.SetAlignment(coreVector2(0.0f,-1.0f));

    // create objects on the lower screen
    m_BottomInfo.Construct   (FONT_ROCKS, 21u, OUTLINE_SIZE);
    m_BottomInfo.SetPosition (coreVector2(0.01333f,0.00933f));
    m_BottomInfo.SetCenter   (coreVector2(-0.5f,-0.5f));
    m_BottomInfo.SetAlignment(coreVector2(1.0f,1.0f));
    m_BottomInfo.SetText     (coreData::StrUpper(PRINT("(c) 2013 Martin Mauersics - v1.5.0 - %s %.5s", __DATE__, __TIME__)));

    m_BottomCredit.Construct   (FONT_ROCKS, 21u, OUTLINE_SIZE);
    m_BottomCredit.SetPosition (coreVector2(-0.01333f,0.00933f));
    m_BottomCredit.SetCenter   (coreVector2(0.5f,-0.5f));
    m_BottomCredit.SetAlignment(coreVector2(-1.0f,1.0f));
    m_BottomCredit.SetText     (coreData::StrUpper("Music by Kevin MacLeod"));

    m_BottomLoginName.Construct   (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_BottomLoginName.SetPosition (coreVector2(-0.01333f,0.00667f));
    m_BottomLoginName.SetCenter   (coreVector2(0.5f,-0.5f));
    m_BottomLoginName.SetAlignment(coreVector2(-1.0f,1.0f));

    m_BottomLoginJolt.DefineTexture(0u, "gamejolt_jolt.png");
    m_BottomLoginJolt.DefineProgram("2d_simple_program");
    m_BottomLoginJolt.SetPosition  (coreVector2(-0.02f,0.016f));
    m_BottomLoginJolt.SetSize      (coreVector2(0.036f,0.036f)*2.0f);
    m_BottomLoginJolt.SetCenter    (coreVector2(0.5f,-0.5f));
    m_BottomLoginJolt.SetAlignment (coreVector2(-1.0f,1.0f));

    // create question objects
    m_QuestionBlack.DefineProgram("2d_color_program");
    m_QuestionBlack.SetColor3    (coreVector3(0.0f,0.0f,0.0f));
    m_QuestionBlack.SetFocusable (true);

    m_Question.Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_Question.SetPosition(coreVector2(0.0f,0.08f));
    m_Question.SetText    ("Do you want to go out with me?");

    m_Yes.Construct       ("button_ok.png", "button_ok.png");
    m_Yes.SetPosition     (coreVector2(-0.2f,-0.08f));
    m_Yes.SetSize         (coreVector2(0.075f,0.075f));
    m_Yes.SetFocusModifier(coreVector2(1.2f,1.2f));

    m_No.Construct       ("button_cancel.png", "button_cancel.png");
    m_No.SetPosition     (coreVector2(0.2f,-0.08f));
    m_No.SetSize         (m_Yes.GetSize());
    m_No.SetFocusModifier(coreVector2(1.2f,1.2f));

    // create video configuration objects
    m_VideoText.Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_VideoText.SetPosition(coreVector2(LEFT_CENTER,0.235f));
    m_VideoText.SetCenter  (coreVector2(-0.5f,0.0f));
    m_VideoText.SetText    ("VIDEO");

    const coreUintW iCurQuality = CLAMP(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY), 0, 2);

    m_VideoLow.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_VideoLow.DefineProgram("2d_border_program"); // override
    m_VideoLow.SetPosition  (coreVector2(LEFT_CENTER - 0.17f,0.15f)); // old Y: 0.13f
    m_VideoLow.SetSize      (coreVector2(0.15f,0.075f));
    m_VideoLow.SetCenter    (coreVector2(-0.5f,0.0f));
    m_VideoLow.SetColor3    ((iCurQuality == 0u) ? COLOR_GREEN_F : coreVector3(0.5f,0.5f,0.5f));
    m_VideoLow.GetCaption()->SetText ("LOW");
    m_VideoLow.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_VideoLow)

    m_VideoMedium.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_VideoMedium.DefineProgram("2d_border_program"); // override
    m_VideoMedium.SetPosition  (coreVector2(LEFT_CENTER, m_VideoLow.GetPosition().y));
    m_VideoMedium.SetSize      (m_VideoLow.GetSize());
    m_VideoMedium.SetCenter    (coreVector2(-0.5f,0.0f));
    m_VideoMedium.SetColor3    ((iCurQuality == 1u) ? COLOR_YELLOW_F : coreVector3(0.5f,0.5f,0.5f));
    m_VideoMedium.GetCaption()->SetText ("MID");
    m_VideoMedium.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_VideoMedium)

    m_VideoHigh.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_VideoHigh.DefineProgram("2d_border_program"); // override
    m_VideoHigh.SetPosition  (coreVector2(LEFT_CENTER + 0.17f, m_VideoLow.GetPosition().y));
    m_VideoHigh.SetSize      (m_VideoLow.GetSize());
    m_VideoHigh.SetCenter    (coreVector2(-0.5f,0.0f));
    m_VideoHigh.SetColor3    ((iCurQuality == 2u) ? COLOR_RED_F : coreVector3(0.5f,0.5f,0.5f));
    m_VideoHigh.GetCaption()->SetText ("HIGH");
    m_VideoHigh.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_VideoHigh)

    // create audio configuration objects
    m_AudioText.Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_AudioText.SetPosition(coreVector2(LEFT_CENTER,0.04f)); // old Y: 0.02f
    m_AudioText.SetCenter  (coreVector2(-0.5f,0.0f));
    m_AudioText.SetText    ("AUDIO");

    const coreFloat fCurVolume = Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME) * 0.1f;

    m_AudioBarBack.DefineProgram("2d_color_bar_program");
    m_AudioBarBack.SetPosition  (coreVector2(LEFT_CENTER, -0.04f)); // old Y: -0.08f
    m_AudioBarBack.SetSize      (coreVector2(0.5f,0.056f));
    m_AudioBarBack.SetCenter    (coreVector2(-0.5f,0.0f));
    m_AudioBarBack.SetColor3    (coreVector3(0.5f,0.5f,0.5f));

    m_AudioDrag.Construct    ("default_black.png", "default_black.png");
    m_AudioDrag.DefineProgram("2d_border_program"); // override
    m_AudioDrag.SetSize      (coreVector2(0.035f,0.085f));
    m_AudioDrag.SetPosition  (coreVector2(LEFT_CENTER + (fCurVolume - 0.5f) * (m_AudioBarBack.GetSize().x - m_AudioDrag.GetSize().x), m_AudioBarBack.GetPosition().y));
    m_AudioDrag.SetCenter    (coreVector2(-0.5f,0.0f));
    m_AudioDrag.SetColor3    (coreVector3(0.0f,0.0f,0.0f));
    ADJUST_BORDER(m_AudioDrag)

    m_AudioBar.DefineProgram("2d_color_bar_program");
    m_AudioBar.SetSize      (coreVector2((m_AudioBarBack.GetSize().x - m_AudioDrag.GetSize().x) * fCurVolume, 0.056f));
    m_AudioBar.SetPosition  (coreVector2(LEFT_CENTER + (m_AudioBar.GetSize().x - m_AudioBarBack.GetSize().x) * 0.5f, m_AudioBarBack.GetPosition().y));
    m_AudioBar.SetCenter    (coreVector2(-0.5f,0.0f));
    m_AudioBar.SetColor3    (COLOR_BLUE_F * MENU_ALPHA_IDLE_2);

    m_AudioIconHigh.DefineTexture   (0u, "icon_audio_1.png");
    m_AudioIconHigh.DefineProgram   ("2d_color_icon_program");
    m_AudioIconHigh.SetPosition     (coreVector2(LEFT_CENTER + 0.22f, m_AudioBarBack.GetPosition().y));
    m_AudioIconHigh.SetSize         (coreVector2(0.05f,0.05f));
    m_AudioIconHigh.SetCenter       (coreVector2(-0.5f,0.0f));
    m_AudioIconHigh.SetFocusModifier(coreVector2(0.0f,0.0f));

    m_AudioIconLow.DefineTexture   (0u, "icon_audio_2.png");
    m_AudioIconLow.DefineProgram   ("2d_color_icon_program");
    m_AudioIconLow.SetPosition     (coreVector2(LEFT_CENTER - 0.22f, m_AudioBarBack.GetPosition().y));
    m_AudioIconLow.SetSize         (coreVector2(0.05f,0.05f));
    m_AudioIconLow.SetCenter       (coreVector2(-0.5f,0.0f));
    m_AudioIconLow.SetFocusModifier(coreVector2(0.0f,0.0f));

#if defined(_DR_API_GAMEJOLT_)

    // create login configuration objects
    m_LoginConfigOr.Construct  (FONT_ROCKS, 21u, OUTLINE_SIZE);
    m_LoginConfigOr.SetPosition(coreVector2(LEFT_CENTER - 0.047f, -0.195f));
    m_LoginConfigOr.SetCenter  (coreVector2(-0.5f,0.0f));
    m_LoginConfigOr.SetText    ("LOG INTO");

    m_LoginConfigLogo.DefineTexture(0u, "gamejolt_logo.png");
    m_LoginConfigLogo.DefineProgram("2d_simple_program");
    m_LoginConfigLogo.SetPosition  (coreVector2(LEFT_CENTER - 0.047f,-0.238f));
    m_LoginConfigLogo.SetSize      (coreVector2(0.512f,0.064f) * 1.2f);
    m_LoginConfigLogo.SetCenter    (coreVector2(-0.5f,0.0f));

    m_LoginConfigStart.Construct  ("button_login.png", "button_login.png");
    m_LoginConfigStart.SetPosition(coreVector2(LEFT_CENTER + 0.17f, -0.221f));
    m_LoginConfigStart.SetSize    (coreVector2(0.075f,0.075f));
    ADJUST_LEFT(m_LoginConfigStart)

#elif defined(_CORE_MOBILE_) || defined(_DR_EMULATE_MOBILE_)

    // create control configuration objects
    m_ControlText.Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_ControlText.SetPosition(coreVector2(LEFT_CENTER,-0.141f));
    m_ControlText.SetCenter  (coreVector2(-0.5f,0.0f));
    m_ControlText.SetText    ("CONTROLS");

    const coreUintW iCurControl = CLAMP(Core::Config->GetInt("Game", "Control", 0), 0, 2);

    m_ControlType.Construct  (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_ControlType.SetPosition(coreVector2(LEFT_CENTER,-0.221f));
    m_ControlType.SetSize    (coreVector2(0.49f,0.075f));
    m_ControlType.SetCenter  (coreVector2(-0.5f,0.0f));
    m_ControlType.GetCaption()->SetColor3(coreVector3(0.75f,0.75f,0.75f));
    m_ControlType.AddEntry("CLASSIC",    CONTROL_CLASSIC);
    m_ControlType.AddEntry("MOTION",     CONTROL_MOTION);
    m_ControlType.AddEntry("FULLSCREEN", CONTROL_FULLSCREEN);
    m_ControlType.SelectIndex(iCurControl);

    for(coreUintW i = 0u; i < 2u; ++i)
    {
        coreButton* pArrow = m_ControlType.GetArrow(i);

        pArrow->Construct    (NULL, NULL, FONT_ROCKS, 45u, OUTLINE_SIZE);
        pArrow->DefineProgram("2d_border_program");  // override
        pArrow->SetColor3    (coreVector3(0.05f,0.05f,0.05f));
        pArrow->SetTexSize   (coreVector2(0.62f,0.62f) / m_ControlType.GetSize().y * 0.0165f);
        pArrow->GetCaption()->SetText(i ? ">" : "<");
    }

#endif

#if defined(_DR_API_GAMEJOLT_)

    // create login objects
    m_LoginBlack.DefineProgram("2d_color_program");
    m_LoginBlack.SetColor3    (coreVector3(0.0f,0.0f,0.0f));
    m_LoginBlack.SetFocusable (true);

    m_LoginPopup.DefineProgram("2d_border_program");
    m_LoginPopup.SetPosition  (coreVector2(0.0f,0.0f));
    m_LoginPopup.SetSize      (coreVector2(0.62f,0.37f));
    m_LoginPopup.SetColor3    (coreVector3(0.05f,0.05f,0.05f));
    ADJUST_BORDER(m_LoginPopup)

    m_aLoginText[0].Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_aLoginText[0].SetPosition(m_LoginPopup.GetPosition() + coreVector2(0.0f,0.11f));
    m_aLoginText[0].SetText    ("LOGIN");
    m_aLoginText[1].Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_aLoginText[1].SetPosition(m_aLoginText[0].GetPosition());
    m_aLoginText[1].SetText    ("SUBMIT");

    m_LoginEnterName.Construct   (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_LoginEnterName.SetPosition (m_LoginPopup.GetPosition() + coreVector2(-0.27f,0.02f));
    m_LoginEnterName.SetAlignment(coreVector2(1.0f,0.0f));
    m_LoginEnterName.SetText     ("USER NAME");

    m_LoginEnterToken.Construct   (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_LoginEnterToken.SetPosition (m_LoginPopup.GetPosition() + coreVector2(-0.27f,-0.05f));
    m_LoginEnterToken.SetAlignment(coreVector2(1.0f,0.0f));
    m_LoginEnterToken.SetText     ("TOKEN");

    m_LoginEnterGuest.Construct   (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_LoginEnterGuest.SetPosition (m_LoginPopup.GetPosition() + coreVector2(-0.27f,0.02f));
    m_LoginEnterGuest.SetAlignment(coreVector2(1.0f,0.0f));
    m_LoginEnterGuest.SetText     ("NAME");

    m_LoginName.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 29u, OUTLINE_SIZE, 32u);
    m_LoginName.DefineProgram("2d_color_program"); // override
    m_LoginName.SetPosition  (m_LoginEnterName.GetPosition() + coreVector2(0.54f,0.0f));
    m_LoginName.SetSize      (coreVector2(0.35f,0.05f));
    m_LoginName.SetAlignment (coreVector2(-1.0f,0.0f));
    m_LoginName.SetColor3    (coreVector3(0.25f,0.25f,0.25f));
    m_LoginName.SetCursor    ('<');

    m_LoginToken.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 29u, OUTLINE_SIZE, 64u);
    m_LoginToken.DefineProgram("2d_color_program"); // override
    m_LoginToken.SetPosition  (m_LoginEnterToken.GetPosition() + coreVector2(0.54f,0.0f));
    m_LoginToken.SetSize      (m_LoginName.GetSize());
    m_LoginToken.SetAlignment (coreVector2(-1.0f,0.0f));
    m_LoginToken.SetColor3    (coreVector3(0.25f,0.25f,0.25f));
    m_LoginToken.SetCursor    ('<');
    m_LoginToken.SetReplace   ('*');

    m_LoginGuest.Construct    ("default_black.png", "default_black.png", FONT_ROCKS, 29u, OUTLINE_SIZE, 32u);
    m_LoginGuest.DefineProgram("2d_color_program"); // override
    m_LoginGuest.SetPosition  (m_LoginEnterGuest.GetPosition() + coreVector2(0.54f,0.0f));
    m_LoginGuest.SetSize      (m_LoginName.GetSize() + coreVector2(0.085f,0.0f));
    m_LoginGuest.SetAlignment (coreVector2(-1.0f,0.0f));
    m_LoginGuest.SetColor3    (coreVector3(0.25f,0.25f,0.25f));
    m_LoginGuest.SetCursor    ('<');

    m_LoginOK.Construct  ("button_ok.png", "button_ok.png");
    m_LoginOK.SetPosition(m_LoginPopup.GetPosition() + coreVector2(m_LoginPopup.GetSize().x * 0.5f + 0.0575f, m_LoginPopup.GetSize().y * 0.5f - 0.0575f));
    m_LoginOK.SetSize    (coreVector2(0.075f,0.075f));

    m_LoginCancel.Construct  ("button_cancel.png", "button_cancel.png");
    m_LoginCancel.SetPosition(m_LoginPopup.GetPosition() + coreVector2(m_LoginPopup.GetSize().x * 0.5f + 0.0575f, -m_LoginPopup.GetSize().y * 0.5f + 0.0575f));
    m_LoginCancel.SetSize    (coreVector2(0.075f,0.075f));

    m_LoginError.Construct  (FONT_ROCKS, 21u, OUTLINE_SIZE);
    m_LoginError.SetPosition(m_LoginPopup.GetPosition() + coreVector2(0.0f,-0.122f));
    m_LoginError.SetText    ("All your base are belong to us!");

    m_LoginJoltOr.Construct  (FONT_ROCKS, 21u, OUTLINE_SIZE);
    m_LoginJoltOr.SetPosition(m_LoginPopup.GetPosition() + coreVector2(-0.047f,-0.067f));
    m_LoginJoltOr.SetText    ("OR LOG INTO");

    m_LoginJoltLogo.DefineTexture(0u, "gamejolt_logo.png");
    m_LoginJoltLogo.DefineProgram("2d_simple_program");
    m_LoginJoltLogo.SetPosition  (m_LoginPopup.GetPosition() + coreVector2(-0.047f,-0.11f));
    m_LoginJoltLogo.SetSize      (coreVector2(0.512f,0.064f) * 1.2f);

    m_LoginJoltStart.Construct   ("button_login.png", "button_login.png");
    m_LoginJoltStart.SetPosition (m_LoginPopup.GetPosition() + coreVector2(0.17f,-0.093f));
    m_LoginJoltStart.SetSize     (coreVector2(0.075f,0.075f));
    m_LoginJoltStart.SetAlignment(coreVector2(1.0f,0.0f));

#endif

    // create successful submit object
    m_Successful.DefineTexture   (1u, "icon_success.png");
    m_Successful.DefineProgram   (m_Pause.GetProgram());
    m_Successful.SetPosition     (m_Submit.GetPosition() + coreVector2(-0.03f,0.0f));
    m_Successful.SetSize         (coreVector2(0.06f,0.06f));
    m_Successful.SetFocusModifier(coreVector2(0.0f,0.0f));
    ADJUST_RIGHT(m_Successful)

    // create score objects
    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
    {
        constexpr coreVector2 vPos = coreVector2(LEFT_CENTER,0.115f);
        constexpr coreVector2 vCen = coreVector2(-0.5f,0.0f);

        m_aScoreTable[i].Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
        m_aScoreTable[i].SetPosition(vPos + coreVector2(0.0f,0.12f));
        m_aScoreTable[i].SetCenter  (vCen);
        m_aScoreTable[i].SetText    (i ? "TIME" : "SCORE");

        for(coreUintW j = 0u; j < SCORE_ENTRIES; ++j)
        {
            m_aaScoreEntry[i][j][0].Construct   (FONT_ROCKS, 23u, OUTLINE_SIZE);
            m_aaScoreEntry[i][j][0].SetPosition (vPos + coreVector2(-0.208f, 0.045f - I_TO_F(j)*0.04f));
            m_aaScoreEntry[i][j][0].SetCenter   (vCen);
            m_aaScoreEntry[i][j][0].SetAlignment(coreVector2(-1.0f,0.0f));
            m_aaScoreEntry[i][j][0].SetColor3   (coreVector3(0.75f,0.75f,0.75f));
            m_aaScoreEntry[i][j][0].SetText     (PRINT("%zu.", j + 1u));

            m_aaScoreEntry[i][j][1].Construct   (FONT_ROCKS, 23u, OUTLINE_SIZE);
            m_aaScoreEntry[i][j][1].SetPosition (coreVector2(vPos.x - 0.19466f, m_aaScoreEntry[i][j][0].GetPosition().y));
            m_aaScoreEntry[i][j][1].SetCenter   (vCen);
            m_aaScoreEntry[i][j][1].SetAlignment(coreVector2(1.0f,0.0f));
            m_aaScoreEntry[i][j][1].SetColor3   ((j < 3u) ? COLOR_YELLOW_F : COLOR_WHITE_F);
            m_aaScoreEntry[i][j][1].SetText     ("-");

            m_aaScoreEntry[i][j][2].Construct   (FONT_ROCKS, 23u, OUTLINE_SIZE);
            m_aaScoreEntry[i][j][2].SetPosition (coreVector2(vPos.x + 0.232f, m_aaScoreEntry[i][j][0].GetPosition().y));
            m_aaScoreEntry[i][j][2].SetCenter   (vCen);
            m_aaScoreEntry[i][j][2].SetAlignment(coreVector2(-1.0f,0.0f));
            m_aaScoreEntry[i][j][2].SetColor3   (coreVector3(0.75f,0.75f,0.75f));
            m_aaScoreEntry[i][j][2].SetText     ("-");
        }

        m_aScoreBest[i].Construct  (FONT_ROCKS, 29u, OUTLINE_SIZE);
        m_aScoreBest[i].SetPosition(vPos + coreVector2(0.0f,-0.31f));
        m_aScoreBest[i].SetCenter  (vCen);
        m_aScoreBest[i].SetText    ("YOUR BEST");

        m_aScoreBestValue[i].Construct  (FONT_ROCKS, 29u, OUTLINE_SIZE);
        m_aScoreBestValue[i].SetPosition(coreVector2(vPos.x, m_aScoreBest[i].GetPosition().y - 0.04667f));
        m_aScoreBestValue[i].SetCenter  (vCen);
        m_aScoreBestValue[i].SetColor3  (coreVector3(0.75f,0.75f,0.75f));
        m_aScoreBestValue[i].SetText    ("-");

        m_aScoreRecord[i].Construct  (FONT_ROCKS, 21u, OUTLINE_SIZE);
        m_aScoreRecord[i].SetPosition(coreVector2(vPos.x - 0.18f, m_aScoreBestValue[i].GetPosition().y));
        m_aScoreRecord[i].SetCenter  (vCen);
        m_aScoreRecord[i].SetColor3  (coreVector3(0.75f,0.75f,0.75f));

        m_aScoreRank[i].Construct   (FONT_ROCKS, 23u, OUTLINE_SIZE);
        m_aScoreRank[i].SetPosition (coreVector2(vPos.x - 0.228f, m_aScoreBestValue[i].GetPosition().y));
        m_aScoreRank[i].SetCenter   (vCen);
        m_aScoreRank[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aScoreRank[i].SetColor3   (coreVector3(0.75f,0.75f,0.75f));
    }

    m_PageChange.Construct  ("button_scroll.png", "button_scroll.png");
    m_PageChange.SetPosition(coreVector2(LEFT_CENTER + 0.17f,-0.221f));
    m_PageChange.SetSize    (coreVector2(0.075f,0.075f));
    ADJUST_LEFT(m_PageChange)

    // create submit objects
    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
    {
        const     coreVector2 vPos = coreVector2(vRightCenter.x, 0.235f - I_TO_F(i)*0.14f);
        constexpr coreVector2 vCen = coreVector2(0.5f,0.0f);

        m_aAfterBest[i].Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
        m_aAfterBest[i].SetPosition(vPos);
        m_aAfterBest[i].SetCenter  (vCen);
        m_aAfterBest[i].SetText    (i ? "YOUR TIME" : "YOUR SCORE");

        m_aAfterBestValue[i].Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
        m_aAfterBestValue[i].SetPosition(coreVector2(vPos.x, m_aAfterBest[i].GetPosition().y - 0.06f));
        m_aAfterBestValue[i].SetCenter  (vCen);
        m_aAfterBestValue[i].SetColor3  (coreVector3(0.75f,0.75f,0.75f));
        m_aAfterBestValue[i].SetText    ("-");

        m_aAfterRecord[i].Construct  (FONT_ROCKS, 21u, OUTLINE_SIZE);
        m_aAfterRecord[i].SetPosition(coreVector2(vPos.x - m_BackgroundRight.GetSize().x*0.5f + 0.13f, m_aAfterBestValue[i].GetPosition().y));
        m_aAfterRecord[i].SetCenter  (vCen);
        m_aAfterRecord[i].SetColor3  (coreVector3(0.75f,0.75f,0.75f));
    }

    // create trophy objects
    m_TrophyText.Construct  (FONT_ROCKS, 45u, OUTLINE_SIZE);
    m_TrophyText.SetPosition(coreVector2(LEFT_CENTER,0.235f));
    m_TrophyText.SetCenter  (coreVector2(-0.5f,0.0f));
    m_TrophyText.SetText    ("TROPHIES");

    for(coreUintW i = 0u; i < TROPHY_ITEMS; ++i)
    {
        m_aTrophyImage[i].DefineTexture(0u, PRINT("trophy_%d.png", (i == TROPHY_ITEMS-1u) ? 2 : 1));
        m_aTrophyImage[i].DefineProgram("2d_simple_program");
        m_aTrophyImage[i].SetPosition  (coreVector2(LEFT_CENTER + I_TO_F((i%5u)-2u)*0.105f, -0.075f - I_TO_F((i/5u)-2u)*0.105f));
        m_aTrophyImage[i].SetSize      (coreVector2(0.09f,0.09f));
        m_aTrophyImage[i].SetCenter    (coreVector2(-0.5f,0.0f));
        m_aTrophyImage[i].SetColor3    ((i % (COLOR_NUM+1u)) ? g_avColor[(i-(1u+i/7u))%COLOR_NUM] : (COLOR_WHITE_F*0.9f));
        m_aTrophyImage[i].SetFocusable (true);

        m_aTrophyCheck[i].DefineTexture   (1u, "icon_success.png");
        m_aTrophyCheck[i].DefineProgram   (m_Pause.GetProgram());
        m_aTrophyCheck[i].SetPosition     (coreVector2(LEFT_CENTER + I_TO_F((i%5u)-2u)*0.105f, -0.075f - I_TO_F((i/5u)-2u)*0.105f));
        m_aTrophyCheck[i].SetSize         (coreVector2(0.06f,0.06f));
        m_aTrophyCheck[i].SetCenter       (coreVector2(-0.5f,0.0f));
        m_aTrophyCheck[i].SetFocusModifier(coreVector2(0.0f,0.0f));
    }

    for(coreUintW i = 0u; i < TROPHY_SECRETS; ++i)
    {
        m_aTrophySecret[i].Construct       (FONT_ROCKS, 45u, OUTLINE_SIZE);
        m_aTrophySecret[i].SetCenter       (coreVector2(-0.5f,0.0f));
        m_aTrophySecret[i].SetText         ("?");
        m_aTrophySecret[i].SetFocusModifier(coreVector2(0.0f,0.0f));

        m_aTrophySecret[i].SetPosition(m_aTrophyImage[i].GetPosition());
    }

    m_TrophyName.Construct  (FONT_ROCKS, 29u, OUTLINE_SIZE);
    m_TrophyName.SetPosition(coreVector2(LEFT_CENTER,-0.172f));
    m_TrophyName.SetCenter  (coreVector2(-0.5f,0.0f));
    m_TrophyName.SetText    ("-");

    for(coreUintW i = 0u; i < 2u; ++i)
    {
        m_aTrophyDesc[i].Construct  (FONT_ROCKS, 21u, OUTLINE_SIZE);
        m_aTrophyDesc[i].SetPosition(coreVector2(LEFT_CENTER, m_TrophyName.GetPosition().y - 0.04667f - I_TO_F(i)*0.03f));
        m_aTrophyDesc[i].SetCenter  (coreVector2(-0.5f,0.0f));
        m_aTrophyDesc[i].SetColor3  (coreVector3(0.75f,0.75f,0.75f));
        m_aTrophyDesc[i].SetText    ("-");
    }

    // create loading objects
    m_Loading.DefineTexture(0u, "icon_load.png");
    m_Loading.DefineProgram("2d_color_icon_program");
    m_Loading.SetPosition  (coreVector2(LEFT_CENTER - 0.05f, -0.05f) + m_BackgroundLeft.GetSize()*0.5f);
    m_Loading.SetSize      (coreVector2(0.05f,0.05f));
    m_Loading.SetCenter    (coreVector2(-0.5f,0.0f));
    m_Loading.SetColor3    (COLOR_YELLOW_F);

    // start intro
    m_Intro.Play(CORE_TIMER_PLAY_RESET);

    // load sound-effects
    m_pClickSound  = Core::Manager::Resource->Get<coreSound>("menu.wav");
    m_pHappySound  = Core::Manager::Resource->Get<coreSound>("achieve.wav");
    m_pRecordSound = Core::Manager::Resource->Get<coreSound>("record.wav");
    m_pFlashSound  = Core::Manager::Resource->Get<coreSound>("flash.wav");

    // reset fullscreen-objects
    this->ResetResolution();

    // main menu
    // 0  black
    // 1  intro logos
    // 2  empty screen
    // 3  game logo only
    // reserved
    // 5  flash
    // 6  main menu
    // 7  main menu exit
    // 8  main menu login
    // reserved
    // 10 game
    // 11 pause
    // 12 pause abort
    // 13 pause login
    // 14 submit
    // 15 submit return
    // 16 submit login

    // score menu
    // 0 score
    // 1 time
    // 2 coola
    // reserved
    // 5 config

    // login menu
    // 0 login Game Jolt
    // 1 login guest

    this->BindObject(0u, &m_Black);

    this->BindObject(1u, &m_Black);
    this->BindObject(1u, &m_MausLogo);

    this->BindObject(3u, &m_BigLogo);

    this->BindObject(5u, &m_White);

    for(coreUintW i = 6u; i <= 8u; ++i)
    {
        this->BindObject(i, &m_BackgroundLeft);
        this->BindObject(i, &m_BackgroundRight);
        this->BindObject(i, &m_Logo);
        this->BindObject(i, &m_Start);
        this->BindObject(i, &m_Exit);
        this->BindObject(i, &m_Short);
        this->BindObject(i, &m_ButtonScore);
        this->BindObject(i, &m_ButtonTime);
        this->BindObject(i, &m_ButtonTrophy);
        this->BindObject(i, &m_ButtonConfig);
        this->BindObject(i, &m_TopUpdating);
        this->BindObject(i, &m_BottomInfo);
        this->BindObject(i, &m_BottomCredit);
        this->BindObject(i, &m_BottomLoginJolt);
        this->BindObject(i, &m_BottomLoginName);
        this->BindObject(i, &m_ScoreMenu);
    }
    this->BindObject(7u, &m_QuestionBlack);
    this->BindObject(7u, &m_Question);
    this->BindObject(7u, &m_Yes);
    this->BindObject(7u, &m_No);
    this->BindObject(8u, &m_LoginBlack);
    this->BindObject(8u, &m_LoginMenu);

    for(coreUintW i = 11u; i <= 13u; ++i)
    {
        this->BindObject(i, &m_Black);
        this->BindObject(i, &m_BackgroundLeft);
        this->BindObject(i, &m_BackgroundRight);
        this->BindObject(i, &m_Pause);
        this->BindObject(i, &m_Resume);
        this->BindObject(i, &m_Abort);
        this->BindObject(i, &m_Short);
        this->BindObject(i, &m_ButtonTrophy);
        this->BindObject(i, &m_ButtonConfig);
        this->BindObject(i, &m_TopUpdating);
        this->BindObject(i, &m_BottomLoginJolt);
        this->BindObject(i, &m_BottomLoginName);
        this->BindObject(i, &m_ScoreMenu);
    }
    this->BindObject(12u, &m_QuestionBlack);
    this->BindObject(12u, &m_Question);
    this->BindObject(12u, &m_Yes);
    this->BindObject(12u, &m_No);
    this->BindObject(13u, &m_LoginBlack);
    this->BindObject(13u, &m_LoginMenu);

    for(coreUintW i = 14u; i <= 16u; ++i)
    {
        this->BindObject(i, &m_Black);
        this->BindObject(i, &m_BackgroundLeft);
        this->BindObject(i, &m_BackgroundRight);
        for(coreUintW j = 0u; j < SCORE_TABLES; ++j)
        {
            this->BindObject(i, &m_aAfterBest     [j]);
            this->BindObject(i, &m_aAfterBestValue[j]);
            this->BindObject(i, &m_aAfterRecord   [j]);
        }
        this->BindObject(i, &m_Submit);
        this->BindObject(i, &m_Finish);
        this->BindObject(i, &m_Short);
        this->BindObject(i, &m_ButtonScore);
        this->BindObject(i, &m_ButtonTime);
        this->BindObject(i, &m_ButtonTrophy);
        this->BindObject(i, &m_ButtonConfig);
        this->BindObject(i, &m_TopUpdating);
        this->BindObject(i, &m_BottomLoginJolt);
        this->BindObject(i, &m_BottomLoginName);
        this->BindObject(i, &m_Successful);
        this->BindObject(i, &m_ScoreMenu);
    }
    this->BindObject(15u, &m_QuestionBlack);
    this->BindObject(15u, &m_Question);
    this->BindObject(15u, &m_Yes);
    this->BindObject(15u, &m_No);
    this->BindObject(16u, &m_LoginBlack);
    this->BindObject(16u, &m_LoginMenu);

    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
    {
        m_ScoreMenu.BindObject(i, &m_aScoreTable[i]);

        for(coreUintW j = 0u; j < SCORE_ENTRIES; ++j)
            for(coreUintW k = 0u; k < 3u; ++k)
                m_ScoreMenu.BindObject(i, &m_aaScoreEntry[i][j][k]);

        m_ScoreMenu.BindObject(i, &m_aScoreBest     [i]);
        m_ScoreMenu.BindObject(i, &m_aScoreBestValue[i]);
        m_ScoreMenu.BindObject(i, &m_aScoreRecord   [i]);
        m_ScoreMenu.BindObject(i, &m_aScoreRank     [i]);

        m_ScoreMenu.BindObject(i, &m_Loading);
        m_ScoreMenu.BindObject(i, &m_PageChange);
    }

    m_ScoreMenu.BindObject(4u, &m_TrophyText);
    for(coreUintW i = 0u; i < TROPHY_ITEMS; ++i)
        m_ScoreMenu.BindObject(4u, &m_aTrophyImage[i]);
    for(coreUintW i = 0u; i < TROPHY_ITEMS; ++i)
        m_ScoreMenu.BindObject(4u, &m_aTrophyCheck[i]);
    for(coreUintW i = 0u; i < TROPHY_SECRETS; ++i)
        m_ScoreMenu.BindObject(4u, &m_aTrophySecret[i]);
    m_ScoreMenu.BindObject(4u, &m_TrophyName);
    m_ScoreMenu.BindObject(4u, &m_aTrophyDesc[0]);
    m_ScoreMenu.BindObject(4u, &m_aTrophyDesc[1]);
    m_ScoreMenu.BindObject(4u, &m_Loading);

    m_ScoreMenu.BindObject(5u, &m_VideoText);
    m_ScoreMenu.BindObject(5u, &m_VideoLow);
    m_ScoreMenu.BindObject(5u, &m_VideoMedium);
    m_ScoreMenu.BindObject(5u, &m_VideoHigh);
    m_ScoreMenu.BindObject(5u, &m_AudioText);
    m_ScoreMenu.BindObject(5u, &m_AudioBarBack);
    m_ScoreMenu.BindObject(5u, &m_AudioBar);
    m_ScoreMenu.BindObject(5u, &m_AudioIconHigh);
    m_ScoreMenu.BindObject(5u, &m_AudioIconLow);
    m_ScoreMenu.BindObject(5u, &m_AudioDrag);
#if defined(_DR_API_GAMEJOLT_)
    m_ScoreMenu.BindObject(5u, &m_LoginConfigLogo);
    m_ScoreMenu.BindObject(5u, &m_LoginConfigStart);
    m_ScoreMenu.BindObject(5u, &m_LoginConfigOr);
#elif defined(_CORE_MOBILE_) || defined(_DR_EMULATE_MOBILE_)
    m_ScoreMenu.BindObject(5u, &m_ControlText);
    m_ScoreMenu.BindObject(5u, &m_ControlType);
#endif
    m_ScoreMenu.BindObject(5u, &m_Loading);

#if defined(_DR_API_GAMEJOLT_)

    m_LoginMenu.BindObject(0u, &m_LoginPopup);
    m_LoginMenu.BindObject(0u, &m_aLoginText[0]);
    m_LoginMenu.BindObject(0u, &m_LoginEnterName);
    m_LoginMenu.BindObject(0u, &m_LoginEnterToken);
    m_LoginMenu.BindObject(0u, &m_LoginName);
    m_LoginMenu.BindObject(0u, &m_LoginToken);
    m_LoginMenu.BindObject(0u, &m_LoginOK);
    m_LoginMenu.BindObject(0u, &m_LoginCancel);
    m_LoginMenu.BindObject(0u, &m_LoginError);

    m_LoginMenu.BindObject(1u, &m_LoginPopup);
    m_LoginMenu.BindObject(1u, &m_aLoginText[1]);
    m_LoginMenu.BindObject(1u, &m_LoginEnterGuest);
    m_LoginMenu.BindObject(1u, &m_LoginGuest);
    m_LoginMenu.BindObject(1u, &m_LoginOK);
    m_LoginMenu.BindObject(1u, &m_LoginCancel);
    m_LoginMenu.BindObject(1u, &m_LoginJoltOr);
    m_LoginMenu.BindObject(1u, &m_LoginJoltLogo);
    m_LoginMenu.BindObject(1u, &m_LoginJoltStart);

#endif
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = 0;

#if !defined(_CORE_MOBILE_)

    // control mouse with joystick
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
        Core::Input->UseMouseWithJoystick(i, 0u, 1u, 0.4f);

#endif

    // check for back button (especially important on mobile)
    const coreBool bBackButton = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(AC_BACK), CORE_INPUT_PRESS);

    // control the menu
    if((this->GetCurSurface() == 10u) && !this->GetTransition().GetStatus())
    {
#if defined(_CORE_MOBILE_)
        g_pGame->GetInterface()->InteractPause();
        const coreBool bPauseButton = g_pGame->GetInterface()->GetTouchPause()->IsClicked();
#else
        const coreBool bPauseButton = Core::Input->GetJoystickButton(0u, 1u, CORE_INPUT_PRESS) || Core::Input->GetJoystickButton(1u, 1u, CORE_INPUT_PRESS);
#endif

        // check for escape key, interrupt and pause button
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) || Core::System->GetWinFocusLost() || bBackButton || bPauseButton)
        {
            // enter pause menu
            g_bPause = true;
            g_pMusicPlayer->Control()->SetVolume(0.5f);
            this->ChangeSurface(11u, 0.0f);
            m_ScoreMenu.ChangeSurface(4, 0.0f);
            Core::Input->ShowCursor(true);
        }
        else
        {
            if(STATIC_ISVALID(g_pGame))
            {
                // check for game over
                if(g_pGame->GetStatus() == 1)
                {
                    if(g_pGame->GetChallenge() || (g_pGame->GetTime() < 10.0f)) this->End();
                    else
                    {
                        g_pMusicPlayer->Control()->SetVolume(0.5f);

                        // reduce speed
                        g_fTargetSpeed = GAME_SPEED_SLOW;

                        // enter submit menu
                        this->ChangeSurface(14u, 1.0f);
                        m_ScoreMenu.ChangeSurface(0u, 0.0f);
                        Core::Input->ShowCursor(true);

                        // catch current score and time value
                        if(g_pGame->GetTime() > 10.0f)
                        {
                            m_afSubmitValue[0] = coreFloat(g_pGame->GetScore());
                            m_afSubmitValue[1] = g_pGame->GetTime();
                        }
                        else
                        {
                            m_afSubmitValue[0] = 0.0f;
                            m_afSubmitValue[1] = 0.0f;
                        }

                        // convert values
                        const coreInt32 aiValue[2] = {F_TO_SI(m_afSubmitValue[0]),
                                                      F_TO_SI(m_afSubmitValue[1]*100.0f)};

                        // check and save new best values, show new offline record
                        if(Core::Config->GetInt("Game", "Score", 0) < aiValue[0])
                        {
                            if(!g_pOnline->IsUserConnected()) {this->NewRecord(0); SHOW_BEST_SCORE(aiValue[0])}
                            Core::Config->SetInt("Game", "Score", aiValue[0]);
                        }

                        if(Core::Config->GetInt("Game", "Time", 0) < aiValue[1])
                        {
                            if(!g_pOnline->IsUserConnected()) {this->NewRecord(1); SHOW_BEST_TIME(aiValue[1])}
                            Core::Config->SetInt("Game", "Time", aiValue[1]);
                        }

                        // show new online record
                        if(g_pOnline->IsUserConnected())
                        {
                            if(          std::atoi(m_aScoreBestValue[0].GetText())  < aiValue[0])         this->NewRecord(0u);
                            if(coreFloat(std::atof(m_aScoreBestValue[1].GetText())) < m_afSubmitValue[1]) this->NewRecord(1u);
                        }

                        // display both values
                        m_aAfterBestValue[0].SetText(PRINT("%06.0f",      FLOOR(m_afSubmitValue[0])));
                        m_aAfterBestValue[1].SetText(PRINT("%03.0f.%01d", FLOOR(m_afSubmitValue[1]), F_TO_SI(m_afSubmitValue[1] * 10.0f) % 10));

                        // play special sound-effect for new records
                        if(m_aScoreRecord[0].GetText()[0] || m_aScoreRecord[1].GetText()[0])
                        {
                            const coreDouble dCurTime = Core::System->GetTotalTime() + 0.9;
                            Core::Manager::Resource->AttachFunction([this, dCurTime]()
                            {
                                if(Core::System->GetTotalTime() > dCurTime)
                                {
                                    this->PlayRecordSound();
                                    return CORE_OK;
                                }
                                return CORE_BUSY;
                            });
                        }

                        // set submit status
                        m_bSubmitted     = (m_afSubmitValue[1] < 10.0f) ? true : false;
                        m_bInLeaderboard = false;
                        if(!m_bSubmitted)
                        {
                            // already submit when connected
                            if(g_pOnline->IsUserConnected()) this->SubmitScore(NULL);
                                                        else this->RetrieveScores();
                        }
                    }
                }
            }

            // calculate only important stuff in game
            return;
        }
    }

    else if(this->GetCurSurface() == 6u)
    {
        // check for main menu
        if(m_Start.IsClicked() || m_Short.IsClicked())
        {
            // start new game
            m_iStatus = 1;
            this->ChangeSurface(10u, 1.0f);
            Core::Input->ShowCursor(false);
        }
        else if(m_Exit.IsClicked() || bBackButton)
        {
            // ask to exit the application
            this->ChangeSurface(7u, 5.0f);
            this->PlayClickSound();
            m_Question.SetText(QUESTION_EXIT);
        }
    }

    else if(this->GetCurSurface() == 7u)
    {
        // check for exit question
        m_Exit.SetFocused(true);
        if(m_Yes.IsClicked())
        {
            // exit the application
            Core::System->Quit();
        }
        else if(m_No.IsClicked()                                                         ||
                Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(0u, 1u,                 CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(1u, 1u,                 CORE_INPUT_PRESS))
        {
            // return to main menu
            this->ChangeSurface(6u, 5.0f);
            this->PlayClickSound();
        }
    }

    else if(this->GetCurSurface() == 11u)
    {
        // check for pause menu
        if(m_Resume.IsClicked() || m_Short.IsClicked()                              ||
           Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(0u, 1u,                 CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(1u, 1u,                 CORE_INPUT_PRESS))
        {
            // resume current game
            g_bPause = false;
            g_pMusicPlayer->Control()->SetVolume(1.0f);
            this->ChangeSurface(10u, 0.0f);
            Core::Input->ClearMouseButton(1u);
            Core::Input->ShowCursor(false);
        }
        else if(m_Abort.IsClicked() || bBackButton)
        {
            // ask to abort current game
            this->ChangeSurface(12u, 5.0f);
            this->PlayClickSound();
            m_Question.SetText(QUESTION_ABORT);
        }
    }

    else if(this->GetCurSurface() == 12u)
    {
        // check for abort question
        m_Abort.SetFocused(true);
        if(m_Yes.IsClicked())
        {
            // exit the current game
            this->End();
        }
        else if(m_No.IsClicked()                                                         ||
                Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(0u, 1u,                 CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(1u, 1u,                 CORE_INPUT_PRESS))
        {
            // return to pause menu
            this->ChangeSurface(11u, 5.0f);
            this->PlayClickSound();
        }
    }

    else if(this->GetCurSurface() == 14u)
    {
        // check for submit menu
        if(m_Submit.IsClicked())
        {
            if(!m_bSubmitted)
            {
                if(g_pOnline->IsUserConnected()) this->SubmitScore(NULL); // re-send (should only be able on connection problems)
                else
                {
#if defined(_DR_API_GAMEJOLT_)

                    // open login window
                    this->ChangeSurface(this->GetCurSurface() + 2u, 5.0f);
                    m_LoginMenu  .ChangeSurface(1u, 0.0f);
                    m_LoginOK    .SetFocused(false);
                    m_LoginCancel.SetFocused(false);
                    m_LoginError .SetText(LOGIN_START);
                    if(!m_LoginGuest.GetText()[0]) m_LoginGuest.SetInput(true);

#endif
                }
                this->PlayClickSound();
            }
        }
        else if(m_Finish.IsClicked() || m_Short.IsClicked() || bBackButton)
        {
            if(m_bInLeaderboard)
            {
                // ask to finish the game without submitting
                this->ChangeSurface(15u, 5.0f);
                this->PlayClickSound();
                m_Question.SetText(QUESTION_RETURN);
            }
            else
            {
                // finish the game
                this->End();
            }
        }
    }

    else if(this->GetCurSurface() == 15u)
    {
        // check for finish question
        m_Finish.SetFocused(true);
        if(m_Yes.IsClicked())
        {
            // finish the game
            this->End();
        }
        else if(m_No.IsClicked()                                                         ||
                Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(0u, 1u,                 CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(1u, 1u,                 CORE_INPUT_PRESS))
        {
            // return to submit menu
            this->ChangeSurface(14u, 5.0f);
            this->PlayClickSound();
        }
    }

    // animate game logos (and record signs)
    const coreVector2 vLogoSize = coreVector2(0.512f,0.256f) * g_pBackground->GetFlash(0.3f);
    const coreFloat   fTextSize = 1.953f * vLogoSize.x;
    m_BigLogo.SetSize(vLogoSize * 1.40f);
    m_Logo   .SetSize(vLogoSize * 0.87f);
    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
    {
        m_aScoreRecord[i].SetScale(fTextSize);
        m_aAfterRecord[i].SetScale(fTextSize);
    }

    // show either current user or credits
    if(g_pOnline->IsUserConnected())
    {
        m_BottomCredit.SetAlpha(0.0f);
    }
    else
    {
        m_BottomLoginName.SetAlpha(0.0f);
        m_BottomLoginJolt.SetAlpha(0.0f);
    }

    // set transparency of menu backgrounds
    m_BackgroundLeft .SetAlpha(m_BackgroundLeft .GetAlpha() * MENU_ALPHA_ACTIVE_1);
    m_BackgroundRight.SetAlpha(m_BackgroundRight.GetAlpha() * MENU_ALPHA_ACTIVE_1);
    if(this->GetCurSurface() >= 10u && this->GetCurSurface() <= 16u) m_Black        .SetAlpha(m_Black        .GetAlpha() * 0.5f);
    if(this->GetCurSurface() >= 6u  && this->GetCurSurface() <= 15u) m_QuestionBlack.SetAlpha(m_QuestionBlack.GetAlpha() * 0.9f);
    if(this->GetCurSurface() >= 6u  && this->GetCurSurface() <= 16u)
    {
        m_LoginPopup.SetAlpha(m_LoginPopup.GetAlpha() * MENU_ALPHA_ACTIVE_1);
        m_LoginBlack.SetAlpha(m_LoginBlack.GetAlpha() * 0.9f);
    }

    // set transparency of main menu buttons
    ALPHA_BUTTON_TEXT  (m_Start)
    ALPHA_BUTTON_TEXT  (m_Exit)
    ALPHA_BUTTON_TEXT  (m_Resume)
    ALPHA_BUTTON_TEXT  (m_Abort)
    ALPHA_BUTTON_TEXT  (m_Finish)
    ALPHA_BUTTON_SCORE (m_ButtonScore,  0u)
    ALPHA_BUTTON_SCORE (m_ButtonTime,   1u)
    ALPHA_BUTTON_SCORE (m_ButtonTrophy, 4u)
    ALPHA_BUTTON_SCORE (m_ButtonConfig, 5u)
    ALPHA_BUTTON_INSIDE(m_PageChange)
    ALPHA_BUTTON_INSIDE(m_LoginConfigStart)
    ALPHA_BUTTON_INSIDE(m_Short)
    ALPHA_BUTTON_INSIDE(m_Yes)
    ALPHA_BUTTON_INSIDE(m_No)

#if !defined(_DR_API_GAMEJOLT_) && (defined(_CORE_MOBILE_) || defined(_DR_EMULATE_MOBILE_))

    // set transparency of control buttons
    m_ControlType.GetArrow(0u)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 0u) ? 0.5f : 1.0f));
    m_ControlType.GetArrow(1u)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 2u) ? 0.5f : 1.0f));
    m_ControlType.GetCaption()->SetAlpha(m_ControlType.GetAlpha());
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(0u));
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(1u));

#endif

    // set transparency of highscore related objects
    if(m_bSubmitted) m_Submit.SetAlpha(m_Submit.GetAlpha() * MENU_ALPHA_ACTIVE_2 * 0.5f);
    else ALPHA_BUTTON_TEXT(m_Submit)

    if(!m_bSubmitted || (m_afSubmitValue[1] < 10.0f))
        m_Successful.SetAlpha(0.0f);

    if((this->GetCurSurface() >= 14u) && (this->GetCurSurface() <= 16u) && m_bInLeaderboard)
        m_Short.SetAlpha(MENU_ALPHA_ACTIVE_1 * 0.5f);

    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
        if(m_aScoreRecord[i].GetText()[0]) m_aScoreRank[i].SetAlpha(0.0f);

    // toggle score menu
         if(m_ButtonScore .IsClicked()) {m_ScoreMenu.ChangeSurface(0u, 5.0f); this->PlayClickSound();}
    else if(m_ButtonTime  .IsClicked()) {m_ScoreMenu.ChangeSurface(1u, 5.0f); this->PlayClickSound();}
    else if(m_ButtonTrophy.IsClicked()) {m_ScoreMenu.ChangeSurface(4u, 5.0f); this->PlayClickSound();}
    else if(m_ButtonConfig.IsClicked()) {m_ScoreMenu.ChangeSurface(5u, 5.0f); this->PlayClickSound();}

    // control score menu (configuration)
    const coreBool bInNormalMenu = (this->GetCurSurface() == 6u) || (this->GetCurSurface() == 11u) || (this->GetCurSurface() == 14u);

    if((m_ScoreMenu.GetCurSurface() == 4u) || ((m_ScoreMenu.GetOldSurface() == 4u) && m_ScoreMenu.GetTransition().GetStatus()))
    {
        for(coreUintW i = 0u; i < TROPHY_ITEMS; ++i)
        {
            // update trophy description
            if(((m_aTrophyImage[i].IsFocused() && m_iTrophyCurrent != coreInt8(i)) || m_iTrophyCurrent < 0) && bInNormalMenu)
            {
                m_iTrophyCurrent = i;
                g_pOnline->GameJolt()->InterTrophy()->FetchTrophiesCall(GJ_TROPHY_ALL, this, &cMenu::FetchTrophiesCallback1, I_TO_P(i));
            }

            // set transparency of trophies
            m_aTrophyImage[i].SetAlpha(m_aTrophyImage[i].GetAlpha() * ((m_iTrophyCurrent == coreInt8(i)) ? 0.5f : 1.0f));
            m_aTrophyCheck[i].SetAlpha(m_aTrophyCheck[i].GetAlpha() * ((m_iTrophyStatus & BIT(i))        ? 1.0f : 0.0f));
        }

        // set question marks
        for(coreUintW i = 0u; i < TROPHY_SECRETS; ++i)
            if(m_iTrophyStatus & BIT(i)) m_aTrophySecret[i].SetAlpha(0.0f);
    }
    else if(((m_ScoreMenu.GetCurSurface() == 5u) || m_LoginConfigStart.IsClicked()) && bInNormalMenu)
    {
        // set transparency of configuration buttons
        ALPHA_BUTTON_TEXT(m_VideoLow)
        ALPHA_BUTTON_TEXT(m_VideoMedium)
        ALPHA_BUTTON_TEXT(m_VideoHigh)
        ALPHA_BUTTON_TEXT(m_AudioDrag)

        // toggle video quality
        coreInt8 iNewQuality = -1;
             if(m_VideoLow   .IsClicked()) {iNewQuality = 0; this->PlayClickSound();}
        else if(m_VideoMedium.IsClicked()) {iNewQuality = 1; this->PlayClickSound();}
        else if(m_VideoHigh  .IsClicked()) {iNewQuality = 2; this->PlayClickSound();}

        if(iNewQuality >= 0)
        {
            if(iNewQuality != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY))
            {
                // display current selection
                m_VideoLow   .SetColor3((iNewQuality == 0) ? COLOR_GREEN_F  : coreVector3(0.5f,0.5f,0.5f));
                m_VideoMedium.SetColor3((iNewQuality == 1) ? COLOR_YELLOW_F : coreVector3(0.5f,0.5f,0.5f));
                m_VideoHigh  .SetColor3((iNewQuality == 2) ? COLOR_RED_F    : coreVector3(0.5f,0.5f,0.5f));

                // set new video quality and reload all shaders
                Core::Config->SetInt(CORE_CONFIG_GRAPHICS_QUALITY, iNewQuality);
                this->ResetShaders();
            }
        }

        // drag audio volume (check for every sub-object from the volume bar)
        static coreBool bDragStatus = false; // # static
        if(m_AudioBar    .IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD) ||
           m_AudioBarBack.IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD) ||
           m_AudioDrag   .IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
        {
            if(!bDragStatus) this->PlayClickSound();
            bDragStatus = true;
        }
        if(Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_RELEASE))
            bDragStatus = false;

        if(bDragStatus)
        {
#if defined(_CORE_MOBILE_)

            // control volume with last finger position
            coreFloat fMove = 0.0f;
            Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&](const coreUintW i)
            {
                fMove = Core::Input->GetTouchPosition(i).x; // overwrite
            });

#else
            // control volume with mouse position
            const coreFloat fMove = Core::Input->GetMousePosition().x;

#endif
            // calculate range values of the volume bar
            const coreFloat fMaxSide  = (m_AudioBarBack.GetSize().x - m_AudioDrag.GetSize().x) * 0.5f;
            const coreFloat fMaxLeft  =  m_AudioBarBack.GetPosition().x - fMaxSide;
            const coreFloat fMaxRight =  m_AudioBarBack.GetPosition().x + fMaxSide;

            // calculate new controller position and volume
            const coreFloat fNewPosX = CLAMP((fMove + 0.5f) * Core::System->GetResolution().AspectRatio(), fMaxLeft, fMaxRight);
            const coreFloat fVolume  = (fNewPosX - fMaxLeft) * RCP(fMaxSide * 2.0f);

            // set controller
            m_AudioDrag.SetPosition(coreVector2(fNewPosX, m_AudioDrag.GetPosition().y));
            m_AudioDrag.Move();

            // set colored bar
            m_AudioBar.SetSize    (coreVector2(fNewPosX - fMaxLeft, m_AudioBar.GetSize().y));
            m_AudioBar.SetPosition(coreVector2(LEFT_CENTER - (m_AudioBarBack.GetSize().x - m_AudioBar.GetSize().x) * 0.5f, m_AudioBar.GetPosition().y));
            m_AudioBar.Move();

            // set volume
            Core::Config->SetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME, fVolume * 10.0f);
            Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME, fVolume * 0.7f);
            g_pMusicPlayer->Control()->SetVolume((g_bPause || (STATIC_ISVALID(g_pGame) ? g_pGame->GetStatus() : false)) ? 0.5f : 1.0f);
        }

        if(m_LoginConfigStart.IsClicked())
        {
            if(g_pOnline->IsUserConnected())
            {
                // logout
                this->Logout();
            }
            else
            {
#if defined(_DR_API_GAMEJOLT_)

                // open login window
                this->ChangeSurface(this->GetCurSurface() + 2u, 5.0f);
                m_LoginMenu  .ChangeSurface(0u, 0.0f);
                m_LoginOK    .SetFocused(false);
                m_LoginCancel.SetFocused(false);
                m_LoginError .SetText(LOGIN_START);
                     if(!m_LoginName.GetText ()[0]) m_LoginName.SetInput(true);
                else if(!m_LoginToken.GetText()[0]) m_LoginToken.SetInput(true);

#endif
            }
            this->PlayClickSound();
        }

#if !defined(_DR_API_GAMEJOLT_) && (defined(_CORE_MOBILE_) || defined(_DR_EMULATE_MOBILE_))

        // apply and save control changes
        if(m_ControlType.IsClicked())
        {
            if(coreUintW(Core::Config->GetInt("Game", "Control", 0)) != m_ControlType.GetCurIndex())
            {
                Core::Config->SetInt("Game", "Control", m_ControlType.GetCurIndex());

                // also change on running game
                if(STATIC_ISVALID(g_pGame)) g_pGame->GetInterface()->ChangeControlType(m_ControlType.GetCurIndex());
            }
            this->PlayClickSound();
        }

#endif
    }
    else if(m_ScoreMenu.GetCurSurface() < 2u)
    {
        // change score-page
        if(m_PageChange.IsClicked() && m_PageChange.GetAlpha() && bInNormalMenu)
        {
            if(++m_iCurPage >= SCORE_PAGES) m_iCurPage = 0u;
            this->RetrieveScoresCallback3(0u);
            this->RetrieveScoresCallback3(1u);
            this->PlayClickSound();
        }
    }

    // control login menu
    if((this->GetCurSurface() == 8u) || (this->GetCurSurface() == 13u) || (this->GetCurSurface() == 16u))
    {
        // set transparency of login buttons
        ALPHA_BUTTON_INSIDE(m_LoginOK)
        ALPHA_BUTTON_INSIDE(m_LoginCancel)
        ALPHA_BUTTON_INSIDE(m_LoginJoltStart)

        // set color of text boxes
        m_LoginName .SetColor3(m_LoginName .IsFocused() ? coreVector3(0.5f,0.5f,0.5f) : coreVector3(0.25f,0.25f,0.25f));
        m_LoginToken.SetColor3(m_LoginToken.IsFocused() ? coreVector3(0.5f,0.5f,0.5f) : coreVector3(0.25f,0.25f,0.25f));
        m_LoginGuest.SetColor3(m_LoginGuest.IsFocused() ? coreVector3(0.5f,0.5f,0.5f) : coreVector3(0.25f,0.25f,0.25f));

        // switch from guest login to normal login
        if(m_LoginJoltStart.IsClicked())
        {
            m_LoginMenu.ChangeSurface(0u, 5.0f);
                 if(!m_LoginName .GetText()[0]) m_LoginName .SetInput(true);
            else if(!m_LoginToken.GetText()[0]) m_LoginToken.SetInput(true);
            m_bFromGuest = true;
            this->PlayClickSound();
        }

        if(!g_pOnline->GetNumConnections()) // prevent multiple login attempts, or window-close during login attempt
        {
            // start and cancel login
            coreBool bDoLogin = m_LoginOK.IsClicked();
            if(m_LoginCancel.IsClicked() || bBackButton)
            {
                // switch back to guest login or cancel
                if(m_bFromGuest) m_LoginMenu.ChangeSurface(1u, 5.0f);
                else this->ChangeSurface(this->GetCurSurface() - 2u, 5.0f);
                m_bFromGuest = false;
                this->PlayClickSound();
            }

            if(m_LoginMenu.GetCurSurface() == 0u)
            {
                if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RETURN), CORE_INPUT_PRESS) || (Core::Input->GetKeyboardChar() == CORE_INPUT_CHAR(RETURN)))
                {
                    // switch input focus or start login with enter-key
                         if(!m_LoginName .GetText()[0]) m_LoginName .SetInput(true);
                    else if(!m_LoginToken.GetText()[0]) m_LoginToken.SetInput(true);
                    else bDoLogin = true;
                }
                else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(TAB), CORE_INPUT_PRESS) || (Core::Input->GetKeyboardChar() == CORE_INPUT_CHAR(TAB)))
                {
                    // switch input focus with tab-key
                         if(m_LoginName .GetInput()) {m_LoginName.SetInput(false); m_LoginToken.SetInput(true);}
                    else if(m_LoginToken.GetInput()) {m_LoginName.SetInput(true);  m_LoginToken.SetInput(false);}
                }
            }

            if(bDoLogin)
            {
                if(m_LoginMenu.GetCurSurface() == 0u)
                {
                    // start login
                    if(this->Login(m_LoginName.GetText(), m_LoginToken.GetText()) == GJ_OK)
                        m_LoginError.SetText(LOGIN_CONNECTING);
                }
                else
                {
                    if(m_LoginGuest.GetText()[0])
                    {
                        // trim possible whitespaces
                        coreString sTrimmed = m_LoginGuest.GetText();
                        sTrimmed.trim();

                        // send guest score
                        this->SubmitScore(sTrimmed.c_str());

                        // close login menu
                        this->ChangeSurface(this->GetCurSurface() - 2u, 5.0f);
                    }
                }
                m_bFromGuest = false;
                this->PlayClickSound();
            }
        }
    }

    // update score tables
    if(m_iTableUpdate)
    {
        for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
        {
            if(m_iTableUpdate & BIT(i))
            {
                REMOVE_BIT(m_iTableUpdate, i)
                this->RetrieveScoresCallback3(i);
            }
        }
    }

#if defined(_CORE_DEBUG_)

    // show full network load
    m_TopUpdating.SetText(PRINT("COMMUNICATING WITH SERVER... (%u)", g_pOnline->GetNumConnections()));

#endif

    // show light network load
    if(g_pOnline->GetNumConnections())
    {
        m_Loading.SetDirection(coreVector2::Direction(coreFloat(Core::System->GetTotalTime()) * 4.0f));
        m_Loading.Move();
    }
    else m_Loading.SetAlpha(0.0f);

    // update intro
    m_Intro.Update(1.0f);
    if(m_Intro.GetStatus())
    {
        // skip intro when pressing specific keys
        if(Core::Input->GetMouseButton   (CORE_INPUT_LEFT,        CORE_INPUT_PRESS) ||
           Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) ||
           Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RETURN), CORE_INPUT_PRESS) ||
           Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE),  CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(0u, 0u,                 CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(1u, 0u,                 CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(0u, 1u,                 CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(1u, 1u,                 CORE_INPUT_PRESS) ||
           bBackButton)
        {
            this->ChangeSurface(6u, 1.0f);
            m_Intro.Stop();
        }

        // change surface over time
             if((this->GetCurSurface() == 0u) && (m_Intro.GetValue(CORE_TIMER_GET_NORMAL) >= 0.0f)) this->ChangeSurface(1u, 1.0f);
        else if((this->GetCurSurface() == 1u) && (m_Intro.GetValue(CORE_TIMER_GET_NORMAL) >= 3.0f)) this->ChangeSurface(2u, 1.0f);
        else if((this->GetCurSurface() == 2u) && (m_Intro.GetValue(CORE_TIMER_GET_NORMAL) >= 5.5f)) this->ChangeSurface(3u, 1.0f);
        else if((this->GetCurSurface() == 3u) && (m_Intro.GetValue(CORE_TIMER_GET_NORMAL) >= 8.0f))
        {
            this->ChangeSurface(6u, 1.0f);
            m_Intro.Stop();
        }
    }
}


// ****************************************************************
// end current game and return to main menu
void cMenu::End()
{
    if(!STATIC_ISVALID(g_pGame)) return;

    // reset pause and menu status
    g_bPause  = false;
    m_iStatus = 2;
    g_pMusicPlayer->Control()->SetVolume(1.0f);
    Core::Input->ShowCursor(true);

    // reset record notification
    this->ResetRecord();

    // flash screen and return to main menu
    this->ChangeSurface(5u, 0.0f);
    this->ChangeSurface(6u, 1.0f);
    m_ScoreMenu.ChangeSurface(0u, 0.0f);
    m_White .SetAlpha(1.0f);
    m_Black .SetAlpha(0.0f);
    m_Finish.SetFocused(false);

    // play sound-effect
    this->PlayFlashSound();

    // play next music file
    g_pMusicPlayer->Next();

    // update leaderboards
    m_bSubmitted = true;
    if(!g_pGame->GetStatus() && (g_pGame->GetTime() >= 10.0f))
        this->RetrieveScores();
}


// ****************************************************************
// reset all relevant shaders after quality changes
void cMenu::ResetShaders()
{
    // reload shader-programs
    Core::Manager::Resource->Get<coreProgram>("fill_program")       ->Reload();
    Core::Manager::Resource->Get<coreProgram>("floor_program")      ->Reload();
    Core::Manager::Resource->Get<coreProgram>("floor_plate_program")->Reload();

    // force immediate resource update
    glFinish();
    Core::Manager::Resource->UpdateResources();
}


// ****************************************************************
// reset fullscreen-objects after resolution changes
void cMenu::ResetResolution()
{
    // reset fullscreen-objects
    m_Black        .FitToScreen();
    m_White        .FitToScreen();
    m_QuestionBlack.FitToScreen();
    m_LoginBlack   .FitToScreen();
}


// ****************************************************************
// activate notification for new record
void cMenu::NewRecord(const coreUintW iIndex)
{
    ASSERT(iIndex < SCORE_TABLES)

    const coreBool bBitTooSmall = Core::System->GetResolution().AspectRatio() < 1.4f;

    // set score color
    m_aScoreBestValue[iIndex].SetColor3(COLOR_YELLOW_F);
    m_aAfterBestValue[iIndex].SetColor3(COLOR_YELLOW_F);

    // set notification text
    m_aScoreRecord[iIndex].SetText("NEW RECORD");
    m_aAfterRecord[iIndex].SetText(bBitTooSmall ? "" : "NEW RECORD");
}


// ****************************************************************
// reset notification for new record
void cMenu::ResetRecord()
{
    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
    {
        // reset score colors
        m_aScoreBestValue[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));
        m_aAfterBestValue[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));

        // reset notification texts
        m_aScoreRecord[i].SetText("");
        m_aAfterRecord[i].SetText("");
    }
}


// ****************************************************************
// submit score
void cMenu::SubmitScore(const coreChar* pcGuestName)
{
    ASSERT(STATIC_ISVALID(g_pGame))
    if(m_bSubmitted) return;

    // convert values
    const coreInt32 aiValue[2] = {F_TO_SI(m_afSubmitValue[0]),
                                  F_TO_SI(m_afSubmitValue[1]*100.0f)};

    // save guest name
    if(pcGuestName) Core::Config->SetString("Game", "Guest", pcGuestName);

    // create extra-data string
    const coreString sExtra = PRINT("%.0f %.3f - %d - %d %d %d %d %d", coreFloat(m_afSubmitValue[0]), coreFloat(m_afSubmitValue[1]), g_pGame->GetMaxCombo(),
                                    g_pGame->GetStat(0u), g_pGame->GetStat(1u), g_pGame->GetStat(2u), g_pGame->GetStat(3u), g_pGame->GetStat(4u));

    // send score and time values
    const coreInt32 iStateA = g_pOnline->SubmitScore(GJ_LEADERBOARD_01, PRINT("%d Points",    aiValue[0]),                           aiValue[0], sExtra, pcGuestName ? pcGuestName : "", this, &cMenu::SubmitScoreCallback, NULL);
    const coreInt32 iStateB = g_pOnline->SubmitScore(GJ_LEADERBOARD_02, PRINT("%.1f Seconds", FLOOR(m_afSubmitValue[1]*10.0f)*0.1f), aiValue[1], sExtra, pcGuestName ? pcGuestName : "", this, &cMenu::SubmitScoreCallback, NULL);

    // still show score as submitted
    if((iStateA == GJ_REQUEST_CANCELED) || (iStateB == GJ_REQUEST_CANCELED))
        m_bSubmitted = true;
}


// ****************************************************************
// callback for score submit
void cMenu::SubmitScoreCallback(const gjScorePtr& pScore, void* pData)
{
    if(!pScore) return;   // error submitting

    // update status
    m_bSubmitted     = true;
    m_bInLeaderboard = false;

    if(STATIC_ISVALID(g_pGame))
    {
        // achieve submit-trophy
        g_pGame->AchieveTrophy(GJ_TROPHY_05, 4u);
    }

    // fetch all top values
    g_pOnline->FetchScores(pScore->GetScoreTable()->GetID(), false, SCORE_ENTRIES * SCORE_PAGES, this, &cMenu::RetrieveScoresCallback2, I_TO_P(0));

    // fetch best values of the current user
    if(g_pOnline->IsUserConnected()) g_pOnline->FetchScores(pScore->GetScoreTable()->GetID(), true, 1u, this, &cMenu::RetrieveScoresCallback2, I_TO_P(1));
}


// ****************************************************************
// retrieve scores
void cMenu::RetrieveScores()
{
    // reset current score page
    m_iCurPage = 0u;
    this->RetrieveScoresCallback3(0u);
    this->RetrieveScoresCallback3(1u);

    // fetch leaderboards
    g_pOnline->FetchLeaderboards(this, &cMenu::RetrieveScoresCallback1, NULL);
}


// ****************************************************************
// callback for score retrieval (tables)
void cMenu::RetrieveScoresCallback1(const gjScoreTableMap& apTable, void* pData)
{
    FOR_EACH(it, apTable)
    {
        // fetch all top values
        g_pOnline->FetchScores(it->second->GetID(), false, SCORE_ENTRIES * SCORE_PAGES, this, &cMenu::RetrieveScoresCallback2, I_TO_P(0));

        // fetch best values of the current user
        if(g_pOnline->IsUserConnected()) g_pOnline->FetchScores(it->second->GetID(), true, 1u, this, &cMenu::RetrieveScoresCallback2, I_TO_P(1));
    }
}


// ****************************************************************
// callback for score retrieval (entries)
void cMenu::RetrieveScoresCallback2(const gjScoreList& apScore, void* pData)
{
    if(apScore.empty()) return;

    const coreUintW iTableID  = apScore[0]->GetScoreTable()->GetID();
    const coreUintW iTableNum = (iTableID == GJ_LEADERBOARD_01) ? 0u : 1u;

    if(!pData)
    {
        // copy scores for better paging
        m_aapCurScores[iTableNum] = apScore;

        // update leaderboard
        this->RetrieveScoresCallback3(iTableNum);

        if(STATIC_ISVALID(g_pGame))
        {
            // mark values which would make it into the visual leaderboard
            if(iTableID == GJ_LEADERBOARD_01)
            {
                // check for score
                const coreBool bInLeaderboard = m_afSubmitValue[0] > I_TO_F((apScore.size() >= SCORE_ENTRIES * SCORE_PAGES) ? apScore[(SCORE_ENTRIES * SCORE_PAGES) - 1u]->GetSort() : 0);
                if(!m_bSubmitted) m_bInLeaderboard |= bInLeaderboard;
            }
            else // == GJ_LEADERBOARD_02
            {
                // check for time
                const coreBool bInLeaderboard = FLOOR(m_afSubmitValue[1]*100.0f) > I_TO_F((apScore.size() >= SCORE_ENTRIES * SCORE_PAGES) ? apScore[(SCORE_ENTRIES * SCORE_PAGES) - 1u]->GetSort() : 0);
                if(!m_bSubmitted) m_bInLeaderboard |= bInLeaderboard;
            }
        }
    }
    else
    {
        // fill best values of the current user
        if(iTableID == GJ_LEADERBOARD_01)
            SHOW_BEST_SCORE(apScore[0]->GetSort())
        else // == GJ_LEADERBOARD_02
            SHOW_BEST_TIME (apScore[0]->GetSort())

        // show player rank
        if(g_pOnline->IsUserConnected())
        {
            // TODO 3: implement   
            //coreInt32 iTest = 0;
            //apScore[0]->GetRankNow(&iTest);
            //m_aScoreRank[iTableNum].SetText(coreData::ToChars(iTest));
        }
    }
}


// ****************************************************************
// callback for score retrieval (update leaderboards)
void cMenu::RetrieveScoresCallback3(const coreUintW iTableNum)
{
    ASSERT(iTableNum  < SCORE_TABLES)
    ASSERT(m_iCurPage < SCORE_PAGES)

    // set table and page
    const gjScoreList& apScore     = m_aapCurScores[iTableNum];
    const coreUintW    iScoreStart = m_iCurPage * 8u;

    // loop trough all retrieved leaderboard entries
    for(coreUintW i = 0u; i < SCORE_ENTRIES; ++i)
    {
        const gjScore* pScore = (i + iScoreStart < apScore.size()) ? apScore[i + iScoreStart] : NULL;
        const coreBool bOver  = pScore ? (pScore->GetUserName().length() > NAME_LEN) : false;

        // trim possible whitespaces
        coreString sTrimmed = pScore ? (pScore->GetUserName().substr(0u, NAME_LEN) + (bOver ? ">" : "")).c_str() : "-";
        sTrimmed.trim();

        if(iTableNum == 0u) // == GJ_LEADERBOARD_01
        {
            // fill score leaderboard
            m_aaScoreEntry[0][i][0].SetText(PRINT("%zu.", i+1u + iScoreStart));
            m_aaScoreEntry[0][i][1].SetText(sTrimmed.c_str());
            m_aaScoreEntry[0][i][2].SetText(pScore ? PRINT("%06d", pScore->GetSort()) : "-");

            // highlight best players
            m_aaScoreEntry[0][i][1].SetColor3(LERP(COLOR_YELLOW_F, COLOR_WHITE_F, I_TO_F(MIN(i + iScoreStart, 3u) / 3u)));
        }
        else // == GJ_LEADERBOARD_02
        {
            // fill time leaderboard
            m_aaScoreEntry[1][i][0].SetText(PRINT("%zu.", i+1u + iScoreStart));
            m_aaScoreEntry[1][i][1].SetText(sTrimmed.c_str());
            m_aaScoreEntry[1][i][2].SetText(pScore ? PRINT("%03d.%01d", pScore->GetSort() / 100, ((pScore->GetSort() % 100) / 10)) : "-");

            // highlight best players
            m_aaScoreEntry[1][i][1].SetColor3(LERP(COLOR_YELLOW_F, COLOR_WHITE_F, I_TO_F(MIN(i + iScoreStart, 3u) / 3u)));
        }
    }
}


// ****************************************************************
// fetch trophies
void cMenu::FetchTrophies()
{
    // fetch trophies
    g_pOnline->FetchTrophies(this, &cMenu::FetchTrophiesCallback2, NULL);
}


// ****************************************************************
// callback for trophy fetch (description)
void cMenu::FetchTrophiesCallback1(const gjTrophyList& apTrophy, void* pData)
{
    const coreUintW iNum = P_TO_I(pData);
    if(iNum >= apTrophy.size()) return;

    // set trophy title
    m_TrophyName.SetText(apTrophy[iNum]->GetTitle().c_str());

    // split trophy description into two even parts
    const coreChar* pcDesc = PRINT("%s", (m_iTrophyStatus & BIT(iNum) || !apTrophy[iNum]->IsSecret()) ? apTrophy[iNum]->GetDescriptionTrue().c_str() : GJ_API_TEXT_SECRET);
    coreUintW iMid = std::strlen(pcDesc) / 2u;
    while(iMid > 0u && pcDesc[iMid] != ' ') --iMid;

    // set trophy description
    m_aTrophyDesc[0].SetText(pcDesc,  iMid);
    m_aTrophyDesc[1].SetText(pcDesc + iMid);
}


// ****************************************************************
// callback for trophy fetch (values)
void cMenu::FetchTrophiesCallback2(const gjTrophyList& apTrophy, void* pData)
{
    // save trophy status
    m_iTrophyStatus = 0u;
    for(coreUintW i = 0u, ie = MIN(apTrophy.size(), TROPHY_ITEMS); i < ie; ++i)
        if(apTrophy[i]->IsAchieved()) ADD_BIT(m_iTrophyStatus, i)

    // reset current trophy
    this->FetchTrophiesCallback1(apTrophy, I_TO_P(0));
    m_iTrophyCurrent = 0;
}


// ****************************************************************
// invoke quickplay login
coreInt32 cMenu::QuickPlay()
{
    // load guest name and local values
    m_LoginGuest.SetText(Core::Config->GetString("Game", "Guest", ""));
    SHOW_BEST_SCORE     (Core::Config->GetInt   ("Game", "Score",  0))
    SHOW_BEST_TIME      (Core::Config->GetInt   ("Game", "Time",   0))
    m_iTrophyStatus =    Core::Config->GetInt   ("Game", "Trophy", 0);

    // check for quickplay
    coreInt32 iStatus = g_pOnline->Login("", "", this, &cMenu::LoginCallback, I_TO_P(0));

    if(iStatus != GJ_OK)
    {
        // check for saved credentials
        iStatus = g_pOnline->Login(Core::Config->GetString("Game", "Name", ""), Core::Config->GetString("Game", "Token", ""), this, &cMenu::LoginCallback, I_TO_P(0));

        // fetch leaderboards early on failure
        if(iStatus != GJ_OK) g_pMenu->RetrieveScores();
    }

    return iStatus;
}


// ****************************************************************
// invoke normal login
coreInt32 cMenu::Login(const coreChar* pcName, const coreChar* pcToken)
{
    // try to login the specific user
    return g_pOnline->Login(pcName, pcToken, this, &cMenu::LoginCallback, I_TO_P(1));
}


// ****************************************************************
// callback for login attempt
void cMenu::LoginCallback(const coreInt32& iStatus, void* pData)
{
    if(iStatus == GJ_OK)
    {
        // login successful
        m_LoginName .SetText(g_pOnline->GetUserName());
        m_LoginToken.SetText(g_pOnline->GetUserToken());
        m_BottomLoginName.SetText(PRINT("LOGGED IN AS %s", coreData::StrUpper(g_pOnline->GetUserName())));

        // save credentials
        Core::Config->SetString("Game", "Name",  g_pOnline->GetUserName());
        Core::Config->SetString("Game", "Token", g_pOnline->GetUserToken());

        // submit or check scores
        this->ResetRecord();
        if(!m_bSubmitted) this->SubmitScore(NULL);
                     else this->RetrieveScores();

        // fetch trophies
        this->FetchTrophies();

        // change login button
        m_LoginConfigOr.SetText("LOG OUT OF");
        m_LoginConfigStart.Construct("button_logout.png", "button_logout.png");

        // close login menu
        if(pData) this->ChangeSurface(this->GetCurSurface() - 2u, 5.0f);
    }
    else
    {
        // login failed
             if(iStatus == GJ_NETWORK_ERROR)  m_LoginError.SetText(LOGIN_ERROR_CONNECTION);
        else if(iStatus == GJ_REQUEST_FAILED) m_LoginError.SetText(LOGIN_ERROR_CREDENTIALS);
    }
}


// ****************************************************************
// logout
void cMenu::Logout()
{
    // logout
    g_pOnline->Logout();

    // reset credentials
    Core::Config->SetString("Game", "Name",  "");
    Core::Config->SetString("Game", "Token", "");

    // load local values
    SHOW_BEST_SCORE  (Core::Config->GetInt("Game", "Score",  0))
    SHOW_BEST_TIME   (Core::Config->GetInt("Game", "Time",   0))
    m_iTrophyStatus = Core::Config->GetInt("Game", "Trophy", 0);
    this->ResetRecord();

    // change login button
    m_LoginConfigOr.SetText("LOG INTO");
    m_LoginConfigStart.Construct("button_login.png", "button_login.png");

    // reset player rank
    for(coreUintW i = 0u; i < SCORE_TABLES; ++i)
        m_aScoreRank[i].SetText("");
}