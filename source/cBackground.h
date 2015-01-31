/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#pragma once
#ifndef _DR_GUARD_BACKGROUND_H_
#define _DR_GUARD_BACKGROUND_H_


// ****************************************************************
// background definitions
#define BACK_WIDTH  9                                                // vertices per row
#define BACK_HEIGHT 65                                               // vertices per column

#define BACK_BLOCKS_X (BACK_WIDTH-1)                                 // blocks per row
#define BACK_BLOCKS_Y (BACK_HEIGHT-1)                                // blocks per column
#define BACK_BLOCKS   (BACK_BLOCKS_X * BACK_BLOCKS_Y)                // number of all blocks

#define BACK_PER_VERTICES   (4)                                      // vertices per block
#define BACK_PER_INDICES    (6)                                      // indices per block
#define BACK_TOTAL_VERTICES (BACK_PER_VERTICES * BACK_BLOCKS)        // total number of vertices
#define BACK_TOTAL_INDICES  (BACK_PER_INDICES  * BACK_BLOCKS)        // total number of indices

#define BACK_DETAIL_X (24.0f*0.91f)                                  // X size of a block
#define BACK_DETAIL_Y (24.0f)                                        // Y size of a block
#define BACK_OFFSET_Y (2.0f)                                         // Y position offset

#define BACK_VIEW   (24)                                             // visible rows
#define BACK_REPEAT (40)                                             // rows when to repeat (BACK_VIEW + BACK_REPEAT < BACK_HEIGHT)
#define BACK_RANGE  (BACK_BLOCKS_X * BACK_PER_INDICES * BACK_VIEW)   // vertices to draw at once

#define BACK_SPAWN_X(i,o) (BACK_DETAIL_X * (I_TO_F(i) - (I_TO_F(BACK_BLOCKS_X)/2.0f - o)))   // X position at a specific plate number and with offset
#define BACK_SPAWN_Y      (BACK_DETAIL_Y * (BACK_VIEW - BACK_OFFSET_Y + 0.5f))               // Y position at where to spawn objects on the horizon
#define BACK_REMOVE_Y     (-52.0f)                                                           // Y position at where to remove objects


// ****************************************************************
// background class
class cBackground final : public coreObject3D, public coreResourceRelation
{
private:
    struct sVertex
    {
        coreVector2 vPosition;   // vertex position
        coreVector2 vTexCoord;   // texture coordinate
        coreUint    iColor;      // RGBA color-value

        constexpr_func sVertex()noexcept;
    };


private:
    coreObject2D m_Fill;                // filling background with funky disco lights

    coreFlow m_fPositionTime;           // timer for the current dance floor position
    coreFlow m_fFloorTime;              // timer for the light animation on the dance floor
    coreFlow m_fFillTime;               // timer for the light animation in the background

    float*   m_pfHeight;                // height data for height calculations
    coreUint m_iOffset;                 // current drawing offset

    float m_fLightStrength;             // light strength (from the imaginary disco ball)
    int   m_iLightTick;                 // tick count
    float m_fLightTime;                 // time compared with tick count to create regular light flashes

    coreVector3 m_avColor[COLOR_NUM];   // modified default colors
    float m_fCurColorHue;               // current hue offset used to modify default colors
    float m_fLightDirection;            // move-direction of the light animation


public:
    cBackground()noexcept;
    ~cBackground();

    DISABLE_COPY(cBackground)

    // render and move the background
    void Render()override;
    void Move  ()override;

    // make or remove holes, get current horizon line, do other fancy stuff
    void UpdateHoles(const coreUint& iLine, const bool* pbIndex);
    inline int   GetCurLine()const                       {return F_TO_SI(m_fPositionTime);}
    inline float GetFlash  (const float& fStrength)const {return 1.0f + (fStrength * (this->GetAlpha() - 1.0f));}

    // get height value at specific position
    float        GetHeight(const coreVector2& vPos, const coreVector2& vBackPos)const;
    inline float GetHeight(const coreVector2& vPos)const {return this->GetHeight(vPos, this->GetPosition().xy());}

    // load dance floor geometry
    void LoadGeometry();
    void ModifyColor();

    // get object properties
    inline const float&       GetPositionTime()const                       {return m_fPositionTime;}
    inline const coreVector3& GetColor       (const coreUint& iIndex)const {ASSERT(iIndex < COLOR_NUM); return m_avColor[iIndex];}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


// ****************************************************************
// constructor
constexpr_func cBackground::sVertex::sVertex()noexcept
: vPosition (coreVector2(0.0f,0.0f))
, vTexCoord (coreVector2(0.0f,0.0f))
, iColor    (0)
{
}


#endif // _DR_GUARD_BACKGROUND_H_