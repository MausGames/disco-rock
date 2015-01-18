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
cRock::cRock()noexcept
: m_WaveTimer          (coreTimer(1.0f, 4.0f, 1))
, m_fWaveStrength      (60.0f)
, m_WaveSmallTimer     (coreTimer(1.0f, 6.0f, 1))
, m_fWaveSmallStrength (0.0f)
, m_WaveShockTimer     (coreTimer(1.0f, 4.0f, 1))
, m_fWaveShockSpeed    (1.0f)
, m_fForce             (0.0f)
, m_fHeight            (100.0f)
, m_bColored           (false)
, m_bFallen            (false)
, m_bJumped            (true)
, m_bReflected         (false)
, m_iNumJumps          (0)
, m_iNumAirJumps       (0)
, m_fRotation          (0.0f)
, m_Effect             (g_pParticleSystem)
{
    // load object resources
    this->DefineModel  ("rock.md5mesh");
    this->DefineTexture(0, "rock.png");
    this->DefineProgram("rock_program");

    // set object properties
    this->SetSize             (coreVector3(1.0f,1.0f,1.0f)*5.0f);
    this->SetDirection        (coreVector3(1.0f,0.0f,0.0f));
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f)*ROCK_RANGE_OBJ);

    // create shadow
    m_Shadow.DefineModel  (Core::Manager::Object->GetLowModel());
    m_Shadow.DefineTexture(0, "effect_shadow.png");
    m_Shadow.DefineProgram("shadow_program");

    // create big wave
    m_Wave.DefineModel   (Core::Manager::Object->GetLowModel());
    m_Wave.DefineTexture (0, "effect_wave.png");
    m_Wave.DefineProgram ("wave_program");
    m_Wave.SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    m_Wave.SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // create small wave
    m_WaveSmall.DefineModel   (Core::Manager::Object->GetLowModel());
    m_WaveSmall.DefineTexture (0, "effect_wave.png");
    m_WaveSmall.DefineProgram ("wave_program");
    m_WaveSmall.SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    m_WaveSmall.SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // create shock-wave
    m_WaveShock.DefineModel  (Core::Manager::Object->GetLowModel());
    m_WaveShock.DefineTexture(0, "effect_wave.png");
    m_WaveShock.DefineProgram("wave_program");

    // load sound-effects
    m_pUp    = Core::Manager::Resource->Get<coreSound>("dust.wav");
    m_pDown  = Core::Manager::Resource->Get<coreSound>("bump.wav");
    m_pWoosh = Core::Manager::Resource->Get<coreSound>("woosh.wav");
}

// ****************************************************************
// destructor
cRock::~cRock()
{
}


// ****************************************************************
// move the rock
void cRock::Move()
{
    // rotate the rock
    m_fRotation.Update(10.0f, 0);
    this->SetOrientation(coreVector3(0.0f, coreVector2::Direction(-m_fRotation)));

    // get minimum Z position above the ground
    const float fGround = GAME_HEIGHT + m_pModel->GetBoundingRadius() * this->GetSize().x;

#if defined(_CORE_ANDROID_)

    bool bJump = false;

    // jump with right touch button
    if(g_pGame->GetInterface()->GetControlType() == CONTROL_CLASSIC)
        bJump = g_pGame->GetInterface()->GetTouchJump()->IsClicked();

    // jump when touching the whole screen
    else if(g_pGame->GetInterface()->GetControlType() == CONTROL_MOTION)
    {
        if(!g_pGame->GetInterface()->GetTouchPause()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
            Core::Input->ForEachFinger(CORE_INPUT_PRESS, [&bJump](const coreUint& i) {bJump = true;});
    }

    // jump then touching the right third of the screen
    else // == CONTROL_FULLSCREEN
    {
        if(!g_pGame->GetInterface()->GetTouchPause()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
        {
            Core::Input->ForEachFinger(CORE_INPUT_PRESS, [&bJump](const coreUint& i)
            {
                if(Core::Input->GetTouchPosition(i).x > 0.0f)
                    bJump = true;
            });
        }
    }

    if(bJump)

#else

    // jump with keyboard (W, UP, SPACE), mouse and joystick
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(W),     CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(UP),    CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE), CORE_INPUT_PRESS) ||
       Core::Input->GetMouseButton   (CORE_INPUT_LEFT,       CORE_INPUT_PRESS) ||
       Core::Input->GetJoystickButton(0, 0,                  CORE_INPUT_PRESS) ||
       Core::Input->GetJoystickButton(1, 0,                  CORE_INPUT_PRESS))

#endif
    {
        // just jump
        this->Jump(6.0f);
    }

#if !defined(_CORE_DEBUG_) || 0

    if(g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2(-ROCK_RANGE_BACK, 0.0f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( ROCK_RANGE_BACK, 0.0f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 0.0f,-ROCK_RANGE_BACK)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 0.0f, ROCK_RANGE_BACK)) > 0.0f &&
       this->GetPosition().z < fGround && m_fForce <= 0.0f && !m_bFallen)
    {
        // see me falling
        m_bFallen = true;
        g_pCombatText->AddTextTransformed(g_MsgFallen.Get(), this->GetPosition(), coreVector4(COLOR_WHITE_F, 1.0f));
    }

#endif

    // reset reflected status
    m_bReflected = false;

    if(this->GetPosition().z >= fGround || m_bFallen)
        m_fForce -= Core::System->GetTime(1)*20.0f;   // fall down
    else if(m_fForce < 0.0f)
    {
        // play sound-effect for hitting the ground
        if(m_fForce < -1.0f) m_pDown->PlayPosition(NULL, ABS(m_fForce)*0.04f, 0.75f - 0.2f * MAX(3.0f - ABS(m_fForce), 0.0f), 0.05f, false, this->GetPosition());

        if(m_fForce < -8.0f)
        {
            // start big wave animation on heavy fall (at the beginning)
            m_WaveTimer.Play(CORE_TIMER_PLAY_RESET);
            m_Wave.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
            m_fWaveStrength = 60.0f;
        }
        else if(m_fForce < -1.0f)
        {
            // start small wave animation
            m_WaveSmallTimer.Play(CORE_TIMER_PLAY_RESET);
            m_WaveSmall.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
            m_fWaveSmallStrength = CLAMP(-m_fForce*0.2f, 0.0f, 1.0f);
        }

        // reverse and reduce force, re-enable jumping
        m_fForce    *= -0.5f;
        m_bJumped    = false;
        m_bReflected = true;
    }

    // init new X position
    float fNewPos = this->GetPosition().x;

#if defined(_CORE_ANDROID_)

    const float fMove = 100.0f * Core::System->GetTime(1);

    // move with left touch buttons
    if(g_pGame->GetInterface()->GetControlType() == CONTROL_CLASSIC)
    {
             if(g_pGame->GetInterface()->GetOverlayLeft ()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD)) fNewPos -= fMove;
        else if(g_pGame->GetInterface()->GetOverlayRight()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD)) fNewPos += fMove;
    }

    // move with device motion
    else if(g_pGame->GetInterface()->GetControlType() == CONTROL_MOTION)
        fNewPos += fMove * Core::Input->GetJoystickRelative(0).x * 1.1f;

    // move with screen space
    else // == CONTROL_FULLSCREEN)
    {
        Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&fNewPos, &fMove](const coreUint& i)
        {
            const float& fX = Core::Input->GetTouchPosition(i).x;

                                 if(fX < -0.25f) fNewPos -= fMove;
            else if(-0.25f <= fX && fX <=  0.0f) fNewPos += fMove;
        });
    }

#else

    // move with keyboard (A, D, LEFT, RIGHT) and joystick
    const float fMove = 100.0f * Core::System->GetTime(1);

         if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(A),     CORE_INPUT_HOLD) ||
            Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LEFT),  CORE_INPUT_HOLD) ||
            Core::Input->GetJoystickRelative(0).x < 0.0f                           ||
            Core::Input->GetJoystickRelative(1).x < 0.0f) fNewPos -= fMove;

    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(D),     CORE_INPUT_HOLD) ||
            Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RIGHT), CORE_INPUT_HOLD) ||
            Core::Input->GetJoystickRelative(0).x > 0.0f                           ||
            Core::Input->GetJoystickRelative(1).x > 0.0f) fNewPos += fMove;

#endif

#if defined(_DR_EMULATE_MOBILE_)

    // emulate key presses
    g_pGame->GetInterface()->GetOverlayLeft ()->SetFocus(fNewPos < this->GetPosition().x);
    g_pGame->GetInterface()->GetOverlayRight()->SetFocus(fNewPos > this->GetPosition().x);

#endif

    // control the rock height
    m_fHeight += m_fForce*Core::System->GetTime(1)*20.0f;

    // control the rock position
    this->SetPosition(coreVector3(CLAMP(fNewPos, -60.0f, 60.0f), 0.0f, m_fHeight+fGround));

    if(g_pGame->GetTime() >= GAME_SHOCK_TIME)
    {
        // define smoke color
        const coreVector4 vSmokeColor = (m_bColored && Core::Rand->Int(3)) ? coreVector4(g_avColor[F_TO_UI(g_pGame->GetTime()*3.0f) % COLOR_NUM], 0.9f) : coreVector4(COLOR_WHITE_F, 0.6f);

        // create smoke trail
        m_Effect.CreateParticle(2, 60.0f, [this, &vSmokeColor](coreParticle* pParticle)
        {
            const coreVector2 vRand = coreVector2::Rand(10.0f);

            pParticle->SetPositionRel(this->GetPosition() + coreVector3::Rand(3.8f), coreVector3(vRand.x, -30.0f*g_fCurSpeed, vRand.y));
            pParticle->SetScaleStc   (6.2f);
            pParticle->SetColor4Stc  (vSmokeColor);
            pParticle->SetAngleStc   (0.0f);
            pParticle->SetSpeed      (2.0f);
        });
    }

    // update shadow
    m_Shadow.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
    m_Shadow.SetSize    (this->GetSize() * 2.344f * MAX(1.0f - 0.018f*(GAME_HEIGHT + ABS(GAME_HEIGHT-m_fHeight)), 0.0f));
    m_Shadow.SetAlpha   (m_bFallen ? MIN(1.0f + m_fHeight*0.1f, 1.0f) : 1.0f);
    m_Shadow.Move();

    // calculate wave movement
    const float fWaveMove = -Core::System->GetTime(0) * 1.25f * BACK_DETAIL_Y;

    if(m_WaveTimer.GetStatus())
    {
        // update big wave
        m_WaveTimer.Update(1.0f);
        m_Wave.SetPosition(m_Wave.GetPosition() + coreVector3(0.0f, fWaveMove, 0.0f));
        m_Wave.SetSize    (coreVector3(1.0f,1.0f,1.0f) * 0.781f * m_fWaveStrength * m_WaveTimer.GetValue(CORE_TIMER_GET_NORMAL));
        m_Wave.SetAlpha   (1.5f * m_WaveTimer.GetValue(CORE_TIMER_GET_REVERSED));
        m_Wave.Move();
    }
    if(m_WaveSmallTimer.GetStatus())
    {
        // update small wave
        m_WaveSmallTimer.Update(1.0f);
        m_WaveSmall.SetPosition(m_WaveSmall.GetPosition() + coreVector3(0.0f, fWaveMove, 0.0f));
        m_WaveSmall.SetSize    (coreVector3(1.0f,1.0f,1.0f) * 23.44f * m_WaveSmallTimer.GetValue(CORE_TIMER_GET_NORMAL));
        m_WaveSmall.SetAlpha   (m_fWaveSmallStrength * m_WaveSmallTimer.GetValue(CORE_TIMER_GET_REVERSED));
        m_WaveSmall.Move();
    }
    if(m_WaveShockTimer.GetStatus())
    {
        // update shock-wave
        m_WaveShockTimer.Update(1.0f);
        m_WaveShock.SetPosition(m_WaveShock.GetPosition() + coreVector3(0.0f, fWaveMove * m_fWaveShockSpeed, 0.0f));
        m_WaveShock.SetSize    (coreVector3(1.0f,1.0f,1.0f) * 70.0f * m_WaveShockTimer.GetValue(CORE_TIMER_GET_NORMAL));
        m_WaveShock.SetAlpha   (1.5f * m_WaveShockTimer.GetValue(CORE_TIMER_GET_REVERSED));
        m_WaveShock.Move();
    }

    // move the object
    coreObject3D::Move();
}


// ****************************************************************
// just jump
bool cRock::Jump(const float& fForce)
{
    if(m_bFallen || m_bJumped) return false;

    // make a jump, disable additional jumping
    m_fForce        = fForce;
    m_fWaveStrength = fForce * 10.0f;
    m_bJumped       = true;

    // increase stats
    ++m_iNumJumps;
    if(g_pBackground->GetHeight(this->GetPosition().xy()) > 0.0f) ++m_iNumAirJumps;

    // play jump sound-effect and start big wave animation
    m_pUp->PlayPosition(NULL, 0.4f, 1.8f, 0.05f, false, this->GetPosition());
    m_WaveTimer.Play(CORE_TIMER_PLAY_RESET);
    m_Wave.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));

    return true;
}


// ****************************************************************
// create shock-wave
void cRock::CreateShockWave(const coreByte& iType)
{
    if(iType == 0)
    {
        // start shock-wave animation
        m_WaveShockTimer.Play(CORE_TIMER_PLAY_RESET);
        m_fWaveShockSpeed = 2.0f;

        // adjust shock-wave object (to the back)
        m_WaveShock.SetPosition   (this->GetPosition() - coreVector3(0.0f,4.0f,0.0f));
        m_WaveShock.SetDirection  (coreVector3(0.0f, 0.0f,1.0f));
        m_WaveShock.SetOrientation(coreVector3(0.0f,-1.0f,0.0f));

        // play sound-effect
        m_pWoosh->PlayPosition(NULL, 0.3f, 0.9f, 0.0f, false, this->GetPosition());

        // throw up some dust
        m_Effect.CreateParticle(14, [this](coreParticle* pParticle)
        {
            const coreVector2 vRand = coreVector2::Rand(40.0f);

            pParticle->SetPositionRel(this->GetPosition() + coreVector3::Rand(3.8f), coreVector3(vRand.x, -40.0f*g_fCurSpeed, vRand.y));
            pParticle->SetScaleStc   (6.2f);
            pParticle->SetColor4Stc  (coreVector4(COLOR_WHITE_F, 0.6f));
            pParticle->SetAngleStc   (0.0f);
            pParticle->SetSpeed      (2.0f);
        });
    }
    else if(iType == 1)
    {
        const coreVector3 vToSide = coreVector3(-SIGN(this->GetPosition().x), 0.0f, 0.0f);
        const coreVector3 vToCam  = (Core::Graphics->GetCamPosition() - this->GetPosition()).Normalize();

        // start shock-wave animation
        m_WaveShockTimer.Play(CORE_TIMER_PLAY_RESET);
        m_fWaveShockSpeed = 1.0f;

        // adjust shock-wave object (to the side)
        m_WaveShock.SetPosition   (this->GetPosition());
        m_WaveShock.SetOrientation(LERP(vToSide, vToCam, 0.3f).Normalize());
        m_WaveShock.SetDirection  (coreVector3::Cross(m_WaveShock.GetOrientation(), coreVector3(0.0f,1.0f,0.0f)).Normalize());

        // play sound-effect
        m_pWoosh->PlayPosition(NULL, 0.3f, 0.9f, 0.0f, false, this->GetPosition());

        // define smoke color
        const coreVector4 vSmokeColor = coreVector4(g_avColor[F_TO_UI(g_pGame->GetTime()*3.0f) % COLOR_NUM], 1.0f);

        // throw up some colored dust
        m_Effect.CreateParticle(22, [this, &vSmokeColor](coreParticle* pParticle)
        {
            pParticle->SetPositionRel(this->GetPosition(), coreVector3(Core::Rand->Float(-70.0f, 70.0f), coreVector2::Rand(45.0f)));
            pParticle->SetScaleStc   (6.2f);
            pParticle->SetColor4Stc  (vSmokeColor);
            pParticle->SetAngleStc   (0.0f);
            pParticle->SetSpeed      (2.0f);
        });
    }
    else
    {
        // play sound-effect
        m_pUp->PlayPosition(NULL, 0.45f, 0.9f, 0.0f, false, this->GetPosition());

        // throw up some dust
        m_Effect.CreateParticle(22, [this](coreParticle* pParticle)
        {
            pParticle->SetPositionRel(this->GetPosition(), coreVector3(Core::Rand->Float(-70.0f, 70.0f), coreVector2::Rand(45.0f)));
            pParticle->SetScaleStc   (6.2f);
            pParticle->SetColor4Stc  (coreVector4(COLOR_WHITE_F, 1.0f));
            pParticle->SetAngleStc   (0.0f);
            pParticle->SetSpeed      (2.0f);
        });
    }
}