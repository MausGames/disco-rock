/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"
#include "gjKey.h"

cBackground*        g_pBackground     = NULL;
cMenu*              g_pMenu           = NULL;
cGame*              g_pGame           = NULL;
cCombatText*        g_pCombatText     = NULL;

gjAPI*              g_pGJ             = NULL;
coreMusicPlayer*    g_pMusicPlayer    = NULL;
coreParticleSystem* g_pParticleSystem = NULL;

float               g_fTargetSpeed    = 0.0f;
float               g_fCurSpeed       = 0.0f;                 
bool                g_bPause          = false;

int                 g_iNumFails       = 0;

coreObject3D* m_apSave[8];   // pre-allocation of required ressources, still need to implement a more efficient way

// simple FPS display
//#define _FPS_
#if defined(_FPS_)
    coreLabel* m_pFPS = NULL;
#endif


// ****************************************************************
const std::string g_asIntro[] = {"SHOW US WHAT YOU'VE GOT",
                                 "GIVE THE KID SOME ROOM",
                                 "KEEP ON JUMPIN'",
                                 "FOLLOW THE BEAT",
                                 //"HE'S TAKING OVER AGAIN",
                                 //"MOVE ON TILL THE DAWN",
                                 //"LET THE ROCKS HIT THE FLOOR",
                                 //"ONCE MORE UNTO THE BREACH",
                                 "GET READY TO DISCO",
                                 "YOU SHOULD BE DANCING"};
sMsgList g_MsgIntro(g_asIntro, ARRAY_SIZE(g_asIntro));


// ****************************************************************
const std::string g_asFallen[] = {"SEE ME FALLING",
                                  "NOOOOO!",
                                  "COME ON",
                                  "AVENGE ME",
                                  //"FREE FALL",
                                  "WHAT WAS THAT?",
                                  //"DROP IT",
                                  //"MY BAD",
                                  //"STOP IT",
                                  "OPEN YOUR EYES",
                                  "REALLY?"};
sMsgList g_MsgFallen(g_asFallen, ARRAY_SIZE(g_asFallen));


// ****************************************************************
const std::string g_asTrap[] = {"TO THE SKY",
                                //"FREE STYLE",
                                "WEEEEE!",
                                "MAKE SOME NOISE",
                                "YEAH BABY YEAH",
                                //"360 KICKFLIP",
                                "THEY SEE ME ROLLIN'",
                                "SHOW ME THE LIGHT"};
sMsgList g_MsgTrap(g_asTrap, ARRAY_SIZE(g_asTrap));


// ****************************************************************
const std::string g_asBegin[] = {"LET'S GO",
                                 "BREAK IT",
                                 "SO IT BEGINS",
                                 "SHOW THEM",
                                 "TAKE THAT",
                                 "RIDE ON"};
sMsgList g_MsgBegin(g_asBegin, ARRAY_SIZE(g_asBegin));


// ****************************************************************
void CoreApp::Init()
{
    // set window title and icon
    Core::System->SetTitle("Disco Rock");
    Core::System->SetIcon("data/textures/game_icon.png");

    // set curor
#if defined(_CORE_LINUX_)
    // X11 has somehow problems with 24/32-bit colored cursors
    Core::Input->DefineCursor("data/textures/standard_cursor_low.png");
#else
    Core::Input->DefineCursor("data/textures/standard_cursor.png");
#endif

    // set view frustum
    Core::Graphics->ResizeView(Core::System->GetResolution(), TO_RAD(55.0f), 0.1f, 1000.0f);

    // set camera
    coreVector3 vCamPos = coreVector3(0.0f,-70.0f,50.0f);
    coreVector3 vCamDir = -vCamPos;
    coreVector3 vCamOri = coreVector3(0.0f,0.0f,1.0f);
    Core::Graphics->SetCamera(vCamPos, vCamDir, vCamOri);

    // set audio listener (for 3d sound)
    vCamPos = coreVector3(0.0f,-20.0f,-20.0f);
    Core::Audio->SetListener(vCamPos, coreVector3(0.0f,0.0f,0.0f), vCamDir, vCamOri);

    // override audio configuration
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_VOLUME_SOUND, 4.0f);
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_VOLUME_MUSIC, 0.7f);

    // create main components
    g_pBackground = new cBackground();
    g_pMenu       = new cMenu();
    g_pCombatText = new cCombatText();

    // create Game Jolt API access
    g_pGJ = new gjAPI(18996, GJ_KEY);
    g_pMenu->QuickPlay();

    // create music player and load music files (loading-order is important, hardcoded music speed in background class)
    g_pMusicPlayer = new coreMusicPlayer();
    g_pMusicPlayer->AddFile("data/music/Aurea Carmina.ogg");
    g_pMusicPlayer->AddFile("data/music/Ether Disco.ogg");
    g_pMusicPlayer->AddFile("data/music/Stringed Disco.ogg");

    g_pMusicPlayer->Shuffle();
    g_pMusicPlayer->SetRepeat(CORE_MUSIC_ALL_REPEAT);

    // create particle system
    g_pParticleSystem = new coreParticleSystem(128);
    g_pParticleSystem->DefineTextureFile(0, "data/textures/effect_particle.png");
    g_pParticleSystem->DefineProgramShare("particle_shader")
                       ->AttachShaderFile("data/shaders/default_particle.vs")
                       ->AttachShaderFile("data/shaders/particle.fs")
                       ->Finish();

    // pre-allocate all required ressources
    m_apSave[0] = new cSunrise();
    m_apSave[1] = new cMojito();
    m_apSave[2] = new cBlue();
    m_apSave[3] = new cCoola();
    m_apSave[4] = new cRock();
    m_apSave[5] = new cPlate(0.0f, coreVector2());
    m_apSave[6] = new cRay(coreVector3());
    m_apSave[7] = new cTrap();

#if defined(_FPS_)
    m_pFPS = new coreLabel(FONT_ROCKS, 30, 8);
    m_pFPS->SetCenter(coreVector2(-0.5f,0.5f));
    m_pFPS->SetAlignment(coreVector2(1.0f,-1.0f));
#endif
}


// ****************************************************************
void CoreApp::Exit()
{
    // delete all allocation objects
    for(coreUint i = 0; i < ARRAY_SIZE(m_apSave); ++i)
        SAFE_DELETE(m_apSave[i])

    // delete all main components
    SAFE_DELETE(g_pBackground)
    SAFE_DELETE(g_pMenu)
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pCombatText)

    // delete all sub components
    SAFE_DELETE(g_pGJ)
    SAFE_DELETE(g_pMusicPlayer)
    SAFE_DELETE(g_pParticleSystem)

#if defined(_FPS_)
    SAFE_DELETE(m_pFPS)
#endif
}


// ****************************************************************
void CoreApp::Render()
{
    // render background and game
    g_pBackground->Render();
    if(g_pGame) g_pGame->Render();

    glDisable(GL_DEPTH_TEST);

    // render combat text and menu
    g_pCombatText->Render();
    g_pMenu->Render();

#if defined(_FPS_)
    m_pFPS->Render();
#endif

    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
void CoreApp::Move()
{
    // move menu
    g_pMenu->Move();
    if(g_pMenu->GetStatus() == 1)
    {
        // create/start a new game
        SAFE_DELETE(g_pGame)
        g_pGame = new cGame(Core::Input->GetKeyboardButton(SDL_SCANCODE_C, CORE_INPUT_HOLD));
    }
    else if(g_pMenu->GetStatus() == 2)
    {
        // delete/exit the current game
        SAFE_DELETE(g_pGame)

        // reset game speed
        g_fCurSpeed    = 1.0f;
        g_fTargetSpeed = 1.0f;

        // reset all holes in the dance floor
        bool abIndex[BACK_BLOCKS_X];
        for(int i = 0; i < BACK_BLOCKS_X; ++i) abIndex[i] = false;
        for(int i = 0; i < 62;            ++i) g_pBackground->UpdateHoles(i, abIndex);
    }

    // smoothly update the real game speed with an additional target value
    if(!g_bPause) g_fCurSpeed += CLAMP(g_fTargetSpeed - g_fCurSpeed, -1.0f, 1.0f) * Core::System->GetTime() * 5.0f;
    Core::System->SetTimeSpeed(0, g_bPause ? 0.0f : g_fCurSpeed);

    if(!g_bPause)
    {
        // move background
        g_pBackground->Move();

        // move game
        if(g_pGame) g_pGame->Move();
        else g_fTargetSpeed = 1.0f;

        // move particle system (render is in the game object)
        g_pParticleSystem->Move();

        // move combat text
        g_pCombatText->Move();
    }

    // update Game Jolt API access
    g_pGJ->SetSession(g_pGame ? true : false);
    g_pGJ->Update();

    // start music delayed
    if(Core::System->GetTotalTime() > 0.5 && !g_pMusicPlayer->Control()->IsPlaying())
        g_pMusicPlayer->Control()->Play();

    // adjust music speed/pitch and update the streaming
    g_pMusicPlayer->Control()->SetPitch(1.0f + MAX((g_fCurSpeed - 1.5f) * 0.16667f, 0.0f));
    g_pMusicPlayer->Update();

#if defined(_FPS_)
    m_pFPS->SetText(coreData::Print("%.1f", 1.0f / Core::System->GetTime()));
    m_pFPS->Move();
#endif
}