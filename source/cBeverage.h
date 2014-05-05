/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_BEVERAGE_H_
#define _DR_GUARD_BEVERAGE_H_


// ****************************************************************
class cBeverage : public coreObject3D
{
protected:
    coreObject3D  m_Shadow;          // shadow object
    coreObject3D* m_pStraw;          // optional straw object
    coreObject3D* m_pGlass;          // optional separate glass object
    coreProgramShr m_pGlasProgram;   // glass shader-program used with the drink-model when no separate glass object was created

    int m_iScore;                    // score value of the beverage

    float m_fHeight;                 // Z position-offset on the dance floor
    float m_fLiquidAlpha;            // alpha value of the drink
    
    coreTimer m_pDestroy;            // timer for the fly-animation
    coreVector3 m_vFlyRotation;      // rotation-parameter for the fly-animation
    coreVector3 m_vFlyImpact;        // impact-direction to throw the object into a specific direction

    coreSoundPtr m_pClink;           // clink sound-effect
    float m_fVolume;                 // sound-volume parameter
    float m_fPitch;                  // sound-pitch parameter


public:
    cBeverage(const int& iScore, const float& fHeight, const float& fAlpha, const float& fVolume, const float& fPitch)noexcept;
    virtual ~cBeverage();

    // render and move the beverage
    inline void Render()override {SDL_assert(false);}
    void Move()override;

    // render different parts of the object separately for batched rendering
    inline void RenderShadow() {m_Shadow.Render();}
    inline void RenderStraw()  {if(m_pStraw) m_pStraw->Render();}
    inline void RenderDrink()  {coreObject3D::Render();}
    inline void RenderGlass()  {if(m_pGlass) m_pGlass->Render(); else coreObject3D::Render(m_pGlasProgram, true);}

    // destroy the object (send it flying)
    inline void Destroy(const coreVector3& vFlyImpact) {m_pDestroy.Play(true); m_vFlyRotation = coreVector3(coreVector2::Rand(), 0.0f); m_vFlyImpact = vFlyImpact; m_vFlyImpact.x += Core::Rand->Float(-0.7f, 0.7f);}

    // play a completely shitty glass clinking sound effect
    inline void PlaySound() {m_pClink->PlayPosition(NULL, m_fVolume, m_fPitch, 0.05f, false, this->GetPosition());}

    // get object properties
    inline const int& GetScore()const {return m_iScore;}
    virtual coreByte GetSigID()const = 0;
    virtual coreVector4 GetSigColor()const = 0;


private:
    DISABLE_COPY(cBeverage)
};


// ****************************************************************
// tequila sunrise (orange)
class cSunrise final : public cBeverage
{
public:
    cSunrise()noexcept;
    ~cSunrise();

    // get object properties
    inline coreByte GetSigID()const override       {return 1;}
    inline coreVector4 GetSigColor()const override {return COLOR_YELLOW_F;}
};


// ****************************************************************
// mojito! (green)
class cMojito final : public cBeverage
{
public:
    cMojito()noexcept;
    ~cMojito();

    // get object properties
    inline coreByte GetSigID()const override       {return 2;}
    inline coreVector4 GetSigColor()const override {return COLOR_GREEN_F;}
};


// ****************************************************************
// a weird blue drink with a lemon slice (blue)
class cBlue final : public cBeverage
{
public:
    cBlue()noexcept;
    ~cBlue();

    // get object properties
    inline coreByte GetSigID()const override       {return 3;}
    inline coreVector4 GetSigColor()const override {return COLOR_BLUE_F;}
};


// ****************************************************************
// coola (black)
class cCoola final : public cBeverage
{
public:
    cCoola()noexcept;
    ~cCoola();

    // get object properties
    inline coreByte GetSigID()const override       {return 4;}
    inline coreVector4 GetSigColor()const override {return COLOR_PURPLE_F;}
};


#endif // _DR_GUARD_BEVERAGE_H_