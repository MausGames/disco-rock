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
bool Intersection(const coreObject3D &Object1, const coreObject3D &Object2, coreVector3* pvPoint, const float& fFactor)
{
    // get bounding spheres
    const float fDistance1   = Object1.GetModel()->GetRadius() * Object1.GetSize().Max();
    const float fDistance2   = Object2.GetModel()->GetRadius() * Object2.GetSize().Max();
    const float fMaxDistance = (fDistance1 + fDistance2) * fFactor;

    // calculate distance between both objects
    const coreVector3 vDiff = Object2.GetPosition() - Object1.GetPosition();

    // check for intersection
    if(vDiff.LengthSq() > fMaxDistance * fMaxDistance)
    {
        if(pvPoint) *pvPoint = coreVector3(0.0f,0.0f,0.0f);
        return false;
    }

    // return impact point
    if(pvPoint) *pvPoint = vDiff.Normalized() * fDistance1 + Object1.GetPosition();
    return true;
}

// macro function for calculating the current score multiplier
#define COMBO_MAX    18
#define COMBO_MULTI (1.0f + 0.5f * float(MIN(m_iCombo, (unsigned)COMBO_MAX)))

// macro function for moving and removing game objects
#define PROCESS_OBJECT_ARRAY(a,m)                           \
    FOR_EACH_DYN(it, a)                                     \
    {                                                       \
        coreObject3D* o = (*it);                            \
        const coreVector3& p = o->GetPosition();            \
                                                            \
        if(o->GetStatus() == 1)                             \
        {                                                   \
            SAFE_DELETE(o)                                  \
            DYN_REMOVE(it, a)                               \
        }                                                   \
        else                                                \
        {                                                   \
            o->SetPosition(coreVector3(p.x, p.y - m, p.z)); \
            o->Move();                                      \
            DYN_KEEP(it)                                    \
        }                                                   \
    }


// ****************************************************************
cGame::cGame(const bool& bChallenge)noexcept
: m_iCurLine       (g_pBackground->GetCurLine())
, m_iCurSpawn      (Core::Rand->Int(2,3))   // not sync
, m_bLastHole      (true)
, m_dScore         (0.0)
, m_fTime          (0.0f)
, m_iCombo         (0)
, m_iMaxCombo      (0)
, m_fComboTime     (0.0f)
, m_fComboDelay    (0.0f)
, m_iCoolaCounter  (0)
, m_iRayCounter    (0)
, m_iCanyonCounter (-CANYON_DISTANCE)
, m_iTrapChance    (0)
, m_bFirstLine     (true)
, m_bFirstMsg      (true)
, m_bTrapSpawn     (false)
, m_bChallenge     (bChallenge)
, m_iNarrow        (0)
, m_Message        (FONT_ROCKS, 34, 0)
, m_ShowMessage    (coreTimer(1.0f, 0.333f, 1))
{
    // reserve object memory
    m_apBeverage.reserve(64);
    m_apDestroyed.reserve(16);
    m_apTrap.reserve(16);
    m_apPlate.reserve(64);
    m_apRay.reserve(16);

    // create beginning message
    m_ShowMessage.Play(false);
    m_ShowMessage.SetCurrent(-0.333f);

    m_Message.SetPosition(coreVector2(0.0f,0.15f));
    m_Message.SetText(g_MsgIntro.Get());
    
    // reset statistics and trophy cache
    for(int i = 0; i < 5;             ++i) m_aiCollected[i]   = 0;
    for(int i = 0; i < GAME_TROPHIES; ++i) m_bTrophyHelper[i] = false;

    // load trap sound effect
    m_pTrapSound = Core::Manager::Resource->LoadFile<coreSound>("data/sounds/trap.wav");
}


// ****************************************************************
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
void cGame::Render()
{
    glDisable(GL_DEPTH_TEST);

    // render beverage shadows
    FOR_EACH_REV(it, m_apBeverage)  (*it)->RenderShadow();
    FOR_EACH_REV(it, m_apDestroyed) (*it)->RenderShadow();

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

    // render traps
    glCullFace(GL_FRONT);
    FOR_EACH_REV(it, m_apTrap) (*it)->Render();

    glCullFace(GL_BACK);
    FOR_EACH_REV(it, m_apTrap) (*it)->Render();

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

    // render rays
    FOR_EACH_REV(it, m_apRay) (*it)->Render();

    // render global particle system
    g_pParticleSystem->Render();

    glDepthMask(true);
    
    // render interface
    m_Interface.Render();
    if(m_ShowMessage.GetStatus())
    {
        // render beginning message
        if(m_ShowMessage.GetCurrent(false) >= 0.0f)
            m_Message.Render();
    }
}


// ****************************************************************
void cGame::Move()
{
    // update game time and calculate target speed (speed is interpolated smoothly in the main file)
    m_fTime.Update(1.0f);
    g_fTargetSpeed = this->GetStatus() ? 1.5f : (1.5f + m_fTime*0.018f);

    // show message on first jump (over the first line)
    if(m_bFirstMsg)
    {
        if(m_fTime >= 5.1f && m_Rock.GetJumped())
        {
            m_bFirstMsg = false;
            g_pCombatText->AddTextTransformed(g_MsgBegin.Get(), m_Rock.GetPosition(), COLOR_WHITE_F);
        }
    }

    // calculate movement value
    const float fMove10 = Core::System->GetTime(0) * 2.5f * BACK_DETAIL_Y;

    // check for the next line on the horizon
    const int iNewCurLine = g_pBackground->GetCurLine();
    if(m_iCurLine != iNewCurLine)   // start line processing magic
    {
        m_iCurLine = iNewCurLine;

        // array used to create holes in the dance floor (true = hole, false = no hole)
        bool abHole[BACK_BLOCKS_X];

        // compensate framerate (background position) and movement when spawning objects
        const float fSpawn = BACK_SPAWN_Y + fMove10 - BACK_DETAIL_Y * (g_pBackground->GetPositionTime() - std::floor(g_pBackground->GetPositionTime()));

        if(m_bFirstLine)
        {
            // completely remove the first block line to force the player to jump
            m_bFirstLine = false;
            for(int i = 0; i < BACK_BLOCKS_X; ++i) abHole[i] = true;
        }
        else
        {
            // calculate canyon length, so it can only be crossed by using a trap jump
            const int iBorder = int(std::floor(1.0f + 2.2f*g_fCurSpeed));

            // update canyon counter, prevent uncontrolled holes from spawning before and after a canyon
            if(!m_iNarrow) if(++m_iCanyonCounter >= 0) m_bLastHole = true;

            if(m_iCanyonCounter < 0 || m_iCanyonCounter > iBorder)
            { 
                // calculate next spawn column (not in sync with BACK_BLOCKS_X, assumes 8 real columns)
                if(Core::Rand->Int(0,1)) m_iCurSpawn = CLAMP(m_iCurSpawn + CLAMP(Core::Rand->Int((m_iCurSpawn >= 3) ? -(m_iCurSpawn-2) : -1, (m_iCurSpawn <= 2) ? (3-m_iCurSpawn) : 1), -1, 1), 0+m_iNarrow, 5-m_iNarrow);

                // add random holes modified by current time
                for(int i = 1; i < BACK_BLOCKS_X-1; ++i) abHole[i] = (Core::Rand->Float(0.0f, 1.0f + (30.0f / m_fTime)) < (m_fTime / 180.0f) || m_bChallenge) ? true : false;

                // define the next thing to spawn (-1 = hole)
                const int iSelection = Core::Rand->Int(m_bLastHole ? 0 : -1, 10);
                if(m_fTime > 10.0f) m_bLastHole = false;

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
                    m_bLastHole           = true;
                    abHole[m_iCurSpawn+1] = true;
                    abHole[CLAMP(m_iCurSpawn,   0, BACK_BLOCKS_X-1)] = true;
                    abHole[CLAMP(m_iCurSpawn+2, 0, BACK_BLOCKS_X-1)] = true;
                }

                if(m_bTrapSpawn)
                {
                    // random trap spawn
                    if(Core::Rand->Int(0, 70) < m_iTrapChance)
                    {
                        m_iTrapChance = 0;

                        // add trap
                        const int iTrapSpawn = Core::Rand->Int(0+m_iNarrow, BACK_BLOCKS_X-3-m_iNarrow);
                        this->AddTrap(iTrapSpawn, fSpawn);

                        // prevent hole
                        abHole[iTrapSpawn+1] = false;
                    }
                    else ++m_iTrapChance;
                }

                if(!m_bChallenge)
                {
                    // regularly create coolas
                    if(++m_iCoolaCounter >= 100)
                    {
                        m_iCoolaCounter = 0;

                        // calculate spawn on free plate (not in sync with BACK_BLOCKS_X, assumes 8 real columns)
                        const int iColaSpawn = CLAMP(m_iCurSpawn + Core::Rand->Int(2, 3) * (m_iCurSpawn <= 2 ? 1 : -1), 0+m_iNarrow, 5-m_iNarrow);

                        // create new coola and add to list
                        cCoola* pCoola = new cCoola();
                        pCoola->SetPosition(coreVector3(BACK_SPAWN_X(iColaSpawn, 1.5f), fSpawn, 0.0f));
                        m_apBeverage.push_back(pCoola);

                        // remove hole under coola
                        abHole[iColaSpawn+1] = false;
                    }
                }

                // always create at least one plate per row
                abHole[Core::Rand->Int(1, BACK_BLOCKS_X-2)] = false;
            }

            if(m_iCanyonCounter >= 0)
            {
                // check for canyon end
                if(m_iCanyonCounter == iBorder + CANYON_AFTER)
                {
                    // reset counter and enable trap generation
                    m_iCanyonCounter = -CANYON_DISTANCE;
                    m_bTrapSpawn = true;

                    // activate narrow stage later
                    if(m_fTime >= 90.0f) m_iNarrow = 1;
                }
                else m_bTrapSpawn = false;

                // create the canyon, always prevent holes before and after the canyon
                if((m_iCanyonCounter <= CANYON_BEFORE || m_iCanyonCounter > iBorder) && !m_iNarrow) {for(int i = 0; i < BACK_BLOCKS_X; ++i) abHole[i] = false;}
                                                                                               else {for(int i = 0; i < BACK_BLOCKS_X; ++i) abHole[i] = true;}

                // always add trap at the canyon edge to jump across
                if(m_iCanyonCounter == CANYON_BEFORE) this->AddTrap(CLAMP(m_iCurSpawn + Core::Rand->Int(-1, 1), 1, BACK_BLOCKS_X-4), fSpawn);     
            }
            else
            {
                // randomly create moving plates inside of holes
                for(int i = 1+m_iNarrow; i < BACK_BLOCKS_X-1-m_iNarrow; ++i)
                {
                    if(abHole[i] && (Core::Rand->Float(0.0f,1.0f) < (1.0f - (m_fTime / 100.0f)) || m_fTime < 15.0f))
                    {
                        // create plate and add to list
                        cPlate* pPlate = new cPlate(90.0f + Core::Rand->Float(0.0f,120.0f), coreVector2(float(i), -std::floor(g_pBackground->GetPositionTime())));
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
        if(m_iNarrow) abHole[m_iNarrow] = abHole[BACK_BLOCKS_X-1-m_iNarrow] = true;

        // update the holes in the dance floor (sometimes twice to sync for the infinit look)
        g_pBackground->UpdateHoles(m_iCurLine + BACK_VIEW, abHole);
        if(m_iCurLine + BACK_VIEW >= BACK_REPEAT) g_pBackground->UpdateHoles((m_iCurLine + BACK_VIEW) % BACK_REPEAT, abHole);
    }

    // move rock
    if(!this->GetStatus()) m_Rock.Move();

    // update all active beverages
    FOR_EACH_DYN(it, m_apBeverage)
    {
        cBeverage* pBeverage = (*it);
        const coreVector3& vPos = pBeverage->GetPosition();

        if(vPos.y <= -48.0f)
        {
            // remove beverage when finished
            SAFE_DELETE(pBeverage)
            DYN_REMOVE(it, m_apBeverage)
        }
        else
        {
            // check for intersection with beverages
            coreVector3 vPoint;
            if(pBeverage->GetPosition().y < 10.0f && Intersection(*pBeverage, m_Rock, &vPoint, 1.0f))
            {
                // calculate and increase score
                const float fValue = float(pBeverage->GetScore()) * COMBO_MULTI;
                m_dScore += (double)fValue;

                // increase statistics
                ++m_aiCollected[0];
                ++m_aiCollected[pBeverage->GetSigID()];

                // increase combo
                ++m_iCombo;
                m_fComboDelay = 1.0f;

                const float fTextAlpha = 1.0f - m_fTime * 0.02f * 0.666667f;
                if(pBeverage->GetSigID() == 4 || fTextAlpha > 0.2f)
                {
                    // get and modify beverage color
                    coreVector4 vColor = pBeverage->GetSigColor();
                    if(pBeverage->GetSigID() != 4) vColor.a *= CLAMP(fTextAlpha + 0.2f, 0.0f, 1.0f);

                    // create floating score text
                    g_pCombatText->AddTextTransformed(coreData::Print("%.0f", fValue), m_Rock.GetPosition(), vColor);
                }
                
                // create max combo text (after score text)
                if(m_iCombo == 18) g_pCombatText->AddTextTransformed("+MAX COMBO", m_Rock.GetPosition(), COLOR_ORANGE_F);

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

    // update all inactive beverages, plates and rays
    PROCESS_OBJECT_ARRAY(m_apDestroyed, Core::System->GetTime() * 3.0f * BACK_DETAIL_Y)
    PROCESS_OBJECT_ARRAY(m_apTrap,      fMove10)
    PROCESS_OBJECT_ARRAY(m_apPlate,     fMove10)
    PROCESS_OBJECT_ARRAY(m_apRay,       fMove10)

    FOR_EACH(it, m_apTrap)
    {
        cTrap* pTrap = (*it);

        // check for intersection with traps
        if(pTrap->GetPosition().y < 10.0f && Intersection(*pTrap, m_Rock, NULL, 0.5f))
        {
            if(m_Rock.Jump(10.0f))
            {
                // play trap sound effect and show message
                m_pTrapSound->PlayPosition(NULL, 0.4f, 1.1f, 0.05f, false, m_Rock.GetPosition());
                g_pCombatText->AddTextTransformed(g_MsgTrap.Get(), m_Rock.GetPosition(), COLOR_WHITE_F);

                // reset combo timer (a little bit more than a beverage)
                m_fComboDelay = 1.1f;
            }
        }
    }

    // update combo
    m_fComboDelay -= Core::System->GetTime();
    if(m_fComboDelay <= 0.0f && m_iCombo)
    {
        // reset combo
        g_pCombatText->AddTextTransformed("-COMBO", m_Rock.GetPosition(), COLOR_RED_F);
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
    m_Interface.Update((float)m_dScore, m_fTime, COMBO_MULTI);
    m_Interface.Move();

    if(m_ShowMessage.GetStatus())
    {
        m_ShowMessage.Update(1.0f);

        // update message at the beginning
        m_Message.SetAlpha(MIN(2.0f * coreMath::Sin(m_ShowMessage.GetCurrent(false) * PI), 1.0f));
        m_Message.SetDirection(coreVector2::Direction(0.2f * coreMath::Sin(m_ShowMessage.GetCurrent(false) * PI * 4.0f)));
        m_Message.SetScale(g_pBackground->GetFlash(0.4f));
        m_Message.Move();
    }

    // check for achievements/trophies/whatever you call that unnecessary game enlargement bullshit everybody likes, maybe I should implement a share-button with selfie-attachement-function as well
    if(!m_bTrophyHelper[0] && m_fComboTime >= 20.0f)                  {m_bTrophyHelper[0] = true; this->AchieveTrophy(4665);}    
    if(!m_bTrophyHelper[1] && !m_aiCollected[0] && m_fTime >= 70.0f)  {m_bTrophyHelper[1] = true; this->AchieveTrophy(4636);}
    if(!m_bTrophyHelper[2] && m_fTime >= 100.0f)                      {m_bTrophyHelper[2] = true; this->AchieveTrophy(4638);}
    if(!m_bTrophyHelper[3] && m_dScore >= 30000.0 && !m_bChallenge)   {m_bTrophyHelper[3] = true; this->AchieveTrophy(4667);}
    if(!m_bTrophyHelper[4] && m_dScore >= 200000.0 && m_bChallenge)   {m_bTrophyHelper[4] = true; this->AchieveTrophy(4671);}
    if(!m_bTrophyHelper[5] && m_Rock.GetFallen() && m_fTime < 10.0f)  {m_bTrophyHelper[5] = true; this->AchieveTrophy(4635); if(++g_iNumFails == 5) coreData::OpenURL(Core::Rand->Int(0,1) ? "https://images.search.yahoo.com/search/images?p=facepalm" : "https://www.google.com/search?q=facepalm&tbm=isch");}
    if(!m_bTrophyHelper[6] && m_aiCollected[4] == 2 && !m_bChallenge) {m_bTrophyHelper[6] = true; this->AchieveTrophy(4637);}
}


// ****************************************************************
void cGame::AddTrap(const int& iBlock, const float& fSpawn)
{
    // create new trap and add to list
    cTrap* pTrap = new cTrap();
    pTrap->SetPosition(coreVector3(BACK_SPAWN_X(iBlock, 1.5f), fSpawn, GAME_HEIGHT + pTrap->GetSize().z*0.51f));
    m_apTrap.push_back(pTrap);
}


// ****************************************************************
void cGame::AchieveTrophy(const int& iID)
{
    // achieve trophy and show message
    if(g_pGJ->InterTrophy()->GetTrophy(iID)->AchieveCall() == GJ_OK)
        g_pCombatText->AddTextTransformed(g_pGJ->InterTrophy()->GetTrophy(iID)->GetTitle().c_str(), m_Rock.GetPosition(), COLOR_WHITE_F);
}