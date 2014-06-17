/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

// static texts
#define LOGIN_START             "YOUR TOKEN IS NOT YOUR PASSWORD"
#define LOGIN_CONNECTING        "CONNECTING..."
#define LOGIN_ERROR_CONNECTION  "COULD NOT CONNECT TO GAME JOLT"
#define LOGIN_ERROR_CREDENTIALS "WRONG USER NAME OR TOKEN"

#define QUESTION_EXIT   "DO YOU REALLY WANT TO EXIT?"
#define QUESTION_ABORT  "DO YOU REALLY WANT TO ABORT THE CURRENT GAME?"
#define QUESTION_RETURN "DO YOU REALLY WANT TO RETURN WITHOUT SUBMITTING?"

// update displayed score and time value
#define SHOW_BEST_SCORE(x) {m_aScoreBestValue[0].SetText(coreData::Print("%06d", (x)));}
#define SHOW_BEST_TIME(x)  {m_aScoreBestValue[1].SetText(coreData::Print("%03d.%01d", (x) / 100, (((x) % 100) / 10)));}


// ****************************************************************
cMenu::cMenu()noexcept
: coreMenu         (20, 0)
, m_ScoreMenu      (8,  0)
, m_LoginMenu      (4,  0)
, m_Intro          (coreTimer(10.0f, 1.0f, 1))
, m_bSubmited      (true)
, m_bInLeaderboard (false)
, m_iCurPage       (0)
, m_iTableUpdate   (0)
, m_iTrophyStatus  (0)
, m_iTrophyCurrent (-1)
, m_bFromGuest     (false)
{
    SDL_assert(sizeof(m_iTrophyStatus)*8 >= TROPHIES);
    
    // create intro objects
    m_Made.Construct(FONT_ROCKS, 29, 0);
    m_Made.SetPosition(coreVector2(0.0f,0.176f));
    m_Made.SetText("MADE BY");

    m_For.Construct(FONT_ROCKS, 29, 0);
    m_For.SetPosition(coreVector2(0.0f,-0.06667f));
    m_For.SetText("FOR");

    m_Maus.DefineTextureFile(0, "data/textures/maus_logo.png");
    m_Maus.DefineProgramShare("2d_shader")
        ->AttachShaderFile("data/shaders/default_2d_simple.vs")
        ->AttachShaderFile("data/shaders/default.fs")
        ->Finish();
    m_Maus.SetPosition(coreVector2(0.0f,0.13333f));
    m_Maus.SetSize(coreVector2(0.512f,0.256f) * 0.93333f);

    m_GameJolt.DefineTextureFile(0, "data/textures/gamejolt_logo.png");
    m_GameJolt.DefineProgramShare("2d_shader");
    m_GameJolt.SetPosition(coreVector2(0.0f,-0.13333f));
    m_GameJolt.SetSize(coreVector2(0.512f,0.064f) * 1.33333f);

    m_BigLogo.DefineTextureFile(0, "data/textures/game_logo.png");
    m_BigLogo.DefineProgramShare("2d_shader");
    m_BigLogo.SetPosition(coreVector2(0.0f,0.05f));

    // create background objects
    m_Black.DefineProgramShare("2d_shader_color")
        ->AttachShaderFile("data/shaders/default_2d_simple.vs")
        ->AttachShaderFile("data/shaders/color.fs")
        ->Finish();
    m_Black.SetColor3(coreVector3(0.0f,0.0f,0.0f));
    m_Black.FitToScreen();

    m_White.DefineProgramShare("2d_shader_color");
    m_White.FitToScreen();
    m_White.Move();

    m_BackgroundLeft.DefineProgramShare("2d_shader_border")
        ->AttachShaderFile("data/shaders/default_2d_simple.vs")
        ->AttachShaderFile("data/shaders/border.fs")
        ->Finish();
    m_BackgroundLeft.SetPosition(coreVector2(0.15f,0.0f));
    m_BackgroundLeft.SetSize(coreVector2(0.62f,0.62f));
    m_BackgroundLeft.SetColor3(coreVector3(0.05f,0.05f,0.05f));
    ADJUST_LEFT(m_BackgroundLeft)
    ADJUST_BORDER(m_BackgroundLeft)

    m_BackgroundRight.DefineProgramShare("2d_shader_border");
    m_BackgroundRight.SetPosition(coreVector2(-0.03f,0.0f));
    m_BackgroundRight.SetSize(coreVector2(MIN(Core::System->GetResolution().AspectRatio() - 0.62f - 0.15f - 0.06f, 0.62f + 0.12f), 0.62f));
    m_BackgroundRight.SetColor3(coreVector3(0.05f,0.05f,0.05f));
    ADJUST_RIGHT(m_BackgroundRight)
    ADJUST_BORDER(m_BackgroundRight)

    const coreVector2 vRightCenter = m_BackgroundRight.GetPosition() - 0.5f*m_BackgroundRight.GetSize();

    // create header objects
    m_Logo.DefineTextureFile(0, "data/textures/game_logo.png");
    m_Logo.DefineProgramShare("2d_shader");
    m_Logo.SetPosition(coreVector2(vRightCenter.x, 0.129f));
    m_Logo.SetCenter(coreVector2(0.5f,0.0f));
    
    m_Pause.Construct(FONT_ROCKS, 80, 0);
    m_Pause.SetPosition(m_Logo.GetPosition());
    m_Pause.SetCenter(coreVector2(0.5f,0.0f));
    m_Pause.SetText("PAUSE");

    // create labeled buttons
    m_Start.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Start.DefineProgramShare("2d_shader_border"); // override
    m_Start.SetPosition(coreVector2(-0.06f,-0.092f));
    m_Start.SetSize(coreVector2(m_BackgroundRight.GetSize().x - 0.06f,0.1f));
    m_Start.SetColor3(COLOR_BLUE_F.xyz());
    m_Start.GetCaption()->SetText("START GAME");
    m_Start.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT(m_Start)
    ADJUST_BORDER(m_Start)

    m_Exit.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Exit.DefineProgramShare("2d_shader_border"); // override
    m_Exit.SetPosition(coreVector2(-0.06f,-0.222f));
    m_Exit.SetSize(m_Start.GetSize());
    m_Exit.SetColor3(COLOR_RED_F.xyz());
    m_Exit.GetCaption()->SetText("EXIT GAME");
    m_Exit.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT(m_Exit)
    ADJUST_BORDER(m_Exit)

    const bool bBitTooSmall = Core::System->GetResolution().AspectRatio() < 1.4f;
    const bool bTooSmall    = Core::System->GetResolution().AspectRatio() < 1.3f;

    m_Resume.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Resume.DefineProgramShare("2d_shader_border"); // override
    m_Resume.SetPosition(m_Start.GetPosition());
    m_Resume.SetSize(m_Start.GetSize());
    m_Resume.SetColor3(COLOR_BLUE_F.xyz());
    m_Resume.GetCaption()->SetText(bTooSmall ? "RESUME" : "RESUME GAME");
    m_Resume.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT(m_Resume)
    ADJUST_BORDER(m_Resume)

    m_Abort.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Abort.DefineProgramShare("2d_shader_border"); // override
    m_Abort.SetPosition(m_Exit.GetPosition());
    m_Abort.SetSize(m_Start.GetSize());
    m_Abort.SetColor3(COLOR_RED_F.xyz());
    m_Abort.GetCaption()->SetText(bTooSmall ? "ABORT" : "ABORT GAME");
    m_Abort.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT(m_Abort)
    ADJUST_BORDER(m_Abort)

    m_Submit.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Submit.DefineProgramShare("2d_shader_border"); // override
    m_Submit.SetPosition(m_Start.GetPosition());
    m_Submit.SetSize(m_Start.GetSize());
    m_Submit.SetColor3(COLOR_ORANGE_F.xyz());
    m_Submit.GetCaption()->SetText(bTooSmall ? "SUBMIT" : "SUBMIT ONLINE");
    m_Submit.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT(m_Submit)
    ADJUST_BORDER(m_Submit)

    m_Finish.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 45, 0);
    m_Finish.DefineProgramShare("2d_shader_border"); // override
    m_Finish.SetPosition(m_Exit.GetPosition());
    m_Finish.SetSize(m_Start.GetSize());
    m_Finish.SetColor3(COLOR_PURPLE_F.xyz());
    m_Finish.GetCaption()->SetText("RETURN");
    m_Finish.GetCaption()->SetAlpha(0.0f);
    ADJUST_RIGHT(m_Finish)
    ADJUST_BORDER(m_Finish)

    // create graphical buttons
#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    m_Short.Construct("data/textures/button_play.png", "data/textures/button_play.png");
    m_Short.SetPosition(coreVector2(-0.06f,0.2425f));
    m_Short.SetSize(coreVector2(0.075f,0.075f));
    m_Short.SetFocusRange(1.2f);
    ADJUST_RIGHT(m_Short)

#else

    m_Short.Construct("data/textures/standard_black.png", "data/textures/standard_black.png");
    m_Short.SetPosition(coreVector2(1000.0f,1000.0f));

#endif

    m_ButtonScore.Construct("data/textures/button_score.png", "data/textures/button_score.png");
    m_ButtonScore.SetPosition(coreVector2(0.03f,0.24f));
    m_ButtonScore.SetSize(coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonScore)

    m_ButtonTime.Construct("data/textures/button_time.png", "data/textures/button_time.png");
    m_ButtonTime.SetPosition(coreVector2(0.03f,0.12f));
    m_ButtonTime.SetSize(coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonTime)

    m_ButtonTrophy.Construct("data/textures/button_trophy.png", "data/textures/button_trophy.png");
    m_ButtonTrophy.SetPosition(coreVector2(0.03f,-0.12f));
    m_ButtonTrophy.SetSize(coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonTrophy)

    m_ButtonConfig.Construct("data/textures/button_config.png", "data/textures/button_config.png");
    m_ButtonConfig.SetPosition(coreVector2(0.03f,-0.24f));
    m_ButtonConfig.SetSize(coreVector2(0.1f,0.1f));
    ADJUST_LEFT(m_ButtonConfig)

    // create objects on the upper screen
#if defined(_CORE_ANDROID_) || defined(_CORE_DEBUG_)

    m_TopBatteryBolt.DefineTextureFile(0, "data/textures/icon_power.png");
    m_TopBatteryBolt.DefineProgramShare("2d_shader_color_icon");
    m_TopBatteryBolt.SetPosition(coreVector2(0.00333f,-0.00933f));
    m_TopBatteryBolt.SetSize(coreVector2(0.05f,0.05f));
    m_TopBatteryBolt.SetCenter(coreVector2(-0.5f,0.5f));
    m_TopBatteryBolt.SetAlignment(coreVector2(1.0f,-1.0f));
    m_TopBatteryBolt.SetColor3(COLOR_BLUE_F.xyz());

    m_TopBatteryValue.Construct(FONT_ROCKS, 29, 8);
    m_TopBatteryValue.SetPosition(coreVector2(0.048f,-0.015f));
    m_TopBatteryValue.SetCenter(coreVector2(-0.5f,0.5f));
    m_TopBatteryValue.SetAlignment(coreVector2(1.0f,-1.0f));
    m_TopBatteryValue.SetColor3(COLOR_BLUE_F.xyz());

#endif

    m_TopFPSTacho.DefineTextureFile(0, "data/textures/icon_speed.png");
    m_TopFPSTacho.DefineProgramShare("2d_shader_color_icon")
        ->AttachShaderFile("data/shaders/default_2d_simple.vs")
        ->AttachShaderFile("data/shaders/color_icon.fs")
        ->Finish();
    m_TopFPSTacho.SetPosition(coreVector2(-0.01633f,-0.007f));
    m_TopFPSTacho.SetSize(coreVector2(0.05f,0.05f));
    m_TopFPSTacho.SetCenter(coreVector2(0.5f,0.5f));
    m_TopFPSTacho.SetAlignment(coreVector2(-1.0f,-1.0f));

    m_TopFPSSec.Construct(FONT_ROCKS, 29, 8);
    m_TopFPSSec.SetPosition(coreVector2(-0.075f - 0.0187f,-0.015f));
    m_TopFPSSec.SetCenter(coreVector2(0.5f,0.5f));
    m_TopFPSSec.SetAlignment(coreVector2(-1.0f,-1.0f));

    m_TopFPSMil.Construct(FONT_ROCKS, 29, 4);
    m_TopFPSMil.SetPosition(coreVector2(-0.075f,-0.015f));
    m_TopFPSMil.SetCenter(coreVector2(0.5f,0.5f));
    m_TopFPSMil.SetAlignment(coreVector2(-1.0f,-1.0f));

#if defined(_CORE_DEBUG_)

    m_TopUpdating.Construct(FONT_ROCKS, 21, 0);
    m_TopUpdating.SetPosition(coreVector2(0.0f,-0.015f));
    m_TopUpdating.SetCenter(coreVector2(0.0f,0.5f));
    m_TopUpdating.SetAlignment(coreVector2(0.0f,-1.0f));

#endif

    // create objects on the lower screen
    m_BottomInfo.Construct(FONT_ROCKS, 21, 0);
    m_BottomInfo.SetPosition(coreVector2(0.01333f,0.00933f));
    m_BottomInfo.SetCenter(coreVector2(-0.5f,-0.5f));
    m_BottomInfo.SetAlignment(coreVector2(1.0f,1.0f));
    m_BottomInfo.SetText(coreData::StrUpper(coreData::Print("(c) 2013-2014 Maus Games - v1.2 - %s %.5s", __DATE__, __TIME__)));

    m_BottomCredit.Construct(FONT_ROCKS, 21, 0);
    m_BottomCredit.SetPosition(coreVector2(-0.01333f,0.00933f));
    m_BottomCredit.SetCenter(coreVector2(0.5f,-0.5f));
    m_BottomCredit.SetAlignment(coreVector2(-1.0f,1.0f));
    m_BottomCredit.SetText(coreData::StrUpper("Music by Kevin MacLeod"));

    m_BottomLoginName.Construct(FONT_ROCKS, 29, 0);
    m_BottomLoginName.SetPosition(coreVector2(-0.01333f,0.00667f));
    m_BottomLoginName.SetCenter(coreVector2(0.5f,-0.5f));
    m_BottomLoginName.SetAlignment(coreVector2(-1.0f,1.0f));

#if defined(_API_GOOGLE_PLAY_)
    m_BottomLoginJolt.DefineTextureFile(0, "data/textures/google_controller.png");
    m_BottomLoginJolt.SetPosition(coreVector2(-0.025f,0.045f));
    m_BottomLoginJolt.SetSize(coreVector2(0.036f,0.036f)*2.5f);
#else
    m_BottomLoginJolt.DefineTextureFile(0, "data/textures/gamejolt_jolt.png");
    m_BottomLoginJolt.SetPosition(coreVector2(-0.02f,0.016f));
    m_BottomLoginJolt.SetSize(coreVector2(0.036f,0.036f)*2.0f);
#endif
    m_BottomLoginJolt.DefineProgramShare("2d_shader");
    m_BottomLoginJolt.SetCenter(coreVector2(0.5f,-0.5f));
    m_BottomLoginJolt.SetAlignment(coreVector2(-1.0f,1.0f));

#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)

    m_GoogleFullTrophy.Construct("data/textures/button_full_trophy.png", "data/textures/button_full_trophy.png");
    m_GoogleFullTrophy.SetPosition(coreVector2(-0.017f - 0.23f, 0.06f));
    m_GoogleFullTrophy.SetSize(coreVector2(0.075f,0.075f));
    m_GoogleFullTrophy.SetCenter(coreVector2(0.5f,-0.5f));
    m_GoogleFullTrophy.SetAlignment(coreVector2(-1.0f,1.0f));

    m_GoogleFullScore.Construct("data/textures/button_full_score.png", "data/textures/button_full_score.png");
    m_GoogleFullScore.SetPosition(coreVector2(-0.017f - 0.335f, m_GoogleFullTrophy.GetPosition().y));
    m_GoogleFullScore.SetSize(coreVector2(0.075f,0.075f));
    m_GoogleFullScore.SetCenter(coreVector2(0.5f,-0.5f));
    m_GoogleFullScore.SetAlignment(coreVector2(-1.0f,1.0f));

#endif

    // create question objects
    m_QuestionBlack.DefineProgramShare("2d_shader_color");
    m_QuestionBlack.SetColor3(coreVector3(0.0f,0.0f,0.0f));
    m_QuestionBlack.FitToScreen();
    m_QuestionBlack.Move();

    m_Question.Construct(FONT_ROCKS, bBitTooSmall ? 35 : 45, 0);
    m_Question.SetPosition(coreVector2(0.0f,0.08f));
    m_Question.SetText("Do you want to go out with me?");
    m_Question.Move();

    m_Yes.Construct("data/textures/button_ok.png", "data/textures/button_ok.png");
    m_Yes.SetPosition(coreVector2(-0.2f,-0.08f));
    m_Yes.SetSize(coreVector2(0.075f,0.075f));
    m_Yes.SetColor3(LERP(COLOR_GREEN_F.xyz(), COLOR_WHITE_F.xyz(), 0.75f));
    m_Yes.SetFocusRange(1.2f);
    m_Yes.Move();

    m_No.Construct("data/textures/button_cancel.png", "data/textures/button_cancel.png");
    m_No.SetPosition(coreVector2(0.2f,-0.08f));
    m_No.SetSize(m_Yes.GetSize());
    m_No.SetColor3(LERP(COLOR_RED_F.xyz(), COLOR_WHITE_F.xyz(), 0.75f));
    m_No.SetFocusRange(1.2f);
    m_No.Move();

    // create video configuration objects
    m_VideoText.Construct(FONT_ROCKS, 45, 0);
    m_VideoText.SetPosition(coreVector2(LEFT_CENTER,0.235f));
    m_VideoText.SetCenter(coreVector2(-0.5f,0.0f));
    m_VideoText.SetText("GRAPHICS");

    const int iCurQuality = CLAMP(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY), 0, 2);

    m_VideoLow.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 29, 0);
    m_VideoLow.DefineProgramShare("2d_shader_border"); // override
    m_VideoLow.SetPosition(coreVector2(LEFT_CENTER - 0.17f,0.15f)); // old Y: 0.13f
    m_VideoLow.SetSize(coreVector2(0.15f,0.075f));
    m_VideoLow.SetCenter(coreVector2(-0.5f,0.0f));
    m_VideoLow.SetColor3((iCurQuality == 0) ? COLOR_GREEN_F.xyz() : coreVector3(0.5f,0.5f,0.5f));
    m_VideoLow.GetCaption()->SetText("LOW");
    m_VideoLow.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_VideoLow)

    m_VideoMedium.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 29, 0);
    m_VideoMedium.DefineProgramShare("2d_shader_border"); // override
    m_VideoMedium.SetPosition(coreVector2(LEFT_CENTER, m_VideoLow.GetPosition().y));
    m_VideoMedium.SetSize(m_VideoLow.GetSize());
    m_VideoMedium.SetCenter(coreVector2(-0.5f,0.0f));
    m_VideoMedium.SetColor3((iCurQuality == 1) ? COLOR_YELLOW_F.xyz() : coreVector3(0.5f,0.5f,0.5f));
    m_VideoMedium.GetCaption()->SetText("MID");
    m_VideoMedium.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_VideoMedium)

    m_VideoHigh.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 29, 0);
    m_VideoHigh.DefineProgramShare("2d_shader_border"); // override
    m_VideoHigh.SetPosition(coreVector2(LEFT_CENTER + 0.17f, m_VideoLow.GetPosition().y));
    m_VideoHigh.SetSize(m_VideoLow.GetSize());
    m_VideoHigh.SetCenter(coreVector2(-0.5f,0.0f));
    m_VideoHigh.SetColor3((iCurQuality == 2) ? COLOR_RED_F.xyz() : coreVector3(0.5f,0.5f,0.5f));
    m_VideoHigh.GetCaption()->SetText("HIGH");
    m_VideoHigh.GetCaption()->SetAlpha(0.0f);
    ADJUST_BORDER(m_VideoHigh)

    // create audio configuration objects
    m_AudioText.Construct(FONT_ROCKS, 45, 0);
    m_AudioText.SetPosition(coreVector2(LEFT_CENTER,0.04f)); // old Y: 0.02f
    m_AudioText.SetCenter(coreVector2(-0.5f,0.0f));
    m_AudioText.SetText("AUDIO");

    const float fCurVolume = Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME) * 0.1f;

    m_AudioBarBack.DefineProgramShare("2d_shader_color_bar")
        ->AttachShaderFile("data/shaders/default_2d_simple.vs")
        ->AttachShaderFile("data/shaders/color_bar.fs")
        ->Finish();
    m_AudioBarBack.SetPosition(coreVector2(LEFT_CENTER, -0.04f)); // old Y: -0.08f
    m_AudioBarBack.SetSize(coreVector2(0.5f,0.056f));
    m_AudioBarBack.SetCenter(coreVector2(-0.5f,0.0f));
    m_AudioBarBack.SetColor3(coreVector3(0.5f,0.5f,0.5f));

    m_AudioDrag.Construct("data/textures/standard_black.png", "data/textures/standard_black.png");
    m_AudioDrag.DefineProgramShare("2d_shader_border"); // override
    m_AudioDrag.SetSize(coreVector2(0.035f,0.085f));
    m_AudioDrag.SetPosition(coreVector2(LEFT_CENTER + (fCurVolume - 0.5f) * (m_AudioBarBack.GetSize().x - m_AudioDrag.GetSize().x), m_AudioBarBack.GetPosition().y));
    m_AudioDrag.SetCenter(coreVector2(-0.5f,0.0f));
    m_AudioDrag.SetColor3(coreVector3(0.0f,0.0f,0.0f));
    ADJUST_BORDER(m_AudioDrag)

    m_AudioBar.DefineProgramShare("2d_shader_color_bar");
    m_AudioBar.SetSize(coreVector2((m_AudioBarBack.GetSize().x - m_AudioDrag.GetSize().x) * fCurVolume, 0.056f));
    m_AudioBar.SetPosition(coreVector2(LEFT_CENTER + (m_AudioBar.GetSize().x - m_AudioBarBack.GetSize().x) * 0.5f, m_AudioBarBack.GetPosition().y));
    m_AudioBar.SetCenter(coreVector2(-0.5f,0.0f));
    m_AudioBar.SetColor3(COLOR_BLUE_F.xyz() * MENU_ALPHA_IDLE_2);

    m_AudioIconHigh.DefineTextureFile(0, "data/textures/icon_audio_1.png");
    m_AudioIconHigh.DefineProgramShare("2d_shader_color_icon");
    m_AudioIconHigh.SetPosition(coreVector2(LEFT_CENTER + 0.22f, m_AudioBarBack.GetPosition().y));
    m_AudioIconHigh.SetSize(coreVector2(0.05f,0.05f));
    m_AudioIconHigh.SetCenter(coreVector2(-0.5f,0.0f));
    m_AudioIconHigh.SetFocusRange(0.0f);

    m_AudioIconLow.DefineTextureFile(0, "data/textures/icon_audio_2.png");
    m_AudioIconLow.DefineProgramShare("2d_shader_color_icon");
    m_AudioIconLow.SetPosition(coreVector2(LEFT_CENTER - 0.22f, m_AudioBarBack.GetPosition().y));
    m_AudioIconLow.SetSize(coreVector2(0.05f,0.05f));
    m_AudioIconLow.SetCenter(coreVector2(-0.5f,0.0f));
    m_AudioIconLow.SetFocusRange(0.0f);
    
    // create login configuration objects
#if !defined(_API_GOOGLE_PLAY_)

    m_LoginConfigOr.Construct(FONT_ROCKS, 21, 0);
    m_LoginConfigOr.SetPosition(coreVector2(LEFT_CENTER - 0.047f, -0.195f));
    m_LoginConfigOr.SetCenter(coreVector2(-0.5f,0.0f));
    m_LoginConfigOr.SetText("LOG INTO");

    m_LoginConfigLogo.DefineTextureFile(0, "data/textures/gamejolt_logo.png");
    m_LoginConfigLogo.DefineProgramShare("2d_shader");
    m_LoginConfigLogo.SetPosition(coreVector2(LEFT_CENTER - 0.047f,-0.238f));
    m_LoginConfigLogo.SetSize(coreVector2(0.512f,0.064f) * 1.2f);
    m_LoginConfigLogo.SetCenter(coreVector2(-0.5f,0.0f));

    m_LoginConfigStart.Construct("data/textures/button_login.png", "data/textures/button_login.png");
    m_LoginConfigStart.SetPosition(coreVector2(LEFT_CENTER + 0.17f, g_bCoreDebug ? -0.33f : -0.221f));
    m_LoginConfigStart.SetSize(coreVector2(0.075f,0.075f));
    ADJUST_LEFT(m_LoginConfigStart)

#else

    m_LoginConfigStart.Construct("data/textures/button_login.png", "data/textures/button_login.png");
    m_LoginConfigStart.SetPosition(coreVector2(-0.017f - 0.125f, m_GoogleFullTrophy.GetPosition().y));
    m_LoginConfigStart.SetSize(coreVector2(0.075f,0.075f));
    m_LoginConfigStart.SetCenter(coreVector2(0.5f,-0.5f));
    m_LoginConfigStart.SetAlignment(coreVector2(-1.0f,1.0f));

#endif

    // create control configuration objects
#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)

    m_ControlText.Construct(FONT_ROCKS, 45, 0);
    m_ControlText.SetPosition(coreVector2(LEFT_CENTER,-0.141f));
    m_ControlText.SetCenter(coreVector2(-0.5f,0.0f));
    m_ControlText.SetText("INPUT");

    const int iCurControl = CLAMP(Core::Config->GetInt("Game", "Control", 0), 0, 2);

    m_ControlType.Construct(FONT_ROCKS, 29, 16, 3);
    m_ControlType.SetPosition(coreVector2(LEFT_CENTER,-0.221f));
    m_ControlType.SetSize(coreVector2(0.49f,0.075f));
    m_ControlType.SetCenter(coreVector2(-0.5f,0.0f));
    m_ControlType.GetCaption()->SetColor3(coreVector3(0.75f,0.75f,0.75f));
    m_ControlType.AddEntry("CLASSIC",    CONTROL_CLASSIC);
    m_ControlType.AddEntry("MOTION",     CONTROL_MOTION);
    m_ControlType.AddEntry("FULLSCREEN", CONTROL_FULLSCREEN);
    m_ControlType.Select(iCurControl);

    for(int i = 0; i < 2; ++i)
    {
        coreButton* pArrow = m_ControlType.GetArrow(i);

        pArrow->Construct(NULL, NULL, FONT_ROCKS, 45, 2);
        pArrow->DefineProgramShare("2d_shader_border");
        pArrow->SetColor3(coreVector3(0.05f,0.05f,0.05f));
        pArrow->SetTexSize(coreVector2(0.62f,0.62f) / m_ControlType.GetSize().y * 0.0165f);
        pArrow->GetCaption()->SetText(i ? ">" : "<");
    }

#endif

#if !defined(_API_GOOGLE_PLAY_)

    // create login objects
    m_LoginBlack.DefineProgramShare("2d_shader_color");
    m_LoginBlack.SetColor3(coreVector3(0.0f,0.0f,0.0f));
    m_LoginBlack.FitToScreen();
    m_LoginBlack.Move();

    m_LoginPopup.DefineProgramShare("2d_shader_border");
    m_LoginPopup.SetPosition(coreVector2(0.0f,0.25f));
    m_LoginPopup.SetSize(coreVector2(0.62f,0.37f));
    m_LoginPopup.SetColor3(coreVector3(0.05f,0.05f,0.05f));
    ADJUST_BORDER(m_LoginPopup)

    m_aLoginText[0].Construct(FONT_ROCKS, 45, 0);
    m_aLoginText[0].SetPosition(m_LoginPopup.GetPosition() + coreVector2(0.0f,0.11f));
    m_aLoginText[0].SetText("LOGIN");
    m_aLoginText[1].Construct(FONT_ROCKS, 45, 0);
    m_aLoginText[1].SetPosition(m_aLoginText[0].GetPosition());
    m_aLoginText[1].SetText("SUBMIT");

    m_LoginEnterName.Construct(FONT_ROCKS, 29, 0);
    m_LoginEnterName.SetPosition(m_LoginPopup.GetPosition() + coreVector2(-0.27f,0.02f));
    m_LoginEnterName.SetAlignment(coreVector2(1.0f,0.0f));
    m_LoginEnterName.SetText("USER NAME");

    m_LoginEnterToken.Construct(FONT_ROCKS, 29, 0);
    m_LoginEnterToken.SetPosition(m_LoginPopup.GetPosition() + coreVector2(-0.27f,-0.05f));
    m_LoginEnterToken.SetAlignment(coreVector2(1.0f,0.0f));
    m_LoginEnterToken.SetText("TOKEN");

    m_LoginEnterGuest.Construct(FONT_ROCKS, 29, 0);
    m_LoginEnterGuest.SetPosition(m_LoginPopup.GetPosition() + coreVector2(-0.27f,0.02f));
    m_LoginEnterGuest.SetAlignment(coreVector2(1.0f,0.0f));
    m_LoginEnterGuest.SetText("NAME");

    m_LoginName.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 29, 32);
    m_LoginName.DefineProgramShare("2d_shader_color"); // override
    m_LoginName.SetPosition(m_LoginEnterName.GetPosition() + coreVector2(0.54f,0.0f));
    m_LoginName.SetSize(coreVector2(0.35f,0.05f));
    m_LoginName.SetAlignment(coreVector2(-1.0f,0.0f));
    m_LoginName.SetColor3(coreVector3(0.25f,0.25f,0.25f));
    m_LoginName.SetCursor('<');

    m_LoginToken.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 29, 64);
    m_LoginToken.DefineProgramShare("2d_shader_color"); // override
    m_LoginToken.SetPosition(m_LoginEnterToken.GetPosition() + coreVector2(0.54f,0.0f));
    m_LoginToken.SetSize(m_LoginName.GetSize());
    m_LoginToken.SetAlignment(coreVector2(-1.0f,0.0f));
    m_LoginToken.SetColor3(coreVector3(0.25f,0.25f,0.25f));
    m_LoginToken.SetCursor('<');
    m_LoginToken.SetReplace('*');

    m_LoginGuest.Construct("data/textures/standard_black.png", "data/textures/standard_black.png", FONT_ROCKS, 29, 32);
    m_LoginGuest.DefineProgramShare("2d_shader_color"); // override
    m_LoginGuest.SetPosition(m_LoginEnterGuest.GetPosition() + coreVector2(0.54f,0.0f));
    m_LoginGuest.SetSize(m_LoginName.GetSize() + coreVector2(0.085f,0.0f));
    m_LoginGuest.SetAlignment(coreVector2(-1.0f,0.0f));
    m_LoginGuest.SetColor3(coreVector3(0.25f,0.25f,0.25f));
    m_LoginGuest.SetCursor('<');

    m_LoginOK.Construct("data/textures/button_ok.png", "data/textures/button_ok.png");
    m_LoginOK.SetPosition(m_LoginPopup.GetPosition() + coreVector2(m_LoginPopup.GetSize().x * 0.5f + 0.0575f, m_LoginPopup.GetSize().y * 0.5f - 0.0575f));
    m_LoginOK.SetSize(coreVector2(0.075f,0.075f));
    m_LoginOK.SetColor3(LERP(COLOR_GREEN_F.xyz(), COLOR_WHITE_F.xyz(), 0.75f));

    m_LoginCancel.Construct("data/textures/button_cancel.png", "data/textures/button_cancel.png");
    m_LoginCancel.SetPosition(m_LoginPopup.GetPosition() + coreVector2(m_LoginPopup.GetSize().x * 0.5f + 0.0575f, -m_LoginPopup.GetSize().y * 0.5f + 0.0575f));
    m_LoginCancel.SetSize(coreVector2(0.075f,0.075f));
    m_LoginCancel.SetColor3(LERP(COLOR_RED_F.xyz(), COLOR_WHITE_F.xyz(), 0.75f));

    m_LoginError.Construct(FONT_ROCKS, 21, 32);
    m_LoginError.SetPosition(m_LoginPopup.GetPosition() + coreVector2(0.0f,-0.122f));
    m_LoginError.SetText("All your base are belong to us!");

    m_LoginJoltOr.Construct(FONT_ROCKS, 21, 0);
    m_LoginJoltOr.SetPosition(m_LoginPopup.GetPosition() + coreVector2(-0.047f,-0.067f));
    m_LoginJoltOr.SetText("OR LOG INTO");

    m_LoginJoltLogo.DefineTextureFile(0, "data/textures/gamejolt_logo.png");
    m_LoginJoltLogo.DefineProgramShare("2d_shader");
    m_LoginJoltLogo.SetPosition(m_LoginPopup.GetPosition() + coreVector2(-0.047f,-0.11f));
    m_LoginJoltLogo.SetSize(coreVector2(0.512f,0.064f) * 1.2f);

    m_LoginJoltStart.Construct("data/textures/button_login.png", "data/textures/button_login.png");
    m_LoginJoltStart.SetPosition(m_LoginPopup.GetPosition() + coreVector2(0.17f,-0.093f));
    m_LoginJoltStart.SetSize(coreVector2(0.075f,0.075f));
    m_LoginJoltStart.SetAlignment(coreVector2(1.0f,0.0f));

#endif

    // create successful submit object
    m_Successful.DefineTextureFile(0, "data/textures/icon_success.png");
    m_Successful.DefineProgram(m_Made.GetProgram());
    m_Successful.SetPosition(m_Submit.GetPosition() + coreVector2(-0.03f,0.0f));
    m_Successful.SetSize(coreVector2(0.06f,0.06f));
    m_Successful.SetFocusRange(0.0f);
    ADJUST_RIGHT(m_Successful)

    // create score objects
    for(int i = 0; i < SCORE_TABLES; ++i)
    {
        constexpr_var coreVector2 vPos = coreVector2(LEFT_CENTER,0.115f);
        constexpr_var coreVector2 vCen = coreVector2(-0.5f,0.0f);

        m_aScoreTable[i].Construct(FONT_ROCKS, 45, 0);
        m_aScoreTable[i].SetPosition(vPos + coreVector2(0.0f,0.12f));
        m_aScoreTable[i].SetCenter(vCen);
        m_aScoreTable[i].SetText(i ? "TIME" : "SCORE");

        for(int j = 0; j < SCORE_ENTRIES; ++j)
        {
            m_aaScoreEntry[i][j][0].Construct(FONT_ROCKS, 23, 4);
            m_aaScoreEntry[i][j][0].SetPosition(vPos + coreVector2(-0.208f, 0.045f - j*0.04f));
            m_aaScoreEntry[i][j][0].SetCenter(vCen);
            m_aaScoreEntry[i][j][0].SetAlignment(coreVector2(-1.0f,0.0f));
            m_aaScoreEntry[i][j][0].SetColor3(coreVector3(0.75f,0.75f,0.75f));
            m_aaScoreEntry[i][j][0].SetText(coreData::Print("%d.", j+1));

            m_aaScoreEntry[i][j][1].Construct(FONT_ROCKS, 23, 24);
            m_aaScoreEntry[i][j][1].SetPosition(coreVector2(vPos.x - 0.19466f, m_aaScoreEntry[i][j][0].GetPosition().y));
            m_aaScoreEntry[i][j][1].SetCenter(vCen);
            m_aaScoreEntry[i][j][1].SetAlignment(coreVector2(1.0f,0.0f));
            m_aaScoreEntry[i][j][1].SetColor3(LERP(COLOR_YELLOW_F.xyz(), COLOR_WHITE_F.xyz(), float(MIN(j, 3) / 3)));
            m_aaScoreEntry[i][j][1].SetText("-");

            m_aaScoreEntry[i][j][2].Construct(FONT_ROCKS, 23, 16);
            m_aaScoreEntry[i][j][2].SetPosition(coreVector2(vPos.x + 0.232f, m_aaScoreEntry[i][j][0].GetPosition().y));
            m_aaScoreEntry[i][j][2].SetCenter(vCen);
            m_aaScoreEntry[i][j][2].SetAlignment(coreVector2(-1.0f,0.0f));
            m_aaScoreEntry[i][j][2].SetColor3(coreVector3(0.75f,0.75f,0.75f));
            m_aaScoreEntry[i][j][2].SetText("-");
        }

        m_aScoreBest[i].Construct(FONT_ROCKS, 29, 0);
        m_aScoreBest[i].SetPosition(vPos + coreVector2(0.0f,-0.31f));
        m_aScoreBest[i].SetCenter(vCen);
        m_aScoreBest[i].SetText("YOUR BEST");

        m_aScoreBestValue[i].Construct(FONT_ROCKS, 29, 16);
        m_aScoreBestValue[i].SetPosition(coreVector2(vPos.x, m_aScoreBest[i].GetPosition().y - 0.04667f));
        m_aScoreBestValue[i].SetCenter(vCen);
        m_aScoreBestValue[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));
        m_aScoreBestValue[i].SetText("-");

        m_aScoreRecord[i].Construct(FONT_ROCKS, 21, 16);
        m_aScoreRecord[i].SetPosition(coreVector2(vPos.x - 0.25f, m_aScoreBestValue[i].GetPosition().y));
        m_aScoreRecord[i].SetCenter(vCen);
        m_aScoreRecord[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aScoreRecord[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));

#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)

        m_aScoreRank[i].Construct(FONT_ROCKS, 23, 16);
        m_aScoreRank[i].SetPosition(coreVector2(vPos.x - 0.228f, m_aScoreBestValue[i].GetPosition().y));
        m_aScoreRank[i].SetCenter(vCen);
        m_aScoreRank[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aScoreRank[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));
    #if defined(_CORE_DEBUG_)
        m_aScoreRank[i].SetText("123.");
    #endif

#endif
    }

    m_PageChange.Construct("data/textures/button_scroll.png", "data/textures/button_scroll.png");
    m_PageChange.SetPosition(coreVector2(LEFT_CENTER + 0.17f,-0.221f));
    m_PageChange.SetSize(coreVector2(0.075f,0.075f));
    ADJUST_LEFT(m_PageChange)

    // create submit objects
    for(int i = 0; i < SCORE_TABLES; ++i)
    {
        const         coreVector2 vPos = coreVector2(vRightCenter.x, 0.235f - i*0.14f);
        constexpr_var coreVector2 vCen = coreVector2(0.5f,0.0f);

        m_aAfterBest[i].Construct(FONT_ROCKS, 45, 0);
        m_aAfterBest[i].SetPosition(vPos);
        m_aAfterBest[i].SetCenter(vCen);
        m_aAfterBest[i].SetText(i ? "YOUR TIME" : "YOUR SCORE");

        m_aAfterBestValue[i].Construct(FONT_ROCKS, 45, 16);
        m_aAfterBestValue[i].SetPosition(coreVector2(vPos.x, m_aAfterBest[i].GetPosition().y - 0.06f));
        m_aAfterBestValue[i].SetCenter(vCen);
        m_aAfterBestValue[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));
        m_aAfterBestValue[i].SetText("-");

        m_aAfterRecord[i].Construct(FONT_ROCKS, 21, 16);
        m_aAfterRecord[i].SetPosition(coreVector2(vPos.x - m_BackgroundRight.GetSize().x*0.5f + 0.06f, m_aAfterBestValue[i].GetPosition().y));
        m_aAfterRecord[i].SetCenter(vCen);
        m_aAfterRecord[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aAfterRecord[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));

        m_afSubmitValue[i] = 0.0f;
    }

    // create trophy objects
    m_TrophyText.Construct(FONT_ROCKS, 45, 0);
    m_TrophyText.SetPosition(coreVector2(LEFT_CENTER,0.235f));
    m_TrophyText.SetCenter(coreVector2(-0.5f,0.0f));
    m_TrophyText.SetText("TROPHIES");

    for(int i = 0; i < TROPHIES; ++i)
    {
        m_aTrophyImage[i].DefineTextureFile(0, coreData::Print("data/textures/trophy_%i.png", (i == TROPHIES-1) ? 2 : 1));
        m_aTrophyImage[i].DefineProgramShare("2d_shader");
        m_aTrophyImage[i].SetPosition(coreVector2(LEFT_CENTER + ((i%5)-2)*0.105f, -0.075f - ((i/5)-2)*0.105f));
        m_aTrophyImage[i].SetSize(coreVector2(0.09f,0.09f));
        m_aTrophyImage[i].SetCenter(coreVector2(-0.5f,0.0f));
        m_aTrophyImage[i].SetColor3((i % (COLOR_NUM+1)) ? g_avColor[(i-(1+i/7))%COLOR_NUM].xyz() : (COLOR_WHITE_F.xyz()*0.9f));

        m_aTrophyCheck[i].DefineTextureFile(0, "data/textures/icon_success.png");
        m_aTrophyCheck[i].DefineProgram(m_TrophyText.GetProgram());
        m_aTrophyCheck[i].SetPosition(coreVector2(LEFT_CENTER + ((i%5)-2)*0.105f, -0.075f - ((i/5)-2)*0.105f));
        m_aTrophyCheck[i].SetSize(coreVector2(0.06f,0.06f));
        m_aTrophyCheck[i].SetCenter(coreVector2(-0.5f,0.0f));
        m_aTrophyCheck[i].SetFocusRange(0.0f);
    }

    for(int i = 0; i < TROPHY_SECRETS; ++i)
    {
        m_aTrophySecret[i].Construct(FONT_ROCKS, 45, 2);
        m_aTrophySecret[i].SetCenter(coreVector2(-0.5f,0.0f));
        m_aTrophySecret[i].SetText("?");
        m_aTrophySecret[i].SetFocusRange(0.0f);
    }
    m_aTrophySecret[0].SetPosition(m_aTrophyImage[0].GetPosition());
    m_aTrophySecret[1].SetPosition(m_aTrophyImage[1].GetPosition());
    m_aTrophySecret[2].SetPosition(m_aTrophyImage[2].GetPosition());
    m_aTrophySecret[3].SetPosition(m_aTrophyImage[4].GetPosition());

    m_TrophyName.Construct(FONT_ROCKS, 29, 32);
    m_TrophyName.SetPosition(coreVector2(LEFT_CENTER,-0.172f));
    m_TrophyName.SetCenter(coreVector2(-0.5f,0.0f));
    m_TrophyName.SetText("-");

    for(int i = 0; i < 2; ++i)
    {
        m_aTrophyDesc[i].Construct(FONT_ROCKS, 21, 64);
        m_aTrophyDesc[i].SetPosition(coreVector2(LEFT_CENTER, m_TrophyName.GetPosition().y - 0.04667f - i*0.03f));
        m_aTrophyDesc[i].SetCenter(coreVector2(-0.5f,0.0f));
        m_aTrophyDesc[i].SetColor3(coreVector3(0.75f,0.75f,0.75f));
        m_aTrophyDesc[i].SetText("-");
    }

    // create connection objects
    for(int i = 0; i < 3; ++i)
    {
        m_aConnectionError[i].Construct(FONT_ROCKS, 29, 24);
        m_aConnectionError[i].SetPosition(coreVector2(LEFT_CENTER, 0.095f - i*0.05f));
        m_aConnectionError[i].SetCenter(coreVector2(-0.5f,0.0f));
    }

#if defined(_CORE_DEBUG_)
    this->SetErrorMessage(COLOR_ORANGE_F.xyz(), "DEBUG ERROR", "MORE DEBUG ERROR", "");
#endif

#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)

    m_AuthLogo.DefineTextureFile(0, "data/textures/google_controller.png");
    m_AuthLogo.DefineProgramShare("2d_shader");
    m_AuthLogo.SetPosition(coreVector2(LEFT_CENTER - 0.07f,-0.045f));
    m_AuthLogo.SetSize(coreVector2(0.036f,0.036f) * 3.0f);
    m_AuthLogo.SetCenter(coreVector2(-0.5f,0.0f));

    m_AuthButton.Construct("data/textures/button_login.png", "data/textures/button_login.png");
    m_AuthButton.SetPosition(coreVector2(LEFT_CENTER + 0.03f,-0.045f));
    m_AuthButton.SetSize(coreVector2(0.075f,0.075f));
    ADJUST_LEFT(m_AuthButton)

#endif

    // create loading objects
    m_Loading.DefineTextureFile(0, "data/textures/icon_load.png");
    m_Loading.DefineProgramShare("2d_shader_color_icon");
    m_Loading.SetPosition(coreVector2(LEFT_CENTER - 0.05f, -0.05f) + m_BackgroundLeft.GetSize()*0.5f);
    m_Loading.SetSize(coreVector2(0.05f,0.05f));
    m_Loading.SetCenter(coreVector2(-0.5f,0.0f));
    m_Loading.SetColor3(COLOR_YELLOW_F.xyz());

    // start intro
    m_Intro.Play(true);

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

    this->AddObject(0, &m_Black);

    this->AddObject(1, &m_Black);
    this->AddObject(1, &m_Made);
    this->AddObject(1, &m_For);
    this->AddObject(1, &m_Maus);
    this->AddObject(1, &m_GameJolt);

    this->AddObject(3, &m_BigLogo);

    this->AddObject(5, &m_White);

    for(int i = 6; i <= 8; ++i)
    {
        this->AddObject(i, &m_BackgroundLeft);
        this->AddObject(i, &m_BackgroundRight);
        this->AddObject(i, &m_Logo);
        this->AddObject(i, &m_Start);
        this->AddObject(i, &m_Exit);
        this->AddObject(i, &m_Short);
        this->AddObject(i, &m_ButtonScore);
        this->AddObject(i, &m_ButtonTime);
        this->AddObject(i, &m_ButtonTrophy);
        this->AddObject(i, &m_ButtonConfig);
        this->AddObject(i, &m_TopBatteryBolt);
        this->AddObject(i, &m_TopBatteryValue);
        this->AddObject(i, &m_TopFPSTacho);
        this->AddObject(i, &m_TopFPSSec);
        this->AddObject(i, &m_TopFPSMil);
        this->AddObject(i, &m_TopUpdating);
        this->AddObject(i, &m_BottomInfo);
        this->AddObject(i, &m_BottomCredit);
        this->AddObject(i, &m_BottomLoginJolt);
        this->AddObject(i, &m_BottomLoginName);
#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)
        this->AddObject(i, &m_GoogleFullTrophy);
        this->AddObject(i, &m_GoogleFullScore);
        this->AddObject(i, &m_LoginConfigStart);
#endif
        this->AddObject(i, &m_ScoreMenu);
    }
    this->AddObject(7, &m_QuestionBlack);
    this->AddObject(7, &m_Question);
    this->AddObject(7, &m_Yes);
    this->AddObject(7, &m_No);
    this->AddObject(8, &m_LoginBlack);
    this->AddObject(8, &m_LoginMenu);

    for(int i = 11; i <= 13; ++i)
    {
        this->AddObject(i, &m_Black);
        this->AddObject(i, &m_BackgroundLeft);
        this->AddObject(i, &m_BackgroundRight);
        this->AddObject(i, &m_Pause);
        this->AddObject(i, &m_Resume);
        this->AddObject(i, &m_Abort);
        this->AddObject(i, &m_Short);
        this->AddObject(i, &m_ButtonTrophy);
        this->AddObject(i, &m_ButtonConfig);
        this->AddObject(i, &m_TopBatteryBolt);
        this->AddObject(i, &m_TopBatteryValue);
        this->AddObject(i, &m_TopFPSTacho);
        this->AddObject(i, &m_TopFPSSec);
        this->AddObject(i, &m_TopFPSMil);
        this->AddObject(i, &m_TopUpdating);
        this->AddObject(i, &m_BottomLoginJolt);
        this->AddObject(i, &m_BottomLoginName);
#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)
        this->AddObject(i, &m_GoogleFullTrophy);
        this->AddObject(i, &m_GoogleFullScore);
        this->AddObject(i, &m_LoginConfigStart);
#endif
        this->AddObject(i, &m_ScoreMenu);
    }
    this->AddObject(12, &m_QuestionBlack);
    this->AddObject(12, &m_Question);
    this->AddObject(12, &m_Yes);
    this->AddObject(12, &m_No);
    this->AddObject(13, &m_LoginBlack);
    this->AddObject(13, &m_LoginMenu);

    for(int i = 14; i <= 16; ++i)
    {
        this->AddObject(i, &m_Black);
        this->AddObject(i, &m_BackgroundLeft);
        this->AddObject(i, &m_BackgroundRight);
        for(int j = 0; j < SCORE_TABLES; ++j)
        {
            this->AddObject(i, &m_aAfterBest[j]);
            this->AddObject(i, &m_aAfterBestValue[j]);
            this->AddObject(i, &m_aAfterRecord[j]);
        }
        this->AddObject(i, &m_Submit);
        this->AddObject(i, &m_Finish);
        this->AddObject(i, &m_Short);
        this->AddObject(i, &m_ButtonScore);
        this->AddObject(i, &m_ButtonTime);
        this->AddObject(i, &m_ButtonTrophy);
        this->AddObject(i, &m_ButtonConfig);
        this->AddObject(i, &m_TopBatteryBolt);
        this->AddObject(i, &m_TopBatteryValue);
        this->AddObject(i, &m_TopFPSTacho);
        this->AddObject(i, &m_TopFPSSec);
        this->AddObject(i, &m_TopFPSMil);
        this->AddObject(i, &m_TopUpdating);
        this->AddObject(i, &m_BottomLoginJolt);
        this->AddObject(i, &m_BottomLoginName);
#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)
        this->AddObject(i, &m_GoogleFullTrophy);
        this->AddObject(i, &m_GoogleFullScore);
        this->AddObject(i, &m_LoginConfigStart);
#endif
        this->AddObject(i, &m_Successful);
        this->AddObject(i, &m_ScoreMenu);
    }
    this->AddObject(15, &m_QuestionBlack);
    this->AddObject(15, &m_Question);
    this->AddObject(15, &m_Yes);
    this->AddObject(15, &m_No);
    this->AddObject(16, &m_LoginBlack);
    this->AddObject(16, &m_LoginMenu);

    for(int i = 0; i < SCORE_TABLES; ++i)
    {
        m_ScoreMenu.AddObject(i, &m_aScoreTable[i]);

        for(int j = 0; j < SCORE_ENTRIES; ++j)
            for(int k = 0; k < 3; ++k)
                m_ScoreMenu.AddObject(i, &m_aaScoreEntry[i][j][k]);

        m_ScoreMenu.AddObject(i, &m_aScoreBest[i]);
        m_ScoreMenu.AddObject(i, &m_aScoreBestValue[i]);
        m_ScoreMenu.AddObject(i, &m_aScoreRecord[i]);
        m_ScoreMenu.AddObject(i, &m_aScoreRank[i]);

        for(int j = 0; j < 3; ++j)
            m_ScoreMenu.AddObject(i, &m_aConnectionError[j]);
        m_ScoreMenu.AddObject(i, &m_AuthLogo);
        m_ScoreMenu.AddObject(i, &m_AuthButton);
        m_ScoreMenu.AddObject(i, &m_Loading);
        m_ScoreMenu.AddObject(i, &m_PageChange);
    }

    m_ScoreMenu.AddObject(4, &m_TrophyText);
    for(int i = 0; i < TROPHIES; ++i)
    {
        m_ScoreMenu.AddObject(4, &m_aTrophyImage[i]);
        m_ScoreMenu.AddObject(4, &m_aTrophyCheck[i]);
    }
    for(int i = 0; i < TROPHY_SECRETS; ++i)
    {
        m_ScoreMenu.AddObject(4, &m_aTrophySecret[i]);
    }
    m_ScoreMenu.AddObject(4, &m_TrophyName);
    m_ScoreMenu.AddObject(4, &m_aTrophyDesc[0]);
    m_ScoreMenu.AddObject(4, &m_aTrophyDesc[1]);
    m_ScoreMenu.AddObject(4, &m_Loading);
    
    m_ScoreMenu.AddObject(5, &m_VideoText);
    m_ScoreMenu.AddObject(5, &m_VideoLow);
    m_ScoreMenu.AddObject(5, &m_VideoMedium);
    m_ScoreMenu.AddObject(5, &m_VideoHigh);
    m_ScoreMenu.AddObject(5, &m_AudioText);
    m_ScoreMenu.AddObject(5, &m_AudioBarBack);
    m_ScoreMenu.AddObject(5, &m_AudioBar);
    m_ScoreMenu.AddObject(5, &m_AudioIconHigh);
    m_ScoreMenu.AddObject(5, &m_AudioIconLow);
    m_ScoreMenu.AddObject(5, &m_AudioDrag);
#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)
    m_ScoreMenu.AddObject(5, &m_ControlText);
    m_ScoreMenu.AddObject(5, &m_ControlType);
#endif
#if !defined(_API_GOOGLE_PLAY_)
    m_ScoreMenu.AddObject(5, &m_LoginConfigLogo);
    m_ScoreMenu.AddObject(5, &m_LoginConfigStart);
    m_ScoreMenu.AddObject(5, &m_LoginConfigOr);
#endif
    m_ScoreMenu.AddObject(5, &m_Loading);

#if !defined(_API_GOOGLE_PLAY_)

    m_LoginMenu.AddObject(0, &m_LoginPopup);
    m_LoginMenu.AddObject(0, &m_aLoginText[0]);
    m_LoginMenu.AddObject(0, &m_LoginEnterName);
    m_LoginMenu.AddObject(0, &m_LoginEnterToken);
    m_LoginMenu.AddObject(0, &m_LoginName);
    m_LoginMenu.AddObject(0, &m_LoginToken);
    m_LoginMenu.AddObject(0, &m_LoginOK);
    m_LoginMenu.AddObject(0, &m_LoginCancel);
    m_LoginMenu.AddObject(0, &m_LoginError);

    m_LoginMenu.AddObject(1, &m_LoginPopup);
    m_LoginMenu.AddObject(1, &m_aLoginText[1]);
    m_LoginMenu.AddObject(1, &m_LoginEnterGuest);
    m_LoginMenu.AddObject(1, &m_LoginGuest);
    m_LoginMenu.AddObject(1, &m_LoginOK);
    m_LoginMenu.AddObject(1, &m_LoginCancel);
    m_LoginMenu.AddObject(1, &m_LoginJoltOr);
    m_LoginMenu.AddObject(1, &m_LoginJoltLogo);
    m_LoginMenu.AddObject(1, &m_LoginJoltStart);

#endif
}


// ****************************************************************
cMenu::~cMenu()
{
}


// ****************************************************************
void cMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

#if !defined(_CORE_ANDROID_)

    // move mouse with joystick
    Core::Input->UseMouseWithJoystick(0, 0, 1, 0.4f);
    Core::Input->UseMouseWithJoystick(1, 0, 1, 0.4f);

#endif

    // check for back button (especially important on Android)
    const bool bBackButton = Core::Input->GetKeyboardButton(SDL_SCANCODE_AC_BACK, CORE_INPUT_PRESS);

    // associate banner transparency with menu background
    SetBannerAlpha(m_BackgroundLeft.GetAlpha());

    // control the menu
    if(this->GetCurSurface() == 10 && !this->GetTransition().GetStatus()) 
    {
        // check for escape key, interrupt and pause button
#if defined(_CORE_ANDROID_)
        g_pGame->GetInterface()->InteractPause();     
        if(Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) || Core::System->GetMinimized() || bBackButton || g_pGame->GetInterface()->GetTouchPause()->IsClicked())
#else
        if(Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) || Core::System->GetMinimized() || bBackButton || Core::Input->GetJoystickButton(0, 1, CORE_INPUT_PRESS) || Core::Input->GetJoystickButton(1, 1, CORE_INPUT_PRESS))
#endif
        {
            // enter pause menu
            g_bPause = true;
            g_pMusicPlayer->Control()->SetVolume(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME) * 0.5f);
            this->ChangeSurface(11, 0.0f);
            m_ScoreMenu.ChangeSurface(4, 0.0f);
            Core::Input->ShowCursor(true);
        }
        else
        {
            if(g_pGame)
            {
                // check for game over
                if(g_pGame->GetStatus() == 1)
                {
                    if(g_pGame->GetChallenge()) this->End();
                    else
                    {
                        // enter submit menu
                        this->ChangeSurface(14, 1.0f);
                        m_ScoreMenu.ChangeSurface(0, 0.0f);
                        Core::Input->ShowCursor(true);

                        // catch current score and time value
                        if(g_pGame->GetTime() > 10.0f)
                        {
                            m_afSubmitValue[0] = (float)g_pGame->GetScore();
                            m_afSubmitValue[1] = g_pGame->GetTime();
                        }
                        else
                        {
                            m_afSubmitValue[0] = 0.0f;
                            m_afSubmitValue[1] = 0.0f;
                        }

                        // convert values
                        const int aiValue[2] = {(int)std::floor(m_afSubmitValue[0]),
                                                (int)std::floor(m_afSubmitValue[1]*100.0f)};

                        // check and save new best values, show new offline record
                        if(Core::Config->GetInt("Game", "Score", 0) < aiValue[0])
                        {
                            if(!g_pOnline->IsUserConnected()) {this->NewRecord(0); SHOW_BEST_SCORE(aiValue[0])}
                            Core::Config->SetInt("Game", "Score", 0, aiValue[0]);
                        }

                        if(Core::Config->GetInt("Game", "Time", 0) < aiValue[1])
                        {
                            if(!g_pOnline->IsUserConnected()) {this->NewRecord(1); SHOW_BEST_TIME(aiValue[1])}
                            Core::Config->SetInt("Game", "Time", 0, aiValue[1]);
                        }

                        // show new online record
                        if(g_pOnline->IsUserConnected())
                        {
                            if(      std::atoi(m_aScoreBestValue[0].GetText())  < aiValue[0])         this->NewRecord(0);
                            if(float(std::atof(m_aScoreBestValue[1].GetText())) < m_afSubmitValue[1]) this->NewRecord(1);
                        }

                        // display both values
                        m_aAfterBestValue[0].SetText(coreData::Print("%06.0f",      std::floor(m_afSubmitValue[0])));
                        m_aAfterBestValue[1].SetText(coreData::Print("%03.0f.%01d", std::floor(m_afSubmitValue[1]), int(std::floor(m_afSubmitValue[1] * 10.0f)) % 10));

                        // set submit status
                        m_bSubmited      = (m_afSubmitValue[1] >= 10.0f) ? false : true;
                        m_bInLeaderboard = false;
                        if(!m_bSubmited) 
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

    else if(this->GetCurSurface() == 6)
    {
        // check for main menu
        if(m_Start.IsClicked() || m_Short.IsClicked())
        {
            // start new game
            m_iStatus = 1;
            this->ChangeSurface(10, 1.0f);
            Core::Input->ShowCursor(false);
        }
        else if(m_Exit.IsClicked() || bBackButton)
        {
            // ask to exit the application
            this->ChangeSurface(7, 5.0f);
            m_Question.SetText(QUESTION_EXIT);
        }
    }

    else if(this->GetCurSurface() == 7)
    {
        // check for exit question
        m_Exit.SetFocus(true);
        if(m_Yes.IsClicked())
        {
            // exit the application
            Core::System->Quit();
        }
        else if(m_No.IsClicked() ||
                Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(0, 1,                CORE_INPUT_PRESS) || 
                Core::Input->GetJoystickButton(1, 1,                CORE_INPUT_PRESS))
        {
            // return to main menu
            this->ChangeSurface(6, 5.0f);
        }
    }

    else if(this->GetCurSurface() == 11)
    {
        // check for pause menu
        if(m_Resume.IsClicked() || m_Short.IsClicked()                           || 
           Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) || 
           Core::Input->GetJoystickButton(0, 1,                CORE_INPUT_PRESS) || 
           Core::Input->GetJoystickButton(1, 1,                CORE_INPUT_PRESS))
        {
            // resume current game
            g_bPause = false;
            g_pMusicPlayer->Control()->SetVolume(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME) * 1.0f);
            this->ChangeSurface(10, 0.0f);
            Core::Input->ClearMouseButton(1);
            Core::Input->ShowCursor(false);
        }
        else if(m_Abort.IsClicked() || bBackButton)
        {
            // ask to abort current game
            this->ChangeSurface(12, 5.0f);
            m_Question.SetText(QUESTION_ABORT);
        }
    }

    else if(this->GetCurSurface() == 12)
    {
        // check for abort question
        m_Abort.SetFocus(true);
        if(m_Yes.IsClicked())
        {
            // exit the current game
            this->End();
        }
        else if(m_No.IsClicked() ||
                Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(0, 1,                CORE_INPUT_PRESS) || 
                Core::Input->GetJoystickButton(1, 1,                CORE_INPUT_PRESS))
        {
            // return to pause menu
            this->ChangeSurface(11, 5.0f);
        }
    }

    else if(this->GetCurSurface() == 14)
    {
        // check for submit menu
        if(m_Submit.IsClicked())
        {
            if(!m_bSubmited)
            {
                if(g_pOnline->IsUserConnected()) this->SubmitScore(NULL); // re-send (should only be able on connection problems)
                else
                {
#if defined(_API_GOOGLE_PLAY_)

                    // login with Google Play Games
                    this->Login("", "");

#else
                    // open login window
                    this->ChangeSurface(this->GetCurSurface() + 2, 5.0f);
                    m_LoginMenu.ChangeSurface(1, 0.0f);
                    m_LoginOK.SetFocus(false);
                    m_LoginCancel.SetFocus(false);
                    m_LoginError.SetText(LOGIN_START);
                    if(!m_LoginGuest.GetText()[0]) m_LoginGuest.SetInput(true);

#endif
                }
            }
        }
        else if(m_Finish.IsClicked() || m_Short.IsClicked() || bBackButton)
        {
            if(m_bInLeaderboard)
            {
                // ask to finish the game without submitting
                this->ChangeSurface(15, 5.0f);
                m_Question.SetText(QUESTION_RETURN);
            }
            else
            {
                // finish the game
                this->End();
            }
        }
    }

    else if(this->GetCurSurface() == 15)
    {
        // check for finish question
        m_Finish.SetFocus(true);
        if(m_Yes.IsClicked())
        {
            // finish the game
            this->End();
        }
        else if(m_No.IsClicked() || 
                Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) ||
                Core::Input->GetJoystickButton(0, 1,                CORE_INPUT_PRESS) || 
                Core::Input->GetJoystickButton(1, 1,                CORE_INPUT_PRESS))
        {
            // return to submit menu
            this->ChangeSurface(14, 5.0f);
        }
    }

    // animate game logos
    const coreVector2 vLogoSize = coreVector2(0.512f,0.256f) * g_pBackground->GetFlash(0.3f);
    m_BigLogo.SetSize(vLogoSize * 1.40f);
    m_Logo.SetSize   (vLogoSize * 0.87f);

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
    m_BackgroundLeft.SetAlpha (m_BackgroundLeft.GetAlpha()  * MENU_ALPHA_ACTIVE_1);
    m_BackgroundRight.SetAlpha(m_BackgroundRight.GetAlpha() * MENU_ALPHA_ACTIVE_1);
    if(this->GetCurSurface() >= 10 && this->GetCurSurface() <= 16) m_Black.SetAlpha(m_Black.GetAlpha() * 0.5f);
    if(this->GetCurSurface() >= 6  && this->GetCurSurface() <= 15) m_QuestionBlack.SetAlpha(m_QuestionBlack.GetAlpha() * 0.75f);
    if(this->GetCurSurface() >= 6  && this->GetCurSurface() <= 16)
    {
        m_LoginPopup.SetAlpha(m_LoginPopup.GetAlpha() * MENU_ALPHA_ACTIVE_1);
        m_LoginBlack.SetAlpha(m_LoginBlack.GetAlpha() * 0.75f);
    }
    
    // set transparency of main menu buttons
    ALPHA_BUTTON_TEXT(m_Start);
    ALPHA_BUTTON_TEXT(m_Exit);
    ALPHA_BUTTON_TEXT(m_Resume);
    ALPHA_BUTTON_TEXT(m_Abort);
    ALPHA_BUTTON_TEXT(m_Finish);
    ALPHA_BUTTON_SCORE(m_ButtonScore,  0);
    ALPHA_BUTTON_SCORE(m_ButtonTime,   1);
    ALPHA_BUTTON_SCORE(m_ButtonTrophy, 4);
    ALPHA_BUTTON_SCORE(m_ButtonConfig, 5);
    ALPHA_BUTTON_INSIDE(m_PageChange);
    ALPHA_BUTTON_INSIDE(m_LoginConfigStart);
    ALPHA_BUTTON_INSIDE(m_Short);
    ALPHA_BUTTON_INSIDE(m_Yes);
    ALPHA_BUTTON_INSIDE(m_No);

#if defined(_API_GOOGLE_PLAY_) || defined(_CORE_DEBUG_)

    // set transparency of control buttons
    m_ControlType.GetArrow(0)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 0) ? 0.5f : 1.0f));
    m_ControlType.GetArrow(1)->SetAlpha(m_ControlType.GetAlpha() * ((m_ControlType.GetCurIndex() == 2) ? 0.5f : 1.0f));
    m_ControlType.GetCaption()->SetAlpha(m_ControlType.GetAlpha());
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(0));
    ALPHA_BUTTON_INSIDE(*m_ControlType.GetArrow(1));

    // set transparency of Google Play Games buttons
    if(g_pOnline->IsUserConnected())
    {
        ALPHA_BUTTON_INSIDE(m_GoogleFullScore);
        ALPHA_BUTTON_INSIDE(m_GoogleFullTrophy);
    }
    else
    {
        if(!g_bCoreDebug) m_LoginConfigStart.SetAlpha(0.0f);
        m_GoogleFullScore.SetAlpha(0.0f);
        m_GoogleFullTrophy.SetAlpha(0.0f);
    }
    if(m_aConnectionError[0].GetText()[0] && !m_aConnectionError[2].GetText()[0])
    {
        ALPHA_BUTTON_INSIDE(m_AuthButton);
        if(!g_bCoreDebug) m_PageChange.SetAlpha(0.0f);

        if(m_AuthButton.IsClicked())
        {
            // quick-login with Google Play Games
            if(!g_pOnline->IsUserConnected()) this->Login("", "");
        }       
    }
    else
    {
        m_AuthLogo.SetAlpha(0.0f);
        m_AuthButton.SetAlpha(0.0f);
    }
    
#endif

    // set transparency of highscore related objects
    if(m_bSubmited) m_Submit.SetAlpha(m_Submit.GetAlpha() * MENU_ALPHA_ACTIVE_2 * 0.5f);
    else ALPHA_BUTTON_TEXT(m_Submit); 

    if(!m_bSubmited || m_afSubmitValue[1] < 10.0f) 
        m_Successful.SetAlpha(0.0f);

    if(this->GetCurSurface() >= 14 && this->GetCurSurface() <= 16 && m_bInLeaderboard) 
        m_Short.SetAlpha(MENU_ALPHA_ACTIVE_1 * 0.5f);

    for(int i = 0; i < SCORE_TABLES; ++i)
        if(m_aScoreRecord[i].GetText()[0]) m_aScoreRank[i].SetAlpha(0.0f);

    // toggle score menu
         if(m_ButtonScore.IsClicked())  m_ScoreMenu.ChangeSurface(0, 5.0f);
    else if(m_ButtonTime.IsClicked())   m_ScoreMenu.ChangeSurface(1, 5.0f);
    else if(m_ButtonTrophy.IsClicked()) m_ScoreMenu.ChangeSurface(4, 5.0f);
    else if(m_ButtonConfig.IsClicked()) m_ScoreMenu.ChangeSurface(5, 5.0f);

    // control score menu (configuration)
    const bool bInNormalMenu = this->GetCurSurface() == 6 || this->GetCurSurface() == 11 || this->GetCurSurface() == 14;
    
    if(m_ScoreMenu.GetCurSurface() == 4 || (m_ScoreMenu.GetOldSurface() == 4 && m_ScoreMenu.GetTransition().GetStatus()))
    {
        for(int i = 0; i < TROPHIES; ++i)
        {
            // update trophy description
            if(((m_aTrophyImage[i].IsFocused() && m_iTrophyCurrent != i) || m_iTrophyCurrent < 0) && bInNormalMenu)
            {
                m_iTrophyCurrent = i;
                g_pOnline->GameJolt()->InterTrophy()->FetchTrophiesCall(GJ_TROPHY_ALL, this, &cMenu::FetchTrophiesCallback1, (void*)(long)i);
            }

            // set transparency of tropies
            m_aTrophyImage[i].SetAlpha(m_aTrophyImage[i].GetAlpha() * ((m_iTrophyCurrent == i) ? 0.5f : 1.0f));
            m_aTrophyCheck[i].SetAlpha(m_aTrophyCheck[i].GetAlpha() * ((m_iTrophyStatus & (1 << i)) ? 1.0f : 0.0f));
        }

        // set question marks
        if(m_iTrophyStatus & (1 << 0)) m_aTrophySecret[0].SetAlpha(0.0f);
        if(m_iTrophyStatus & (1 << 1)) m_aTrophySecret[1].SetAlpha(0.0f);
        if(m_iTrophyStatus & (1 << 2)) m_aTrophySecret[2].SetAlpha(0.0f);
        if(m_iTrophyStatus & (1 << 4)) m_aTrophySecret[3].SetAlpha(0.0f);
    }
    else if((m_ScoreMenu.GetCurSurface() == 5 || m_LoginConfigStart.IsClicked()) && bInNormalMenu)
    {
        // set transparency of configuration buttons
        ALPHA_BUTTON_TEXT(m_VideoLow);
        ALPHA_BUTTON_TEXT(m_VideoMedium);
        ALPHA_BUTTON_TEXT(m_VideoHigh);
        ALPHA_BUTTON_TEXT(m_AudioDrag);

        // toggle video quality
        int iNewQuality = -1;
             if(m_VideoLow.IsClicked())    iNewQuality = 0;
        else if(m_VideoMedium.IsClicked()) iNewQuality = 1;
        else if(m_VideoHigh.IsClicked())   iNewQuality = 2;

        if(iNewQuality >= 0)
        {
            if(iNewQuality != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY))
            {
                // display current selection
                m_VideoLow.SetColor3   ((iNewQuality == 0) ? COLOR_GREEN_F.xyz()  : coreVector3(0.5f,0.5f,0.5f));
                m_VideoMedium.SetColor3((iNewQuality == 1) ? COLOR_YELLOW_F.xyz() : coreVector3(0.5f,0.5f,0.5f));
                m_VideoHigh.SetColor3  ((iNewQuality == 2) ? COLOR_RED_F.xyz()    : coreVector3(0.5f,0.5f,0.5f));

                // set new video quality and reload all shaders
                Core::Config->SetInt(CORE_CONFIG_GRAPHICS_QUALITY, iNewQuality);
                this->ResetShaders();
            }
        }

        // drag audio volume (check for every sub-object from the volume bar)
        static bool bDragStatus = false; // # static
        if(m_AudioBar.IsClicked     (CORE_INPUT_LEFT, CORE_INPUT_HOLD) ||
           m_AudioBarBack.IsClicked (CORE_INPUT_LEFT, CORE_INPUT_HOLD) ||
           m_AudioDrag.IsClicked    (CORE_INPUT_LEFT, CORE_INPUT_HOLD))      bDragStatus = true;
        if(Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_RELEASE)) bDragStatus = false;

        if(bDragStatus)
        {
#if defined(_CORE_ANDROID_)

            // control volume with last finger position
            float fMove = 0.0f;
            Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&](const coreUint& i)
            {
                fMove = Core::Input->GetTouchPosition(i).x; // overwrite
            });

#else
            // control volume with mouse position
            const float fMove = Core::Input->GetMousePosition().x;

#endif
            // calculate range values of the volume bar
            const float fMaxSide  = (m_AudioBarBack.GetSize().x - m_AudioDrag.GetSize().x) * 0.5f;
            const float fMaxLeft  = m_AudioBarBack.GetPosition().x - fMaxSide;
            const float fMaxRight = m_AudioBarBack.GetPosition().x + fMaxSide;

            // calculate new controller position and volume
            const float fNewPosX = CLAMP((fMove + 0.5f) * Core::System->GetResolution().AspectRatio(), fMaxLeft, fMaxRight);
            const float fVolume  = (fNewPosX - fMaxLeft) * RCP(fMaxSide * 2.0f);

            // set controller
            m_AudioDrag.SetPosition(coreVector2(fNewPosX, m_AudioDrag.GetPosition().y));
            m_AudioDrag.Move();

            // set colored bar
            m_AudioBar.SetSize(coreVector2(fNewPosX - fMaxLeft, m_AudioBar.GetSize().y));
            m_AudioBar.SetPosition(coreVector2(LEFT_CENTER - (m_AudioBarBack.GetSize().x - m_AudioBar.GetSize().x) * 0.5f, m_AudioBar.GetPosition().y));
            m_AudioBar.Move();

            // set volume
            Core::Config->SetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME, fVolume * 10.0f);
            Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME, fVolume * 0.7f);
            g_pMusicPlayer->Control()->SetVolume(fVolume * (g_bPause ? 0.35f : 0.7f));
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
#if defined(_API_GOOGLE_PLAY_)

                // login with Google Play Games
                this->Login("", "");

#else
                // open login window
                this->ChangeSurface(this->GetCurSurface() + 2, 5.0f);
                m_LoginMenu.ChangeSurface(0, 0.0f);
                m_LoginOK.SetFocus(false);
                m_LoginCancel.SetFocus(false);
                m_LoginError.SetText(LOGIN_START);
                     if(!m_LoginName.GetText()[0])  m_LoginName.SetInput(true);
                else if(!m_LoginToken.GetText()[0]) m_LoginToken.SetInput(true);

#endif
            }
        }

#if defined(_API_GOOGLE_PLAY_)
        
        // apply and save control changes
        if(m_ControlType.IsClicked())
        {
            if(Core::Config->GetInt("Game", "Control", 0) != m_ControlType.GetCurIndex())
            {
                Core::Config->SetInt("Game", "Control", 0, m_ControlType.GetCurIndex());

                // also change on running game
                if(g_pGame) g_pGame->GetInterface()->ChangeControlType(m_ControlType.GetCurIndex());
            }
        }

#endif
    }
    else if(m_ScoreMenu.GetCurSurface() < 2)
    {
        // change score-page
        if(m_PageChange.IsClicked() && m_PageChange.GetAlpha())
        {
            if(++m_iCurPage >= SCORE_PAGES) m_iCurPage = 0;
            this->RetrieveScoresCallback3(0);
            this->RetrieveScoresCallback3(1);
        }
    }

    // control login menu
    if(this->GetCurSurface() == 8 || this->GetCurSurface() == 13 || this->GetCurSurface() == 16)
    {
        // set transparency of login buttons
        ALPHA_BUTTON_INSIDE(m_LoginOK);
        ALPHA_BUTTON_INSIDE(m_LoginCancel);
        ALPHA_BUTTON_INSIDE(m_LoginJoltStart);

        // set color of text boxes
        m_LoginName.SetColor3 (m_LoginName.IsFocused()  ? coreVector3(0.5f,0.5f,0.5f) : coreVector3(0.25f,0.25f,0.25f));
        m_LoginToken.SetColor3(m_LoginToken.IsFocused() ? coreVector3(0.5f,0.5f,0.5f) : coreVector3(0.25f,0.25f,0.25f));
        m_LoginGuest.SetColor3(m_LoginGuest.IsFocused() ? coreVector3(0.5f,0.5f,0.5f) : coreVector3(0.25f,0.25f,0.25f));

        // switch from guest login to normal login
        if(m_LoginJoltStart.IsClicked())
        {
            m_LoginMenu.ChangeSurface(0, 5.0f);
                 if(!m_LoginName.GetText()[0])  m_LoginName.SetInput(true);
            else if(!m_LoginToken.GetText()[0]) m_LoginToken.SetInput(true);
            m_bFromGuest = true;
        }

        if(!g_pOnline->GetNumConnections()) // prevent multiple login attempts, or window-close during login attempt
        {
            // start and cancel login
            bool bDoLogin = m_LoginOK.IsClicked();
            if(m_LoginCancel.IsClicked() || bBackButton)
            {
                // switch back to guest login or cancel
                if(m_bFromGuest) m_LoginMenu.ChangeSurface(1, 5.0f);
                else this->ChangeSurface(this->GetCurSurface() - 2, 5.0f);
                m_bFromGuest = false;
            }

            if(m_LoginMenu.GetCurSurface() == 0)
            {
                if(Core::Input->GetKeyboardButton(SDL_SCANCODE_RETURN, CORE_INPUT_PRESS) || Core::Input->GetKeyboardChar() == SDLK_RETURN)
                {
                    // switch input focus or start login with enter-key
                         if(!m_LoginName.GetText()[0])  m_LoginName.SetInput(true);
                    else if(!m_LoginToken.GetText()[0]) m_LoginToken.SetInput(true);
                    else bDoLogin = true;
                }
                else if(Core::Input->GetKeyboardButton(SDL_SCANCODE_TAB, CORE_INPUT_PRESS) || Core::Input->GetKeyboardChar() == SDLK_TAB)
                {
                    // switch input focus with tab-key
                         if(m_LoginName.GetInput())  {m_LoginName.SetInput(false); m_LoginToken.SetInput(true);}
                    else if(m_LoginToken.GetInput()) {m_LoginName.SetInput(true);  m_LoginToken.SetInput(false);}
                }
            }

            if(bDoLogin)
            {
                if(m_LoginMenu.GetCurSurface() == 0)
                {
                    // start login
                    if(this->Login(m_LoginName.GetText(), m_LoginToken.GetText()) == GJ_OK)
                        m_LoginError.SetText(LOGIN_CONNECTING);
                }
                else
                {
                    if(m_LoginGuest.GetText()[0])
                    {
                        // send guest score
                        this->SubmitScore(m_LoginGuest.GetText());

                        // close login menu
                        this->ChangeSurface(this->GetCurSurface() - 2, 5.0f);
                    }
                }
                m_bFromGuest = false;
            }
        }
    }

    // update score tables
    if(m_iTableUpdate)
    {
        for(int i = 0; i < SCORE_TABLES; ++i)
        {
            if(m_iTableUpdate & (1 << i)) 
            {
                m_iTableUpdate &= ~(1 << i);
                this->RetrieveScoresCallback3(i); 
            }
        }
    }

#if defined(_API_GOOGLE_PLAY_)

    // open Google Play Games screens
    if(m_GoogleFullTrophy.IsClicked()) g_pOnline->OpenTrophy();
    if(m_GoogleFullScore.IsClicked())  g_pOnline->OpenScore();

#endif

#if defined(_CORE_ANDROID_)

    // update current battery status
    int aiPower[2];
    SDL_GetPowerInfo(&aiPower[0], &aiPower[1]);
    if(aiPower[0] >= 0)
    {
        // show remaining time if available
        const int iHour =  aiPower[0] / 3600;
        const int iMin  = (aiPower[0] % 3600) / 60;
        m_TopBatteryValue.SetText(coreData::Print("%d:%02d", iHour, iMin));
    }
    else if(aiPower[1] >= 0)
    {
        // show remaining percent
        m_TopBatteryValue.SetText(coreData::Print("%d%%", aiPower[1]));
    }

#elif defined(_CORE_DEBUG_)

    m_TopBatteryValue.SetText("50%");

#endif

    // calculate and display current FPS (# static)
    static float fFPSValue = 0.0f;
    if(Core::System->GetTime()) fFPSValue = fFPSValue * 0.95f + RCP(Core::System->GetTime()) * 0.05f;
    m_TopFPSSec.SetText(coreData::Print("%.0f.", std::floor(fFPSValue)));
    m_TopFPSMil.SetText(coreData::Print("%01d",  int(std::floor(fFPSValue * 10.0f)) % 10));

    // adjust color in relation to the FPS
    const float fFPSLerp        = MIN(fFPSValue * 0.01666667f, 1.0f);
    const coreVector3 vFPSColor = LERP(COLOR_RED_F.xyz(), LERP(COLOR_YELLOW_F.xyz(), COLOR_GREEN_F.xyz(), MAX((fFPSLerp-0.5f) * 2.0f, 0.0f)), MIN(fFPSLerp * 2.0f, 1.0f));
    m_TopFPSTacho.SetColor3(vFPSColor);
    m_TopFPSSec.SetColor3(vFPSColor);
    m_TopFPSMil.SetColor3(vFPSColor);

#if defined(_CORE_DEBUG_)

    // show full network load
    m_TopUpdating.SetText(coreData::Print("COMMUNICATING WITH SERVER... (%u)", g_pOnline->GetNumConnections()));
        
#endif

    // show light network load
    if(g_pOnline->GetNumConnections())
    {
        m_Loading.SetDirection(coreVector2::Direction((float)Core::System->GetTotalTime() * 4.0f));
        m_Loading.Move();
    }
    else m_Loading.SetAlpha(0.0f);

    // update intro
    m_Intro.Update(1.0f);
    if(m_Intro.GetStatus())
    {
        // skip intro when pressing specific keys
        if(Core::Input->GetMouseButton(1, CORE_INPUT_PRESS) ||
           Core::Input->GetKeyboardButton(SDL_SCANCODE_ESCAPE, CORE_INPUT_PRESS) || 
           Core::Input->GetKeyboardButton(SDL_SCANCODE_RETURN, CORE_INPUT_PRESS) || 
           Core::Input->GetKeyboardButton(SDL_SCANCODE_SPACE,  CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(0, 0,                CORE_INPUT_PRESS) || 
           Core::Input->GetJoystickButton(1, 0,                CORE_INPUT_PRESS) ||
           Core::Input->GetJoystickButton(0, 1,                CORE_INPUT_PRESS) || 
           Core::Input->GetJoystickButton(1, 1,                CORE_INPUT_PRESS) ||
           bBackButton)
        {
            this->ChangeSurface(6, 1.0f);
            m_Intro.Stop();
        }

        // change surface over time
             if(this->GetCurSurface() == 0 && m_Intro.GetCurrent(false) >= 0.0f) this->ChangeSurface(1, 1.0f);
        else if(this->GetCurSurface() == 1 && m_Intro.GetCurrent(false) >= 3.0f) this->ChangeSurface(2, 1.0f);
        else if(this->GetCurSurface() == 2 && m_Intro.GetCurrent(false) >= 5.5f) this->ChangeSurface(3, 1.0f);
        else if(this->GetCurSurface() == 3 && m_Intro.GetCurrent(false) >= 8.0f)
        {
            this->ChangeSurface(6, 1.0f);
            m_Intro.Stop();
        }
    }
}


// ****************************************************************
void cMenu::End()
{
    if(!g_pGame) return;

    // reset pause and menu status
    g_bPause  = false;
    m_iStatus = 2;
    Core::Input->ShowCursor(true);

    // reset record notification
    this->ResetRecord();

    // flash screen and return to main menu
    this->ChangeSurface(5, 0.0f);
    this->ChangeSurface(6, 1.0f);
    m_ScoreMenu.ChangeSurface(0, 0.0f);
    m_White.SetAlpha(1.0f);
    m_Black.SetAlpha(0.0f);
    m_Finish.SetFocus(false);

    // update leaderboards
    m_bSubmited = true;
    if(!g_pGame->GetStatus() && g_pGame->GetTime() >= 10.0f)
        this->RetrieveScores();
}


// ****************************************************************
// reset all relevant shaders after quality changes
void cMenu::ResetShaders()
{
    // invoke a refresh on all relevant shader-programs
    Core::Manager::Memory->Share<coreProgram>("fill_shader")->Refresh();
    Core::Manager::Memory->Share<coreProgram>("floor_shader")->Refresh();
    Core::Manager::Memory->Share<coreProgram>("floor_plate_shader")->Refresh();

    // define shader reset lambda
    auto ResetShaderLambda = [](const char* pcPath)
    {
        coreShaderPtr pShader = Core::Manager::Resource->LoadFile<coreShader>(pcPath);
        pShader->Unload();

        coreFile* pFile = Core::Manager::Resource->RetrieveFile(pcPath);
        pShader->Load(pFile);
        pFile->UnloadData();
    };

    // unload and reload all relevant shaders
    ResetShaderLambda("data/shaders/fill.vs");
    ResetShaderLambda("data/shaders/fill.fs");
    ResetShaderLambda("data/shaders/floor.vs");
    ResetShaderLambda("data/shaders/floor.fs");
    ResetShaderLambda("data/shaders/floor_plate.vs");
    ResetShaderLambda("data/shaders/floor_plate.fs");
}


// ****************************************************************
void cMenu::NewRecord(const coreByte& iIndex)
{
    SDL_assert(iIndex < SCORE_TABLES);

    const bool bBitTooSmall = Core::System->GetResolution().AspectRatio() < 1.4f;

    // set score color
    m_aScoreBestValue[iIndex].SetColor3(COLOR_YELLOW_F.xyz());
    m_aAfterBestValue[iIndex].SetColor3(COLOR_YELLOW_F.xyz());

    // set notification text
    m_aScoreRecord[iIndex].SetText("NEW RECORD");
    m_aAfterRecord[iIndex].SetText(bBitTooSmall ? "" : "NEW RECORD");
}


// ****************************************************************
void cMenu::ResetRecord()
{
    for(int i = 0; i < SCORE_TABLES; ++i)
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
void cMenu::SetErrorMessage(const coreVector3& vColor, const char* pcMessage1, const char* pcMessage2, const char* pcMessage3)
{
    if(pcMessage1 && pcMessage2 && pcMessage3)
    {
        // show error message (currently only for Android to reduce panic, empty text has no overhead)
        m_aConnectionError[0].SetText(pcMessage1);
        m_aConnectionError[1].SetText(pcMessage2);
        m_aConnectionError[2].SetText(pcMessage3);

        // set message color
        for(int i = 0; i < 3; ++i) m_aConnectionError[i].SetColor3(vColor);
    }
    else
    {
        // hide error message
        m_aConnectionError[0].SetText("");
        m_aConnectionError[1].SetText("");
        m_aConnectionError[2].SetText("");
    }
}


// ****************************************************************
void cMenu::SubmitScore(const char* pcGuestName)
{
    SDL_assert(g_pGame);
    if(m_bSubmited) return;

    // convert values
    const int aiValue[2] = {(int)std::floor(m_afSubmitValue[0]),
                            (int)std::floor(m_afSubmitValue[1]*100.0f)};
    
    // save guest name
    if(pcGuestName) Core::Config->SetString("Game", "Guest", NULL, pcGuestName);

    // create extra-data string
    const std::string sExtra = coreData::Print("%.0f %.3f - %d - %d %d %d %d %d", m_afSubmitValue[0], m_afSubmitValue[1], g_pGame->GetMaxCombo(), 
                                               g_pGame->GetStat(0), g_pGame->GetStat(1), g_pGame->GetStat(2), g_pGame->GetStat(3), g_pGame->GetStat(4));

    // send score and time values
    g_pOnline->SubmitScore(GJ_LEADERBOARD_01, coreData::Print("%d Points",    aiValue[0]),                                aiValue[0], sExtra, pcGuestName ? pcGuestName : "", this, &cMenu::SubmitScoreCallback, NULL);
    g_pOnline->SubmitScore(GJ_LEADERBOARD_02, coreData::Print("%.1f Seconds", std::floor(m_afSubmitValue[1]*10.0f)*0.1f), aiValue[1], sExtra, pcGuestName ? pcGuestName : "", this, &cMenu::SubmitScoreCallback, NULL);
}


// ****************************************************************
void cMenu::SubmitScoreCallback(const gjScorePtr& pScore, void* pData)
{
    if(!pScore) return;   // error submitting

    // update status
    m_bSubmited      = true;
    m_bInLeaderboard = false;

    if(g_pGame)
    {
        // achieve submit-trophy
        g_pGame->AchieveTrophy(GJ_TROPHY_04, 3);
    }

    // fetch all top values
    g_pOnline->FetchScores(pScore->GetScoreTable()->GetID(), false, SCORE_ENTRIES * SCORE_PAGES, this, &cMenu::RetrieveScoresCallback2, (void*)0);

    // fetch best values of the current user
    if(g_pOnline->IsUserConnected()) g_pOnline->FetchScores(pScore->GetScoreTable()->GetID(), true, 1, this, &cMenu::RetrieveScoresCallback2, (void*)1);
}


// ****************************************************************
void cMenu::RetrieveScores()
{
    // reset current score page
    m_iCurPage = 0;
    this->RetrieveScoresCallback3(0);
    this->RetrieveScoresCallback3(1);

    // fetch leaderboards
    g_pOnline->FetchLeaderboards(this, &cMenu::RetrieveScoresCallback1, NULL);
}


// ****************************************************************
void cMenu::RetrieveScoresCallback1(const gjScoreTableMap& apTable, void* pData)
{
    FOR_EACH(it, apTable)
    {
        // fetch all top values
        g_pOnline->FetchScores(it->second->GetID(), false, SCORE_ENTRIES * SCORE_PAGES, this, &cMenu::RetrieveScoresCallback2, (void*)0);

        // fetch best values of the current user
        if(g_pOnline->IsUserConnected()) g_pOnline->FetchScores(it->second->GetID(), true, 1, this, &cMenu::RetrieveScoresCallback2, (void*)1);
    }
}


// ****************************************************************
void cMenu::RetrieveScoresCallback2(const gjScoreList& apScore, void* pData)
{
    if(apScore.empty()) return;

    const int iTableID  = apScore[0]->GetScoreTable()->GetID();
    const int iTableNum = (iTableID == GJ_LEADERBOARD_01) ? 0 : 1;

    if(!pData)
    {
        // copy scores for better paging
        m_aapCurScores[iTableNum] = apScore;

        // update leaderboard
        this->RetrieveScoresCallback3(iTableNum);

        if(g_pGame)
        {
            // mark values which would make it into the visual leaderboard
            if(iTableID == GJ_LEADERBOARD_01)
            {
                // check for score
                const bool bInLeaderboard = m_afSubmitValue[0] > float((apScore.size() >= SCORE_ENTRIES * SCORE_PAGES) ? apScore[(SCORE_ENTRIES * SCORE_PAGES) - 1]->GetSort() : 0);
                if(!m_bSubmited) m_bInLeaderboard |= bInLeaderboard;
            }
            else // == GJ_LEADERBOARD_02
            {
                // check for time
                const bool bInLeaderboard = std::floor(m_afSubmitValue[1]*100.0f) > float((apScore.size() >= SCORE_ENTRIES * SCORE_PAGES) ? apScore[(SCORE_ENTRIES * SCORE_PAGES) - 1]->GetSort() : 0);
                if(!m_bSubmited) m_bInLeaderboard |= bInLeaderboard;
            }
        }
    }
    else
    {
        // fill best values of the current user
        if(iTableID == GJ_LEADERBOARD_01)
            SHOW_BEST_SCORE(apScore[0]->GetSort())
        else // == GJ_LEADERBOARD_02
            SHOW_BEST_TIME(apScore[0]->GetSort())

#if defined(_API_GOOGLE_PLAY_)

        // show player rank
        if(g_pOnline->IsUserConnected()) 
            m_aScoreRank[iTableNum].SetText(apScore[0]->GetExtraData().c_str());

#endif
    }
}


// ****************************************************************   
void cMenu::RetrieveScoresCallback3(const int& iTableNum)
{
    SDL_assert(iTableNum < SCORE_TABLES);
    SDL_assert(m_iCurPage < SCORE_PAGES);

    // set table and page
    const gjScoreList& apScore = m_aapCurScores[iTableNum];
    const int iScoreStart = m_iCurPage * 8;

    // loop trough all retrieved leaderboard entries
    for(int i = 0; i < SCORE_ENTRIES; ++i)
    {
        gjScore* pScore  = (i + iScoreStart < (int)apScore.size()) ? apScore[i + iScoreStart] : NULL;
        const bool bOver = pScore ? (pScore->GetUserName().length() > 16) : false;

        if(iTableNum == 0) // == GJ_LEADERBOARD_01
        {
            // fill score leaderboard
            m_aaScoreEntry[0][i][0].SetText(coreData::Print("%d.", i+1 + iScoreStart));
            m_aaScoreEntry[0][i][1].SetText(pScore ? (pScore->GetUserName().substr(0, 16) + (bOver ? ">" : "")).c_str() : "-");
            m_aaScoreEntry[0][i][2].SetText(pScore ? coreData::Print("%06d", pScore->GetSort()) : "-");

            // highlight best players
            m_aaScoreEntry[0][i][1].SetColor3(LERP(COLOR_YELLOW_F.xyz(), COLOR_WHITE_F.xyz(), float(MIN(i + iScoreStart, 3) / 3)));
        }
        else // == GJ_LEADERBOARD_02
        {
            // fill time leaderboard
            m_aaScoreEntry[1][i][0].SetText(coreData::Print("%d.", i+1 + iScoreStart));
            m_aaScoreEntry[1][i][1].SetText(pScore ? (pScore->GetUserName().substr(0, 16) + (bOver ? ">" : "")).c_str() : "-");
            m_aaScoreEntry[1][i][2].SetText(pScore ? coreData::Print("%03d.%01d", pScore->GetSort() / 100, ((pScore->GetSort() % 100) / 10)) : "-");
        
            // highlight best players
            m_aaScoreEntry[1][i][1].SetColor3(LERP(COLOR_YELLOW_F.xyz(), COLOR_WHITE_F.xyz(), float(MIN(i + iScoreStart, 3) / 3)));
        }
    }
}


// ****************************************************************   
void cMenu::FetchTrophies()
{
    // fetch trophies
    g_pOnline->FetchTrophies(this, &cMenu::FetchTrophiesCallback2, NULL);
}


// ****************************************************************   
void cMenu::FetchTrophiesCallback1(const gjTrophyList& apTrophy, void* pData)
{
    const int iNum = (long)pData;
    if(iNum >= (int)apTrophy.size()) return;

    // set trophy title
    m_TrophyName.SetText(apTrophy[iNum]->GetTitle().c_str());

    // split trophy description into two even parts
    const char* pcDesc = coreData::Print("%s", (m_iTrophyStatus & (1 << iNum) || !apTrophy[iNum]->IsSecret()) ? apTrophy[iNum]->GetDescriptionTrue().c_str() : GJ_API_TEXT_SECRET);
    coreUint iMid = std::strlen(pcDesc) / 2;
    while(iMid > 0 && pcDesc[iMid] != ' ') --iMid;

    // set trophy description
    m_aTrophyDesc[0].SetText(pcDesc, iMid);
    m_aTrophyDesc[1].SetText(pcDesc + iMid);
}


// ****************************************************************   
void cMenu::FetchTrophiesCallback2(const gjTrophyList& apTrophy, void* pData)
{
    // save trophy status
    m_iTrophyStatus = 0;
    for(int i = 0; i < MIN((int)apTrophy.size(), TROPHIES); ++i)
        if(apTrophy[i]->IsAchieved()) m_iTrophyStatus |= (1 << i);

    // reset current trophy
    this->FetchTrophiesCallback1(apTrophy, (void*)0);
    m_iTrophyCurrent = 0;
}


// ****************************************************************   
int cMenu::QuickPlay()
{
    // load guest name and local values
    m_LoginGuest.SetText(Core::Config->GetString("Game", "Guest", ""));
    SHOW_BEST_SCORE(Core::Config->GetInt("Game", "Score", 0))
    SHOW_BEST_TIME (Core::Config->GetInt("Game", "Time",  0))
    m_iTrophyStatus = Core::Config->GetInt("Game", "Trophy", 0);

    // check for quickplay
    int iStatus = g_pOnline->Login("", "", this, &cMenu::LoginCallback, (void*)0);
  
#if !defined(_API_GOOGLE_PLAY_)

    if(iStatus != GJ_OK)
    {
        // check for saved credentials
        iStatus = g_pOnline->Login(Core::Config->GetString("Game", "Name", ""), Core::Config->GetString("Game", "Token", ""), this, &cMenu::LoginCallback, (void*)0);

        // fetch leaderboards early on failure
        if(iStatus != GJ_OK) g_pMenu->RetrieveScores();
    }

#endif

    return iStatus;
}


// ****************************************************************   
int cMenu::Login(const char* pcName, const char* pcToken)
{
    // try to login the specific user
    return g_pOnline->Login(pcName, pcToken, this, &cMenu::LoginCallback, (void*)1);
}


// ****************************************************************   
void cMenu::LoginCallback(const int& iStatus, void* pData)
{
    if(iStatus == GJ_OK)
    {
        // login successful
        m_LoginName.SetText(g_pOnline->GetUserName());
        m_LoginToken.SetText(g_pOnline->GetUserToken());
        m_BottomLoginName.SetText(coreData::Print("LOGGED IN AS %s", coreData::StrUpper(g_pOnline->GetUserName())));

        // save credentials
        Core::Config->SetString("Game", "Name",  NULL, g_pOnline->GetUserName());
        Core::Config->SetString("Game", "Token", NULL, g_pOnline->GetUserToken());

        // submit or check scores
        this->ResetRecord();
        if(!m_bSubmited) this->SubmitScore(NULL);
                    else this->RetrieveScores();

        // fetch trophies
        this->FetchTrophies();

        // change login button
        m_LoginConfigOr.SetText("LOG OUT OF");
        m_LoginConfigStart.Construct("data/textures/button_logout.png", "data/textures/button_logout.png");

#if !defined(_API_GOOGLE_PLAY_)

        // close login menu
        if(pData) this->ChangeSurface(this->GetCurSurface() - 2, 5.0f);

#endif
    }
    else
    {
        // login failed
             if(iStatus == GJ_NETWORK_ERROR)  m_LoginError.SetText(LOGIN_ERROR_CONNECTION);
        else if(iStatus == GJ_REQUEST_FAILED) m_LoginError.SetText(LOGIN_ERROR_CREDENTIALS);
    }
}


// ****************************************************************   
void cMenu::Logout()
{
    // logout
    g_pOnline->Logout();

    // reset credentials
    Core::Config->SetString("Game", "Name",  NULL, "");
    Core::Config->SetString("Game", "Token", NULL, "");

    // load local values
    SHOW_BEST_SCORE(Core::Config->GetInt("Game", "Score", 0))
    SHOW_BEST_TIME (Core::Config->GetInt("Game", "Time",  0))
    m_iTrophyStatus = Core::Config->GetInt("Game", "Trophy", 0);
    this->ResetRecord();

    // change login button
    m_LoginConfigOr.SetText("LOG INTO");
    m_LoginConfigStart.Construct("data/textures/button_login.png", "data/textures/button_login.png");

#if defined(_API_GOOGLE_PLAY_)

    for(int i = 0; i < SCORE_TABLES; ++i)
    {
        // reset player rank
        m_aScoreRank[i].SetText("");

        // remove all scores from the leaderboards
        m_aapCurScores[i].clear();
        for(int j = 0; j < SCORE_ENTRIES; ++j)
        {
            m_aaScoreEntry[i][j][1].SetText("-");
            m_aaScoreEntry[i][j][2].SetText("-");
        }
    }

    // reset score paging
    m_iCurPage = 0;
    for(int i = 0; i < SCORE_TABLES; ++i)
        this->InvokeScoreUpdate(i);

#endif
}