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
cGame::cGame(const bool& bChallenge)noexcept
: m_iCurLine         (g_pBackground->GetCurLine())
, m_iCurSpawn        (Core::Rand->Int(2,3))   // not sync
, m_dScore           (0.0)
, m_fTime            (0.0f)
, m_iCombo           (0)
, m_iMaxCombo        (0)
, m_fComboTime       (0.0f)
, m_fComboDelay      (0.0f)
, m_iCollectedTraps  (0)
, m_iCollectedNoBlue (0)
, m_iCoolaCounter    (0)
, m_iRayCounter      (0)
, m_iCanyonCounter   (CANYON_PREVENT)
, m_iTrapChance      (1)
, m_iThreeDrinks     (0)
, m_iFinalHoles      (0)
, m_iFirstJump       (0)
, m_bFirstLine       (true)
, m_bFirstMsg        (true)
, m_bTrapSpawn       (false)
, m_bTrapJump        (false)
, m_bShakeEnable     (Core::Config->GetInt("Game", "Score", 0) ? true : false)
, m_bChallenge       (bChallenge)
, m_iNarrow          (0)
, m_bSideOrder       (Core::Rand->Int(0,1) ? true : false)
, m_iCamStatus       (0)
, m_Message          (FONT_ROCKS, 45, 0)
, m_ShowMessage      (coreTimer(1.0f, 0.333f, 1))
{
    // create beginning message
    m_ShowMessage.Play(CORE_TIMER_PLAY_CURRENT);
    m_ShowMessage.SetValue(-0.333f);

    m_Message.SetPosition(coreVector2(0.0f,0.2f));
    m_Message.SetText(g_MsgIntro.Get());
    
    // reset statistics and trophy cache
    for(int i = 0; i < (int)ARRAY_SIZE(m_aiCollected); ++i) m_aiCollected[i]   = 0;
    for(int i = 0; i < TROPHY_ITEMS;                   ++i) m_bTrophyHelper[i] = false;
    ++g_iNumGames;

    // load sound-effects
    m_pTrapSound   = Core::Manager::Resource->Get<coreSound>("trap.wav");
    m_pTrophySound = Core::Manager::Resource->Get<coreSound>("achieve.wav");
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
    m_apBeverage.clear();
    m_apDestroyed.clear();
    m_apTrap.clear();
    m_apPlate.clear();
    m_apRay.clear();
}


// ****************************************************************
// render the game
void cGame::Render()
{
    glDisable(GL_DEPTH_TEST);
    {
        // render beverage shadows
        FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderShadow();
        FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderShadow();
    }
    glEnable(GL_DEPTH_TEST);

    // render plates
    FOR_EACH_REV(it, m_apPlate) (*it)->Render();

    // render beverage straws
    FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderStraw();
    FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderStraw();

    // render beverage drinks
    FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderDrink();
    FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderDrink();

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
    FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderGlass();
    FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderGlass();

    glCullFace(GL_BACK);
    FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderGlass();
    FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderGlass();

    // render rock
    m_Rock.Render();

    glDepthMask(false);
    {
        // render rays
        FOR_EACH_REV(it, m_apRay) (*it)->Render();

        // render global particle system
        g_pParticleSystem->Render();
    }
    glDepthMask(true);
    
    // render interface
    m_Interface.Render();
    if(m_ShowMessage.GetStatus() && !g_bPause)
    {
        // render beginning message
        if(m_ShowMessage.GetValue(CORE_TIMER_GET_NORMAL) >= 0.0f)
            m_Message.Render();
    }
}


// ****************************************************************
// move the game
void cGame::Move()
{
    // update game time and calculate target speed (speed is interpolated smoothly in the main file)
    m_fTime.Update(1.0f);
    g_fTargetSpeed = this->GetStatus() ? 1.5f : (1.5f + m_fTime*0.018f);

    // show message on first jump (over the first line)
    if(m_bFirstMsg)
    {
        if((m_fTime >= 5.8f && m_Rock.GetJumped()) || (m_fTime >= 6.3f && !m_Rock.GetFallen()))
        {
            m_bFirstMsg = false;
            g_pCombatText->AddTextTransformed(g_MsgBegin.Get(), m_Rock.GetPosition(), coreVector4(COLOR_WHITE_F, 1.0f));
        }
    }
    
    // calculate movement values
    const float fMove10 = Core::System->GetTime(0) * 2.5f * BACK_DETAIL_Y;
    const float fMove30 = Core::System->GetTime()  * 3.0f * BACK_DETAIL_Y;

    // check for the next line on the horizon
    const int iNewCurLine = g_pBackground->GetCurLine();
    if(m_iCurLine != iNewCurLine)   // start line processing magic
    {
        m_iCurLine = iNewCurLine;

        // array used to create holes in the dance floor (true = hole, false = no hole)
        bool abHole[BACK_BLOCKS_X];
        for(int i = 0; i < BACK_BLOCKS_X; ++i) abHole[i] = true; // reset safely

        // compensate framerate (background position) and movement when spawning objects
        const float fSpawn = BACK_SPAWN_Y + fMove10 - BACK_DETAIL_Y * (g_pBackground->GetPositionTime() - FLOOR(g_pBackground->GetPositionTime()));

        if(m_bFirstLine)
        {
            // completely remove the first block line to force the player to jump
            m_bFirstLine = false;
            for(int i = 0; i < BACK_BLOCKS_X; ++i) abHole[i] = true;
        }
        else
        {
            // calculate canyon length, so it can only be crossed by using a trap jump
            const int iBorder = int(FLOOR(1.0f + 2.2f*g_fCurSpeed));

            // activate narrow stage later
            if(m_fTime >= STAGE_FINAL+0.5f) m_iNarrow = 1;
            const int iNarrowUp = 5 - m_iNarrow;

            // update canyon counter
            if((!m_iNarrow || m_iCanyonCounter >= 0) && !m_bChallenge) ++m_iCanyonCounter;

            if(m_iCanyonCounter < 0 || m_iCanyonCounter > iBorder)
            { 
                // define the next thing to spawn (-1 = hole)
                int iSelection = Core::Rand->Int(0, 10);

                if(m_bChallenge)
                {
                    // calculate next spawn column in challenge mode
                    if(Core::Rand->Int(0,1)) m_iCurSpawn = CLAMP(m_iCurSpawn + CLAMP(Core::Rand->Int((m_iCurSpawn >= 3) ? -(m_iCurSpawn-2) : -1, (m_iCurSpawn <= 2) ? (3-m_iCurSpawn) : 1), -1, 1), m_iNarrow, iNarrowUp);
                    
                    // remove all unused plates
                    for(int i = 1; i < BACK_BLOCKS_X-1; ++i)
                        abHole[i] = true;

                    // change beverage to triple-hole
                    if(!(m_iCurLine % 8) && m_iCanyonCounter < 0) iSelection = -1;
                }
                else
                {
                    // control spawn column
                    if(m_fTime >= STAGE_TOGGLE && m_fTime < STAGE_SIDE_1)   // # toggle stage
                    {
                        // calculate next spawn column
                        if(m_iCurLine % 2)
                        {
                                 if(m_iCurSpawn == m_iNarrow) ++m_iCurSpawn;
                            else if(m_iCurSpawn == iNarrowUp) --m_iCurSpawn;
                            else m_iCurSpawn = CLAMP(m_iCurSpawn + (Core::Rand->Int(0, 1) ? 1 : -1), m_iNarrow, iNarrowUp);
                        }

                        // toggle current side 
                        if(!(m_iCurLine % 8) && m_iCanyonCounter < 0) m_iCurSpawn = 5 - m_iCurSpawn;

                        // prevent canyon
                        m_iCanyonCounter = CANYON_PREVENT;
                    }
                    else if(m_fTime >= STAGE_SIDE_1 && m_fTime < STAGE_NET)   // # side stage
                    {
                        const bool bToLeft = m_bSideOrder ? (m_fTime >= STAGE_SIDE_2) : (m_fTime < STAGE_SIDE_2);

                        // calculate next spawn column
                        if(m_fTime >= STAGE_SIDE_2 || m_iCurLine % 2)
                        {
                            if(bToLeft) {if(--m_iCurSpawn < m_iNarrow) m_iCurSpawn = iNarrowUp;}
                                   else {if(++m_iCurSpawn > iNarrowUp) m_iCurSpawn = m_iNarrow;}
                        }

                        // activate interpolated camera
                        if(m_iCamStatus == 0 || 
                          (m_iCamStatus == 1 && m_fTime >= STAGE_SIDE_2))
                        {
                            if(m_iCamStatus)
                            {
                                g_fCamSpeed = 0.8f;
                                g_fOldCam   = g_fTargetCam;
                            }
                            g_fCamTime = 0.0f;
                            g_bCamMode = true;
                            ++m_iCamStatus;
                        }

                        // move camera to the side
                        if(bToLeft) g_fTargetCam =  1.0f;
                               else g_fTargetCam = -1.0f;

                        // prevent canyon
                        m_iCanyonCounter = CANYON_PREVENT;
                    }
                    else
                    {
                        if(m_fTime >= STAGE_MASS && m_fTime < STAGE_FINAL)   // # mass-canyon stage
                        {
                            // always change column on the mass-canyon stage (but only once per jump)
                            if(m_iCanyonCounter == iBorder+1)
                                m_iCurSpawn = CLAMP(m_iCurSpawn + (Core::Rand->Int((m_iCurSpawn <= 1) ? 1 : 0, (m_iCurSpawn >= BACK_BLOCKS_X-4) ? 0 : 1) ? 1 : -1) * Core::Rand->Int(1,2), m_iNarrow, iNarrowUp);
                        }
                        else   // # all other stages
                        {
                            if(m_iThreeDrinks) --m_iThreeDrinks;
                            else 
                            {
                                // calculate next spawn column
                                const int iOldSpawn = m_iCurSpawn;
                                if(Core::Rand->Int(0, (m_fTime < STAGE_NET || m_fTime > STAGE_FINAL) ? 2 : 1)) m_iCurSpawn = CLAMP(m_iCurSpawn + CLAMP(Core::Rand->Int((m_iCurSpawn >= 3) ? -(m_iCurSpawn-2) : -1, (m_iCurSpawn <= 2) ? (3-m_iCurSpawn) : 1), -1, 1), m_iNarrow, iNarrowUp);
                            
                                // create at least three drinks in a row after a column change
                                if(iOldSpawn != m_iCurSpawn)
                                    m_iThreeDrinks = 2;
                            }
                        }

                        // move camera back to center
                        if(m_fTime < STAGE_MASS && m_iCamStatus == 2)
                        {
                            g_fCamSpeed  = 1.0f;
                            g_fOldCam    = g_fTargetCam;
                            g_fCamTime   = 0.0f;
                            g_bCamMode   = true;
                            g_fTargetCam = 0.0f;
                            ++m_iCamStatus;
                        }
                        else m_iCurSpawn = CLAMP(m_iCurSpawn, 1, BACK_BLOCKS_X-4);   // force early narrow
                    }

                    // control path-holes
                    if(m_fTime < STAGE_NET)
                    {
                        // add holes to create a path around spawn locations
                        const int iBroad = MAX(5 - int(FLOOR(m_fTime * 0.2f)), 2);
                        for(int i = 1; i < BACK_BLOCKS_X-1; ++i)
                            abHole[i] = !(m_iCurSpawn >= i-iBroad && m_iCurSpawn <= i+(iBroad-2));
                    }
                    else
                    {
                        if(m_fTime >= STAGE_FINAL)
                        {
                            // create broad path in the final stage
                            for(int i = 1; i < BACK_BLOCKS_X-1; ++i)
                                abHole[i] = !(m_iCurSpawn >= i-3 && m_iCurSpawn <= i+1);
                        }
                        else // >= STAGE_NET
                        {
                            // add holes to create a net pattern
                            for(int i = 1; i < BACK_BLOCKS_X-1; ++i)
                                abHole[i] = !(m_iCurSpawn % 2 != i % 2);
                        }

                        // prevent canyon (and force mass-canyon later, by altering PREVENT and BEGIN)
                        m_iCanyonCounter = CANYON_PREVENT;
                    }

                    // control triple-holes
                    if(m_fTime >= STAGE_HOLES && m_fTime < STAGE_SIDE_1)
                    {
                        // change beverage to triple-hole
                        if(!(m_iCurLine % 8) && m_iCanyonCounter < 0) iSelection = -1;
                    }
                    else if(m_fTime >= STAGE_FINAL+4.0f)
                    {
                        // create triple-holes with increasing rate at the final stage
                        if(++m_iFinalHoles >= int(2.0f + 6.0f / (1.0f + 0.1f*(m_fTime-(STAGE_FINAL+4.0f)))))
                        {
                            m_iFinalHoles =  0;
                            iSelection    = -1;
                        }
                    }

                    // begin regular canyon creation at specific times 
                    if(m_iCanyonCounter < CANYON_LIMIT)
                    {
                        if((m_fTime >= STAGE_CANYON && m_fTime < STAGE_CANYON+10.0f) ||
                           (m_fTime >= STAGE_MASS   && m_fTime < STAGE_FINAL))
                            m_iCanyonCounter = CANYON_BEGIN;
                    }

                    // override everything on the random area
                    if(m_fTime >= STAGE_RANDOM && m_fTime < STAGE_MASS)
                    {
                        m_iCurSpawn = Core::Rand->Int(m_iNarrow, iNarrowUp);
                        m_iCanyonCounter = CANYON_PREVENT;

                        for(int i = 1; i < BACK_BLOCKS_X-1; ++i)
                            abHole[i] = false;
                    }
                }

                cBeverage* pBeverage = NULL;
                if(m_bChallenge)
                {
                    // create only coola bottles in challenge mode
                    if(iSelection >= 0) pBeverage = new cCoola();
                }
                else
                {
                    // create new beverage
                         if(iSelection >= 0  && iSelection <=  5) pBeverage = new cSunrise();
                    else if(iSelection >= 6  && iSelection <=  9) pBeverage = new cMojito();
                    else if(iSelection >= 10 && iSelection <= 10) pBeverage = new cBlue();
                }

                if(pBeverage)
                {
                    // prepare beverage and add to list
                    pBeverage->SetPosition(coreVector3(BACK_SPAWN_X(m_iCurSpawn, 1.5f), fSpawn, 0.0f));
                    m_apBeverage.push_back(pBeverage);

                    // prevent hole
                    abHole[m_iCurSpawn+1] = false;
                }
                else
                {
                    // create triple-holes instead of beverage
                    abHole[m_iCurSpawn+1] = true;
                    abHole[CLAMP(m_iCurSpawn,   0, BACK_BLOCKS_X-1)] = true;
                    abHole[CLAMP(m_iCurSpawn+2, 0, BACK_BLOCKS_X-1)] = true;
                }

                if(m_bTrapSpawn)
                {
                    // random trap spawn
                    if(Core::Rand->Int(0, 65) < m_iTrapChance && iSelection >= 0)
                    {
                        m_iTrapChance = 0;

                        // add trap
                        int iTrapSpawn = 0;
                        do
                        {
                            iTrapSpawn = CLAMP(m_iCurSpawn + Core::Rand->Int(-1,2), 1, BACK_BLOCKS_X-3);
                        }
                        while(iTrapSpawn == m_iCurSpawn);   // not overlapping with a beverage
                        this->AddTrap(iTrapSpawn, fSpawn);

                        // prevent hole
                        abHole[iTrapSpawn+1] = false;
                    }
                    else ++m_iTrapChance;
                }

                if(!m_bChallenge)
                {
                    // regularly create coolas
                    if(++m_iCoolaCounter >= 100 && iSelection >= 0 && m_iTrapChance > 0)
                    {
                        m_iCoolaCounter = 0;

                        // calculate spawn on free plate (not in sync with BACK_BLOCKS_X, assumes 8 real columns)
                        const int iColaSpawn = CLAMP(m_iCurSpawn + ((m_fTime < STAGE_NET) ? 1 : Core::Rand->Int(1, 2)) * (m_iCurSpawn <= 2 ? 1 : -1), m_iNarrow+1, iNarrowUp-1);

                        // create new coola and add to list
                        cCoola* pCoola = new cCoola();
                        pCoola->SetPosition(coreVector3(BACK_SPAWN_X(iColaSpawn, 1.5f), fSpawn, 0.0f));
                        m_apBeverage.push_back(pCoola);

                        // prevent hole
                        abHole[iColaSpawn+1] = false;
                    }

                    // create Franka the polar bear
                    if(g_iNumGames >= 3 && m_fTime >= 10.0f)
                    {
                        if(!Core::Rand->Int(0,4))
                        {
                            g_iNumGames = 0;

                            // calculate spawn
                            const int iBearSpawn = (m_iCurSpawn > 3) ? m_iNarrow : iNarrowUp;

                            // create new Franka and add to list
                            cFranka* pFranka = new cFranka();
                            pFranka->SetPosition(coreVector3(BACK_SPAWN_X(iBearSpawn, 1.5f), fSpawn, 0.0f));
                            m_apBeverage.push_back(pFranka);

                            // prevent hole
                            abHole[iBearSpawn+1] = false;
                        }
                    }

                    // always create at least one plate per row
                    // abHole[Core::Rand->Int(1, BACK_BLOCKS_X-2)] = false; #deactivated
                }
            }

            // remove additional sides when narrow
            if(m_iNarrow) abHole[m_iNarrow] = abHole[BACK_BLOCKS_X-1-m_iNarrow] = true;

            if(m_iCanyonCounter >= 0)
            {
                // check for canyon end
                if(m_iCanyonCounter == iBorder + CANYON_AFTER)
                {
                    // reset counter and enable trap generation
                    m_iCanyonCounter = -CANYON_DISTANCE;
                    m_bTrapSpawn = true;
                }
                else m_bTrapSpawn = false;

                // create the canyon, always prevent holes before and after the canyon
                if((m_iCanyonCounter <= CANYON_BEFORE || m_iCanyonCounter > iBorder)) {for(int i = 1; i < BACK_BLOCKS_X-1; ++i) abHole[i] = !(i >= m_iCurSpawn && i <= m_iCurSpawn+2);}
                                                                                 else {for(int i = 1; i < BACK_BLOCKS_X-1; ++i) abHole[i] = true;}

                // always add trap at the canyon edge to jump across
                if(m_iCanyonCounter == CANYON_BEFORE) this->AddTrap(CLAMP(m_iCurSpawn, 1, BACK_BLOCKS_X-4), fSpawn);     
            }
            else
            {
                // randomly create moving plates inside of holes
                const float fPlateCmp = MAX(1.0f - ((m_fTime-0.0f) / 30.0f), 0.1f) * (m_fTime < 77.0f ? 3.0f : 1.0f);
                for(int i = 1; i < BACK_BLOCKS_X-1; ++i)
                {
                    if(abHole[i] && (m_fTime < 10.0f || (Core::Rand->Float(0.0f,1.0f) < fPlateCmp && (!abHole[MAX(i-1,1)] || !abHole[MIN(i+1,BACK_BLOCKS_X-2)]))))
                    {
                        // create plate and add to list
                        cPlate* pPlate = new cPlate(90.0f + Core::Rand->Float(0.0f,120.0f), coreVector2(float(i), -FLOOR(g_pBackground->GetPositionTime())));
                        pPlate->SetPosition(coreVector3(BACK_SPAWN_X(i, 0.5f), fSpawn, GAME_HEIGHT));
                        m_apPlate.push_back(pPlate);
                    }
                }
            }
        }

        // regularly create rays
        if(++m_iRayCounter >= 2)
        {
            m_iRayCounter = 0;

            // calculate start-position
            const coreVector2 vAround = coreVector2(Core::Rand->Float(-2.0f, 2.0f), 1.0f).Normalize() * 90.0f;

            // create new ray and add to list
            cRay* pRay = new cRay(coreVector3(vAround.x, fSpawn, vAround.y));
            m_apRay.push_back(pRay);
        }

        // always remove both outer blocks
        abHole[0] = abHole[BACK_BLOCKS_X-1] = true;

        // update the holes in the dance floor (sometimes twice to sync for the infinit look)
        g_pBackground->UpdateHoles(m_iCurLine + BACK_VIEW, abHole);
        if(m_iCurLine + BACK_VIEW >= BACK_REPEAT) g_pBackground->UpdateHoles((m_iCurLine + BACK_VIEW) % BACK_REPEAT, abHole);
    }

    // move camera at the final stage
    if(m_fTime >= STAGE_MASS+4.0f && (g_fTargetCam || !this->GetStatus()))
    {
        g_fTargetCam += Core::System->GetTime() * 0.15f;
        g_bCamMode    = false;
    }

    // shake camera upside-down
    if(m_fTime < 5.0f && ABS(m_Rock.GetShake()) >= 1.0f && !g_bUpsideDown && m_bShakeEnable)
    {
        g_bUpsideDown = true;
        g_fTargetCam  = 0.0f;
        g_fCurCam     = 0.0f;

        // flash screen
        g_pMenu->ChangeSurface(5,  0.0f);
        g_pMenu->ChangeSurface(10, 1.0f);

        // play sound
        m_pTrapSound->PlayPosition(NULL, 0.38f, 0.8f, 0.05f, false, m_Rock.GetPosition());
    }
    if(!m_iCamStatus)
    {
        if(m_fTime >= 5.0f || g_bUpsideDown)
        {
            // reset camera smoothly
            g_fTargetCam = 0.0f;
            g_fCamSpeed  = 1.0f;
        }
        else
        {
            if(m_bShakeEnable)
            {
                // change camera and increase animation speed
                g_fTargetCam = m_Rock.GetShake() * 0.55f;
                g_fCamSpeed  = 1.0f + ABS(m_Rock.GetShake()) * 11.0f;
            }
        }
    }

    // move rock
    if(!this->GetStatus())
    {
#if defined(_CORE_ANDROID_)
        m_Interface.InteractControl();
#endif
        m_Rock.Move();
    }

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
            if(pBeverage->GetPosition().y < 10.0f && coreObject3D::Collision(*pBeverage, m_Rock))
            {
                // calculate and increase score
                const float fValue = float(pBeverage->GetScore()) * COMBO_MULTI;
                m_dScore += (double)fValue;

                // increase statistics
                ++m_aiCollected[0];
                ++m_aiCollected[pBeverage->GetSigID()];
                     if(pBeverage->GetSigID() == 3) m_iCollectedNoBlue = 0;
                else if(pBeverage->GetSigID() <  3) ++m_iCollectedNoBlue;

                // increase combo
                ++m_iCombo;
                m_fComboDelay = 1.0f;

                const float fTextAlpha = 1.0f - m_fTime * 0.008f;
                if(pBeverage->GetSigID() == 4 || fTextAlpha > 0.2f)
                {
                    // get and modify beverage color
                    coreVector4 vColor = coreVector4(pBeverage->GetSigColor(), 1.0f);
                    if(pBeverage->GetSigID() != 4) vColor.a *= CLAMP(fTextAlpha + 0.2f, 0.0f, 1.0f);

                    // create floating score text
                    g_pCombatText->AddTextTransformed(fValue ? PRINT("%.0f", fValue) : "RAMPAGE", m_Rock.GetPosition(), vColor);
                }
                
                // create max combo text (after score text)
                if(m_iCombo == 18) g_pCombatText->AddTextTransformed("+MAXIMUM", m_Rock.GetPosition(), coreVector4(COLOR_ORANGE_F, 1.0f));

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

    // update all inactive beverages, traps, plates and rays
    PROCESS_OBJECT_ARRAY(m_apDestroyed, fMove30)
    PROCESS_OBJECT_ARRAY(m_apTrap,      fMove10)
    PROCESS_OBJECT_ARRAY(m_apPlate,     fMove10)
    PROCESS_OBJECT_ARRAY(m_apRay,       fMove10)

    // test trap collisions
    FOR_EACH(it, m_apTrap)
    {
        cTrap* pTrap = (*it);

        // check for collision with traps
        if(pTrap->GetPosition().y < 10.0f && coreObject3D::Collision(*pTrap, m_Rock))
        {
            if(m_Rock.Jump(10.0f))
            {
                m_bTrapJump = true;
                ++m_iCollectedTraps;

                // play trap sound effect and show message
                m_pTrapSound->PlayPosition(NULL, 0.38f, 1.1f, 0.05f, false, m_Rock.GetPosition());
                g_pCombatText->AddTextTransformed(g_MsgTrap.Get(), m_Rock.GetPosition(), coreVector4(COLOR_WHITE_F, 1.0f));

                // reset combo timer (a little bit more than a beverage)
                m_fComboDelay = 1.1f;
            }
        }
    }
    if(!m_Rock.GetJumped()) m_bTrapJump = false;

    // update combo
    m_fComboDelay -= Core::System->GetTime();
    if(m_fComboDelay <= 0.0f && m_iCombo)
    {
        // reset combo
        g_pCombatText->AddTextTransformed("-COMBO", m_Rock.GetPosition(), coreVector4(COLOR_RED_F, 1.0f));
        m_iCombo     = 0;
        m_fComboTime = 0.0f;
    }
    if(m_iCombo >= COMBO_MAX) m_fComboTime.Update(1.0f);

    // update max combo value
    m_iMaxCombo = MAX(m_iMaxCombo, m_iCombo);

    // increase score over time
    m_dScore += double(Core::System->GetTime() * 10.0f * (1.0f + m_fTime*0.022f));

    // update the interface object
    if(this->GetStatus()) m_Interface.Hide();
    m_Interface.Update((float)m_dScore, m_fTime, COMBO_MULTI, MIN((float)m_fComboDelay, 0.7f) * 1.4286f);
    m_Interface.Move();

    if(m_ShowMessage.GetStatus())
    {
        m_ShowMessage.Update(1.0f);

        // update message at the beginning
        m_Message.SetAlpha(MIN(2.0f * SIN(m_ShowMessage.GetValue(CORE_TIMER_GET_NORMAL) * PI), 1.0f));
        m_Message.SetDirection(coreVector2::Direction(0.2f * SIN(m_ShowMessage.GetValue(CORE_TIMER_GET_NORMAL) * PI * 4.0f)));
        m_Message.SetScale(g_pBackground->GetFlash(0.4f));
        m_Message.Move();
    }

    if(!this->GetStatus())
    {
        // handle first big air-jump
        if(m_iFirstJump < 2 && m_Rock.GetReflected()) ++m_iFirstJump;

        // check for achievements/trophies/whatever you call that stuff everybody likes, maybe I should implement a share-button with selfie-attachement-function as well
        if(!m_bTrophyHelper[ 0] && m_iFirstJump == 1 && m_Rock.GetJumped())                         {this->AchieveTrophy(GJ_TROPHY_01,  0);}
        if(!m_bTrophyHelper[ 1] && m_Rock.GetFallen() && m_fTime < 10.0f)                           {this->AchieveTrophy(GJ_TROPHY_02,  1); if(++g_iNumFails == 5) coreData::OpenURL(Core::Rand->Int(0,1) ? "https://images.search.yahoo.com/search/images?p=facepalm" : "https://www.google.com/search?q=facepalm&tbm=isch");}
        if(!m_bTrophyHelper[ 2] && m_Rock.GetFallen() && m_bTrapJump)                               {this->AchieveTrophy(GJ_TROPHY_03,  2);}
        if(!m_bTrophyHelper[ 4] && m_aiCollected[5])                                                {this->AchieveTrophy(GJ_TROPHY_05,  4);}
        if(!m_bTrophyHelper[ 5] && m_fComboTime >= 20.0f)                                           {this->AchieveTrophy(GJ_TROPHY_06,  5);}  
        if(!m_bTrophyHelper[ 6] && m_aiCollected[4] >= 2 && !m_bChallenge)                          {this->AchieveTrophy(GJ_TROPHY_07,  6);}
        if(!m_bTrophyHelper[ 7] && m_iCollectedNoBlue >= 90 && !m_bChallenge)                       {this->AchieveTrophy(GJ_TROPHY_08,  7);}
        if(!m_bTrophyHelper[ 8] && m_Rock.GetNumJumps() >= 80)                                      {this->AchieveTrophy(GJ_TROPHY_09,  8);}
        if(!m_bTrophyHelper[ 9] && !m_aiCollected[0] && m_bTrapJump)                                {this->AchieveTrophy(GJ_TROPHY_10,  9);}
        if(!m_bTrophyHelper[10] && m_dScore >= 30000.0 && !m_bChallenge)                            {this->AchieveTrophy(GJ_TROPHY_11, 10);}
        if(!m_bTrophyHelper[11] && m_dScore >= 20000.0 && !m_bChallenge && g_bUpsideDown)           {this->AchieveTrophy(GJ_TROPHY_12, 11);}
        if(!m_bTrophyHelper[12] && m_fTime >= 100.0f)                                               {this->AchieveTrophy(GJ_TROPHY_13, 12);}
        if(!m_bTrophyHelper[13] && m_dScore >= 10000.0 && !m_bChallenge && m_iMaxCombo < COMBO_MAX) {this->AchieveTrophy(GJ_TROPHY_14, 13);}
        if(!m_bTrophyHelper[14] && m_dScore >= 250000.0 && m_bChallenge)                            {this->AchieveTrophy(GJ_TROPHY_15, 14);}
        
        // [ 0] X(Get the Party Started,    s1) Make an air-jump at the beginning.
        // [ 1]  (I don't like Disco,       s1) Fall down the dance floor in less than 10 seconds.
        // [ 2] X(Too Much Too Soon,        s1) Fall down the dance floor while on a high-jump.
        // [ 3] X(Show me your Moves,        1) Submit your score and time online. <<in menu, with centered animation>>

        // [ 4] X(Party Animal,             s2) Meet Franka the polar bear.
        // [ 5]  (Binge Drinker,             2) Hold a combo multiplier of x10 for at least 20 seconds.
        // [ 6]  (Designated Driver,         2) Bump into 2 Coola bottles within one normal run.
        // [ 7] X(Before there was any Blue, 2) Hit 90 normal beverages in a row, without touching a blue drink.

        // [ 8] X(The Floor is Lava,         3) Jump 80 times in one run. (high-jumps count as one jump)
        // [ 9] X(Sober in the 70s,          3) Roll to the first high-jumper without touching beverages, not even a Coola bottle.
        // [10]  (Shagadelic,                3) Get at least 30.000 points in a normal run. (Tip: don't drop your combo and hit Coola bottles)	
        // [11] X(Boy, You Turn Me,          3) Shake the world upside-down and get 20.000 points in a normal run.

        // [12]  (You Should Be Dancing,     4) Survive at least 100 seconds on the dance floor.
        // [13]  (Get Up and Boogie,         4) Reach 10.000 points without ever getting a combo multiplier of x10.
        // [14] X(The Coola Challenge,       4) Hold 'C' (or a screen corner) while starting a game and get at least 250.000 points.
    }
}


// ****************************************************************
// add trap object
void cGame::AddTrap(const int& iBlock, const float& fSpawn)
{
    // create new trap and add to list
    cTrap* pTrap = new cTrap();
    pTrap->SetPosition(coreVector3(BACK_SPAWN_X(iBlock, 1.5f), fSpawn, GAME_HEIGHT + pTrap->GetSize().z*0.51f));
    m_apTrap.push_back(pTrap);
}


// ****************************************************************
// achieve a trophy
void cGame::AchieveTrophy(const int& iID, const int& iNum)
{
    // update helper
    m_bTrophyHelper[iNum] = true; 

    // achieve trophy
    gjTrophy* pTrophy = g_pOnline->GameJolt()->InterTrophy()->GetTrophy(iID);
    if(g_pOnline->IsUserConnected()) g_pOnline->AchieveTrophy(pTrophy, this, &cGame::AchieveTrophyCallback, (void*)(long)iNum);
    else
    {
        // handle guest user
        this->AchieveTrophyCallback(pTrophy, (void*)(long)iNum);
    }
}


// ****************************************************************
// callback for trophy achievements
void cGame::AchieveTrophyCallback(const gjTrophyPtr& pTrophy, void* pData)
{
    const int iNum = (long)pData;

    if(pTrophy)
    {
        if(!(g_pMenu->GetTrophyStatus() & BIT(iNum)))
        {
            const coreVector3 vPos = this->GetStatus() ? coreVector3(0.0f,0.0f,0.0f) : m_Rock.GetPosition();

            // show achievement title and play sound
            g_pCombatText->ShowTrophy(coreData::StrUpper(("<< " + pTrophy->GetTitle() + " >>").c_str()), vPos);
            m_pTrophySound->PlayPosition(NULL, 0.2f, 1.0f, 0.0f, false, vPos);

            // set and save current trophy status
            g_pMenu->SetTrophyStatus(g_pMenu->GetTrophyStatus() | BIT(iNum));
            g_pMenu->SetTrophyCurrent(-1);
        }

        // save status always in offline config
        Core::Config->SetInt("Game", "Trophy", Core::Config->GetInt("Game", "Trophy", 0) | BIT(iNum));
    }
}