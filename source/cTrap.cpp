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
cTrap::cTrap()noexcept
: m_pGlow   (0.0f)
, m_Effect  (g_pParticleSystem)
, m_bStatic (false)
{
    // load object resources
    this->DefineModel  ("default_cube.md5mesh");
    this->DefineProgram("trap_program");

    // set object properties
    this->SetSize(coreVector3(BACK_DETAIL_X*0.9f, BACK_DETAIL_Y*0.9f, 10.0f));
    this->SetCollisionRange(0.44f);

    // attach sparkles to object
    m_Effect.SetOrigin(this);
}


// ****************************************************************
// destructor
cTrap::~cTrap()
{
}


// ****************************************************************
// move the trap
void cTrap::Move()
{
    // update animation
    m_pGlow.Update(2.5f*PI);

    if(m_bStatic)
    {
        // animate object (and fade out)
        const float fFall = FRACT(m_pGlow * 0.29f);
        this->SetSize (coreVector3(BACK_DETAIL_X*0.9f * fFall, BACK_DETAIL_Y*0.9f * fFall, 10.0f));
        this->SetAlpha((1.0f - fFall) * CLAMP(this->GetPosition().y * 0.02f, 0.0f, 1.0f));
    }
    else
    {
        // animate object (only glow effect)
        this->SetAlpha(SIN(m_pGlow) * 0.25f + 0.75f);

        // create light sparkles
        if(this->GetPosition().y < 380.0f)
        {
            m_Effect.CreateParticle(1, 10.0f, [](coreParticle* pParticle)
            {
                pParticle->SetPositionRel(coreVector3(coreVector2::Rand(1.0f, BACK_DETAIL_X*0.37f), -5.0f), coreVector3(0.0f, 0.0f, 25.0f));
                pParticle->SetScaleStc   (4.0f);
                pParticle->SetAngleStc   (0.0f);
                pParticle->SetColor4Stc  (coreVector4(COLOR_WHITE_F, 1.0f));
                pParticle->SetSpeed      (2.0f);
            });
        }
    }

    // mark as finished
    if(this->GetPosition().y <= BACK_REMOVE_Y) m_iStatus = 1;

    // move the object
    coreObject3D::Move();
}