/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_MENU_H_
#define _DR_GUARD_MENU_H_


// ****************************************************************
// menu definitions
#define LEFT_CENTER (0.46f)   // fixed center-value for the left menu

// transparency values
#define MENU_ALPHA_ACTIVE_1 (0.8f)
#define MENU_ALPHA_IDLE_1   (0.5f)
#define MENU_ALPHA_ACTIVE_2 (0.65f)
#define MENU_ALPHA_IDLE_2   (0.95f)

// values for menu object adjustments
#define ADJUST_LEFT(x)   {x.SetCenter(coreVector2(-0.5f,0.0f)); x.SetAlignment(coreVector2( 1.0f,0.0f));}
#define ADJUST_RIGHT(x)  {x.SetCenter(coreVector2( 0.5f,0.0f)); x.SetAlignment(coreVector2(-1.0f,0.0f));}
#define ADJUST_BORDER(x) {x.SetTexSize(coreVector2(0.62f,0.62f) / x.GetSize() * 0.0165f);}

// macro function for changing object transparency
#define ALPHA_BUTTON_TEXT(b)                                                                                             \
    {                                                                                                                    \
        const float a = (b).IsFocused() ? MENU_ALPHA_ACTIVE_2 : MENU_ALPHA_IDLE_2;                                       \
        (b).SetAlpha(b.GetAlpha() * a);                                                                                  \
    }
#define ALPHA_BUTTON_SCORE(b,i)                                                                                          \
    {                                                                                                                    \
        const float a = ((b).IsFocused() || m_ScoreMenu.GetCurSurface() == i) ? MENU_ALPHA_ACTIVE_1 : MENU_ALPHA_IDLE_1; \
        (b).SetAlpha((b).GetAlpha() * a);                                                                                \
    }
#define ALPHA_BUTTON_INSIDE(b)                                                                                           \
    {                                                                                                                    \
        const float a = (b).IsFocused() ? MENU_ALPHA_IDLE_1 : MENU_ALPHA_ACTIVE_1;                                       \
        (b).SetAlpha((b).GetAlpha() * a);                                                                                \
    }

// static texts
#define LOGIN_START             "YOUR TOKEN IS NOT YOUR PASSWORD"
#define LOGIN_CONNECTING        "CONNECTING..."
#define LOGIN_ERROR_CONNECTION  "COULD NOT CONNECT TO GAME JOLT"
#define LOGIN_ERROR_CREDENTIALS "WRONG USER NAME OR TOKEN"

#define QUESTION_EXIT   "DO YOU REALLY WANT TO EXIT?"
#define QUESTION_ABORT  "DO YOU REALLY WANT TO ABORT THE CURRENT GAME?"
#define QUESTION_RETURN "DO YOU REALLY WANT TO RETURN WITHOUT SUBMITTING?"

// update displayed score and time value
#define SHOW_BEST_SCORE(x) {m_aScoreBestValue[0].SetText(PRINT("%06d", (x)));}
#define SHOW_BEST_TIME(x)  {m_aScoreBestValue[1].SetText(PRINT("%03d.%01d", (x) / 100, (((x) % 100) / 10)));}


// ****************************************************************
// menu class
class cMenu final : public coreMenu
{
private:
    coreLabel    m_Made;
    coreLabel    m_For;
    coreObject2D m_Maus;
    coreObject2D m_GameJolt;
    coreObject2D m_BigLogo;

    coreObject2D m_Black;
    coreObject2D m_White;
    coreObject2D m_BackgroundLeft;
    coreObject2D m_BackgroundRight;

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

    coreObject2D m_TopBatteryBolt;
    coreLabel    m_TopBatteryValue;
    coreObject2D m_TopFPSTacho;
    coreLabel    m_TopFPSSec;
    coreLabel    m_TopFPSMil;

    coreLabel    m_BottomInfo;
    coreLabel    m_BottomCredit;
    coreLabel    m_BottomLoginName;
    coreObject2D m_BottomLoginJolt;

    coreButton m_GoogleFullTrophy;
    coreButton m_GoogleFullScore;

    coreObject2D m_QuestionBlack;
    coreLabel    m_Question;
    coreButton   m_Yes;
    coreButton   m_No;

    coreLabel  m_VideoText;
    coreButton m_VideoLow;
    coreButton m_VideoMedium;
    coreButton m_VideoHigh;

    coreLabel    m_AudioText;
    coreObject2D m_AudioBar;
    coreObject2D m_AudioBarBack;   // (no, not bareback)
    coreObject2D m_AudioIconHigh;
    coreObject2D m_AudioIconLow;
    coreButton   m_AudioDrag;

    coreLabel    m_LoginConfigOr;
    coreObject2D m_LoginConfigLogo;
    coreButton   m_LoginConfigStart;

    coreLabel          m_ControlText;
    coreSwitchBox<int> m_ControlType;

    coreObject2D m_LoginBlack;
    coreObject2D m_LoginPopup;
    coreLabel    m_aLoginText[2];
    coreLabel    m_LoginEnterName;
    coreLabel    m_LoginEnterToken;
    coreLabel    m_LoginEnterGuest;
    coreTextBox  m_LoginName;
    coreTextBox  m_LoginToken;
    coreTextBox  m_LoginGuest;
    coreButton   m_LoginOK;
    coreButton   m_LoginCancel;
    coreLabel    m_LoginError;
    coreLabel    m_LoginJoltOr;
    coreObject2D m_LoginJoltLogo;
    coreButton   m_LoginJoltStart;

    coreObject2D m_Successful;
    coreLabel    m_TopUpdating;

    coreLabel  m_aScoreTable[SCORE_TABLES];
    coreLabel  m_aaScoreEntry[SCORE_TABLES][SCORE_ENTRIES][3];
    coreLabel  m_aScoreBest[SCORE_TABLES];
    coreLabel  m_aScoreBestValue[SCORE_TABLES];
    coreLabel  m_aScoreRecord[SCORE_TABLES];
    coreLabel  m_aScoreRank[SCORE_TABLES];
    coreButton m_PageChange;

    coreLabel m_aAfterBest[SCORE_TABLES];
    coreLabel m_aAfterBestValue[SCORE_TABLES];
    coreLabel m_aAfterRecord[SCORE_TABLES];

    coreLabel    m_TrophyText;
    coreObject2D m_aTrophyImage[TROPHY_ITEMS];
    coreObject2D m_aTrophyCheck[TROPHY_ITEMS];
    coreLabel    m_aTrophySecret[TROPHY_SECRETS];
    coreLabel    m_TrophyName;
    coreLabel    m_aTrophyDesc[2];

    coreLabel    m_aConnectionError[3];
    coreObject2D m_AuthLogo;
    coreButton   m_AuthButton;
    coreObject2D m_Loading;

    coreMenu m_ScoreMenu;
    coreMenu m_LoginMenu;

    coreTimer    m_Intro;                       // intro timer
    coreSoundPtr m_pClickSound;                 // click sound-effect for buttons
    coreSoundPtr m_pHappySound;                 // happy sound-effect for special events
    coreSoundPtr m_pRecordSound;                // sound-effect for highlighting a new record
    coreSoundPtr m_pFlashSound;                 // sound-effect when flashing the screen

    float m_afSubmitValue[SCORE_TABLES];        // fetched values after loosing the game
    bool  m_bSubmited;                          // values where successfully submited
    bool  m_bInLeaderboard;                     // current values would be in the visible leaderboard (ask to submit before leaving)

    gjScoreList m_aapCurScores[SCORE_TABLES];   // saved score pointers for better leaderboard paging
    int m_iCurPage;                             // current page in the leaderboards
    int m_iTableUpdate;                         // status to update the current score table display synchronous

    int m_iTrophyStatus;                        // achieve-status of all trophies (per bit)
    int m_iTrophyCurrent;                       // current selected trophy

    bool m_bFromGuest;                          // login-menu has to go back to guest-login on cancel


public:
    cMenu()noexcept;
    ~cMenu();

    // move the menu
    void Move()override;

    // end current game and return to main menu
    void End();

    // reset all relevant shaders after quality changes
    void ResetShaders();

    // control notification for new record
    void NewRecord(const coreByte& iIndex);
    void ResetRecord();

    // control connection error notification
    void SetErrorMessage(const coreVector3& vColor, const char* pcMessage1, const char* pcMessage2, const char* pcMessage3);

    // update current control type
    inline void UpdateControl() {m_ControlType.Select(CLAMP(Core::Config->GetInt("Game", "Control", 0), 0, 2));}

    // play sound-effects
    inline void PlayClickSound()  {m_pClickSound ->PlayRelative(NULL, 0.08f, 1.0f, 0.0f, false);}
    inline void PlayHappySound()  {m_pHappySound ->PlayRelative(NULL, 0.08f, 1.0f, 0.0f, false);}
    inline void PlayRecordSound() {m_pRecordSound->PlayRelative(NULL, 0.15f, 1.0f, 0.0f, false);}
    inline void PlayFlashSound()  {m_pFlashSound ->PlayRelative(NULL, 0.11f, 1.0f, 0.0f, false);}

    // invoke score table update
    inline void InvokeScoreUpdate(const int& iTableNum) {BIT_SET(m_iTableUpdate, iTableNum)}

    // submit score functions
    void SubmitScore        (const char* pcGuestName);
    void SubmitScoreCallback(const gjScorePtr& pScore, void* pData);

    // retrieve score functions
    void RetrieveScores();
    void RetrieveScoresCallback1(const gjScoreTableMap& apTable, void* pData);
    void RetrieveScoresCallback2(const gjScoreList&     apScore, void* pData);
    void RetrieveScoresCallback3(const int& iTableNum);

    // fetch trophies functions
    void FetchTrophies();
    void FetchTrophiesCallback1(const gjTrophyList& apTrophy, void* pData);
    void FetchTrophiesCallback2(const gjTrophyList& apTrophy, void* pData);

    // login functions
    int  QuickPlay();
    int  Login(const char* pcName, const char* pcToken);
    void LoginCallback(const int& iStatus, void* pData);
    void Logout();

    // control trophy parameters
    inline void SetTrophyStatus (const int& iStatus)  {m_iTrophyStatus  = iStatus;}
    inline void SetTrophyCurrent(const int& iCurrent) {m_iTrophyCurrent = iCurrent;}
    inline const int& GetTrophyStatus ()const         {return m_iTrophyStatus;}
    inline const int& GetTrophyCurrent()const         {return m_iTrophyCurrent;}


private:
    DISABLE_COPY(cMenu)
};


#endif // _DR_GUARD_MENU_H_