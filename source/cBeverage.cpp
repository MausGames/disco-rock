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
// constructor
cBeverage::cBeverage(const int& iScore, const float& fHeight, const float& fAlpha, const float& fVolume, const float& fPitch)noexcept
: m_pStraw       (NULL)
, m_pGlass       (NULL)
, m_iScore       (iScore)
, m_fHeight      (fHeight)
, m_pDestroy     (coreTimer(30.0f, 20.0f, 1))
, m_vFlyRotation (coreVector3(0.0f,0.0f,0.0f))
, m_vFlyImpact   (coreVector3(0.0f,0.0f,0.0f))
, m_fVolume      (fVolume*0.6f)
, m_fPitch       (fPitch*0.65f)
{
    // load drink shader
    this->DefineProgram("drink_program");

    // set alpha value
    this->SetAlpha(fAlpha);

    // create shadow
    m_Shadow.DefineTexture(0, "effect_shadow.png");
    m_Shadow.DefineModel  ("default_square.md5mesh");
    m_Shadow.DefineProgram("shadow_program");
    m_Shadow.SetDirection (coreVector3(0.0f,0.0f,-1.0f));

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
    float fSideSet = 0.0f;
    if(m_pDestroy.GetStatus())
    {
        // update the fly-animation
        if(m_pDestroy.Update(1.0f))
            m_iStatus = 1;   // mark as finished

        // calculate rotation matrix
        const float       T    = Core::System->GetTime() * PI * 5.0f;
        const coreVector2 A    = coreVector2::Direction(T * m_vFlyRotation.y);
        const coreVector2 B    = coreVector2::Direction(T * m_vFlyRotation.x);
        const coreMatrix4 mRot = coreMatrix4( A.y,  A.x*B.x, -A.x*B.y, 0.0f,
                                             0.0f,      B.y,      B.x, 0.0f,
                                              A.x, -A.y*B.x,  A.y*B.y, 0.0f,
                                             0.0f,     0.0f,     0.0f, 1.0f);

        // rotate the object
        this->SetDirection  (this->GetDirection()   * mRot);
        this->SetOrientation(this->GetOrientation() * mRot);

        // calculate horizontal position
        fSideSet = Core::System->GetTime() * CLAMP(m_vFlyImpact.x, -3.5f, 3.5f) * 30.0f;
    }

    // calculate vertical position above the ground
    const float fGround = m_pModel->GetRadius()*this->GetSize().x + m_pDestroy.GetValue(CORE_TIMER_GET_NORMAL)*10.0f + m_fHeight + GAME_HEIGHT;
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
    coreObject3D::Move();
}


// ****************************************************************
// constructor
cSunrise::cSunrise()noexcept
: cBeverage (5, 0.5f, 0.91f, 0.2f, 1.0f)
{
    // load object resources
    this->DefineModel  ("drink_sunrise.md5mesh");
    this->DefineTexture(0, "drink_sunrise.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.0f);
    this->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModel  ("drink_sunrise_straw.md5mesh");
    m_pStraw->DefineTexture(0, "default_black.png");
    m_pStraw->DefineProgram("drink_program");
}


// ****************************************************************
// destructor
cSunrise::~cSunrise()
{
}


// ****************************************************************
// constructor
cMojito::cMojito()noexcept
: cBeverage (10, -0.2f, 0.85f, 0.4f, 1.2f)
{
    // load object resources
    this->DefineModel  ("drink_mojito.md5mesh");
    this->DefineTexture(0, "drink_mojito.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.0f);
    this->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(coreVector2::Direction(PI*0.125f),0.0f));
    m_fCollisionRange = 1.3f;

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModel  ("drink_mojito_straw.md5mesh");
    m_pStraw->DefineTexture(0, "default_black.png");
    m_pStraw->DefineProgram("drink_program");
}


// ****************************************************************
// destructor
cMojito::~cMojito()
{
}


// ****************************************************************
// constructor
cBlue::cBlue()noexcept
: cBeverage (30, 5.8f, 0.87f, 0.6f, 1.6f)
{
    // load object resources
    this->DefineModel  ("drink_blue.md5mesh");
    this->DefineTexture(0, "drink_blue.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.5f);
    this->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(coreVector2::Direction(PI*0.375f),0.0f));

    // create straw
    m_pStraw = new coreObject3D();
    m_pStraw->DefineModel  ("drink_blue_straw.md5mesh");
    m_pStraw->DefineTexture(0, "drink_citrus.png");
    m_pStraw->DefineProgram("drink_program");

    // create glass
    m_pGlass = new coreObject3D();
    m_pGlass->DefineModel  ("drink_blue_glass.md5mesh");
    m_pGlass->DefineTexture(0, "drink_blue.png");
    m_pGlass->DefineProgram("glass_program");
}


// ****************************************************************
// destructor
cBlue::~cBlue()
{
}


// ****************************************************************
// constructor
cCoola::cCoola()noexcept
: cBeverage (200, -3.0f, 0.8f, 0.5f, 0.9f)
{
    // load object resources
    this->DefineModel  ("drink_cola.md5mesh");
    this->DefineTexture(0, "drink_cola.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*3.5f);
    this->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(coreVector2::Direction(PI*0.0f),0.0f));

    // create glass
    m_pGlass = new coreObject3D();
    m_pGlass->DefineModel  ("drink_cola.md5mesh");
    m_pGlass->DefineTexture(0, "drink_cola_glass.png");
    m_pGlass->DefineProgram("glass_cola_program");
}


// ****************************************************************
// destructor
cCoola::~cCoola()
{
}


// ****************************************************************
// constructor
cFranka::cFranka()noexcept
: cBeverage (0, -2.6f, 1.0f, 1.0f, 1.2f)
{
    // load object resources
    this->DefineModel  ("bear.md5mesh");
    this->DefineTexture(0, "bear.png");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f)*2.25f);
    this->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
    this->SetOrientation(coreVector3(0.0f,-1.0f,0.0f));

    // create glass
    m_pGlass = new coreObject3D();

    // override sound
    m_pClink = Core::Manager::Resource->Get<coreSound>("bump.wav");
}


// ****************************************************************
// destructor
cFranka::~cFranka()
{
}