/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

cBackground*        g_pBackground     = NULL;
cMenu*              g_pMenu           = NULL;
cCombatText*        g_pCombatText     = NULL;
cGame*              g_pGame           = NULL;
cFirst*             g_pFirst          = NULL;

coreMusicPlayer*    g_pMusicPlayer    = NULL;
coreParticleSystem* g_pParticleSystem = NULL;

cOnline*            g_pOnline         = NULL;

coreFloat           g_fTargetSpeed    = 1.0f;
coreFloat           g_fCurSpeed       = 1.0f;
coreFloat           g_fMusicSpeed     = 1.0f;
coreBool            g_bPause          = false;

coreUint16          g_iNumGames       = DEFINED(_CORE_DEBUG_) ? 3u : 0u;
coreUint16          g_iNumFails       = 0u;
coreBool            g_bCamUpsideDown  = false;

static coreObject3D* m_apSave[8];   // pre-allocation of required resources


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
                                "SHOW ME THE LIGHT",
                                "BOY, YOU TURN ME"};
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
    const coreVector3 vCamDir = coreVector3(0.0f, 70.0f,-51.0f).Normalized();
    const coreVector3 vCamOri = coreVector3(0.0f,  0.0f,  1.0f);
    Core::Audio->SetListener(vCamPos, coreVector3(0.0f,0.0f,0.0f), vCamDir, vCamOri);

    // override sound and music volume
    coreFloat fSoundVolume = Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME);
    if(coreMath::InRange(fSoundVolume, 1.0f, CORE_MATH_PRECISION))
    {
        fSoundVolume = 7.0f;
        Core::Config->SetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME, fSoundVolume);
    }
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME, fSoundVolume * 0.07f);

    // override quality settings
    if(Core::Config->GetInt("Graphics", "Quality", 10) == 10)
    {
        Core::Config->SetInt("Graphics", "Quality", 2);

        // also create and show first-time menu
        if(DEFINED(_CORE_ANDROID_)) g_pFirst = new cFirst();
    }

    // override context settings
    Core::Config->SetInt (CORE_CONFIG_GRAPHICS_STENCILSIZE,  0);
    Core::Config->SetBool(CORE_CONFIG_GRAPHICS_ALPHACHANNEL, false);

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
        if((Core::System->GetTotalTime() > 0.5) && !g_pMusicPlayer->Control()->IsPlaying() && !g_pFirst)
        {
            g_pMusicPlayer->Control()->Play();
            return CORE_OK;
        }
        return CORE_BUSY;
    });

    // create particle system
    g_pParticleSystem = new coreParticleSystem(128u);
    g_pParticleSystem->DefineTexture(0u, "effect_particle.png");
    g_pParticleSystem->DefineProgram("particle_program");

    // pre-allocate all required resources
    m_apSave[0] = new cSunrise();
    m_apSave[1] = new cMojito ();
    m_apSave[2] = new cBlue   ();
    m_apSave[3] = new cCoola  ();
    m_apSave[4] = new cRock   ();
    m_apSave[5] = new cPlate  (0.0f, coreVector2(1.0f,1.0f));
    m_apSave[6] = new cRay    (coreVector3(1.0f,1.0f,1.0f));
    m_apSave[7] = new cTrap   ();
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete all pre-allocated objects
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_apSave); ++i)
        SAFE_DELETE(m_apSave[i])

    // delete network access
    SAFE_DELETE(g_pOnline)

    // delete all main components
    SAFE_DELETE(g_pFirst)
    SAFE_DELETE(g_pCombatText)
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pMenu)
    SAFE_DELETE(g_pBackground)

    // delete all sub components
    SAFE_DELETE(g_pParticleSystem)
    SAFE_DELETE(g_pMusicPlayer)
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

    Core::Debug->MeasureStart("Game");
    {
        // render background and game
        if(g_pGame) g_pGame->RenderPre();
        g_pBackground->Render();
        if(g_pGame) g_pGame->Render();
    }
    Core::Debug->MeasureEnd("Game");
    Core::Debug->MeasureStart("Menu");
    {
        glDisable(GL_DEPTH_TEST);
        {
            // render combat text and menu
            g_pCombatText->Render();
            g_pMenu->Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
    Core::Debug->MeasureEnd("Menu");
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

    Core::Debug->MeasureStart("Move");
    {
        // move menu
        g_pMenu->Move();
        if(g_pMenu->GetStatus() == 1)
        {
            coreBool bChallenge = false;

            // check finger positions for Coola challenge
            Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&](const coreUintW i)
            {
                bChallenge |= (ABS(Core::Input->GetTouchPosition(i).x) > 0.4f) &&
                              (ABS(Core::Input->GetTouchPosition(i).y) > 0.4f);
            });

            // check C key for Coola challenge
            bChallenge |= (Core::Input->GetKeyboardButton(CORE_INPUT_KEY(C), CORE_INPUT_HOLD)) ? true : false;

            // create/start a new game
            SAFE_DELETE(g_pGame)
            g_pGame = new cGame(bChallenge);
        }
        else if(g_pMenu->GetStatus() == 2)
        {
            // delete/exit the current game
            SAFE_DELETE(g_pGame)

            // reset game speed and camera
            g_fTargetSpeed   = 1.0f;
            g_fCurSpeed      = 1.0f;
            g_bCamUpsideDown = false;

            // reset all holes in the dance floor
            g_pBackground->ModifyColor();
            g_pBackground->LoadGeometry();
        }

        // smoothly update the real game speed
        if(!g_bPause) g_fCurSpeed += CLAMP(g_fTargetSpeed - g_fCurSpeed, -1.0f, 1.0f) * Core::System->GetTime() * 5.0f;
        Core::System->SetTimeSpeed(0u, g_bPause ? 0.0f :     g_fCurSpeed);                                            // general game speed
        Core::System->SetTimeSpeed(1u, g_bPause ? 0.0f : MAX(g_fCurSpeed, GAME_SPEED_FAST) / GAME_SPEED_FAST_REAL);   // rock movement speed

        // set camera properties
        const coreVector3 vCamPos =  coreVector3(0.0f,-70.0f,51.0f);
        const coreVector3 vCamDir = -vCamPos.Normalized();
        const coreVector3 vCamOri =  coreVector3(0.0f, 0.0f, g_bCamUpsideDown ? -1.0f : 1.0f);
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
        g_fMusicSpeed += CLAMP((1.05f + MAX((g_fCurSpeed - GAME_SPEED_SLOW) * 0.16667f, 0.0f)) - g_fMusicSpeed, -1.0f, 1.0f) * Core::System->GetTime() * 0.8f;
        g_pMusicPlayer->Control()->SetPitch(g_fMusicSpeed);
        if(g_pMusicPlayer->Update())
        {
            // update music volume
            g_pMusicPlayer->Control()->SetVolume(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME) * ((g_bPause || (g_pGame ? g_pGame->GetStatus() : false)) ? 0.5f : 1.0f));
        }
    }
    Core::Debug->MeasureEnd("Move");
}