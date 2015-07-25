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
cGame::cGame(const coreBool& bChallenge)noexcept
: m_iCurLine         (g_pBackground->GetCurLine())
, m_iAlgoCurIndex    (0u)
, m_iAlgoCurCount    (0)
, m_bAlgoEmptyLines  (true)
, m_dScore           (0.0)
, m_fTime            (0.0f)
, m_iCombo           (0u)
, m_iMaxCombo        (0u)
, m_fComboTime       (0.0f)
, m_fComboDelay      (0.0f)
, m_iCollectedTraps  (0u)
, m_iCollectedNoBlue (0u)
, m_iCoolaCounter    (0u)
, m_iRayCounter      (0u)
, m_iFirstJump       (0u)
, m_bFirstLine       (true)
, m_bFirstText       (true)
, m_bTrapSpawn       (false)
, m_bTrapJump        (false)
, m_bChallenge       (bChallenge)
, m_PowerUpTimer     (coreTimer(GAME_COOLA_TIME, 1.0f, 1u))
, m_Message          (FONT_ROCKS, 45u, OUTLINE_SIZE, 0u)
, m_MessageTimer     (coreTimer(1.0f, 0.333f, 1u))
{
    // add and shuffle all algorithms
    m_aiAlgo.reserve(STAGE_TOTAL_NUM);
    for(coreUintW i = 0u; i < STAGE_TOTAL_NUM; ++i) m_aiAlgo.push_back(i);
    std::shuffle(m_aiAlgo.begin(), m_aiAlgo.end(), std::default_random_engine(CORE_RAND_TIME));
    std::memset (m_aiAlgoStatus, 0, sizeof(m_aiAlgoStatus));

    // create beginning message
    m_Message.SetPosition(coreVector2(0.0f,0.2f));
    m_Message.SetText    (g_MsgIntro.Get());
    m_MessageTimer.Play    (CORE_TIMER_PLAY_CURRENT);
    m_MessageTimer.SetValue(-0.333f);

    // reset statistics and trophy cache
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aiCollected); ++i) m_aiCollected  [i] = 0u;
    for(coreUintW i = 0u; i < TROPHY_ITEMS;              ++i) m_bTrophyHelper[i] = false;
    ++g_iNumGames;

    // load sound-effects
    m_pTrapSound   = Core::Manager::Resource->Get<coreSound>("trap.wav");
    m_pTrophySound = Core::Manager::Resource->Get<coreSound>("achieve.wav");

    // set initial speed
    g_fTargetSpeed = GAME_SPEED_SLOW;

    if(m_bChallenge)
    {
        // flash screen and turn camera
        g_pMenu->ChangeSurface(5u,  0.0f);
        g_pMenu->ChangeSurface(10u, 1.0f);
        g_bCamUpsideDown = true;

        // play sound-effect
        g_pMenu->PlayFlashSound();
    }
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // delete all remaining objects
    FOR_EACH(it, m_apBeverage)  SAFE_DELETE(*it)
    FOR_EACH(it, m_apDestroyed) SAFE_DELETE(*it)
    FOR_EACH(it, m_apTrap)      SAFE_DELETE(*it)
    FOR_EACH(it, m_apPlate)     SAFE_DELETE(*it)
    FOR_EACH(it, m_apRay)       SAFE_DELETE(*it)

    // clear memory
    m_apBeverage .clear();
    m_apDestroyed.clear();
    m_apTrap     .clear();
    m_apPlate    .clear();
    m_apRay      .clear();
}


// ****************************************************************
// render specific objects before the background
void cGame::RenderPre()
{
    // render rock
    m_Rock.RenderRock();

    // render plates (not reversed)
    FOR_EACH(it, m_apPlate) (*it)->Render();

    // render beverage straws (not reversed)
    RENDER_BATCH(m_apBeverage,  RenderStraw)
    RENDER_BATCH(m_apDestroyed, RenderStraw)
}


// ****************************************************************
// render the game
void cGame::Render()
{
    glDepthMask(false);
    {
        // render beverage shadows
        FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderShadow();
        FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderShadow();

        // render rock shadow
        m_Rock.RenderShadow();
    }
    glDepthMask(true);

    // render beverage drinks
    RENDER_BATCH_REV(m_apBeverage,  RenderDrink)
    RENDER_BATCH_REV(m_apDestroyed, RenderDrink)

    glDepthMask(false);
    {
        // render traps
        glCullFace(GL_FRONT);
        FOR_EACH_REV(it, m_apTrap) (*it)->Render();

        glCullFace(GL_BACK);
        FOR_EACH_REV(it, m_apTrap) (*it)->Render();
    }
    glDepthMask(true);

    // render beverage glasses
    glCullFace(GL_FRONT);
    RENDER_BATCH_REV(m_apBeverage,  RenderGlass)
    RENDER_BATCH_REV(m_apDestroyed, RenderGlass)

    glCullFace(GL_BACK);
    RENDER_BATCH_REV(m_apBeverage,  RenderGlass)
    RENDER_BATCH_REV(m_apDestroyed, RenderGlass)

    glDepthMask(false);
    {
        // render rock waves
        m_Rock.RenderWaves();

        // render rays
        FOR_EACH_REV(it, m_apRay) (*it)->Render();

        // render global particle system
        g_pParticleSystem->Render();
    }
    glDepthMask(true);

    glDisable(GL_DEPTH_TEST);
    {
        // render interface
        m_Interface.Render();
        if(m_MessageTimer.GetStatus() && !g_bPause)
        {
            // render beginning message
            if(m_MessageTimer.GetValue(CORE_TIMER_GET_NORMAL) >= 0.0f)
                m_Message.Render();
        }
    }   // keep disabled
}


// ****************************************************************
// move the game
void cGame::Move()
{
    // update game time
    m_fTime.Update(1.0f);

    // calculate movement values
    const coreFloat fMove10 = Core::System->GetTime(0) * 2.5f * BACK_DETAIL_Y;   // speed-modified
    const coreFloat fMove30 = Core::System->GetTime()  * 3.0f * BACK_DETAIL_Y;   // normal

    // check for the next line on the horizon
    const coreInt32 iNewCurLine = g_pBackground->GetCurLine();
    if(m_iCurLine != iNewCurLine)   // start line processing magic
    {
        m_iCurLine = iNewCurLine;

        // array used to create holes in the dance floor (true = hole, false = no hole)
        coreBool abHole[BACK_BLOCKS_X];
        for(coreUintW i = 0u; i < BACK_BLOCKS_X; ++i) abHole[i] = true;

        // calculate spawn position (compensate framerate (background position) and movement when spawning objects)
        const coreFloat fSpawnY = BACK_SPAWN_Y + fMove10 - BACK_DETAIL_Y * FRACT(g_pBackground->GetPositionTime());

        // increment Cool bottle counter
        ++m_iCoolaCounter;

        // completely remove first block line to force players to jump
        if(m_bFirstLine) m_bFirstLine = false;
        else
        {
            // apply stage algorithms to the game (implemented in cStage.cpp)
            this->ProcessStage(fSpawnY, abHole);
        }

#if !defined(_CORE_DEBUG_) || 1

        // randomly create moving plates inside of holes (only connected to solid plates)
        const coreFloat fPlateCmp = MAX(1.0f - (m_fTime / 10.0f), 0.1f) * 3.0f;
        for(coreUintW i = 1u; i < BACK_BLOCKS_X-1u; ++i)
        {
            if(abHole[i] && (Core::Rand->Float(1.0f) < fPlateCmp) && (!abHole[MAX(i-1u, 1u)] || !abHole[MIN(i+1u, BACK_BLOCKS_X-2u)]))
                this->AddPlate(fSpawnY, i);
        }

        // regularly create rays
        if(++m_iRayCounter >= 2u)
        {
            m_iRayCounter = 0u;
            this->AddRay(fSpawnY);
        }

#endif
        // always remove both outer blocks
        abHole[0] = abHole[BACK_BLOCKS_X-1u] = true;

        // update the holes in the dance floor (sometimes twice to sync for the infinite look)
        g_pBackground->UpdateHoles(m_iCurLine + BACK_VIEW, abHole);
        if(m_iCurLine + BACK_VIEW >= BACK_REPEAT) g_pBackground->UpdateHoles((m_iCurLine + BACK_VIEW) % BACK_REPEAT, abHole);
    }

    // move rock
    if(!this->GetStatus())
    {
        m_Interface.InteractControl();
        m_Rock.Move();
    }

    // update all inactive beverages, traps, plates and rays
    PROCESS_OBJECT_ARRAY(m_apDestroyed, fMove30)
    PROCESS_OBJECT_ARRAY(m_apTrap,      fMove10)
    PROCESS_OBJECT_ARRAY(m_apPlate,     fMove10)
    PROCESS_OBJECT_ARRAY(m_apRay,       fMove10)

    // update all active beverages
    FOR_EACH_DYN(it, m_apBeverage)
    {
        cBeverage* pBeverage = (*it);
        const coreVector3& vPos = pBeverage->GetPosition();

        if(vPos.y <= BACK_REMOVE_Y)
        {
            // remove beverage when finished
            SAFE_DELETE(pBeverage)
            DYN_REMOVE(it, m_apBeverage)
        }
        else
        {
            // check for collision with beverages
            if((pBeverage->GetPosition().y < 10.0f) && (m_PowerUpTimer.GetStatus() || Core::Manager::Object->TestCollision(pBeverage, &m_Rock)))
            {
                const coreUintW iSigID = pBeverage->GetSigID();

                // calculate and increase score
                const coreFloat fValue = I_TO_F(pBeverage->GetScore()) * COMBO_MULTI;
                m_dScore += coreDouble(fValue);

                // increase statistics
                ++m_aiCollected[0];
                ++m_aiCollected[iSigID];
                     if(iSigID == 3u) m_iCollectedNoBlue = 0u;
                else if(iSigID <  3u) ++m_iCollectedNoBlue;

                // increase combo
                ++m_iCombo;
                m_fComboDelay = 1.0f;

                const coreFloat fTextAlpha = 1.0f - m_fTime * 0.008f;
                if((iSigID == 4u) || (fTextAlpha > 0.2f))
                {
                    // get and modify beverage color
                    coreVector4 vColor = coreVector4(pBeverage->GetSigColor(), 1.0f);
                    if(iSigID != 4u) vColor.a *= CLAMP(fTextAlpha + 0.2f, 0.0f, 1.0f);

                    // create floating score text
                    g_pCombatText->AddTextTransformed(fValue ? PRINT("%.0f", fValue) : "RAMPAGE", m_Rock.GetPosition(), vColor);
                }

                // create max combo text (after score text)
                if(m_iCombo == 18u) g_pCombatText->AddTextTransformed("+MAXIMUM", m_Rock.GetPosition(), coreVector4(COLOR_ORANGE_F, 1.0f));

                // start power-up (remove all beverages in the rock-line)
                if((iSigID == 4u) && !m_bChallenge)
                {
                    m_PowerUpTimer.Play(CORE_TIMER_PLAY_RESET);
                    m_Rock.SetColored(true);
                    m_Rock.CreateShockWave(1u);
                }

                // send beverage into the air, try not to spill it
                pBeverage->Destroy(pBeverage->GetPosition() - m_Rock.GetPosition());
                pBeverage->PlaySound();

                // move beverage from active to inactive list
                m_apDestroyed.push_back(pBeverage);
                DYN_REMOVE(it, m_apBeverage)
            }
            else
            {
                // set position and move beverage
                pBeverage->SetPosition(coreVector3(vPos.x, vPos.y - fMove10, vPos.z));
                pBeverage->Move();
                DYN_KEEP(it)
            }
        }
    }

    // test trap collisions
    FOR_EACH(it, m_apTrap)
    {
        cTrap* pTrap = (*it);

        // check for collision with traps
        if(pTrap->GetPosition().y < 10.0f && !pTrap->IsStatic() && Core::Manager::Object->TestCollision(pTrap, &m_Rock))
        {
            if(m_Rock.Jump(10.0f))
            {
                m_bTrapJump = true;
                ++m_iCollectedTraps;

                // play trap sound effect and show message
                m_pTrapSound->PlayPosition(NULL, 0.3f, 1.1f, 0.05f, false, m_Rock.GetPosition());
                g_pCombatText->AddTextTransformed(g_MsgTrap.Get(), m_Rock.GetPosition(), coreVector4(COLOR_WHITE_F, 1.0f));

                // reset combo timer (a little bit more than a beverage)
                m_fComboDelay = 1.1f;
            }
        }
    }
    if(!m_Rock.GetJumped()) m_bTrapJump = false;

    // increase score over time
    m_dScore += coreDouble(Core::System->GetTime() * GAME_SCORE_TIME * (1.0f + m_fTime*0.023f));

    // update combo
    m_fComboDelay -= Core::System->GetTime() * (GAME_SPEED_FAST/GAME_SPEED_FAST_REAL);
    if(m_fComboDelay <= 0.0f && m_iCombo)
    {
        // reset combo
        g_pCombatText->AddTextTransformed("-COMBO", m_Rock.GetPosition(), coreVector4(COLOR_RED_F, 1.0f));
        m_iCombo     = 0u;
        m_fComboTime = 0.0f;
    }

    // update max combo value
    if(m_iCombo >= COMBO_MAX) m_fComboTime.Update(1.0f);
    m_iMaxCombo = MAX(m_iMaxCombo, m_iCombo);

    // update power-up
    if(m_PowerUpTimer.Update(1.0f))
    {
        m_Rock.SetColored(false);
        m_Rock.CreateShockWave(2u);
    }

    // update the interface object
    if(this->GetStatus()) m_Interface.Hide();
    m_Interface.Update(coreFloat(m_dScore), m_fTime, COMBO_MULTI, MIN(m_fComboDelay, 0.7f) * 1.4286f);
    m_Interface.Move();

    if(m_MessageTimer.GetStatus())
    {
        m_MessageTimer.Update(1.0f);

        // update big message at the beginning
        m_Message.SetDirection(coreVector2::Direction(0.2f * SIN(m_MessageTimer.GetValue(CORE_TIMER_GET_NORMAL) * PI * 4.0f)));
        m_Message.SetScale    (g_pBackground->GetFlash(0.4f));
        m_Message.SetAlpha    (MIN(2.0f * SIN(m_MessageTimer.GetValue(CORE_TIMER_GET_NORMAL) * PI), 1.0f));
        m_Message.Move();
    }

    if(m_bFirstText && (m_fTime >= GAME_SHOCK_TIME) && !m_Rock.GetFallen())
    {
        // show floating text on first jump (over the first line)
        m_bFirstText = false;
        g_pCombatText->AddTextTransformed(g_MsgBegin.Get(), m_Rock.GetPosition(), coreVector4(COLOR_WHITE_F, 1.0f));

        // also increase speed with shock-wave
        g_fTargetSpeed = GAME_SPEED_FAST;
        g_fCurSpeed    = GAME_SPEED_FAST + 0.2f;
        m_Rock.CreateShockWave(0u);
    }

    if(!this->GetStatus())
    {
        // handle first big air-jump
        if(m_iFirstJump < 2u && m_Rock.GetReflected()) ++m_iFirstJump;

        // check for achievements/trophies/whatever
        if(m_iFirstJump == 1u && m_Rock.GetJumped())                       {this->AchieveTrophy(GJ_TROPHY_01,  0);}
        if(m_Rock.GetFallen() && m_fTime < 10.0f && !m_bTrophyHelper[1])   {this->AchieveTrophy(GJ_TROPHY_02,  1); if(++g_iNumFails == 5u && !DEFINED(_CORE_ANDROID_)) coreData::OpenURL(Core::Rand->Int(1) ? "https://images.search.yahoo.com/search/images?p=facepalm" : "https://www.google.com/search?q=facepalm&tbm=isch");}
        if(m_Rock.GetFallen() && m_bTrapJump)                              {this->AchieveTrophy(GJ_TROPHY_03,  2);}
        if(m_Rock.GetFallen() && m_PowerUpTimer.GetStatus())               {this->AchieveTrophy(GJ_TROPHY_04,  3);}
        if(m_fComboTime >= 20.0f)                                          {this->AchieveTrophy(GJ_TROPHY_06,  5);}
        if(m_aiCollected[4] >= 3u && !m_bChallenge)                        {this->AchieveTrophy(GJ_TROPHY_07,  6);}
        if(m_iCollectedNoBlue >= 100 && !m_bChallenge)                     {this->AchieveTrophy(GJ_TROPHY_08,  7);}
        if(m_Rock.GetNumJumps() >= 150)                                    {this->AchieveTrophy(GJ_TROPHY_09,  8);}
        if(m_dScore < 1000.0 && m_fTime >= 30.0f)                          {this->AchieveTrophy(GJ_TROPHY_10,  9);}
        if(m_dScore >= 50000.0 && !m_bChallenge)                           {this->AchieveTrophy(GJ_TROPHY_11, 10);}
        if(m_Rock.GetNumAirJumps() >= 10u)                                 {this->AchieveTrophy(GJ_TROPHY_12, 11);}
        if(m_dScore >= 5000.0 && !m_bChallenge && m_iMaxCombo < COMBO_MAX) {this->AchieveTrophy(GJ_TROPHY_14, 13);}
        if(m_dScore >= 500000.0 && m_bChallenge)                           {this->AchieveTrophy(GJ_TROPHY_15, 14);}

        // [ 0] X(Get the Party Started,    s1) Make an air-jump at the beginning.
        // [ 1]  (I don't like Disco,       s1) Fall down the dance floor in less than 10 seconds.
        // [ 2] X(Too Much Too Soon,        s1) Fall down the dance floor while on a high-jump.
        // [ 3] X(I Will Survive,           s1) Fall down the dance floor after hitting a Coola bottle.

        // [ 4] X(Show me your Moves,        2) Submit your score and time online. <<in cMenu, with centered animation>>
        // [ 5]  (Binge Drinker,             2) Hold a combo multiplier of x10 for at least 20 seconds.
        // [ 6]  (Designated Driver,         2) Bump into 3 Coola bottles within a single normal run.
        // [ 7] X(Before there was any Blue, 2) Hit 100 normal beverages in a row, without touching a blue drink.

        // [ 8] X(The Floor is Lava,         3) Jump 150 times in a single run. (high-jumps count as one jump)
        // [ 9] X(Sober in the 70s,          3) Roll for 30 seconds with less than 1.000 points.
        // [10]  (Shagadelic,                3) Get at least 50.000 points in a normal run.
        // [11] X(Love is in the Air,        3) Make 10 air-jumps while not over solid ground.

        // [12]  (You Should Be Dancing,     4) Survive long enough to get another speed boost. <<in cStage>>
        // [13]  (Get Up and Boogie,         4) Reach 5.000 points without ever getting a combo multiplier of x10.
        // [14] X(The Coola Challenge,       4) Hold ['C']/[a screen-corner] while starting a game and get at least 500.000 points.
    }
}


// ****************************************************************
// add beverage object
void cGame::AddBeverage(const coreFloat& fSpawnY, const coreInt32& iBlockX, coreBool* OUTPUT pbHole)
{
    ASSERT((iBlockX+1) < coreInt32(BACK_BLOCKS_X))
    cBeverage* pBeverage = NULL;

    if((m_iCoolaCounter >= GAME_COOLA_RATE) || m_bChallenge)
    {
        // reset counter (incremented in row-processing body)
        m_iCoolaCounter = 0u;

        // create Coola bottle
        pBeverage = new cCoola();

        if(!m_bChallenge)
        {
            // create static highlight
            this->AddTrap(fSpawnY, iBlockX, pbHole);
            m_apTrap.back()->SetStatic(true);
        }
    }
    else
    {
        // define the next thing to spawn
        const coreUintW iSelection = Core::Rand->Int(10);

        // create new beverage
             if(                     iSelection <=  5u) pBeverage = new cSunrise();
        else if( 6u <= iSelection && iSelection <=  9u) pBeverage = new cMojito();
        else if(10u <= iSelection && iSelection <= 10u) pBeverage = new cBlue();
    }

    // prepare beverage and add to list
    pBeverage->SetPosition(coreVector3(BACK_SPAWN_X(iBlockX, 1.5f), fSpawnY, 0.0f));
    m_apBeverage.push_back(pBeverage);

    // prevent hole
    pbHole[iBlockX+1] = false;
}


// ****************************************************************
// add trap object
void cGame::AddTrap(const coreFloat& fSpawnY, const coreInt32& iBlockX, coreBool* OUTPUT pbHole)
{
    ASSERT((iBlockX+1) < coreInt32(BACK_BLOCKS_X))

    // create new trap and add to list
    cTrap* pTrap = new cTrap();
    pTrap->SetPosition(coreVector3(BACK_SPAWN_X(iBlockX, 1.5f), fSpawnY, GAME_HEIGHT + pTrap->GetSize().z*0.51f));
    m_apTrap.push_back(pTrap);

    // prevent hole
    pbHole[iBlockX+1] = false;
}


// ****************************************************************
// add plate object
void cGame::AddPlate(const coreFloat& fSpawnY, const coreInt32& iBlockX)
{
    // create plate and add to list
    cPlate* pPlate = new cPlate(90.0f + Core::Rand->Float(120.0f), coreVector2(I_TO_F(iBlockX), -FLOOR(g_pBackground->GetPositionTime())));
    pPlate->SetPosition(coreVector3(BACK_SPAWN_X(iBlockX, 0.5f), fSpawnY, GAME_HEIGHT));
    m_apPlate.push_back(pPlate);
}


// ****************************************************************
// add ray object
void cGame::AddRay(const coreFloat& fSpawnY)
{
     // calculate start-position
    const coreVector2 vAround = coreVector2(Core::Rand->Float(-2.0f, 2.0f), 1.0f).Normalize() * 90.0f;

    // create new ray and add to list
    cRay* pRay = new cRay(coreVector3(vAround.x, fSpawnY, vAround.y));
    m_apRay.push_back(pRay);
}


// ****************************************************************
// prevent holes in the ground
void cGame::AddStreet(const coreInt32& iBlockX, const coreBool& bCenter, const coreUintW& iLeft, const coreUintW& iRight, coreBool* OUTPUT pbHole)
{
    ASSERT((iBlockX+1) < coreInt32(BACK_BLOCKS_X))

    // prevent center hole
    if(bCenter) pbHole[iBlockX+1] = false;

    // prevent left and right holes
    for(coreUintW i = 0u; i < iLeft;  ++i) pbHole[CLAMP(iBlockX  -i, 0u, BACK_BLOCKS_X-1u)] = false;
    for(coreUintW i = 0u; i < iRight; ++i) pbHole[CLAMP(iBlockX+2+i, 0u, BACK_BLOCKS_X-1u)] = false;
}


// ****************************************************************
// achieve a trophy
void cGame::AchieveTrophy(const coreUintW& iID, const coreUintW& iNum)
{
    // update helper
    if(m_bTrophyHelper[iNum]) return;
    m_bTrophyHelper[iNum] = true;

    // achieve trophy
    gjTrophy* pTrophy = g_pOnline->GameJolt()->InterTrophy()->GetTrophy(iID);
    if(g_pOnline->IsUserConnected()) g_pOnline->AchieveTrophy(pTrophy, this, &cGame::AchieveTrophyCallback, I_TO_P(iNum));
    else
    {
        // handle guest user
        this->AchieveTrophyCallback(pTrophy, I_TO_P(iNum));
    }
}


// ****************************************************************
// callback for trophy achievements
void cGame::AchieveTrophyCallback(const gjTrophyPtr& pTrophy, void* pData)
{
    const coreUintW iNum = P_TO_I(pData);

    if(pTrophy)
    {
        if(!(g_pMenu->GetTrophyStatus() & BIT(iNum)))
        {
            const coreVector3 vPos = this->GetStatus() ? coreVector3(0.0f,0.0f,0.0f) : m_Rock.GetPosition();

            // show achievement title and play sound
            g_pCombatText->ShowTrophy(coreData::StrUpper(("<< " + pTrophy->GetTitle() + " >>").c_str()), vPos);
            m_pTrophySound->PlayRelative(NULL, 0.09f, 1.0f, 0.0f, false);

            // set and save current trophy status
            g_pMenu->SetTrophyStatus(g_pMenu->GetTrophyStatus() | BIT(iNum));
            g_pMenu->SetTrophyCurrent(-1);
        }

        // save status always in offline config
        Core::Config->SetInt("Game", "Trophy", Core::Config->GetInt("Game", "Trophy", 0) | BIT(iNum));
    }
}