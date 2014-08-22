/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
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
    float       m_fAngle;    // calculated starting-angle of the ray

    coreFlow m_fAnimation;   // timer for move-animation


public:
    cRay(const coreVector3& vStart)noexcept;
    ~cRay();

    // move the ray
    void Move()override;
};


#endif // _DR_GUARD_RAY_H_