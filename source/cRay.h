//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_RAY_H_
#define _DR_GUARD_RAY_H_


// ****************************************************************
// ray definitions
#define RAY_LENGTH (120.0f)   // length of the ray
#define RAY_WIDTH  (7.5f)     // width value


// ****************************************************************
// ray class
class cRay final : public coreObject3D
{
private:
    coreVector3 m_vStart;    // starting-position of the ray
    coreFloat   m_fAngle;    // calculated starting-angle of the ray

    coreFlow m_fAnimation;   // timer for move-animation


public:
    cRay(const coreVector3 vStart)noexcept;

    DISABLE_COPY(cRay)

    // move the ray
    void Move()final;
};


#endif // _DR_GUARD_RAY_H_