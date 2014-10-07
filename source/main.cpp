/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

cOnline*            g_pOnline         = NULL;

cBackground*        g_pBackground     = NULL;
cMenu*              g_pMenu           = NULL;
cCombatText*        g_pCombatText     = NULL;
cGame*              g_pGame           = NULL;
cFirst*             g_pFirst          = NULL;

coreMusicPlayer*    g_pMusicPlayer    = NULL;
coreParticleSystem* g_pParticleSystem = NULL;

float               g_fTargetSpeed    = 1.0f;
float               g_fCurSpeed       = 1.0f;
float               g_fTargetCam      = 0.0f;
float               g_fCurCam         = 0.0f;
bool                g_bPause          = false;

float               g_fCamSpeed       = 1.0f;
float               g_fCamTime        = 0.0f;
float               g_fOldCam         = 0.0f;
bool                g_bCamMode        = false;
bool                g_bUpsideDown     = false;

int                 g_iNumGames       = DEFINED(_CORE_DEBUG_) ? 3 : 0;
int                 g_iNumFails       = 0;

coreObject3D* m_apSave[9];   // pre-allocation of required ressources, still need to implement a more efficient way


// ****************************************************************
// intro message container
const std::string g_asIntro[] = {"SHOW US WHAT YOU'VE GOT",
                                 "GIVE THE KID SOME ROOM",
                                 "KEEP ON JUMPIN'",
                                 "FOLLOW THE BEAT",
                                 "SOUND FLY THROUGH THE NIGHT",
                                 "GET READY TO DISCO",
                                 "YOU SHOULD BE DANCING"};
sMsgList g_MsgIntro(g_asIntro, ARRAY_SIZE(g_asIntro));


// ****************************************************************
// end message container
const std::string g_asFallen[] = {"SEE ME FALLING",
                                  "NOOOOO!",
                                  "COME ON",
                                  "AVENGE ME",
                                  "WHAT WAS THAT?",
                                  "OPEN YOUR EYES",
                                  "REALLY?"};
sMsgList g_MsgFallen(g_asFallen, ARRAY_SIZE(g_asFallen));


// ****************************************************************
// trap jump message container
const std::string g_asTrap[] = {"TO THE SKY",
                                "WEEEEE!",
                                "MAKE SOME NOISE",
                                "YEAH BABY YEAH",
                                "SHAKE YOUR GROOVE THING",
                                "THEY SEE ME ROLLIN'",
                                "ROCK 'N ROLL",
                                "SHOW ME THE LIGHT"};
sMsgList g_MsgTrap(g_asTrap, ARRAY_SIZE(g_asTrap));


// ****************************************************************
// first jump message container
const std::string g_asBegin[] = {"LET'S GO",
                                 "BREAK IT",
                                 "SO IT BEGINS",
                                 "SHOW THEM",
                                 "TAKE THAT",
                                 "RIDE ON"};
sMsgList g_MsgBegin(g_asBegin, ARRAY_SIZE(g_asBegin));


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // set audio listener (for 3d sound)
    const coreVector3 vCamPos = coreVector3(0.0f,-20.0f,-20.0f);
    const coreVector3 vCamDir = coreVector3(0.0f,70.0f,-51.0f).Normalize();
    const coreVector3 vCamOri = coreVector3(0.0f,0.0f,1.0f);
    Core::Audio->SetListener(vCamPos, coreVector3(0.0f,0.0f,0.0f), vCamDir, vCamOri);

    // override sound and music volume
    float fSoundVolume = Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME);
    if(fSoundVolume == 1.0f)
    {
        fSoundVolume = 7.0f;
        Core::Config->SetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME, fSoundVolume);
    }
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME, fSoundVolume * 0.07f);

    // override quality settings
    if(Core::Config->GetInt("Graphics", "Quality", 10) == 10)
    {
        Core::Config->SetInt("Graphics", "Quality", 2);

#if defined(_CORE_ANDROID_)

        // also create and show first-time menu
        g_pFirst = new cFirst();

#endif
    }

    // create main components
    g_pBackground = new cBackground();
    g_pMenu       = new cMenu();
    g_pCombatText = new cCombatText();

    // create network access
    g_pOnline = new cOnline();
    if(!g_pFirst) g_pMenu->QuickPlay();

    // create music player and load music files (loading-order is important, hardcoded music speed in background class)
    g_pMusicPlayer = new coreMusicPlayer();
    g_pMusicPlayer->AddMusicFile("data/music/Aurea Carmina.ogg");
    g_pMusicPlayer->AddMusicFile("data/music/Ether Disco.ogg");
    g_pMusicPlayer->AddMusicFile("data/music/Stringed Disco.ogg");

    g_pMusicPlayer->Shuffle();
    g_pMusicPlayer->SetRepeat(CORE_MUSIC_ALL_REPEAT);

    // start music delayed
    Core::Manager::Resource->AttachFunction([]()
    {
        if(Core::System->GetTotalTime() > 0.5 && !g_pMusicPlayer->Control()->IsPlaying())
        {
            g_pMusicPlayer->Control()->Play();
            return CORE_OK;
        }
        return CORE_BUSY;
    });

    // create particle system
    g_pParticleSystem = new coreParticleSystem(128);
    g_pParticleSystem->DefineTexture(0, "effect_particle.png");
    g_pParticleSystem->DefineProgram("particle_program");

#if defined(_CORE_ANDROID_)

    // pre-allocate all required ressources
    m_apSave[0] = new cSunrise();
    m_apSave[1] = new cMojito();
    m_apSave[2] = new cBlue();
    m_apSave[3] = new cCoola();
    m_apSave[4] = new cFranka();
    m_apSave[5] = new cRock();
    m_apSave[6] = new cPlate(0.0f, coreVector2(1.0f,1.0f));
    m_apSave[7] = new cRay(coreVector3(1.0f,1.0f,1.0f));
    m_apSave[8] = new cTrap();

#endif
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete network access
    SAFE_DELETE(g_pOnline)

#if defined(_CORE_ANDROID_)

    // delete all pre-allocated objects
    for(coreUint i = 0; i < ARRAY_SIZE(m_apSave); ++i)
        SAFE_DELETE(m_apSave[i])

#endif

    // delete all main components
    SAFE_DELETE(g_pBackground)
    SAFE_DELETE(g_pMenu)
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pFirst)
    SAFE_DELETE(g_pCombatText)

    // delete all sub components
    SAFE_DELETE(g_pMusicPlayer)
    SAFE_DELETE(g_pParticleSystem)
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    if(g_pFirst)
    {
        // render first-time menu
        g_pFirst->Render();
        return;
    }

    // render background and game
    g_pBackground->Render();
    if(g_pGame) g_pGame->Render();

    glDisable(GL_DEPTH_TEST);
    {
        // render combat text and menu
        g_pCombatText->Render();
        g_pMenu->Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    if(g_pFirst)
    {
        // move first-time menu
        g_pFirst->Move();
        if(g_pFirst->GetStatus())
        {
            SAFE_DELETE(g_pFirst)
            g_pMenu->QuickPlay();
        }
        return;
    }

    // move menu
    g_pMenu->Move();
    if(g_pMenu->GetStatus() == 1)
    {
#if defined(_CORE_ANDROID_)

        // check finger positions for Coola challenge
        bool bChallenge = false;
        Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&](const coreUint& i)
        {
            bChallenge |= (ABS(Core::Input->GetTouchPosition(i).x) > 0.4f) &&
                          (ABS(Core::Input->GetTouchPosition(i).y) > 0.4f);
        });

#else
        // check C key for Coola challenge
        const bool bChallenge = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(C), CORE_INPUT_HOLD);

#endif
        // create/start a new game
        SAFE_DELETE(g_pGame)
        g_pGame = new cGame(bChallenge);
    }
    else if(g_pMenu->GetStatus() == 2)
    {
        // delete/exit the current game
        SAFE_DELETE(g_pGame)

        // reset game speed and camera
        g_fCurSpeed    = 1.0f;
        g_fTargetSpeed = 1.0f;
        g_fCurCam      = 0.0f;
        g_fTargetCam   = 0.0f;
        g_fCamSpeed    = 1.0f;
        g_fCamTime     = 0.0f;
        g_fOldCam      = 0.0f;
        g_bCamMode     = false;
        g_bUpsideDown  = false;

        // reset all holes in the dance floor
        g_pBackground->ModifyColor();
        g_pBackground->LoadGeometry();
    }

    // smoothly update the real game speed with an additional target value
    if(!g_bPause) g_fCurSpeed += CLAMP(g_fTargetSpeed - g_fCurSpeed, -1.0f, 1.0f) * Core::System->GetTime() * 5.0f;
    Core::System->SetTimeSpeed(0, g_bPause ? 0.0f : g_fCurSpeed);

    // smoothly move the camera
    if(!g_bPause)
    {
        const float fSpeed = Core::System->GetTime() * 0.25f * g_fCamSpeed;
        if(g_bCamMode) {g_fCurCam  = 4.0f * LERPS(g_fOldCam, g_fTargetCam, g_fCamTime); g_fCamTime = MIN(g_fCamTime + fSpeed, 1.0f);}
                  else {g_fCurCam += (SIN(g_fTargetCam * PI) * 4.0f - g_fCurCam) * fSpeed;}
    }

    const coreVector3 vCamPos =  coreVector3(0.0f,-70.0f,51.0f);
    const coreVector3 vCamDir = -vCamPos.Normalized();
    const coreVector3 vCamOri =  coreVector3(g_fCurCam * 0.07f, 0.0f, g_bUpsideDown ? -1.0f : 1.0f).Normalize();
    Core::Graphics->SetCamera(vCamPos, vCamDir, vCamOri);

    if(!g_bPause)
    {
        // move background and game
        g_pBackground->Move();
        if(g_pGame) g_pGame->Move();

        // move particle system (render is in the game object)
        g_pParticleSystem->Move();

        // move combat text
        g_pCombatText->Move();
    }

    // update the network object
    g_pOnline->Update();

    // adjust music speed/pitch and update music streaming
    g_pMusicPlayer->Control()->SetPitch(1.0f + MAX((g_fCurSpeed - 1.5f) * 0.16667f, 0.0f));
    if(g_pMusicPlayer->Update())
    {
        // update music volume
        g_pMusicPlayer->Control()->SetVolume(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME) * ((g_bPause || (g_pGame ? g_pGame->GetStatus() : false)) ? 0.5f : 1.0f));
    }

#if !defined(_CORE_ANDROID_)

    // wireframe mode for fun
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LSHIFT), CORE_INPUT_HOLD))
    {
             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

#endif
}