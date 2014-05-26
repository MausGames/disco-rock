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
cBeverage::cBeverage(const int& iScore, const float& fHeight, const float& fAlpha, const float& fVolume, const float& fPitch)noexcept
: m_pStraw       (NULL)
, m_pGlass       (NULL)
, m_iScore       (iScore)
, m_fHeight      (fHeight)
, m_fLiquidAlpha (fAlpha)
, m_pDestroy     (coreTimer(30.0f, 20.0f, 1))
, m_vFlyRotation (coreVector3(0.0f,0.0f,0.0f))
, m_vFlyImpact   (coreVector3(0.0f,0.0f,0.0f))
, m_fVolume      (fVolume*0.8f)
, m_fPitch       (fPitch*0.65f)
{
    // load drink shader
    this->DefineProgramShare("drink_shader")
          ->AttachShaderFile("data/shaders/drink.vs")
          ->AttachShaderFile("data/shaders/drink.fs")
          ->Finish();

    // create shadow
    m_Shadow.DefineTextureFile(0, "data/textures/effect_shadow.png");
    m_Shadow.DefineModelFile("data/models/standard_square.md5mesh");
    m_Shadow.DefineProgramShare("shadow_shader")
             ->AttachShaderFile("data/shaders/default_3d_simple.vs")
             ->AttachShaderFile("data/shaders/shadow.fs")
             ->Finish();
    m_Shadow.SetDirection(coreVector3(0.0f,0.0f,-1.0f));

    // load glass shader
    m_pGlasProgram = Core::Manager::Memory->Share<coreProgram>("glass_shader");
    m_pGlasProgram->AttachShaderFile("data/shaders/glass.vs")
                  ->AttachShaderFile("data/shaders/glass.fs")
                  ->Finish();

    // load sound-effects
    m_pClink = Core::Manager::Resource->LoadFile<coreSound>("data/sounds/clink.wav");
}


// ****************************************************************
cBeverage::~cBeverage()
{
    SAFE_DELETE(m_pStraw)
    SAFE_DELETE(m_pGlass)
}


// ****************************************************************
void cBeverage::Move()
{
    float fSideSet = 0.0f;
    if(m_pDestroy.GetStatus())
    {
        // update the fly-animation
        if(m_pDestroy.Update(1.0f))
            m_iStatus = 1;   // mark as finished

        // calculate rotation matrix
        const float fTime      = Core::System->GetTime() * PI * 5.0f;
        const coreMatrix4 mRot = coreMatrix4::RotationX(fTime * m_vFlyRotation.x)*
                                 coreMatrix4::RotationY(fTime * m_vFlyRotation.y);

        // rotate the object
        this->SetDirection(this->GetDirection() * mRot);
        this->SetOrientation(this->GetOrientation() * mRot);

        // calculate horizontal position
        fSideSet = Core::System->GetTime() * CLAMP(m_vFlyImpact.x, -3.5f, 3.5f) * 30.0f;
    }

    // calculate vertical position above the ground
    const float fGround = m_pModel->GetRadius()*this->GetSize().x + m_pDestroy.GetCurrent(false)*10.0f + m_fHeight + GAME_HEIGHT;
    this->SetPosition(coreVector3(this->GetPosition().xy(), fGround) + coreVector3(fSideSet,0.0f,0.0f));

    // fade in
    const float fAlpha = MIN((BACK_SPAWN_Y - this->GetPosition().y) * 0.03f, 1.0f);
    this->SetAlpha(fAlpha * m_fLiquidAlpha); // affects glass shading

    // mark as finished
    if(this->GetPosition().y <= BACK_REMOVE_Y) m_iStatus = 1;

    // update shadow
    m_Shadow.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
    m_Shadow.SetSize(this->GetSize() * 3.0f * MAX(1.0f - 0.018f*fGround, 0.0f));
    m_Shadow.Move();

    if(m_pStraw) 
    {
        // update straw
        m_pStraw->SetPosition(this->GetPosition());
        m_pStraw->SetSize(this->GetSize());
        m_pStraw->SetDirection(this->GetDirection());
        m_pStraw->SetOrientation(this->GetOrientation());
        m_pStraw->SetAlpha(fAlpha);
        m_pStraw->Move();
    }

    if(m_pGlass) 
    {
        // update glass
        m_pGlass->SetPosition(this->GetPosition());
        m_pGlass->SetSize(this->GetSize());
        m_pGlass->SetDirection(this->GetDirection());
        m_pGlass->SetOrientation(this->GetOrientation());
        m_pGlass->SetAlpha(fAlpha);
        m_pGlass->Move();
    }

    // move the object
    coreObject3D::Move();
}


// ****************************************************************
cSunrise::cSunrise()noexcept
: cBeverage (5, 0.0f, 0.91f, 0.2f, 1.0f)
{
    // load object resources
    this->DefineModelFile("data/models/drink_sunrise.md5mesh");
    this->DefineTextureFile(0, "data/textures/drink_sunrise.png");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*3.0f);
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModelFile("data/models/drink_sunrise_straw.md5mesh");
    m_pStraw->DefineTextureFile(0, "data/textures/standard_black.png");
    m_pStraw->DefineProgramShare("drink_shader");
}


// ****************************************************************
cSunrise::~cSunrise()
{
}


// ****************************************************************
cMojito::cMojito()noexcept
: cBeverage (10, 0.0f, 0.85f, 0.4f, 1.2f)
{
    // load object resources
    this->DefineModelFile("data/models/drink_mojito.md5mesh");
    this->DefineTextureFile(0, "data/textures/drink_mojito.png");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*3.0f);
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(coreVector2::Direction(PI*0.125f),0.0f));

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModelFile("data/models/drink_mojito_straw.md5mesh");
    m_pStraw->DefineTextureFile(0, "data/textures/standard_black.png");
    m_pStraw->DefineProgramShare("drink_shader");
}


// ****************************************************************
cMojito::~cMojito()
{
}


// ****************************************************************
cBlue::cBlue()noexcept
: cBeverage (30, 5.8f, 0.87f, 0.6f, 1.6f)
{
    // load object resources
    this->DefineModelFile("data/models/drink_blue.md5mesh");
    this->DefineTextureFile(0, "data/textures/drink_blue.png");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*3.5f);
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(coreVector2::Direction(PI*0.375f),0.0f));

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModelFile("data/models/drink_blue_straw.md5mesh");
    m_pStraw->DefineTextureFile(0, "data/textures/drink_citrus.png");
    m_pStraw->DefineProgramShare("drink_shader");

    // create glass
    m_pGlass = new coreObject3D();
    m_pGlass->DefineModelFile("data/models/drink_blue_glass.md5mesh");
    m_pGlass->DefineTextureFile(0, "data/textures/drink_blue.png");
    m_pGlass->DefineProgramShare("glass_shader");
}


// ****************************************************************
cBlue::~cBlue()
{
}


// ****************************************************************
cCoola::cCoola()noexcept
: cBeverage (500, -3.0f, 0.8f, 1.0f, 0.3f)
{
    // load object resources
    this->DefineModelFile("data/models/drink_cola.md5mesh");
    this->DefineTextureFile(0, "data/textures/drink_cola.png");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*3.5f);
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(coreVector2::Direction(PI*0.0f),0.0f));

    // create glass
    m_pGlass = new coreObject3D();
    m_pGlass->DefineModelFile("data/models/drink_cola.md5mesh");
    m_pGlass->DefineTextureFile(0, "data/textures/drink_cola_glass.png");
    m_pGlass->DefineProgramShare("glass_cola_shader")
              ->AttachShaderFile("data/shaders/glass.vs")
              ->AttachShaderFile("data/shaders/glass_cola.fs")
              ->Finish();
}


// ****************************************************************
cCoola::~cCoola()
{
}


// ****************************************************************
cFranka::cFranka()noexcept
: cBeverage (0, -2.6f, 1.0f, 1.0f, 1.2f)
{
    // load object resources
    this->DefineModelFile("data/models/bear.md5mesh");
    this->DefineTextureFile(0, "data/textures/bear.png");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)*2.25f);
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(0.0f,-1.0f,0.0f));

    // create glass
    m_pGlass = new coreObject3D();

    // override sound
    m_pClink = Core::Manager::Resource->LoadFile<coreSound>("data/sounds/bump.wav");
}


// ****************************************************************
cFranka::~cFranka()
{
}