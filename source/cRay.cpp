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
cRay::cRay(const coreVector3& vStart)noexcept
: m_vStart     (vStart)
, m_fAngle     (0.0f)
, m_fAnimation (Core::Rand->Float(0.0f, 2.0f*PI))
{
    // load object resources
    this->DefineModel  ("ray.md5mesh");
    this->DefineProgram("ray_program");

    // set object properties
    this->SetPosition (vStart);
    this->SetSize     (coreVector3(RAY_WIDTH, RAY_WIDTH, RAY_LENGTH));
    this->SetDirection((vStart * coreVector3(1.0f,0.0f,1.0f)).Normalize());

    // set random plate color
    const coreVector3& vColor = g_pBackground->GetColor(Core::Rand->Int(0, COLOR_NUM-1));
    this->SetColor4(coreVector4(vColor, 0.4f));

    // rotate ray to the dance floor
    m_fAngle = this->GetDirection().xz().Angle();
}


// ****************************************************************
// destructor
cRay::~cRay()
{
}


// ****************************************************************
// move the ray
void cRay::Move()
{
    // update the move-animation
    // m_fAnimation.Update(1.0f); #deactivated

    // calculate direction
    const coreVector2 vNewDir = coreVector2::Direction(m_fAngle + 0.2f*sinf(m_fAnimation));
    this->SetDirection(coreVector3(vNewDir.x, 0.0f, -vNewDir.y));

    // calculate position
    const coreVector3 vNewPos = m_vStart + this->GetDirection() * RAY_LENGTH * 0.25f;
    this->SetPosition(coreVector3(vNewPos.x, this->GetPosition().y, vNewPos.z));

    // mark as finished
    if(this->GetPosition().y <= BACK_REMOVE_Y) m_iStatus = 1;

    // align the orientation
    this->SetOrientation((this->GetPosition() - Core::Graphics->GetCamPosition()).Normalize());

    // fade out the ray when near the camera and flash it with the beat
    const float fAlpha = MIN((this->GetPosition().y + 45.0f) * 0.02f, 1.0f) * g_pBackground->GetFlash(0.2f);
    this->SetAlpha(fAlpha*0.4f);

    // move the object
    coreObject3D::Move();
}