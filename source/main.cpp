//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

STATIC_MEMORY(cBackground,        g_pBackground)
STATIC_MEMORY(cMenu,              g_pMenu)
STATIC_MEMORY(cCombatText,        g_pCombatText)
STATIC_MEMORY(cGame,              g_pGame)
STATIC_MEMORY(cFirst,             g_pFirst)

STATIC_MEMORY(coreMusicPlayer,    g_pMusicPlayer)
STATIC_MEMORY(coreParticleSystem, g_pParticleSystem)

STATIC_MEMORY(cOnline,            g_pOnline)

coreFloat  g_fTargetSpeed   = 1.0f;
coreFloat  g_fCurSpeed      = 1.0f;
coreFloat  g_fMusicSpeed    = 1.0f;
coreBool   g_bPause         = false;

coreUint16 g_iNumGames      = DEFINED(_CORE_DEBUG_) ? 3u : 0u;
coreUint16 g_iNumFails      = 0u;
coreBool   g_bCamUpsideDown = false;

static coreObject3D* s_apSave[8];   // pre-allocation of required resources


// ****************************************************************
// intro message container
const coreChar* const g_asIntro[] =
{
    "SHOW US WHAT YOU'VE GOT",
    "GIVE THE KID SOME ROOM",
    "KEEP ON JUMPIN'",
    "FOLLOW THE BEAT",
    "SOUND FLY THROUGH THE NIGHT",
    "GET READY TO DISCO",
    "YOU SHOULD BE DANCING"
};
sMsgList g_MsgIntro(g_asIntro, ARRAY_SIZE(g_asIntro));


// ****************************************************************
// end message container
const coreChar* const g_asFallen[] =
{
    "SEE ME FALLING",
    "NOOOOO!",
    "COME ON",
    "AVENGE ME",
    "WHAT WAS THAT?",
    "OPEN YOUR EYES",
    "REALLY?"
};
sMsgList g_MsgFallen(g_asFallen, ARRAY_SIZE(g_asFallen));


// ****************************************************************
// trap jump message container
const coreChar* const g_asTrap[] =
{
    "TO THE SKY",
    "WEEEEE!",
    "MAKE SOME NOISE",
    "YEAH BABY YEAH",
    "SHAKE YOUR GROOVE THING",
    "THEY SEE ME ROLLIN'",
    "ROCK 'N ROLL",
    "SHOW ME THE LIGHT",
    "BOY, YOU TURN ME"
};
sMsgList g_MsgTrap(g_asTrap, ARRAY_SIZE(g_asTrap));


// ****************************************************************
// first jump message container
const coreChar* const g_asBegin[] =
{
    "LET'S GO",
    "BREAK IT",
    "SO IT BEGINS",
    "SHOW THEM",
    "TAKE THAT",
    "RIDE ON"
};
sMsgList g_MsgBegin(g_asBegin, ARRAY_SIZE(g_asBegin));


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // set view frustum
    Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(55.0f), 0.1f, 700.0f);

    // override sound and music volume
    coreFloat fSoundVolume = Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME);
    if(coreMath::IsNear(fSoundVolume, 1.0f))
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
        if(DEFINED(_CORE_MOBILE_) || DEFINED(_DR_EMULATE_MOBILE_)) STATIC_NEW(g_pFirst)
    }

    // create main components
    STATIC_NEW(g_pBackground)
    STATIC_NEW(g_pMenu)
    STATIC_NEW(g_pCombatText)

    // create network access
    STATIC_NEW(g_pOnline)
    if(!STATIC_ISVALID(g_pFirst)) g_pMenu->QuickPlay();

    // create music player and load music files (loading-order is important, hardcoded music speed in background class)
    STATIC_NEW(g_pMusicPlayer);
    g_pMusicPlayer->AddMusicFile("data/music/Aurea Carmina.ogg");
    g_pMusicPlayer->AddMusicFile("data/music/Ether Disco.ogg");
    g_pMusicPlayer->AddMusicFile("data/music/Stringed Disco.ogg");
    g_pMusicPlayer->Shuffle();
    g_pMusicPlayer->SetRepeat(CORE_MUSIC_ALL_REPEAT);

    // start music delayed
    Core::Manager::Resource->AttachFunction([]()
    {
        if((Core::System->GetTotalTime() > 0.5) && !g_pMusicPlayer->Control()->IsPlaying() && !STATIC_ISVALID(g_pFirst))
        {
            g_pMusicPlayer->Control()->Play();
            return CORE_OK;
        }
        return CORE_BUSY;
    });

    // create particle system
    STATIC_NEW(g_pParticleSystem, 64u * ROCK_SPRITE_NUM)
    g_pParticleSystem->DefineTexture(0u, "effect_particle.png");
    g_pParticleSystem->DefineProgram("particle_program");

    // pre-allocate all required resources
    s_apSave[0] = new cSunrise();
    s_apSave[1] = new cMojito ();
    s_apSave[2] = new cBlue   ();
    s_apSave[3] = new cCoola  ();
    s_apSave[4] = new cRock   ();
    s_apSave[5] = new cPlate  (0.0f, coreVector2(1.0f,1.0f));
    s_apSave[6] = new cRay    (coreVector3(1.0f,1.0f,1.0f));
    s_apSave[7] = new cTrap   ();
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete all pre-allocated objects
    for(coreUintW i = 0u; i < ARRAY_SIZE(s_apSave); ++i)
        SAFE_DELETE(s_apSave[i])

    // delete network access
    STATIC_DELETE(g_pOnline)

    // delete all main components
    STATIC_DELETE(g_pFirst)
    STATIC_DELETE(g_pCombatText)
    STATIC_DELETE(g_pGame)
    STATIC_DELETE(g_pMenu)
    STATIC_DELETE(g_pBackground)

    // delete all sub components
    STATIC_DELETE(g_pParticleSystem)
    STATIC_DELETE(g_pMusicPlayer)
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    if(STATIC_ISVALID(g_pFirst))
    {
        // render first-time menu
        g_pFirst->Render();
        return;
    }

    Core::Debug->MeasureStart("Game");
    {
        // render background and game
        if(STATIC_ISVALID(g_pGame)) g_pGame->RenderPre();
        g_pBackground->Render();
        if(STATIC_ISVALID(g_pGame)) g_pGame->Render();
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
    if(Core::System->GetWinSizeChanged())
    {
        // resize game after resolution changes
        g_pBackground->ResetResolution();
        g_pMenu      ->ResetResolution();
        Core::Reshape();
    }

    if(STATIC_ISVALID(g_pFirst))
    {
        // move first-time menu
        g_pFirst->Move();
        if(g_pFirst->GetStatus())
        {
            STATIC_DELETE(g_pFirst)
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
            STATIC_DELETE(g_pGame)
            STATIC_NEW(g_pGame, bChallenge)
        }
        else if(g_pMenu->GetStatus() == 2)
        {
            // delete/exit the current game
            STATIC_DELETE(g_pGame)

            // reset game speed and camera
            g_fTargetSpeed   = 1.0f;
            g_fCurSpeed      = 1.0f;
            g_bCamUpsideDown = false;

            // reset all holes in the dance floor
            g_pBackground->ModifyColor();
            g_pBackground->LoadGeometry();
        }

        // smoothly update the real game speed
        if(!g_bPause) g_fCurSpeed += CLAMP(g_fTargetSpeed - g_fCurSpeed, -1.0f, 1.0f) * TIME * 5.0f;
        Core::System->SetTimeSpeed(0u, g_bPause ? 0.0f :     g_fCurSpeed);                                            // general game speed
        Core::System->SetTimeSpeed(1u, g_bPause ? 0.0f : MAX(g_fCurSpeed, GAME_SPEED_FAST) / GAME_SPEED_FAST_REAL);   // rock movement speed

        // set camera properties
        const coreVector3 vCamPos =  coreVector3(0.0f,-70.0f,51.0f);
        const coreVector3 vCamDir = -vCamPos.Normalized();
        const coreVector3 vCamOri =  coreVector3(0.0f, 0.0f, g_bCamUpsideDown ? -1.0f : 1.0f);
        Core::Graphics->SetCamera(vCamPos, vCamDir, vCamOri);

        // set audio listener (for 3d sound)
        const coreVector3 vAudioCamPos = coreVector3(0.0f,-20.0f,-20.0f);
        const coreVector3 vAudioCamDir = coreVector3(0.0f, 70.0f,-51.0f).Normalized();
        Core::Audio->SetListener(vAudioCamPos, coreVector3(0.0f,0.0f,0.0f), vAudioCamDir, vCamOri);

        if(!g_bPause)
        {
            // move background and game
            g_pBackground->Move();
            if(STATIC_ISVALID(g_pGame)) g_pGame->Move();

            // move particle system (render is in the game object)
            g_pParticleSystem->Move();

            // move combat text
            g_pCombatText->Move();
        }

        // update the network object
        g_pOnline->Update();

        // adjust music speed/pitch and update music streaming
        g_fMusicSpeed += CLAMP((1.05f + MAX((g_fCurSpeed - GAME_SPEED_SLOW) * 0.16667f, 0.0f)) - g_fMusicSpeed, -1.0f, 1.0f) * TIME * 0.8f;
        g_pMusicPlayer->Control()->SetPitch(g_fMusicSpeed);
        if(g_pMusicPlayer->Update())
        {
            // update music volume
            g_pMusicPlayer->Control()->SetVolume((g_bPause || (STATIC_ISVALID(g_pGame) ? g_pGame->GetStatus() : false)) ? 0.5f : 1.0f);
        }
    }
    Core::Debug->MeasureEnd("Move");
}