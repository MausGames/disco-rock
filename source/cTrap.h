//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_TRAP_H_
#define _DR_GUARD_TRAP_H_


// ****************************************************************
// trap class
class CTrap final : public coreObject3D
{
private:
    coreFlow           m_pGlow;    // glowing animation timer
    coreParticleEffect m_Effect;   // particle effect object

    coreBool m_bStatic;            // only for highlighting something


public:
    CTrap()noexcept;

    DISABLE_COPY(CTrap)

    // move the trap
    void Move()final;

    // check for static behavior
    inline void            SetStatic(const coreBool bStatic) {m_bStatic = bStatic;}
    inline const coreBool& IsStatic ()const                  {return m_bStatic;}
};


#endif // _DR_GUARD_TRAP_H_