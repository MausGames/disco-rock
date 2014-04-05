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
class cRock final : public coreObject3D
{
private:
    coreObject3D m_Wave;           // big wave effect
    coreTimer m_WaveTimer;         // timer for the big wave
    float m_fWaveStrength;         // size of the big wave
                                   
    coreObject3D m_WaveSmall;      // small wave effect
    coreTimer m_WaveSmallTimer;    // timer for the small wave
    float m_fWaveSmallStrength;    // visual strength of the small wave (not size)
                                   
    coreObject3D m_Shadow;         // shadow object
                                   
    float m_fForce;                // jump/fall force in Z direction
    float m_fHeight;               // current Z position

    bool m_bFallen;                // touched a hole in the floor and starts to fall
    bool m_bJumped;                // jumped recently and cannot jump again
                                   
    coreFlow m_fRotation;          // rotation-timer
                                   
    coreSoundPtr m_pUp;            // jump sound-effect
    coreSoundPtr m_pDown;          // sound-effect for hitting the ground

    coreParticleEffect m_Effect;   // particle effect object for smoke trail

    float m_fSensitivity;          // mouse sensitivity


public:
    cRock()noexcept;
    ~cRock();

    // render and move the rock
    void Render()override;
    void Move()override;

    // just jump
    bool Jump(const float& fForce);

    // get object properties
    inline const bool& GetFallen()const {return m_bFallen;}
    inline const bool& GetJumped()const {return m_bJumped;}
};


#endif // _DR_GUARD_ROCK_H_