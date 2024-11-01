//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cRay::cRay(const coreVector3 vStart)noexcept
: m_vStart     (vStart)
, m_fAngle     (0.0f)
, m_fAnimation (Core::Rand->Float(0.0f, 2.0f*PI))
{
    // load object resources
    this->DefineModel  ("ray.md5z");
    this->DefineProgram("ray_program");

    // set object properties
    this->SetPosition   (vStart);
    this->SetSize       (coreVector3(RAY_WIDTH, RAY_WIDTH, RAY_LENGTH));
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation((vStart * coreVector3(1.0f,0.0f,1.0f)).Normalized());

    // set random plate color
    const coreVector3 vColor = g_pBackground->GetColor(Core::Rand->Uint(COLOR_NUM-1u));
    this->SetColor4(coreVector4(vColor, 0.4f));

    // rotate ray to the dance floor
    m_fAngle = this->GetOrientation().xz().Angle();
}


// ****************************************************************
// move the ray
void cRay::Move()
{
    // update the move-animation
    // m_fAnimation.Update(1.0f); #deactivated

    // calculate orientation
    const coreVector2 vNewDir = coreVector2::Direction(m_fAngle + 0.2f*SIN(m_fAnimation));
    this->SetOrientation(coreVector3(vNewDir.x, 0.0f, vNewDir.y));

    // calculate position
    const coreVector3 vNewPos = m_vStart - this->GetOrientation() * RAY_LENGTH * 0.2f;
    this->SetPosition(coreVector3(vNewPos.x, this->GetPosition().y, vNewPos.z));

    // mark as finished
    if(this->GetPosition().y <= BACK_REMOVE_Y) m_iStatus = 1;

    // fade out the ray when near the camera and flash it with the beat
    const coreFloat fAlpha = MIN((this->GetPosition().y + 45.0f) * 0.02f, 1.0f) * g_pBackground->GetFlash(0.2f);
    this->SetAlpha(fAlpha*0.4f);

    // move the object
    this->coreObject3D::Move();
}