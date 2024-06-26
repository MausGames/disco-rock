//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_MENU_H_
#define _DR_GUARD_MENU_H_


// ****************************************************************
// menu definitions
#define LEFT_CENTER (0.46f)   // fixed center-value for the left menu
#define NAME_LEN    (18u)     // max characters to display for score names

// transparency values
#define MENU_ALPHA_ACTIVE_1 (0.8f)
#define MENU_ALPHA_IDLE_1   (0.5f)
#define MENU_ALPHA_ACTIVE_2 (0.65f)
#define MENU_ALPHA_IDLE_2   (0.95f)

// values for menu object adjustments
#define ADJUST_LEFT(x)   {(x).SetCenter(coreVector2(-0.5f,0.0f)); (x).SetAlignment(coreVector2( 1.0f,0.0f));}
#define ADJUST_RIGHT(x)  {(x).SetCenter(coreVector2( 0.5f,0.0f)); (x).SetAlignment(coreVector2(-1.0f,0.0f));}
#define ADJUST_BORDER(x) {(x).SetTexSize(coreVector2(0.62f,0.62f) / (x).GetSize() * 0.0165f);}

// macro function for changing object transparency
#define ALPHA_BUTTON_TEXT(b)                                                                                                   \
    {                                                                                                                          \
        const coreFloat a = (b).IsFocused() ? MENU_ALPHA_ACTIVE_2 : MENU_ALPHA_IDLE_2;                                         \
        (b).SetAlpha(b.GetAlpha() * a);                                                                                        \
    }
#define ALPHA_BUTTON_SCORE(b,i)                                                                                                \
    {                                                                                                                          \
        const coreFloat a = ((b).IsFocused() || m_ScoreMenu.GetCurSurface() == (i)) ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1; \
        (b).SetAlpha((b).GetAlpha() * a);                                                                                      \
    }
#define ALPHA_BUTTON_INSIDE(b)                                                                                                 \
    {                                                                                                                          \
        const coreFloat a = (b).IsFocused() ? MENU_ALPHA_IDLE_1 : MENU_ALPHA_ACTIVE_1;                                         \
        (b).SetAlpha((b).GetAlpha() * a);                                                                                      \
    }

// static texts
#define LOGIN_START             "YOUR TOKEN IS NOT YOUR PASSWORD"
#define LOGIN_CONNECTING        "CONNECTING..."
#define LOGIN_ERROR_CONNECTION  "COULD NOT CONNECT TO GAME JOLT"
#define LOGIN_ERROR_CREDENTIALS "WRONG USER NAME OR TOKEN"

#define QUESTION_EXIT   "EXIT GAME?"
#define QUESTION_ABORT  "ABORT GAME?"
#define QUESTION_RETURN "RETURN WITHOUT SENDING SCORE?"

// update displayed score and time value
#define SHOW_BEST_SCORE(x) {m_aScoreBestValue[0].SetText(PRINT("%06d", (x)));}
#define SHOW_BEST_TIME(x)  {m_aScoreBestValue[1].SetText(PRINT("%03d.%01d", (x) / 100, (((x) % 100) / 10)));}


// ****************************************************************
// menu class
class cMenu final : public coreMenu
{
private:
    coreObject2D m_MausLogo;
    coreObject2D m_BigLogo;

    coreFullscreen m_Black;
    coreFullscreen m_White;
    coreObject2D   m_BackgroundLeft;
    coreObject2D   m_BackgroundRight;

    coreObject2D m_Logo;
    coreLabel    m_Pause;

    coreButton m_Start;
    coreButton m_Exit;
    coreButton m_Resume;
    coreButton m_Abort;
    coreButton m_Submit;
    coreButton m_Finish;

    coreButton m_Short;
    coreButton m_ButtonScore;
    coreButton m_ButtonTime;
    coreButton m_ButtonTrophy;
    coreButton m_ButtonConfig;

    coreLabel m_TopUpdating;

    coreLabel    m_BottomInfo;
    coreLabel    m_BottomCredit;
    coreLabel    m_BottomLoginName;
    coreObject2D m_BottomLoginJolt;

    coreFullscreen m_QuestionBlack;
    coreLabel      m_Question;
    coreButton     m_Yes;
    coreButton     m_No;

    coreLabel  m_VideoText;
    coreButton m_VideoLow;
    coreButton m_VideoMedium;
    coreButton m_VideoHigh;

    coreLabel    m_AudioText;
    coreObject2D m_AudioBar;
    coreObject2D m_AudioBarBack;
    coreObject2D m_AudioIconHigh;
    coreObject2D m_AudioIconLow;
    coreButton   m_AudioDrag;

    coreLabel    m_LoginConfigOr;
    coreObject2D m_LoginConfigLogo;
    coreButton   m_LoginConfigStart;

    coreLabel       m_ControlText;
    coreSwitchBoxU8 m_ControlType;

    coreFullscreen m_LoginBlack;
    coreObject2D   m_LoginPopup;
    coreLabel      m_aLoginText[2];
    coreLabel      m_LoginEnterName;
    coreLabel      m_LoginEnterToken;
    coreLabel      m_LoginEnterGuest;
    coreTextBox    m_LoginName;
    coreTextBox    m_LoginToken;
    coreTextBox    m_LoginGuest;
    coreButton     m_LoginOK;
    coreButton     m_LoginCancel;
    coreLabel      m_LoginError;
    coreLabel      m_LoginJoltOr;
    coreObject2D   m_LoginJoltLogo;
    coreButton     m_LoginJoltStart;

    coreObject2D m_Successful;

    coreLabel  m_aScoreTable    [SCORE_TABLES];
    coreLabel  m_aaScoreEntry   [SCORE_TABLES][SCORE_ENTRIES][3];
    coreLabel  m_aScoreBest     [SCORE_TABLES];
    coreLabel  m_aScoreBestValue[SCORE_TABLES];
    coreLabel  m_aScoreRecord   [SCORE_TABLES];
    coreLabel  m_aScoreRank     [SCORE_TABLES];
    coreButton m_PageChange;

    coreLabel m_aAfterBest     [SCORE_TABLES];
    coreLabel m_aAfterBestValue[SCORE_TABLES];
    coreLabel m_aAfterRecord   [SCORE_TABLES];

    coreLabel    m_TrophyText;
    coreObject2D m_aTrophyImage [TROPHY_ITEMS];
    coreObject2D m_aTrophyCheck [TROPHY_ITEMS];
    coreLabel    m_aTrophySecret[TROPHY_SECRETS];
    coreLabel    m_TrophyName;
    coreLabel    m_aTrophyDesc[2];

    coreObject2D m_Loading;

    coreMenu m_ScoreMenu;
    coreMenu m_LoginMenu;

    coreTimer    m_Intro;                                   // intro timer
    coreSoundPtr m_pClickSound;                             // click sound-effect for buttons
    coreSoundPtr m_pHappySound;                             // happy sound-effect for special events
    coreSoundPtr m_pFlashSound;                             // sound-effect when flashing the screen

    coreProtect<coreFloat> m_afSubmitValue[SCORE_TABLES];   // fetched values after loosing the game
    coreBool m_bSubmitted;                                  // values where successfully submitted
    coreBool m_bInLeaderboard;                              // current values would be in the visible leaderboard (ask to submit before leaving)

    gjScoreList m_aapCurScores[SCORE_TABLES];               // saved score pointers for better leaderboard paging
    coreUint8   m_iCurPage;                                 // current page in the leaderboards
    coreUint8   m_iTableUpdate;                             // status to update the current score table display synchronous

    coreUint32 m_iTrophyStatus;                             // achieve-status of all trophies (per bit)
    coreInt8   m_iTrophyCurrent;                            // current selected trophy

    coreBool m_bFromGuest;                                  // login-menu has to go back to guest-login on cancel


public:
    cMenu()noexcept;

    DISABLE_COPY(cMenu)

    // move the menu
    void Move()final;

    // end current game and return to main menu
    void End();

    // reset all relevant shaders after quality changes
    void ResetShaders();

    // reset fullscreen-objects after resolution changes
    void ResetResolution();

    // control notification for new record
    void NewRecord(const coreUintW iIndex);
    void ResetRecord();

    // update current control type
    inline void UpdateControl() {m_ControlType.SelectIndex(CLAMP(Core::Config->GetInt("Game", "Control", 0), 0, 2));}

    // play sound-effects
    inline void PlayClickSound() {m_pClickSound ->PlayRelative(NULL, 0.08f, 1.0f, false, 0u);}
    inline void PlayHappySound() {m_pHappySound ->PlayRelative(NULL, 0.08f, 1.0f, false, 0u);}
    inline void PlayFlashSound() {m_pFlashSound ->PlayRelative(NULL, 0.11f, 1.0f, false, 0u);}

    // invoke score table update
    inline void InvokeScoreUpdate(const coreUintW iTableNum) {ADD_BIT(m_iTableUpdate, iTableNum)}

    // submit score functions
    void SubmitScore        (const coreChar* pcGuestName);
    void SubmitScoreCallback(const gjScorePtr& pScore, void* pData);

    // retrieve score functions
    void RetrieveScores();
    void RetrieveScoresCallback1(const gjScoreTableMap& apTable, void* pData);
    void RetrieveScoresCallback2(const gjScoreList&     apScore, void* pData);
    void RetrieveScoresCallback3(const coreUintW        iTableNum);

    // fetch trophies functions
    void FetchTrophies();
    void FetchTrophiesCallback1(const gjTrophyList& apTrophy, void* pData);
    void FetchTrophiesCallback2(const gjTrophyList& apTrophy, void* pData);

    // login functions
    coreInt32 QuickPlay();
    coreInt32 Login(const coreChar* pcName, const coreChar* pcToken);
    void LoginCallback(const coreInt32& iStatus, void* pData);
    void Logout();

    // control trophy parameters
    inline void SetTrophyStatus (const coreUint32 iStatus)  {m_iTrophyStatus  = iStatus;}
    inline void SetTrophyCurrent(const coreInt8   iCurrent) {m_iTrophyCurrent = iCurrent;}
    inline const coreUint32& GetTrophyStatus ()const        {return m_iTrophyStatus;}
    inline const coreInt8&   GetTrophyCurrent()const        {return m_iTrophyCurrent;}
};


#endif // _DR_GUARD_MENU_H_