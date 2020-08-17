//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_ROCK_H_
#define _DR_GUARD_ROCK_H_


// ****************************************************************
// rock definitions
#if defined(_CORE_ANDROID_) || defined(_DR_EMULATE_MOBILE_)
    #define ROCK_RANGE_BACK (1.85f)   // collision-range for comparisons with the background
    #define ROCK_RANGE_OBJ  (1.1f)    // collision-range for comparisons with other objects
    #define ROCK_SPRITE_NUM (2u)      // number of smoke-sprites per frame
#else
    #define ROCK_RANGE_BACK (1.25f)
    #define ROCK_RANGE_OBJ  (1.0f)
    #define ROCK_SPRITE_NUM (3u)
#endif


// ****************************************************************
// rock class
class cRock final : public coreObject3D
{
private:
    coreObject3D m_Wave;                 // big wave effect
    coreTimer    m_WaveTimer;            // timer for the big wave
    coreFloat    m_fWaveStrength;        // size of the big wave

    coreObject3D m_WaveSmall;            // small wave effect
    coreTimer    m_WaveSmallTimer;       // timer for the small wave
    coreFloat    m_fWaveSmallStrength;   // visual strength of the small wave (not size)

    coreObject3D m_WaveShock;            // shock-wave effect coming from the back
    coreTimer    m_WaveShockTimer;       // timer for the shock-wave
    coreFloat    m_fWaveShockSpeed;      // movement speed modifier of the shock-wave

    coreObject3D m_Shadow;               // shadow object

    coreFloat m_fForce;                  // jump/fall force in Z direction
    coreFloat m_fHeight;                 // current Z position
    coreBool  m_bColored;                // colored smoke trail

    coreBool   m_bFallen;                // touched a hole in the floor and starts to fall
    coreBool   m_bJumped;                // jumped recently and cannot jump again
    coreBool   m_bReflected;             // force was reflected from the ground in this frame
    coreUint16 m_iNumJumps;              // number of successful jumps
    coreUint16 m_iNumAirJumps;           // number of successful air-jumps (but only not over solid ground)

    coreFlow m_fRotation;                // rotation-timer

    coreSoundPtr m_pUp;                  // jump sound-effect
    coreSoundPtr m_pDown;                // sound-effect for hitting the ground
    coreSoundPtr m_pWoosh;               // sound-effect for explosions and acceleration

    coreParticleEffect m_Effect;         // particle effect object for smoke trail


public:
    cRock()noexcept;

    DISABLE_COPY(cRock)

    // move the rock
    void Move()final;

    // render different parts of the object separately for batched rendering
    inline void RenderShadow() {m_Shadow.Render();}
    inline void RenderWaves()  {if(m_WaveTimer.GetStatus()) m_Wave.Render(); if(m_WaveSmallTimer.GetStatus()) m_WaveSmall.Render(); if(m_WaveShockTimer.GetStatus()) {glDisable(GL_DEPTH_TEST); m_WaveShock.Render(); glEnable(GL_DEPTH_TEST);}}
    inline void RenderRock()   {coreObject3D::Render();}

    // just jump
    coreBool Jump(const coreFloat fForce);

    // create shock-wave
    void CreateShockWave(const coreUint8 iType);

    // set object properties
    inline void SetColored(const coreBool bColored) {m_bColored = bColored;}

    // get object properties
    inline const coreBool&   GetFallen     ()const {return m_bFallen;}
    inline const coreBool&   GetJumped     ()const {return m_bJumped;}
    inline const coreBool&   GetReflected  ()const {return m_bReflected;}
    inline const coreUint16& GetNumJumps   ()const {return m_iNumJumps;}
    inline const coreUint16& GetNumAirJumps()const {return m_iNumAirJumps;}


private:
    // hide default render function
    inline void Render()final {ASSERT(false)}
};


#endif // _DR_GUARD_ROCK_H_