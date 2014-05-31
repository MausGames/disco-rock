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
#define MENU_SCORE_TABLES  2     // number of different score tables (leaderboards)
#define MENU_SCORE_ENTRIES 8     // number of score entries to display at once
#define MENU_SCORE_PAGES   5     // number of score pages
                                 
#define MENU_TROPHIES       15   // number of implemented trophies
#define MENU_TROPHY_SECRETS 4    // total number of secret trophies

// transparency values
#define MENU_ALPHA_ACTIVE_1 0.8f
#define MENU_ALPHA_IDLE_1   0.5f
#define MENU_ALPHA_ACTIVE_2 0.65f
#define MENU_ALPHA_IDLE_2   0.95f


// ****************************************************************
class cMenu final : public coreMenu
{
private:
    coreLabel m_Made;
    coreLabel m_For;
    coreObject2D m_Maus;
    coreObject2D m_GameJolt;
    coreObject2D m_BigLogo;

    coreObject2D m_Black;
    coreObject2D m_White;
    coreObject2D m_BackgroundLeft;
    coreObject2D m_BackgroundRight;

    coreObject2D m_Logo;
    coreLabel m_Pause;

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
    coreLabel m_TopBatteryValue;
    coreObject2D m_TopFPSTacho;
    coreLabel m_TopFPSSec;
    coreLabel m_TopFPSMil;

    coreLabel m_BottomInfo;
    coreLabel m_BottomCredit;
    coreLabel m_BottomLoginName;
    coreObject2D m_BottomLoginJolt;

    coreObject2D m_QuestionBlack;
    coreLabel m_Question;
    coreButton m_Yes;
    coreButton m_No;

    coreLabel m_VideoText;
    coreButton m_VideoLow;
    coreButton m_VideoMedium;
    coreButton m_VideoHigh;

    coreLabel m_AudioText;
    coreObject2D m_AudioBar;
    coreObject2D m_AudioBarBack;   // (no, not bareback)
    coreObject2D m_AudioIconHigh;
    coreObject2D m_AudioIconLow;
    coreButton m_AudioDrag;

    coreLabel m_LoginConfigOr;
    coreObject2D m_LoginConfigLogo;
    coreButton m_LoginConfigStart;

    coreObject2D m_LoginBlack;
    coreObject2D m_LoginPopup;
    coreLabel m_aLoginText[2];
    coreLabel m_LoginEnterName;
    coreLabel m_LoginEnterToken;
    coreLabel m_LoginEnterGuest;
    coreTextBox m_LoginName;
    coreTextBox m_LoginToken;
    coreTextBox m_LoginGuest;
    coreButton m_LoginOK;
    coreButton m_LoginCancel;
    coreLabel m_LoginError;
    coreLabel m_LoginJoltOr;
    coreObject2D m_LoginJoltLogo;
    coreButton m_LoginJoltStart;

    coreObject2D m_Successful;
    coreLabel m_TopUpdating;

    coreLabel m_aScoreTable[MENU_SCORE_TABLES];
    coreLabel m_aaScoreEntry[MENU_SCORE_TABLES][MENU_SCORE_ENTRIES][3];
    coreLabel m_aScoreBest[MENU_SCORE_TABLES];
    coreLabel m_aScoreBestValue[MENU_SCORE_TABLES];
    coreLabel m_aScoreRecord[MENU_SCORE_TABLES];
    coreButton m_PageChange;

    coreLabel m_aAfterBest[MENU_SCORE_TABLES];
    coreLabel m_aAfterBestValue[MENU_SCORE_TABLES];
    coreLabel m_aAfterRecord[MENU_SCORE_TABLES];

    coreLabel m_TrophyText;
    coreObject2D m_aTrophyImage[MENU_TROPHIES];
    coreObject2D m_aTrophyCheck[MENU_TROPHIES];
    coreLabel m_aTrophySecret[MENU_TROPHY_SECRETS];
    coreLabel m_TrophyName;
    coreLabel m_aTrophyDesc[2];

    coreMenu m_ScoreMenu;
    coreMenu m_LoginMenu;

    coreTimer m_Intro;                               // intro timer

    float m_afSubmitValue[MENU_SCORE_TABLES];        // fetched values after loosing the game
    bool m_bSubmited;                                // values where successfully submited
    bool m_bInLeaderboard;                           // current values would be in the visible leaderboard (ask to submit before leaving)

    gjScoreList m_aapCurScores[MENU_SCORE_TABLES];   // saved score pointers for better leaderboard paging
    int m_iCurPage;                                  // current page in the leaderboards

    int m_iTrophyStatus;                             // achieve-status of all trophies (per bit)
    int m_iTrophyCurrent;                            // current selected trophy

    bool m_bFromGuest;                               // login-menu has to go back to guest-login on cancel


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

    // submit score functions
    void SubmitScore(const char* pcGuestName);
    void SubmitScoreCallback(const gjScorePtr& pScore, void* pData);

    // retrieve score functions
    void RetrieveScores();
    void RetrieveScoresCallback1(const gjScoreTableMap& apTable, void* pData);
    void RetrieveScoresCallback2(const gjScoreList& apScore, void* pData);
    void RetrieveScoresCallback3(const int& iTableNum);

    // fetch trophies functions
    void FetchTrophies();
    void FetchTrophiesCallback1(const gjTrophyList& apTrophy, void* pData);
    void FetchTrophiesCallback2(const gjTrophyList& apTrophy, void* pData);

    // login functions
    int  QuickPlay();
    int  Login(const char* pcName, const char* pcToken);
    void LoginCallback(const int& iStatus, void* pData);

    // control trophy parameters
    inline void SetTrophyStatus(const int& iStatus)   {m_iTrophyStatus  = iStatus;}
    inline void SetTrophyCurrent(const int& iCurrent) {m_iTrophyCurrent = iCurrent;}
    inline const int& GetTrophyStatus()const          {return m_iTrophyStatus;}
    inline const int& GetTrophyCurrent()const         {return m_iTrophyCurrent;}


private:
    DISABLE_COPY(cMenu)
};


#endif // _DR_GUARD_MENU_H_