//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
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

#if defined(_CORE_MOBILE_) || defined(_DR_EMULATE_MOBILE_)
    #define GAME_SPEED_FAST  (1.9f)     // speed after the first shock-wave (real value)
    #define GAME_SCORE_TIME  (12.5f)    // modifier for score over time
    #define GAME_COOLA_RATE  (95u)      // number of lines between 2 successive Coola bottles (smaller = more often)
    #define GAME_COOLA_TIME  (6.5f)     // duration of the Coola power
#else
    #define GAME_SPEED_FAST  (2.1f)
    #define GAME_SCORE_TIME  (11.0f)
    #define GAME_COOLA_RATE  (100u)
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
            const coreVector3 p = o->GetPosition();           \
            o->SetPosition(coreVector3(p.x, p.y - (m), p.z)); \
            o->Move();                                        \
            DYN_KEEP(it, a)                                   \
        }                                                     \
    }

// macro functions for rendering beverages with improved batching (alpha blending not correct anymore)
#define RENDER_BATCH(a,f)     for(coreUintW __i = 1u; __i <= 4u; ++__i) FOR_EACH    (it, a) {if((*it)->GetSigID() == __i) (*it)->f();}
#define RENDER_BATCH_REV(a,f) for(coreUintW __i = 1u; __i <= 4u; ++__i) FOR_EACH_REV(it, a) {if((*it)->GetSigID() == __i) (*it)->f();}


// ****************************************************************
// game class
class cGame final
{
private:
    cRock m_Rock;                             // rock object

    std::deque<cBeverage*> m_apBeverage;      // list with active beverages
    std::deque<cBeverage*> m_apDestroyed;     // list with inactive beverages

    std::deque<cTrap*> m_apTrap;              // list with trap plates

    std::deque<cPlate*> m_apPlate;            // list with plate objects
    std::deque<cRay*>   m_apRay;              // list with ray objects

    coreInt32 m_iCurLine;                     // current processing line/row

    coreList<coreUint8> m_aiAlgo;             // shuffled container with all algorithm identifiers
    coreUintW m_iAlgoCurIndex;                // current index to the algorithm in the container
    coreInt32 m_iAlgoCurCount;                // current number of lines into the current algorithm
    coreBool  m_bAlgoEmptyLines;              // algorithm has empty lines at the beginning and end of his stage (to prevent double-empty-lines)
    coreInt32 m_aiAlgoStatus[4];              // different status values for individual use in each algorithm

    coreProtect<coreDouble> m_dScore;         // score value
    coreProtect<coreFlow>   m_fTime;          // time value
    coreProtect<coreUint16> m_iCombo;         // current combo value (number of hit objects)

    coreUint16 m_iMaxCombo;                   // best combo value
    coreFlow   m_fComboTime;                  // time with max combo multiplier
    coreFloat  m_fComboDelay;                 // remaining time to the combo-reset

    coreUint16 m_aiCollected[6];              // beverage statistic values (0 = all | 1-4 single | 5 = Franka)
    coreUint16 m_iCollectedTraps;             // trap statistic value
    coreUint16 m_iCollectedNoBlue;            // collected normal drinks in a row without a blue drink

    coreUint8 m_iCoolaCounter;                // counter for regular Coola creation
    coreUint8 m_iRayCounter;                  // counter for regular ray creation

    coreUint8 m_iFirstJump;                   // handle first big air-jump (for trophy)
    coreBool  m_bFirstLine;                   // process first row differently
    coreBool  m_bFirstText;                   // show floating text on the first jump
    coreBool  m_bTrapSpawn;                   // enable trap spawn
    coreBool  m_bTrapJump;                    // current jump was triggered by a trap
    coreBool  m_bChallenge;                   // Coola challenge mode

    coreTimer m_PowerUpTimer;                 // power-up timer (for Coola power)

    cInterface m_Interface;                   // interface object
    coreLabel  m_Message;                     // beginning message
    coreTimer  m_MessageTimer;                // timer for the beginning message

    coreSoundPtr m_pTrapSound;                // sound effect for activated trap
    coreSoundPtr m_pTrophySound;              // sound effect for achieved trophy

    coreBool m_bTrophyHelper[TROPHY_ITEMS];   // trophy cache


public:
    explicit cGame(const coreBool bChallenge)noexcept;
    ~cGame();

    DISABLE_COPY(cGame)

    // render and move the game
    void RenderPre();
    void Render();
    void Move();

    // apply stage algorithms to the game (implemented in cStage.cpp)
    void ProcessStage(const coreFloat fSpawnY, coreBool* OUTPUT pbHole);

    // get game status
    inline coreUint8       GetStatus   ()const {return (m_Rock.GetPosition().z < -150.0f) ? 1u : 0u;}
    inline const coreBool& GetChallenge()const {return m_bChallenge;}

    // add objects
    void AddBeverage(const coreFloat fSpawnY, const coreInt32 iBlockX, coreBool* OUTPUT pbHole);
    void AddTrap    (const coreFloat fSpawnY, const coreInt32 iBlockX, coreBool* OUTPUT pbHole);
    void AddPlate   (const coreFloat fSpawnY, const coreInt32 iBlockX);
    void AddRay     (const coreFloat fSpawnY);
    void AddStreet  (const coreInt32 iBlockX, const coreBool bCenter, const coreUintW iLeft, const coreUintW iRight, coreBool* OUTPUT pbHole);

    // get game properties
    inline coreDouble        GetScore   ()const                       {return m_dScore;}
    inline coreFloat         GetTime    ()const                       {return m_fTime;}
    inline coreUint16        GetCombo   ()const                       {return m_iCombo;}
    inline const coreUint16& GetMaxCombo()const                       {return m_iMaxCombo;}
    inline const coreUint16& GetStat    (const coreUintW iIndex)const {return m_aiCollected[iIndex];}
    inline const coreUint16& GetTraps   ()const                       {return m_iCollectedTraps;}

    // access sub objects
    inline cRock*      GetRock     () {return &m_Rock;}
    inline cInterface* GetInterface() {return &m_Interface;}

    // achieve a trophy
    void AchieveTrophy        (const coreUintW iID, const coreUintW iNum);
    void AchieveTrophyCallback(const gjTrophyPtr& pTrophy, void* pData);
};


#endif // _DR_GUARD_GAME_H_