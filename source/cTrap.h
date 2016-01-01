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
// trap class
class cTrap final : public coreObject3D
{
private:
    coreFlow           m_pGlow;    // glowing animation timer
    coreParticleEffect m_Effect;   // particle effect object

    coreBool m_bStatic;            // only for highlighting something


public:
    cTrap()noexcept;
    ~cTrap();

    DISABLE_COPY(cTrap)

    // move the trap
    void Move()override;

    // check for static behavior
    inline void            SetStatic(const coreBool bStatic) {m_bStatic = bStatic;}
    inline const coreBool& IsStatic ()const                  {return m_bStatic;}
};


#endif // _DR_GUARD_TRAP_H_