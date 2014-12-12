/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_ROCK_H_
#define _DR_GUARD_ROCK_H_


// ****************************************************************
// rock class
class cRock final : public coreObject3D
{
private:
    coreObject3D m_Wave;                 // big wave effect
    coreTimer    m_WaveTimer;            // timer for the big wave
    float        m_fWaveStrength;        // size of the big wave

    coreObject3D m_WaveSmall;            // small wave effect
    coreTimer    m_WaveSmallTimer;       // timer for the small wave
    float        m_fWaveSmallStrength;   // visual strength of the small wave (not size)

    coreObject3D m_WaveShock;            // shock-wave effect coming from the back
    coreTimer    m_WaveShockTimer;       // timer for the shock-wave
    float        m_fWaveShockSpeed;      // movement speed modifier of the shock-wave

    coreObject3D m_Shadow;               // shadow object

    float m_fForce;                      // jump/fall force in Z direction
    float m_fHeight;                     // current Z position
    bool  m_bColored;                    // colored smoke trail

    bool m_bFallen;                      // touched a hole in the floor and starts to fall
    bool m_bJumped;                      // jumped recently and cannot jump again
    bool m_bReflected;                   // force was reflected from the ground in this frame
    int  m_iNumJumps;                    // number of successful jumps
    int  m_iNumAirJumps;                 // number of successful air-jumps (but only not over solid ground)

    coreFlow m_fRotation;                // rotation-timer

    coreSoundPtr m_pUp;                  // jump sound-effect
    coreSoundPtr m_pDown;                // sound-effect for hitting the ground
    coreSoundPtr m_pWoosh;               // sound-effect for explosions and acceleration

    coreParticleEffect m_Effect;         // particle effect object for smoke trail


public:
    cRock()noexcept;
    ~cRock();

    // move the rock
    void Move()override;

    // render different parts of the object separately for batched rendering
    inline void RenderShadow() {m_Shadow.Render();}
    inline void RenderWaves()  {if(m_WaveTimer.GetStatus()) m_Wave.Render(); if(m_WaveSmallTimer.GetStatus()) m_WaveSmall.Render(); if(m_WaveShockTimer.GetStatus()) {glDisable(GL_DEPTH_TEST); m_WaveShock.Render(); glEnable(GL_DEPTH_TEST);}}
    inline void RenderRock()   {coreObject3D::Render();}

    // just jump
    bool Jump(const float& fForce);

    // create shock-wave
    void CreateShockWave(const coreByte& iType);

    // set object properties
    inline void SetColored(const bool& bColored) {m_bColored = bColored;}

    // get object properties
    inline const bool& GetFallen      ()const {return m_bFallen;}
    inline const bool& GetJumped      ()const {return m_bJumped;}
    inline const bool& GetReflected   ()const {return m_bReflected;}
    inline const int&  GetNumJumps    ()const {return m_iNumJumps;}
    inline const int&  GetNumAirJumps ()const {return m_iNumAirJumps;}


private:
    // hide default render function
    inline void Render()override {ASSERT(false)}
};


#endif // _DR_GUARD_ROCK_H_