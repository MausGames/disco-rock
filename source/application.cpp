///////////////////////////////////////////////
//*-----------------------------------------*//
//| Auto-Generated by the Core Engine       |//
//| Direct modifications may be overwritten |//
//*-----------------------------------------*//
///////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// project settings
const coreChar* const CoreApp::Settings::Name                       = "Disco Rock";
const coreChar* const CoreApp::Settings::IconPath                   = "data/textures/game_icon.png";
const coreChar* const CoreApp::Settings::CursorPath                 = "data/textures/default_cursor.png";
const coreUint8       CoreApp::Settings::RenderBuffer::DepthSize    = 24u;
const coreUint8       CoreApp::Settings::RenderBuffer::StencilSize  = 0u;
const coreBool        CoreApp::Settings::RenderBuffer::AlphaChannel = false;
const coreBool        CoreApp::Settings::RenderBuffer::DoubleBuffer = true;
const coreBool        CoreApp::Settings::RenderBuffer::StereoRender = false;


// ****************************************************************
// setup the application
void CoreApp::Setup()
{
    Core::Manager::Resource->Load<coreModel>  ("default_cube.md3",            CORE_RESOURCE_UPDATE_AUTO,   "data/models/default_cube.md3", false);
    Core::Manager::Resource->Load<coreModel>  ("default_square.md5mesh",      CORE_RESOURCE_UPDATE_AUTO,   "data/models/default_square.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_blue.md5mesh",          CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_blue.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_blue_straw.md5mesh",    CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_blue_straw.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_blue_glass.md5mesh",    CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_blue_glass.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_cola.md5mesh",          CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_cola.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_mojito.md5mesh",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_mojito.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_mojito_straw.md5mesh",  CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_mojito_straw.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_sunrise.md5mesh",       CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_sunrise.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("drink_sunrise_straw.md5mesh", CORE_RESOURCE_UPDATE_AUTO,   "data/models/drink_sunrise_straw.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("ray.md5mesh",                 CORE_RESOURCE_UPDATE_AUTO,   "data/models/ray.md5mesh", false);
    Core::Manager::Resource->Load<coreModel>  ("rock.md5mesh",                CORE_RESOURCE_UPDATE_AUTO,   "data/models/rock.md5mesh", false);

    Core::Manager::Resource->Load<coreTexture>("background.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/background.png");
    Core::Manager::Resource->Load<coreTexture>("background_norm.png",         CORE_RESOURCE_UPDATE_AUTO,   "data/textures/background_norm.png", false);
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
    Core::Manager::Resource->Load<coreTexture>("effect_ground.png",           CORE_RESOURCE_UPDATE_AUTO,   "data/textures/effect_ground.png", false);
    Core::Manager::Resource->Load<coreTexture>("effect_particle.png",         CORE_RESOURCE_UPDATE_AUTO,   "data/textures/effect_particle.png", false);
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
    Core::Manager::Resource->Load<coreShader> ("simple_2d.vert",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/simple_2d.vert");
    Core::Manager::Resource->Load<coreShader> ("default_2d.frag",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_2d.frag");
    Core::Manager::Resource->Load<coreShader> ("drink.vert",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/drink.vert");
    Core::Manager::Resource->Load<coreShader> ("drink.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/drink.frag");
    Core::Manager::Resource->Load<coreShader> ("fill.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/fill.vert");
    Core::Manager::Resource->Load<coreShader> ("fill.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/fill.frag");
    Core::Manager::Resource->Load<coreShader> ("floor.vert",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("floor.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor.frag");
    Core::Manager::Resource->Load<coreShader> ("floor_plate.vert",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor_plate.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("floor_plate.frag",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/floor_plate.frag");
    Core::Manager::Resource->Load<coreShader> ("glass.vert",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/glass.vert");
    Core::Manager::Resource->Load<coreShader> ("glass.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/glass.frag");
    Core::Manager::Resource->Load<coreShader> ("glass_cola.frag",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/glass_cola.frag");
    Core::Manager::Resource->Load<coreShader> ("default_particle.vert",       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_particle.vert", CORE_SHADER_OPTION_INSTANCING CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("default_particle.frag",       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_particle.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("ray.vert",                    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/ray.vert");
    Core::Manager::Resource->Load<coreShader> ("ray.frag",                    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/ray.frag");
    Core::Manager::Resource->Load<coreShader> ("rock.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/rock.vert");
    Core::Manager::Resource->Load<coreShader> ("rock.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/rock.frag");
    Core::Manager::Resource->Load<coreShader> ("shadow.vert",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/decal.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("shadow.frag",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/shadow.frag");
    Core::Manager::Resource->Load<coreShader> ("trap.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/trap.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("trap.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/trap.frag");
    Core::Manager::Resource->Load<coreShader> ("wave.vert",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/decal.vert");
    Core::Manager::Resource->Load<coreShader> ("wave.frag",                   CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/wave.frag");

    Core::Manager::Resource->Load<coreSound>  ("achieve.wav",                 CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/achieve.wav");
    Core::Manager::Resource->Load<coreSound>  ("bump.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/bump.wav");
    Core::Manager::Resource->Load<coreSound>  ("clink.wav",                   CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/clink.wav");
    Core::Manager::Resource->Load<coreSound>  ("dust.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/dust.wav");
    Core::Manager::Resource->Load<coreSound>  ("flash.wav",                   CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/flash.wav");
    Core::Manager::Resource->Load<coreSound>  ("menu.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/menu.wav");
    Core::Manager::Resource->Load<coreSound>  ("record.wav",                  CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/record.wav");
    Core::Manager::Resource->Load<coreSound>  ("trap.wav",                    CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/trap.wav");
    Core::Manager::Resource->Load<coreSound>  ("woosh.wav",                   CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/woosh.wav");

    Core::Manager::Resource->Load<coreFont>   ("gomarice_rocks.ttf",          CORE_RESOURCE_UPDATE_AUTO,   "data/fonts/gomarice_rocks.ttf");

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("floor_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("floor.vert")
        ->AttachShader("floor.frag")
        ->BindAttribute("a_v2Position", 0u)
        ->BindAttribute("a_v4Color",    2u)
        ->BindAttribute("a_v1Height",   3u)
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("fill_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("fill.vert")
        ->AttachShader("fill.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("drink_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("drink.vert")
        ->AttachShader("drink.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("shadow_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("shadow.vert")
        ->AttachShader("shadow.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("glass_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("glass.vert")
        ->AttachShader("glass.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("glass_cola_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("glass.vert")
        ->AttachShader("glass_cola.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("2d_simple_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("simple_2d.vert")
        ->AttachShader("default_2d.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("2d_color_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("simple_2d.vert")
        ->AttachShader("color.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("2d_border_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("simple_2d.vert")
        ->AttachShader("border.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("2d_color_icon_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("simple_2d.vert")
        ->AttachShader("color_icon.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("2d_color_bar_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("simple_2d.vert")
        ->AttachShader("color_bar.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("floor_plate_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("floor_plate.vert")
        ->AttachShader("floor_plate.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("ray_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("ray.vert")
        ->AttachShader("ray.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("rock_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("rock.vert")
        ->AttachShader("rock.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("wave_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("wave.vert")
        ->AttachShader("wave.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("trap_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("trap.vert")
        ->AttachShader("trap.frag")
        ->Finish();

    d_cast<coreProgram*>(Core::Manager::Resource->Load<coreProgram>("particle_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetRawResource())
        ->AttachShader("default_particle.vert")
        ->AttachShader("default_particle.frag")
        ->Finish();
}