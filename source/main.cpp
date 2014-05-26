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

float               g_fTargetSpeed    = 1.0f;
float               g_fCurSpeed       = 1.0f;
float               g_fTargetCam      = 0.0f;
float               g_fCurCam         = 0.0f;
bool                g_bPause          = false;

float               g_fCamSpeed       = 1.0f;
bool                g_bUpsideDown     = false;

int                 g_iNumGames       = g_bCoreDebug ? 3 : 0;
int                 g_iNumFails       = 0;

coreObject3D* m_apSave[9];   // pre-allocation of required ressources, still need to implement a more efficient way


// ****************************************************************
const std::string g_asIntro[] = {"SHOW US WHAT YOU'VE GOT",
                                 "GIVE THE KID SOME ROOM",
                                 "KEEP ON JUMPIN'",
                                 "FOLLOW THE BEAT",
                                 "SOUND FLY THROUGH THE NIGHT",
                                 "GET READY TO DISCO",
                                 "YOU SHOULD BE DANCING"};
sMsgList g_MsgIntro(g_asIntro, ARRAY_SIZE(g_asIntro));


// ****************************************************************
const std::string g_asFallen[] = {"SEE ME FALLING",
                                  "NOOOOO!",
                                  "COME ON",
                                  "AVENGE ME",
                                  "WHAT WAS THAT?",
                                  "OPEN YOUR EYES",
                                  "REALLY?"};
sMsgList g_MsgFallen(g_asFallen, ARRAY_SIZE(g_asFallen));


// ****************************************************************
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

    // set audio listener (for 3d sound)
    const coreVector3 vCamPos = coreVector3(0.0f,-20.0f,-20.0f);
    const coreVector3 vCamDir = coreVector3(0.0f,70.0f,-51.0f).Normalize();
    const coreVector3 vCamOri = coreVector3(0.0f,0.0f,1.0f);
    Core::Audio->SetListener(vCamPos, coreVector3(0.0f,0.0f,0.0f), vCamDir, vCamOri);

    // create main components
    g_pBackground = new cBackground();
    g_pMenu       = new cMenu();
    g_pCombatText = new cCombatText();

    // create Game Jolt API access
    g_pGJ = new gjAPI(18996, GJ_KEY);
    g_pMenu->QuickPlay();

    const int aiTrophySort[] = {4666, 4635, 8325, 8326, 8327, 4665, 4637, 8328, 8329, 4636, 4667, 8330, 4638, 8331, 4671};
    g_pGJ->InterTrophy()->SetSort(aiTrophySort, ARRAY_SIZE(aiTrophySort));

    const int aiTrophySecret[] = {4666, 4635, 8325, 8327};
    g_pGJ->InterTrophy()->SetSecret(aiTrophySecret, ARRAY_SIZE(aiTrophySecret));

    // create music player and load music files (loading-order is important, hardcoded music speed in background class)
    g_pMusicPlayer = new coreMusicPlayer();
    g_pMusicPlayer->AddFile("data/music/Aurea Carmina.ogg");
    g_pMusicPlayer->AddFile("data/music/Ether Disco.ogg");
    g_pMusicPlayer->AddFile("data/music/Stringed Disco.ogg");

    g_pMusicPlayer->Shuffle();
    g_pMusicPlayer->SetRepeat(CORE_MUSIC_ALL_REPEAT);

    Core::Config->SetFloat(CORE_CONFIG_AUDIO_VOLUME_MUSIC, Core::Config->GetFloat(CORE_CONFIG_AUDIO_VOLUME_SOUND) * 0.07f);

    // create particle system
    g_pParticleSystem = new coreParticleSystem(128);
    g_pParticleSystem->DefineTextureFile(0, "data/textures/effect_particle.png");
    g_pParticleSystem->DefineProgramShare("particle_shader")
                       ->AttachShaderFile("data/shaders/particle.vs")
                       ->AttachShaderFile("data/shaders/particle.fs")
                       ->Finish();

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
}


// ****************************************************************
void CoreApp::Exit()
{
    // delete Game Jolt API access
    SAFE_DELETE(g_pGJ)

    // delete all allocation objects
    for(coreUint i = 0; i < ARRAY_SIZE(m_apSave); ++i)
        SAFE_DELETE(m_apSave[i])

    // delete all main components
    SAFE_DELETE(g_pBackground)
    SAFE_DELETE(g_pMenu)
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pCombatText)

    // delete all sub components
    SAFE_DELETE(g_pMusicPlayer)
    SAFE_DELETE(g_pParticleSystem)
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

    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
void CoreApp::Move()
{
    // move menu
    g_pMenu->Move();
    if(g_pMenu->GetStatus() == 1)
    {
#if defined(_CORE_ANDROID_)

        // check finger positions for Coola challenge
        bool bChallenge = false;
        Core::Input->ForEachFinger(CORE_INPUT_HOLD, [&](const coreUint& i)
        {
            bChallenge |= (ABS(Core::Input->GetTouchPosition(i).x) > 0.45f*Core::System->GetResolution().AspectRatio()) &&
                          (ABS(Core::Input->GetTouchPosition(i).y) > 0.45f);
        });

#else
        // check C key for Coola challenge
        const bool bChallenge = Core::Input->GetKeyboardButton(SDL_SCANCODE_C, CORE_INPUT_HOLD);

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
        g_bUpsideDown  = false;

        // reset all holes in the dance floor
        bool abIndex[BACK_BLOCKS_X];
        for(int i = 0; i < BACK_BLOCKS_X; ++i) abIndex[i] = false;
        for(int i = 0; i < 62;            ++i) g_pBackground->UpdateHoles(i, abIndex);
    }

    // smoothly update the real game speed with an additional target value
    if(!g_bPause) g_fCurSpeed += CLAMP(g_fTargetSpeed - g_fCurSpeed, -1.0f, 1.0f) * Core::System->GetTime() * 5.0f;
    Core::System->SetTimeSpeed(0, g_bPause ? 0.0f : g_fCurSpeed);

    // smoothly move the camera
    if(!g_bPause) g_fCurCam += (SIN(g_fTargetCam * PI) * 4.0f - g_fCurCam) * Core::System->GetTime() * 0.25f * g_fCamSpeed;

    constexpr_var coreVector3 vCamPos = coreVector3(0.0f,-70.0f,51.0f);
    constexpr_var coreVector3 vCamDir = -vCamPos;
    const         coreVector3 vCamOri = coreVector3(g_fCurCam * 0.07f, 0.0f, g_bUpsideDown ? -1.0f : 1.0f);
    Core::Graphics->SetCamera(vCamPos, vCamDir, vCamOri);

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
    g_pGJ->SetSessionActive(g_pGame ? true : false);
    g_pGJ->Update();

    // start music delayed
    if(Core::System->GetTotalTime() > 0.5 && !g_pMusicPlayer->Control()->IsPlaying())
        g_pMusicPlayer->Control()->Play();

    // adjust music speed/pitch and update the streaming
    g_pMusicPlayer->Control()->SetPitch(1.0f + MAX((g_fCurSpeed - 1.5f) * 0.16667f, 0.0f));
    g_pMusicPlayer->Update();

#if !defined(_CORE_ANDROID_)

    // wireframe mode for fun
    if(Core::Input->GetKeyboardButton(SDL_SCANCODE_LSHIFT, CORE_INPUT_HOLD))
    {
             if(Core::Input->GetKeyboardButton(SDL_SCANCODE_1, CORE_INPUT_PRESS)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if(Core::Input->GetKeyboardButton(SDL_SCANCODE_2, CORE_INPUT_PRESS)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

#endif
}