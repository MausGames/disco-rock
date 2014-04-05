/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_TRAP_H_
#define _DR_GUARD_TRAP_H_


// ****************************************************************
class cTrap final : public coreObject3D
{
protected:
    coreFlow m_pGlow;              // glowing animation timer
    coreParticleEffect m_Effect;   // particle effect object


public:
    cTrap()noexcept;
    ~cTrap();

    // move the trap
    void Move()override;
};


#endif // _DR_GUARD_TRAP_H_