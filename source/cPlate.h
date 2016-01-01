/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_PLATE_H_
#define _DR_GUARD_PLATE_H_


// ****************************************************************
// plate class
class cPlate final : public coreObject3D
{
private:
    coreTimer m_Animation;      // timer for the lift-animation
    coreFloat m_fStartY;        // minimal Y-distance to the front to begin the lift-animation

    coreVector2 m_vTexOffset;   // texture-offset to correctly map the disco-lights on the plate


public:
    cPlate(const coreFloat fStartY, const coreVector2& vTexOffset)noexcept;
    ~cPlate();

    DISABLE_COPY(cPlate)

    // move the plate
    void Move()override;
};


#endif // _DR_GUARD_PLATE_H_