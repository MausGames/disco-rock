//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cBeverage::cBeverage(const coreUint32 iScore, const coreFloat fHeight, const coreFloat fVolume, const coreFloat fPitch)noexcept
: m_pStraw       (NULL)
, m_pGlass       (NULL)
, m_iScore       (iScore)
, m_fHeight      (fHeight)
, m_pDestroy     (coreTimer(30.0f, 20.0f, 1u))
, m_vFlyRotation (coreVector3(0.0f,0.0f,0.0f))
, m_vFlyImpact   (coreVector3(0.0f,0.0f,0.0f))
, m_fVolume      (fVolume*0.6f)
, m_fPitch       (fPitch*0.65f)
{
    // load drink shader
    this->DefineProgram("drink_program");

    // create shadow
    m_Shadow.DefineTexture(0u, "effect_ground.png");
    m_Shadow.DefineModel  (Core::Manager::Object->GetLowQuad());
    m_Shadow.DefineProgram("shadow_program");

    // load glass shader
    m_pGlasProgram = Core::Manager::Resource->Get<coreProgram>("glass_program");

    // load sound-effects
    m_pClink = Core::Manager::Resource->Get<coreSound>("clink.wav");
}


// ****************************************************************
// destructor
cBeverage::~cBeverage()
{
    SAFE_DELETE(m_pStraw)
    SAFE_DELETE(m_pGlass)
}


// ****************************************************************
// move the beverage
void cBeverage::Move()
{
    coreFloat fSideSet = 0.0f;
    if(m_pDestroy.GetStatus())
    {
        // update the fly-animation
        if(m_pDestroy.Update(1.0f))
            m_iStatus = 1;   // mark as finished

        // calculate rotation matrix
        const coreFloat   T    = TIME * PI * 5.0f;
        const coreVector2 A    = coreVector2::Direction(T * m_vFlyRotation.y);
        const coreVector2 B    = coreVector2::Direction(T * m_vFlyRotation.x);
        const coreMatrix3 mRot = coreMatrix3( A.y,  A.x*B.x, -A.x*B.y,
                                             0.0f,      B.y,      B.x,
                                              A.x, -A.y*B.x,  A.y*B.y);

        // rotate the object
        this->SetDirection  (this->GetDirection()   * mRot);
        this->SetOrientation(this->GetOrientation() * mRot);

        // calculate horizontal position
        fSideSet = TIME * CLAMP(m_vFlyImpact.x, -3.5f, 3.5f) * 30.0f;
    }

    // calculate vertical position above the ground
    const coreFloat fGround = m_pModel->GetBoundingRadius()*this->GetSize().x + m_pDestroy.GetValue(CORE_TIMER_GET_NORMAL)*10.0f + m_fHeight + GAME_HEIGHT;
    this->SetPosition(coreVector3(this->GetPosition().xy(), fGround) + coreVector3(fSideSet,0.0f,0.0f));

    // mark as finished
    if(this->GetPosition().y <= BACK_REMOVE_Y) m_iStatus = 1;

    // update shadow
    m_Shadow.SetPosition(coreVector3(this->GetPosition().xy(), GAME_HEIGHT));
    m_Shadow.SetSize(this->GetSize() * 2.344f * MAX(1.0f - 0.018f*fGround, 0.0f));
    m_Shadow.Move();

    if(m_pStraw)
    {
        // update straw
        m_pStraw->SetPosition   (this->GetPosition());
        m_pStraw->SetSize       (this->GetSize());
        m_pStraw->SetDirection  (this->GetDirection());
        m_pStraw->SetOrientation(this->GetOrientation());
        m_pStraw->Move();
    }

    if(m_pGlass)
    {
        // update glass
        m_pGlass->SetPosition   (this->GetPosition());
        m_pGlass->SetSize       (this->GetSize());
        m_pGlass->SetDirection  (this->GetDirection());
        m_pGlass->SetOrientation(this->GetOrientation());
        m_pGlass->Move();
    }

    // move the object
    this->coreObject3D::Move();
}


// ****************************************************************
// constructor
cSunrise::cSunrise()noexcept
: cBeverage (5u, 0.5f, 0.2f, 1.0f)
{
    // load object resources
    this->DefineModel  ("drink_sunrise.md5mesh");
    this->DefineTexture(0, "drink_sunrise.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.0f);
    this->SetDirection  (coreVector3(1.0f,0.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
    this->SetAlpha      (0.91f);

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModel  ("drink_sunrise_straw.md5mesh");
    m_pStraw->DefineTexture(0u, "default_black.png");
    m_pStraw->DefineProgram("drink_program");
}


// ****************************************************************
// constructor
cMojito::cMojito()noexcept
: cBeverage (10u, -0.2f, 0.4f, 1.2f)
{
    // load object resources
    this->DefineModel  ("drink_mojito.md5mesh");
    this->DefineTexture(0u, "drink_mojito.png");

    // set object properties
    this->SetSize             (coreVector3(1.0f,1.0f,1.0f)*3.0f);
    this->SetDirection        (coreVector3(coreVector2::Direction(PI*0.125f),0.0f));
    this->SetOrientation      (coreVector3(0.0f,0.0f,1.0f));
    this->SetCollisionModifier(coreVector3(1.3f,1.3f,1.3f));
    this->SetAlpha            (0.85f);

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModel  ("drink_mojito_straw.md5mesh");
    m_pStraw->DefineTexture(0u, "default_black.png");
    m_pStraw->DefineProgram("drink_program");
}


// ****************************************************************
// constructor
cBlue::cBlue()noexcept
: cBeverage (30u, 5.8f, 0.6f, 1.6f)
{
    // load object resources
    this->DefineModel  ("drink_blue.md5mesh");
    this->DefineTexture(0u, "drink_blue.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.5f);
    this->SetDirection  (coreVector3(coreVector2::Direction(PI*0.375f),0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
    this->SetAlpha      (0.87f);

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModel  ("drink_blue_straw.md5mesh");
    m_pStraw->DefineTexture(0u, "drink_citrus.png");
    m_pStraw->DefineProgram("drink_program");

    // create glass
    m_pGlass = new coreObject3D();
    m_pGlass->DefineModel  ("drink_blue_glass.md5mesh");
    m_pGlass->DefineTexture(0u, "drink_blue.png");
    m_pGlass->DefineProgram("glass_program");
}


// ****************************************************************
// constructor
cCoola::cCoola()noexcept
: cBeverage (200u, -3.0f, 0.5f, 0.9f)
{
    // load object resources
    this->DefineModel  ("drink_cola.md5mesh");
    this->DefineTexture(0u, "drink_cola.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.5f);
    this->SetDirection  (coreVector3(coreVector2::Direction(PI*0.0f),0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
    this->SetAlpha      (0.8f);

    // create glass
    m_pGlass = new coreObject3D();
    m_pGlass->DefineModel  ("drink_cola.md5mesh");
    m_pGlass->DefineTexture(0u, "drink_cola_glass.png");
    m_pGlass->DefineProgram("glass_cola_program");
}


// ****************************************************************
// constructor
cFranka::cFranka()noexcept
: cBeverage (0u, -2.6f, 1.0f, 1.2f)
{
    // load object resources
    this->DefineModel  ("bear.md5mesh");
    this->DefineTexture(0u, "bear.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*2.25f);
    this->SetDirection  (coreVector3(0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
    this->SetAlpha      (1.0f);

    // create glass
    m_pGlass = new coreObject3D();

    // override sound
    m_pClink = Core::Manager::Resource->Get<coreSound>("bump.wav");
}