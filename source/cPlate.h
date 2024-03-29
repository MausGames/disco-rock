//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
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
    cPlate(const coreFloat fStartY, const coreVector2 vTexOffset)noexcept;

    DISABLE_COPY(cPlate)

    // move the plate
    void Move()final;
};


#endif // _DR_GUARD_PLATE_H_