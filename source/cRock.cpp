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
cRock::cRock()noexcept
: m_WaveTimer          (coreTimer(1.0f, 4.0f, 1))
, m_fWaveStrength      (60.0f)
, m_WaveSmallTimer     (coreTimer(1.0f, 6.0f, 1))
, m_fWaveSmallStrength (0.0f)
, m_fForce             (0.0f)
, m_fHeight            (100.0f)
, m_bFallen            (false)
, m_bJumped            (true)
, m_bReflected         (false)
, m_iNumJumps          (0)
, m_fRotation          (0.0f)
, m_Effect             (g_pParticleSystem)
, m_fShake             (0.0f)
{
    // load object resources
    this->DefineModel("rock.md5mesh");
    this->DefineTexture(0, "rock.png");
    this->DefineProgram("rock_program");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*5.0f);
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // create shadow
    m_Shadow.DefineModel("default_square.md5mesh");
    m_Shadow.DefineTexture(0, "effect_shadow.png");
    m_Shadow.DefineProgram("shadow_program");
    m_Shadow.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // create big wave
    m_Wave.DefineModel("default_square.md5mesh");
    m_Wave.DefineTexture(0, "effect_wave.png");
    m_Wave.DefineProgram("wave_program");
    m_Wave.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // create small wave
    m_WaveSmall.DefineModel("default_square.md5mesh");
    m_WaveSmall.DefineTexture(0, "effect_wave.png");
    m_WaveSmall.DefineProgram("wave_program");
    m_WaveSmall.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // load sound-effects
    m_pUp   = Core::Manager::Resource->Get<coreSound>("dust.wav");
    m_pDown = Core::Manager::Resource->Get<coreSound>("bump.wav");

    // schedule model customization
    Core::Manager::Resource->AttachFunction([&]()
    {
        if(m_Shadow.GetModel().IsUsable())
        {
            // enable array drawing
            m_Shadow.GetModel()->GetIndexBuffer()->Delete();
            m_Shadow.GetModel()->SetPrimitiveType(GL_TRIANGLE_STRIP);
            return CORE_OK;
        }
        return CORE_BUSY;
    });
}

// ****************************************************************
cRock::~cRock()
{
}


// ****************************************************************
void cRock::Render()
{
    glDisable(GL_DEPTH_TEST);
    {
        // render shadow
        m_Shadow.Render();

        // render waves
        if(m_WaveTimer.GetStatus())      m_Wave.Render();
        if(m_WaveSmallTimer.GetStatus()) m_WaveSmall.Render();
    }
    glEnable(GL_DEPTH_TEST);

    // render the object
    coreObject3D::Render();
}


// ****************************************************************
void cRock::Move()
{
    // rotate the rock
    m_fRotation.Update(10.0f, 0);
    this->SetDirection(coreVector3(0.0f, coreVector2::Direction(-m_fRotation)));

    // get minimum Z position above the ground
    const float fGround = GAME_HEIGHT + m_pModel->GetRadius() * this->GetSize().x;

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
                if(Core::Input->GetTouchPosition(i).x > 0.1667f)
                    bJump = true;
            });
        }
    }

    if(bJump)

#else

    // jump with keyboard (W, UP, SPACE) and joystick
    if(Core::Input->GetKeyboardButton(KEY(W),     CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(KEY(UP),    CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(KEY(SPACE), CORE_INPUT_PRESS) ||
       Core::Input->GetJoystickButton(0, 0,       CORE_INPUT_PRESS) ||
       Core::Input->GetJoystickButton(1, 0,       CORE_INPUT_PRESS))

#endif
    {
        // just jump
        this->Jump(6.0f);
    }

#if !defined(_CORE_DEBUG_) || 0

    if(g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2(-1.2f, 0.0f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 1.2f, 0.0f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 0.0f,-1.2f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 0.0f, 1.2f)) > 0.0f && 
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
        m_fForce -= Core::System->GetTime()*20.0f;   // fall down
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

    const float fMove = 100.0f * Core::System->GetTime();

    const float fPosDiff = (fNewPos + fMove * Core::Input->GetJoystickRelative(0).x) - this->GetPosition().x;

    // move with left touch buttons
    if(g_pGame->GetInterface()->GetControlType() == CONTROL_CLASSIC)
    {
             if(g_pGame->GetInterface()->GetTouchMoveLeft ()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD)) fNewPos -= fMove;
        else if(g_pGame->GetInterface()->GetTouchMoveRight()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD)) fNewPos += fMove;
    }

    // move with device motion
    else if(g_pGame->GetInterface()->GetControlType() == CONTROL_MOTION)
        fNewPos += fMove * Core::Input->GetJoystickRelative(0).x;

    // move with screen space
    else // == CONTROL_FULLSCREEN)
    {
        Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&fNewPos, &fMove](const coreUint& i)
        {
            const float& fX = Core::Input->GetTouchPosition(i).x;

                                   if(fX < -0.1667f) fNewPos -= fMove;
            else if(-0.1667f <= fX && fX <= 0.1667f) fNewPos += fMove;
        });
    }

#else
    
    // move with keyboard (A, D, LEFT, RIGHT) and joystick
    const float fMove = 100.0f * Core::System->GetTime();

         if(Core::Input->GetKeyboardButton(KEY(A),     CORE_INPUT_HOLD) ||
            Core::Input->GetKeyboardButton(KEY(LEFT),  CORE_INPUT_HOLD) ||
            Core::Input->GetJoystickRelative(0).x < 0.0f                ||
            Core::Input->GetJoystickRelative(1).x < 0.0f) fNewPos -= fMove;

    else if(Core::Input->GetKeyboardButton(KEY(D),     CORE_INPUT_HOLD) ||
            Core::Input->GetKeyboardButton(KEY(RIGHT), CORE_INPUT_HOLD) ||
            Core::Input->GetJoystickRelative(0).x > 0.0f                ||
            Core::Input->GetJoystickRelative(1).x > 0.0f) fNewPos += fMove;

    const float fPosDiff = fNewPos - this->GetPosition().x;

#endif

    // control shaking
         if(m_fShake >= 0.0f) {if(fPosDiff > 0.0f) m_fShake = -m_fShake - ROCK_SHAKE_STRENGTH;}
    else if(m_fShake <  0.0f) {if(fPosDiff < 0.0f) m_fShake = -m_fShake + ROCK_SHAKE_STRENGTH;}
    m_fShake -= SIGN(m_fShake) * Core::System->GetTime() * 0.3f;

    // control the rock height
    m_fHeight += m_fForce*Core::System->GetTime()*20.0f;

    // control the rock position
    this->SetPosition(coreVector3(CLAMP(fNewPos, -60.0f, 60.0f), 0.0f, m_fHeight+fGround));

    if(g_pGame->GetTime() >= 30.0f)
    {
        // define smoke color
        coreVector4 vSmokeColor = coreVector4((g_pGame->GetTime() < 100.0f) ? COLOR_WHITE_F : LERP(COLOR_WHITE_F, coreVector3(g_pBackground->GetColor(Core::Rand->Int(0, COLOR_NUM-1))), MIN(g_pGame->GetTime() - 100.0f, 1.0f)), 1.0f);
        vSmokeColor.a *= CLAMP((g_pGame->GetTime() - 30.0f) * 0.05f, 0.0f, 1.0f);

        // create smoke trail
        m_Effect.CreateParticle(2, 60.0f, [&](coreParticle* pParticle)
        {
            const coreVector2 vRand = coreVector2::Rand(0.0f, 10.0f);

            pParticle->SetPositionRel(this->GetPosition() + coreVector3::Rand(0.0f, 3.8f), coreVector3(vRand.x, -30.0f*g_fCurSpeed, vRand.y));
            pParticle->SetScaleStc(6.2f);
            pParticle->SetAngleStc(0.0f);
            pParticle->SetColor4Stc(vSmokeColor);
            pParticle->SetSpeed(2.0f);
        });
    }

    // update shadow
    m_Shadow.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
    m_Shadow.SetSize(this->GetSize() * 2.344f * MAX(1.0f - 0.018f*(GAME_HEIGHT + ABS(GAME_HEIGHT-m_fHeight)), 0.0f));
    m_Shadow.SetAlpha(m_bFallen ? MIN(1.0f + m_fHeight*0.1f, 1.0f) : 1.0f);
    m_Shadow.Move();

    // calculate wave movement
    const float fWaveMove = -Core::System->GetTime(0) * 1.25f * BACK_DETAIL_Y;

    if(m_WaveTimer.GetStatus())
    {
        // update big wave
        m_WaveTimer.Update(1.0f);
        m_Wave.SetPosition(m_Wave.GetPosition() + coreVector3(0.0f, fWaveMove, 0.0f));
        m_Wave.SetSize(coreVector3(1.0f,1.0f,1.0f) * 0.781f * m_fWaveStrength * m_WaveTimer.GetValue(CORE_TIMER_GET_NORMAL));
        m_Wave.SetAlpha(1.5f * m_WaveTimer.GetValue(CORE_TIMER_GET_REVERSED));
        m_Wave.Move();
    }
    if(m_WaveSmallTimer.GetStatus())
    {
        // update small wave
        m_WaveSmallTimer.Update(1.0f);
        m_WaveSmall.SetPosition(m_WaveSmall.GetPosition() + coreVector3(0.0f, fWaveMove, 0.0f));
        m_WaveSmall.SetSize(coreVector3(1.0f,1.0f,1.0f) * 23.44f * m_WaveSmallTimer.GetValue(CORE_TIMER_GET_NORMAL));
        m_WaveSmall.SetAlpha(m_fWaveSmallStrength * m_WaveSmallTimer.GetValue(CORE_TIMER_GET_REVERSED));
        m_WaveSmall.Move();
    }

    // move the object
    coreObject3D::Move();
}


// ****************************************************************
bool cRock::Jump(const float& fForce)
{
    if(m_bFallen || m_bJumped) return false;

    // make a jump, disable additional jumping
    m_fForce        = fForce;
    m_fWaveStrength = fForce * 10.0f;
    m_bJumped       = true;
    ++m_iNumJumps;

    // play jump sound-effect and start big wave animation
    m_pUp->PlayPosition(NULL, 0.4f, 1.8f, 0.05f, false, this->GetPosition());
    m_WaveTimer.Play(CORE_TIMER_PLAY_RESET);
    m_Wave.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));

    return true;
}