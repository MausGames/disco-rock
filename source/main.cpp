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

static void LoadResources();

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
    Core::System->SetWindowTitle("Disco Rock");
    Core::System->SetWindowIcon("data/textures/game_icon.png");

    // set cursor
#if defined(_CORE_LINUX_)
    Core::Input->DefineCursor("data/textures/default_cursor_low.png");   // X11 has somehow problems with 24/32-bit colored cursors
#else
    Core::Input->DefineCursor("data/textures/default_cursor.png");
#endif

    // set view frustum
    Core::Graphics->ResizeView(Core::System->GetResolution(), DEG_TO_RAD(55.0f), 0.1f, 1000.0f);

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

    // load all resources
    LoadResources();

    if(Core::Config->GetInt("Graphics", "Quality", 10) == 10)
    {
        // override quality settings
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
        const bool bChallenge = Core::Input->GetKeyboardButton(KEY(C), CORE_INPUT_HOLD);

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

    constexpr_var coreVector3 vCamPos =  coreVector3(0.0f,-70.0f,51.0f);
    const         coreVector3 vCamDir = -vCamPos.Normalized();
    const         coreVector3 vCamOri =  coreVector3(g_fCurCam * 0.07f, 0.0f, g_bUpsideDown ? -1.0f : 1.0f).Normalize();
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

    // update the network object
    g_pOnline->Update();

    // start music delayed
    if(Core::System->GetTotalTime() > 0.5 && !g_pMusicPlayer->Control()->IsPlaying())
        g_pMusicPlayer->Control()->Play();

    // adjust music speed/pitch and update music streaming
    g_pMusicPlayer->Control()->SetPitch(1.0f + MAX((g_fCurSpeed - 1.5f) * 0.16667f, 0.0f));
    if(g_pMusicPlayer->Update())
    {
        // update music volume
        g_pMusicPlayer->Control()->SetVolume(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME) * ((g_bPause || (g_pGame ? g_pGame->GetStatus() : false)) ? 0.5f : 1.0f));
    }

#if !defined(_CORE_ANDROID_)

    // wireframe mode for fun
    if(Core::Input->GetKeyboardButton(KEY(LSHIFT), CORE_INPUT_HOLD))
    {
             if(Core::Input->GetKeyboardButton(KEY(1), CORE_INPUT_PRESS)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if(Core::Input->GetKeyboardButton(KEY(2), CORE_INPUT_PRESS)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

#endif
}


// ****************************************************************
static void LoadResources()
{
    Core::Manager::Resource->Load<coreModel>  ("bear.md5mesh",                CORE_RESOURCE_UPDATE_AUTO,   "data/models/bear.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("default_cube.md5mesh",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/default_cube.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("default_square.md5mesh",      CORE_RESOURCE_UPDATE_AUTO,   "data/models/default_square.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_blue.md5mesh",          CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_blue.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_blue_straw.md5mesh",    CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_blue_straw.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_blue_glass.md5mesh",    CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_blue_glass.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_cola.md5mesh",          CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_cola.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_mojito.md5mesh",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_mojito.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_mojito_straw.md5mesh",  CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_mojito_straw.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_sunrise.md5mesh",       CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_sunrise.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("drink_sunrise_straw.md5mesh", CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_sunrise_straw.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("ray.md5mesh",                 CORE_RESOURCE_UPDATE_AUTO,   "data/models/ray.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("rock.md5mesh",                CORE_RESOURCE_UPDATE_AUTO,   "data/models/rock.md5mesh");
    
    Core::Manager::Resource->Load<coreTexture>("background.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/background.png");
    Core::Manager::Resource->Load<coreTexture>("background_norm.png",         CORE_RESOURCE_UPDATE_AUTO,   "data/textures/background_norm.png");
    Core::Manager::Resource->Load<coreTexture>("bear.png",                    CORE_RESOURCE_UPDATE_AUTO,   "data/textures/bear.png");
    Core::Manager::Resource->Load<coreTexture>("button_full_score.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_full_score.png");
    Core::Manager::Resource->Load<coreTexture>("button_full_trophy.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_full_trophy.png");
    Core::Manager::Resource->Load<coreTexture>("button_ok.png",               CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_ok.png");
    Core::Manager::Resource->Load<coreTexture>("button_cancel.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_cancel.png");
    Core::Manager::Resource->Load<coreTexture>("button_login.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_login.png");
    Core::Manager::Resource->Load<coreTexture>("button_move.png",             CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_move.png");
    Core::Manager::Resource->Load<coreTexture>("button_jump.png",             CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_jump.png");
    Core::Manager::Resource->Load<coreTexture>("button_pause.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_pause.png");
    Core::Manager::Resource->Load<coreTexture>("button_play.png",             CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_play.png");
    Core::Manager::Resource->Load<coreTexture>("button_score.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_score.png");
    Core::Manager::Resource->Load<coreTexture>("button_time.png",             CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_time.png");
    Core::Manager::Resource->Load<coreTexture>("button_trophy.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_trophy.png");
    Core::Manager::Resource->Load<coreTexture>("button_config.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_config.png");
    Core::Manager::Resource->Load<coreTexture>("button_scroll.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_scroll.png");
    Core::Manager::Resource->Load<coreTexture>("button_logout.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/button_logout.png");
    Core::Manager::Resource->Load<coreTexture>("default_black.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/default_black.png");
    Core::Manager::Resource->Load<coreTexture>("drink_mojito.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/drink_mojito.png");
    Core::Manager::Resource->Load<coreTexture>("drink_sunrise.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/drink_sunrise.png");
    Core::Manager::Resource->Load<coreTexture>("drink_citrus.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/drink_citrus.png");
    Core::Manager::Resource->Load<coreTexture>("drink_blue.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/drink_blue.png");
    Core::Manager::Resource->Load<coreTexture>("drink_cola.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/drink_cola.png");
    Core::Manager::Resource->Load<coreTexture>("drink_cola_glass.png",        CORE_RESOURCE_UPDATE_AUTO,   "data/textures/drink_cola_glass.png");
    Core::Manager::Resource->Load<coreTexture>("effect_shadow.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/effect_shadow.png");
    Core::Manager::Resource->Load<coreTexture>("effect_particle.png",         CORE_RESOURCE_UPDATE_AUTO,   "data/textures/effect_particle.png");
    Core::Manager::Resource->Load<coreTexture>("effect_wave.png",             CORE_RESOURCE_UPDATE_AUTO,   "data/textures/effect_wave.png");
    Core::Manager::Resource->Load<coreTexture>("game_logo.png",               CORE_RESOURCE_UPDATE_AUTO,   "data/textures/game_logo.png");
    Core::Manager::Resource->Load<coreTexture>("gamejolt_logo.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/gamejolt_logo.png");
    Core::Manager::Resource->Load<coreTexture>("gamejolt_jolt.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/gamejolt_jolt.png");
    Core::Manager::Resource->Load<coreTexture>("google_controller.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/google_controller.png");
    Core::Manager::Resource->Load<coreTexture>("icon_power.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_power.png");
    Core::Manager::Resource->Load<coreTexture>("icon_speed.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_speed.png");
    Core::Manager::Resource->Load<coreTexture>("icon_load.png",               CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_load.png");
    Core::Manager::Resource->Load<coreTexture>("icon_audio_1.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_audio_1.png");
    Core::Manager::Resource->Load<coreTexture>("icon_audio_2.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_audio_2.png");
    Core::Manager::Resource->Load<coreTexture>("icon_success.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_success.png");
    Core::Manager::Resource->Load<coreTexture>("icon_trophy.png",             CORE_RESOURCE_UPDATE_AUTO,   "data/textures/icon_trophy.png");
    Core::Manager::Resource->Load<coreTexture>("maus_logo.png",               CORE_RESOURCE_UPDATE_AUTO,   "data/textures/maus_logo.png");
    Core::Manager::Resource->Load<coreTexture>("rock.png",                    CORE_RESOURCE_UPDATE_AUTO,   "data/textures/rock.png");
    Core::Manager::Resource->Load<coreTexture>("trophy_1.png",                CORE_RESOURCE_UPDATE_AUTO,   "data/textures/trophy_1.png");
    Core::Manager::Resource->Load<coreTexture>("trophy_2.png",                CORE_RESOURCE_UPDATE_AUTO,   "data/textures/trophy_2.png");

    Core::Manager::Resource->Load<coreShader> ("border.frag",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/border.frag");
    Core::Manager::Resource->Load<coreShader> ("color.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/color.frag");
    Core::Manager::Resource->Load<coreShader> ("color_bar.frag",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/color_bar.frag");
    Core::Manager::Resource->Load<coreShader> ("color_icon.frag",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/color_icon.frag");
    Core::Manager::Resource->Load<coreShader> ("default_2d.frag",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_2d.frag");
    Core::Manager::Resource->Load<coreShader> ("default_2d_simple.vert",      CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_2d.vert", CORE_SHADER_OPTION_NO_TEXTURE_TRANSFORM);
    Core::Manager::Resource->Load<coreShader> ("default_3d_simple.vert",      CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_3d.vert", CORE_SHADER_OPTION_NO_TEXTURE_TRANSFORM);
    Core::Manager::Resource->Load<coreShader> ("drink.vert",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/drink.vert");
    Core::Manager::Resource->Load<coreShader> ("drink.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/drink.frag");
    Core::Manager::Resource->Load<coreShader> ("fill.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/fill.vert");
    Core::Manager::Resource->Load<coreShader> ("fill.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/fill.frag");
    Core::Manager::Resource->Load<coreShader> ("floor.vert",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor.vert");
    Core::Manager::Resource->Load<coreShader> ("floor.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor.frag");
    Core::Manager::Resource->Load<coreShader> ("floor_plate.vert",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor_plate.vert");
    Core::Manager::Resource->Load<coreShader> ("floor_plate.frag",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor_plate.frag");
    Core::Manager::Resource->Load<coreShader> ("glass.vert",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/glass.vert");
    Core::Manager::Resource->Load<coreShader> ("glass.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/glass.frag");
    Core::Manager::Resource->Load<coreShader> ("glass_cola.frag",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/glass_cola.frag");
    Core::Manager::Resource->Load<coreShader> ("default_particle.vert",       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_particle.vert", CORE_SHADER_OPTION_NO_PARTICLE_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("default_particle.frag",       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_particle.frag");
    Core::Manager::Resource->Load<coreShader> ("ray.vert",                    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/ray.vert");
    Core::Manager::Resource->Load<coreShader> ("ray.frag",                    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/ray.frag");
    Core::Manager::Resource->Load<coreShader> ("rock.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/rock.vert");
    Core::Manager::Resource->Load<coreShader> ("rock.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/rock.frag");
    Core::Manager::Resource->Load<coreShader> ("shadow.frag",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/shadow.frag");
    Core::Manager::Resource->Load<coreShader> ("trap.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/trap.vert");
    Core::Manager::Resource->Load<coreShader> ("trap.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/trap.frag");
    Core::Manager::Resource->Load<coreShader> ("wave.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/wave.frag");

    Core::Manager::Resource->Load<coreSound>  ("achieve.wav",                 CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/achieve.wav");
    Core::Manager::Resource->Load<coreSound>  ("bump.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/bump.wav");
    Core::Manager::Resource->Load<coreSound>  ("clink.wav",                   CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/clink.wav");
    Core::Manager::Resource->Load<coreSound>  ("dust.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/dust.wav");
    Core::Manager::Resource->Load<coreSound>  ("trap.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/trap.wav");

    Core::Manager::Resource->Load<coreFont>   ("gomarice_rocks.ttf",          CORE_RESOURCE_UPDATE_AUTO,   "data/fonts/gomarice_rocks.ttf");

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("floor_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("floor.vert")
        ->AttachShader("floor.frag")
        ->BindAttribute("a_v2Position", 0)
        ->BindAttribute("a_v4Color",    2)
        ->BindAttribute("a_fHeight",    3)
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("fill_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("fill.vert")
        ->AttachShader("fill.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("drink_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("drink.vert")
        ->AttachShader("drink.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("shadow_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_3d_simple.vert")
        ->AttachShader("shadow.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("glass_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("glass.vert")
        ->AttachShader("glass.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("glass_cola_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("glass.vert")
        ->AttachShader("glass_cola.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("2d_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d_simple.vert")
        ->AttachShader("default_2d.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("2d_program_color", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d_simple.vert")
        ->AttachShader("color.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("2d_program_border", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d_simple.vert")
        ->AttachShader("border.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("2d_program_color_icon", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d_simple.vert")
        ->AttachShader("color_icon.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("2d_program_color_bar", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d_simple.vert")
        ->AttachShader("color_bar.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("floor_plate_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("floor_plate.vert")
        ->AttachShader("floor_plate.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("ray_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("ray.vert")
        ->AttachShader("ray.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("rock_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("rock.vert")
        ->AttachShader("rock.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("wave_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_3d_simple.vert")
        ->AttachShader("wave.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("trap_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("trap.vert")
        ->AttachShader("trap.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("particle_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_particle.vert")
        ->AttachShader("default_particle.frag")
        ->Finish();
}