/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_GAME_H_
#define _DR_GUARD_GAME_H_


// ****************************************************************
#define GAME_TROPHIES 8
#define GAME_HEIGHT   -20.0f

#define CANYON_DISTANCE 138
#define CANYON_BEFORE   1
#define CANYON_AFTER    2


// ****************************************************************
class cGame final
{
private:
    cRock m_Rock;                            // rock object

    std::deque<cBeverage*> m_apBeverage;     // list with active beverages
    std::deque<cBeverage*> m_apDestroyed;    // list with inactive beverages

    std::deque<cTrap*> m_apTrap;             // list with trap plates

    std::deque<cPlate*> m_apPlate;           // list with plate objects
    std::deque<cRay*> m_apRay;               // list with ray objects

    int m_iCurLine;                          // current processing line/row
    int m_iCurSpawn;                         // current spawn column (beverages, triple-holes)
    bool m_bLastHole;                        // helper to prevent two consecutively triple-holes
                                             
    double m_dScore;                         // score value
    coreFlow m_fTime;                        // time value
    coreUint m_iCombo;                       // current combo value (number of hit objects)

    coreUint m_iMaxCombo;                    // best combo value
    coreFlow m_fComboTime;                   // time with max combo multiplier
    float m_fComboDelay;                     // remaining time to the combo-reset
                                             
    coreUint m_aiCollected[5];               // hit statistic values (0 = all | 1-4 single)
                                             
    int m_iCoolaCounter;                     // counter for regular coola creation
    int m_iRayCounter;                       // counter for regular ray creation
    int m_iCanyonCounter;                    // counter for regular canyon creation
    int m_iTrapChance;                       // chance value to create random but regular traps
                                             
    bool m_bFirstLine;                       // process first row differently
    bool m_bFirstMsg;                        // show message on the first jump
    bool m_bTrapSpawn;                       // enable trap spawn
    bool m_bTrapJump;                        // current jump was triggered by a trap
    bool m_bChallenge;                       // coola challenge mode

    int m_iNarrow;                           // number of rows to remove from the dance floor (6 - m_iNarrow*2)
                                             
    cInterface m_Interface;                  // interface object
    coreLabel m_Message;                     // beginning message
    coreTimer m_ShowMessage;                 // timer for the beginning message

    coreSoundPtr m_pTrapSound;               // sound effect for activated trap
                                       
    bool m_bTrophyHelper[GAME_TROPHIES];     // trophy cache


public:
    cGame(const bool& bChallenge)noexcept;
    ~cGame();

    // render and move the game
    void Render();
    void Move();

    // get game status
    inline int GetStatus()const            {return (m_Rock.GetPosition().z < -150.0f) ? 1 : 0;}
    inline const bool& GetChallenge()const {return m_bChallenge;}

    // add objects
    void AddTrap(const int& iBlock, const float& fSpawn);

    // get game properties
    inline const double& GetScore()const                   {return m_dScore;}
    inline const float& GetTime()const                     {return m_fTime;}
    inline const coreUint& GetCombo()const                 {return m_iCombo;}
    inline const coreUint& GetMaxCombo()const              {return m_iMaxCombo;}
    inline const coreUint& GetStat(const int& iIndex)const {return m_aiCollected[iIndex];}

    // access sub objects
    inline cInterface* GetInterface() {return &m_Interface;}

    // achive a trophy
    void AchieveTrophy(const int& iID);


private:
    DISABLE_COPY(cGame)
};


#endif // _DR_GUARD_GAME_H_