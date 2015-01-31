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
// game definitions
#define GAME_HEIGHT          (-20.0f)   // logical height of the dance floor (also inlined in floor-shaders)
#define GAME_SHOCK_TIME      (6.3f)     // time when to increase speed with a shock-wave
#define GAME_JUMP_WIDTH      (6)        // regular distance for jumping with a trap
#define GAME_SPEED_SLOW      (1.5f)     // initial speed after starting a new game
#define GAME_SPEED_FAST_REAL (2.2f)     // speed after the first shock-wave (baseline)

#if defined(_CORE_ANDROID_) || defined(_DR_EMULATE_MOBILE_)
    #define GAME_SPEED_FAST  (1.9f)     // speed after the first shock-wave (real value)
    #define GAME_SCORE_TIME  (12.5f)    // modifier for score over time
    #define GAME_COOLA_RATE  (95)       // number of lines between 2 successive Coola bottles (smaller = more often)
    #define GAME_COOLA_TIME  (6.5f)     // duration of the Coola power
#else
    #define GAME_SPEED_FAST  (2.1f)
    #define GAME_SCORE_TIME  (11.0f)
    #define GAME_COOLA_RATE  (100)
    #define GAME_COOLA_TIME  (5.5f)
#endif

// macro function for calculating the current score multiplier
#define COMBO_MAX   (18u)
#define COMBO_MULTI (1.0f + 0.5f * I_TO_F(MIN(m_iCombo, COMBO_MAX)))

// macro function for moving and removing game objects
#define PROCESS_OBJECT_ARRAY(a,m)                             \
    FOR_EACH_DYN(it, a)                                       \
    {                                                         \
        coreObject3D* o = (*it);                              \
                                                              \
        if(o->GetStatus() == 1)                               \
        {                                                     \
            SAFE_DELETE(o)                                    \
            DYN_REMOVE(it, a)                                 \
        }                                                     \
        else                                                  \
        {                                                     \
            const coreVector3& p = o->GetPosition();          \
            o->SetPosition(coreVector3(p.x, p.y - (m), p.z)); \
            o->Move();                                        \
            DYN_KEEP(it)                                      \
        }                                                     \
    }


// ****************************************************************
// game class
class cGame final
{
private:
    cRock m_Rock;                           // rock object

    std::deque<cBeverage*> m_apBeverage;    // list with active beverages
    std::deque<cBeverage*> m_apDestroyed;   // list with inactive beverages

    std::deque<cTrap*> m_apTrap;            // list with trap plates

    std::deque<cPlate*> m_apPlate;          // list with plate objects
    std::deque<cRay*>   m_apRay;            // list with ray objects

    int m_iCurLine;                         // current processing line/row

    std::vector<coreByte> m_aiAlgo;         // shuffled container with all algorithm identifiers
    coreByte m_iAlgoCurIndex;               // current index to the algorithm in the container
    coreUint m_iAlgoCurCount;               // current number of lines into the current algorithm
    bool     m_bAlgoEmptyLines;             // algorithm has empty lines at the beginning and end of his stage (to prevent double-empty-lines)
    int      m_aiAlgoStatus[4];             // different status values for individual use in each algorithm

    double   m_dScore;                      // score value
    coreFlow m_fTime;                       // time value
    coreUint m_iCombo;                      // current combo value (number of hit objects)

    coreUint m_iMaxCombo;                   // best combo value
    coreFlow m_fComboTime;                  // time with max combo multiplier
    float    m_fComboDelay;                 // remaining time to the combo-reset

    coreUint m_aiCollected[6];              // beverage statistic values (0 = all | 1-4 single | 5 = Franka)
    coreUint m_iCollectedTraps;             // trap statistic value
    coreUint m_iCollectedNoBlue;            // collected normal drinks in a row without a blue drink

    int m_iCoolaCounter;                    // counter for regular Coola creation
    int m_iRayCounter;                      // counter for regular ray creation
    int m_iTrapChance;                      // chance value to create random but regular traps

    int  m_iFirstJump;                      // handle first big air-jump (for trophy)
    bool m_bFirstLine;                      // process first row differently
    bool m_bFirstText;                      // show floating text on the first jump
    bool m_bTrapSpawn;                      // enable trap spawn
    bool m_bTrapJump;                       // current jump was triggered by a trap
    bool m_bChallenge;                      // Coola challenge mode

    coreTimer m_PowerUpTimer;               // power-up timer (for Coola power)

    cInterface m_Interface;                 // interface object
    coreLabel  m_Message;                   // beginning message
    coreTimer  m_MessageTimer;              // timer for the beginning message

    coreSoundPtr m_pTrapSound;              // sound effect for activated trap
    coreSoundPtr m_pTrophySound;            // sound effect for achieved trophy

    bool m_bTrophyHelper[TROPHY_ITEMS];     // trophy cache


public:
    explicit cGame(const bool& bChallenge)noexcept;
    ~cGame();

    DISABLE_COPY(cGame)

    // render and move the game
    void RenderPre();
    void Render();
    void Move();

    // apply stage algorithms to the game (implemented in cStage.cpp)
    void ProcessStage(const float &fSpawnY, bool* OUTPUT pbHole);

    // get game status
    inline int         GetStatus   ()const {return (m_Rock.GetPosition().z < -150.0f) ? 1 : 0;}
    inline const bool& GetChallenge()const {return m_bChallenge;}

    // add objects
    void AddBeverage(const float& fSpawnY, const int& iBlockX, bool* OUTPUT pbHole);
    void AddTrap    (const float& fSpawnY, const int& iBlockX, bool* OUTPUT pbHole);
    void AddPlate   (const float& fSpawnY, const int& iBlockX);
    void AddRay     (const float& fSpawnY);
    void AddStreet  (const int& iBlockX, const bool& bCenter, const coreByte& iLeft, const coreByte& iRight, bool* OUTPUT pbHole);

    // get game properties
    inline const double&   GetScore   ()const                  {return m_dScore;}
    inline const float&    GetTime    ()const                  {return m_fTime;}
    inline const coreUint& GetCombo   ()const                  {return m_iCombo;}
    inline const coreUint& GetMaxCombo()const                  {return m_iMaxCombo;}
    inline const coreUint& GetStat    (const int& iIndex)const {return m_aiCollected[iIndex];}
    inline const coreUint& GetTraps   ()const                  {return m_iCollectedTraps;}

    // access sub objects
    inline cRock*      GetRock     () {return &m_Rock;}
    inline cInterface* GetInterface() {return &m_Interface;}

    // achieve a trophy
    void AchieveTrophy        (const int& iID, const int& iNum);
    void AchieveTrophyCallback(const gjTrophyPtr& pTrophy, void* pData);
};


#endif // _DR_GUARD_GAME_H_