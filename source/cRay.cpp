/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"

#define RAY_LENGTH 120.0f   // length of the ray
#define RAY_WIDTH  7.5f     // width value


// ****************************************************************
cRay::cRay(const coreVector3& vStart)noexcept
: m_vStart     (vStart)
, m_fAngle     (0.0f)
, m_fAnimation (Core::Rand->Float(0.0f, 2.0f*PI))
{
    // load object resources
    this->DefineModelFile("data/models/ray.md5mesh");
    this->DefineProgramShare("ray_shader")
        ->AttachShaderFile("data/shaders/ray.vs")
        ->AttachShaderFile("data/shaders/ray.fs")
        ->Finish();

    // set object properties
    this->SetPosition(vStart);
    this->SetSize(coreVector3(RAY_WIDTH, RAY_WIDTH, RAY_LENGTH));
    this->SetDirection(vStart * coreVector3(1.0f,0.0f,1.0f));

    // set random plate color
    const coreVector4& vColor = g_avColor[Core::Rand->Int(1, COLOR_NUM-1)];
    this->SetColor4(coreVector4(vColor.xyz(), 0.4f));

    // rotate ray to the dance floor
    m_fAngle = this->GetDirection().xz().Angle();
}


// ****************************************************************
cRay::~cRay()
{
}


// ****************************************************************
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
    this->SetOrientation(this->GetPosition() - Core::Graphics->GetCamPosition());

    // fade out the ray when near the camera and flash it with the beat
    const float fAlpha = MIN((this->GetPosition().y + 45.0f) * 0.02f, 1.0f) * g_pBackground->GetFlash(0.2f);
    this->SetAlpha(fAlpha*0.4f);

    // move the object
    coreObject3D::Move();
}