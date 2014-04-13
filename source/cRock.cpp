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
, m_fRotation          (0.0f)
, m_Effect             (g_pParticleSystem)
, m_fSensitivity       (1.0f)
{
    // load object resources
    this->DefineModelFile("data/models/rock.md5mesh");
    this->DefineTextureFile(0, "data/textures/rock.png");
    this->DefineTextureFile(1, "data/textures/rock_norm.png");
    this->DefineProgramShare("rock_shader")
          ->AttachShaderFile("data/shaders/rock.vs")
          ->AttachShaderFile("data/shaders/rock.fs")
          ->Finish();

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*5.0f);
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // create shadow
    m_Shadow.DefineModelFile("data/models/standard_square.md5mesh");
    m_Shadow.DefineTextureFile(0, "data/textures/effect_shadow.png");
    m_Shadow.DefineProgramShare("shadow_shader")
             ->AttachShaderFile("data/shaders/default_3d_simple.vs")
             ->AttachShaderFile("data/shaders/shadow.fs")
             ->Finish();
    m_Shadow.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // create big wave
    m_Wave.DefineModelFile("data/models/standard_square.md5mesh");
    m_Wave.DefineTextureFile(0, "data/textures/effect_wave.png");
    m_Wave.DefineProgramShare("wave_shader")
           ->AttachShaderFile("data/shaders/default_3d_simple.vs")
           ->AttachShaderFile("data/shaders/wave.fs")
           ->Finish();
    m_Wave.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // create small wave
    m_WaveSmall.DefineModelFile("data/models/standard_square.md5mesh");
    m_WaveSmall.DefineTextureFile(0, "data/textures/effect_wave.png");
    m_WaveSmall.DefineProgramShare("wave_shader");
    m_WaveSmall.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // load sound-effects
    m_pUp   = Core::Manager::Resource->LoadFile<coreSound>("data/sounds/dust.wav");
    m_pDown = Core::Manager::Resource->LoadFile<coreSound>("data/sounds/bump.wav");

    // load mouse sensitivity
    m_fSensitivity = Core::Config->GetFloat("Game", "MouseSensitivity", 1.0f);
}

// ****************************************************************
cRock::~cRock()
{
}


// ****************************************************************
void cRock::Render()
{
    glDisable(GL_DEPTH_TEST);

    // render shadow and waves
    m_Shadow.Render();
    if(m_WaveTimer.GetStatus())      m_Wave.Render();
    if(m_WaveSmallTimer.GetStatus()) m_WaveSmall.Render();

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

    // jump with right touch button
    if(g_pGame->GetInterface()->GetTouchJump()->IsClicked())

#else

    // jump with mouse and keyboard (W, UP, SPACE)
    if((Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_PRESS)      ||
        Core::Input->GetKeyboardButton(SDL_SCANCODE_W,     CORE_INPUT_HOLD) ||
        Core::Input->GetKeyboardButton(SDL_SCANCODE_UP,    CORE_INPUT_HOLD) ||
        Core::Input->GetKeyboardButton(SDL_SCANCODE_SPACE, CORE_INPUT_HOLD)))

#endif
    {
        // just jump
        this->Jump(6.0f);
    }

    if(g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2(-1.2f, 0.0f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 1.2f, 0.0f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 0.0f,-1.2f)) > 0.0f &&
       g_pBackground->GetHeight(this->GetPosition().xy() + coreVector2( 0.0f, 1.2f)) > 0.0f && this->GetPosition().z < fGround && m_fForce <= 0.0f && !m_bFallen)
    {
        // see me falling
        m_bFallen = true;
        g_pCombatText->AddTextTransformed(g_MsgFallen.Get(), this->GetPosition(), COLOR_WHITE_F);
    }
    
    if(this->GetPosition().z >= fGround || m_bFallen)
        m_fForce -= Core::System->GetTime()*20.0f;   // fall down
    else if(m_fForce < 0.0f)
    {
        // play sound-effect for hitting the ground
        if(m_fForce < -1.0f) m_pDown->PlayPosition(NULL, ABS(m_fForce)*0.03f, 0.75f - 0.2f * MAX(3.0f - ABS(m_fForce), 0.0f), 0.05f, false, this->GetPosition());

        if(m_fForce < -10.0f)
        {
            // start big wave animation on heavy fall (at the beginning)
            m_WaveTimer.Play(true);
            m_Wave.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
        }
        else if(m_fForce < -1.0f)
        {
            // start small wave animation
            m_WaveSmallTimer.Play(true);
            m_WaveSmall.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
            m_fWaveSmallStrength = CLAMP(-m_fForce*0.2f, 0.0f, 1.0f);
        }
        
        // reverse and reduce force, re-enable jumping
        m_fForce *= -0.5f;
        m_bJumped = false;
    }

#if defined(_CORE_ANDROID_)

    float fNewPos = this->GetPosition().x;

    // move with left touch buttons
    const float fMove = 100.0f * Core::System->GetTime() * m_fSensitivity;
         if(g_pGame->GetInterface()->GetTouchMoveLeft()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD))  fNewPos -= fMove;
    else if(g_pGame->GetInterface()->GetTouchMoveRight()->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_HOLD)) fNewPos += fMove;

    //if(true)
    //{
    //    float fMoveDir = 0.0f;
    //    if(Core::Input->GetTouchButton(0, CORE_INPUT_HOLD) && Core::Input->GetTouchPosition(0).x < 0.0f) fMoveDir += Core::Input->GetTouchPosition(0).x;
    //    if(Core::Input->GetTouchButton(1, CORE_INPUT_HOLD) && Core::Input->GetTouchPosition(1).x < 0.0f) fMoveDir += Core::Input->GetTouchPosition(1).x;
    //    if(Core::Input->GetTouchButton(2, CORE_INPUT_HOLD) && Core::Input->GetTouchPosition(2).x < 0.0f) fMoveDir += Core::Input->GetTouchPosition(2).x;

    //    // move with finger input
    //    if(fMoveDir < 0.0f)
    //    {
    //        const float fMoveStr = CLAMP((fMoveDir + 0.46f) / 0.10f - 0.5f, -0.5f, 0.5f);
    //        if(ABS(fMoveStr) >= 0.035f) fNewPos += Core::System->GetTime() * 170.0f * (fMoveStr - SIGN(fMoveStr)*0.035f);
    //    }
    //}
    //else
    //{
    //    float fMoveDir = 0.0f;
    //    if(Core::Input->GetTouchButton(0, CORE_INPUT_HOLD) && Core::Input->GetTouchPosition(0).x < 0.0f) fMoveDir += Core::Input->GetTouchPosition(0).x;
    //    if(Core::Input->GetTouchButton(1, CORE_INPUT_HOLD) && Core::Input->GetTouchPosition(1).x < 0.0f) fMoveDir += Core::Input->GetTouchPosition(1).x;
    //    if(Core::Input->GetTouchButton(2, CORE_INPUT_HOLD) && Core::Input->GetTouchPosition(2).x < 0.0f) fMoveDir += Core::Input->GetTouchPosition(2).x;

    //    // move with finger input
    //    if(fMoveDir != 0.0f)
    //    {
    //        const float fMoveStr = CLAMP((fMoveDir + 0.39f) * 9.091f * 0.5f + 0.02f, -0.5f, 0.5f);
    //        fNewPos = 120.0f * fMoveStr;
    //    }
    //}

#else

    // move with mouse
    float fNewPos = this->GetPosition().x + 120.0f * Core::Input->GetMouseRelative().x * m_fSensitivity;

    // move with keyboard (A, D, LEFT, RIGHT)
    const float fMove = 100.0f * Core::System->GetTime() * m_fSensitivity;
         if(Core::Input->GetKeyboardButton(SDL_SCANCODE_A,     CORE_INPUT_HOLD) ||
            Core::Input->GetKeyboardButton(SDL_SCANCODE_LEFT,  CORE_INPUT_HOLD)) fNewPos -= fMove;
    else if(Core::Input->GetKeyboardButton(SDL_SCANCODE_D,     CORE_INPUT_HOLD) ||
            Core::Input->GetKeyboardButton(SDL_SCANCODE_RIGHT, CORE_INPUT_HOLD)) fNewPos += fMove;

#endif

    // control the rock height
    m_fHeight += m_fForce*Core::System->GetTime()*20.0f;

    // control the rock position
    this->SetPosition(coreVector3(CLAMP(fNewPos, -60.0f, 60.0f), 0.0f, m_fHeight+fGround));

    if(g_pGame->GetTime() >= 30.0f)
    {
        // define smoke color
        coreVector4 vSmokeColor = (g_pGame->GetTime() < 100.0f) ? COLOR_WHITE_F : g_avColor[Core::Rand->Int(0, COLOR_NUM-1)];
        vSmokeColor.a *= CLAMP((g_pGame->GetTime() - 30.0f) * 0.05f, 0.0f, 1.0f);

        // create smoke trail
        coreParticle* pParticle;
        while((pParticle = m_Effect.CreateParticle(2, 60.0f)))
        {
            coreVector2 vRand = coreVector2::Rand(0.0f, 10.0f);

            pParticle->SetPositionRel(this->GetPosition() + coreVector3::Rand(0.0f, 4.0f), coreVector3(vRand.x, -30.0f*g_fCurSpeed, vRand.y));
            pParticle->SetScaleStc(6.0f);
            pParticle->SetAngleStc(0.0f);
            pParticle->SetColor4Stc(vSmokeColor);
            pParticle->SetSpeed(2.0f);
        }  
    }

    // update shadow
    m_Shadow.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
    m_Shadow.SetSize(this->GetSize() * 3.0f * MAX(1.0f - 0.018f*(GAME_HEIGHT + ABS(GAME_HEIGHT-m_fHeight)), 0.0f));
    m_Shadow.SetAlpha(m_bFallen ? MIN(1.0f + m_fHeight*0.1f, 1.0f) : 1.0f);
    m_Shadow.Move();

    // calculate wave movement
    const float fWaveMove = -Core::System->GetTime(0) * 1.25f * BACK_DETAIL_Y;

    if(m_WaveTimer.GetStatus())
    {
        // update big wave
        m_WaveTimer.Update(1.0f);
        m_Wave.SetPosition(m_Wave.GetPosition() + coreVector3(0.0f, fWaveMove, 0.0f));
        m_Wave.SetSize(coreVector3(1.0f,1.0f,1.0f) * m_fWaveStrength * m_WaveTimer.GetCurrent(false));
        m_Wave.SetAlpha(1.5f * m_WaveTimer.GetCurrent(true));
        m_Wave.Move();
    }
    if(m_WaveSmallTimer.GetStatus())
    {
        // update small wave
        m_WaveSmallTimer.Update(1.0f);
        m_WaveSmall.SetPosition(m_WaveSmall.GetPosition() + coreVector3(0.0f, fWaveMove, 0.0f));
        m_WaveSmall.SetSize(coreVector3(1.0f,1.0f,1.0f) * 30.0f * m_WaveSmallTimer.GetCurrent(false));
        m_WaveSmall.SetAlpha(m_fWaveSmallStrength * m_WaveSmallTimer.GetCurrent(true));
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

    // play jump sound-effect and start big wave animation
    m_pUp->PlayPosition(NULL, 0.2f, 1.8f, 0.05f, false, this->GetPosition());
    m_WaveTimer.Play(true);
    m_Wave.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));

    return true;
}