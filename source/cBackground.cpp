//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cBackground::cBackground()noexcept
: m_fPositionTime   (0.0f)
, m_fFloorTime      (0.0f)
, m_fFillTime       (0.0f)
, m_pfHeight        (NULL)
, m_iOffset         (0u)
, m_fLightStrength  (0.0f)
, m_iLightTick      (0u)
, m_fLightTime      (0.0f)
, m_avColor         {}
, m_fCurColorHue    (-1.0f)
, m_fLightDirection (-1.0f)
{
    // load dance floor geometry
    m_pModel = Core::Manager::Resource->LoadNew<coreModel>();
    this->ModifyColor();
    this->LoadGeometry();

    // load object resources
    this->DefineTexture(0u, "background.png");
    this->DefineTexture(1u, "background_norm.png");
    this->DefineProgram("floor_program");

    // create filling background
    m_Fill.DefineTexture(0u, "background.png");
    m_Fill.DefineProgram("fill_program");

    // reset filling background
    this->ResetResolution();
}


// ****************************************************************
// destructor
cBackground::~cBackground()
{
    // free model
    Core::Manager::Resource->Free(&m_pModel);

    // delete height data
    ALIGNED_DELETE(m_pfHeight)
}


// ****************************************************************
// render the background
void cBackground::Render()
{
    glDisable(GL_BLEND);
    {
        // light flash (in render-function to animate even in pause-mode)
        {
            // get music speed (this is currently hardcoded, and can be improved)
            coreFloat fSpeed = 0.0f;
                 if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(0u)) fSpeed = 1.9166667f;
            else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(1u)) fSpeed = 2.00f;
            else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(2u)) fSpeed = 1.95f;

            // update light time
            m_fLightTime += TIME * fSpeed * g_fMusicSpeed;

            // check for new tick
            const coreUintW iNewTick = F_TO_UI(m_fLightTime);
            if(m_iLightTick < iNewTick)
            {
                // create flash
                m_iLightTick = iNewTick;
                m_fLightStrength = 0.3f;
            }

            // reduce light strength (flash) over time
            m_fLightStrength = MAX(m_fLightStrength * (1.0f - TIME * 4.0f), 0.0f);

            if(!g_bPause)
            {
                // calculate final light power
                const coreFloat fPower = MAX(g_fCurSpeed-1.0f, 0.5f) * 1.5f * m_fLightStrength;

                // set alpha values used as light power in the shader
                m_Fill.SetAlpha((1.0f + 2.0f*fPower) * 0.375f);
                this->SetAlpha(1.0f + fPower);
            }
        }

        // enable all resources
        if(this->Prepare())
        {
            // draw the model
            glDrawRangeElements(m_pModel->GetPrimitiveType(), 0u, BACK_TOTAL_VERTICES, BACK_RANGE, m_pModel->GetIndexType(), I_TO_P(m_iOffset));
        }

        // render the filling background
        glDepthMask(false);
        m_Fill.Render();
        glDepthMask(true);
    }
    glEnable(GL_BLEND);
}


// ****************************************************************
// move the background
void cBackground::Move()
{
    // update dance floor position
    m_fPositionTime.Update(2.5f, 0);
    if(m_fPositionTime < 0.0f)        m_fPositionTime += BACK_REPEAT;
    if(m_fPositionTime > BACK_REPEAT) m_fPositionTime -= BACK_REPEAT;

    // calculate drawing offset and background position
    m_iOffset = F_TO_UI(m_fPositionTime) * BACK_BLOCKS_X * BACK_PER_INDICES * sizeof(coreUint16);
    this->SetPosition(coreVector3(0.0f, -m_fPositionTime * BACK_DETAIL_Y, GAME_HEIGHT));

    // update dance floor light animation
    m_fFloorTime.Update(0.5f, 0);
    if(m_fFloorTime >= 20.0f) m_fFloorTime -= 20.0f;
    this->SetTexOffset(coreVector2(m_fLightDirection,-1.0f) * m_fFloorTime);

    // update and move the filling background
    m_fFillTime.Update(0.75f, 0);
    if(m_fFillTime >= 5.0f) m_fFillTime -= 5.0f;
    m_Fill.SetTexOffset(coreVector2(-m_fFillTime * m_fLightDirection, 0.0f));
    m_Fill.Move();

    // move the object
    this->coreObject3D::Move();
}


// ****************************************************************
// reset fullscreen-objects after resolution changes
void cBackground::ResetResolution()
{
    // reset filling background
    m_Fill.FitToScreen();
    m_Fill.SetTexSize(m_Fill.GetSize() * 7.2f);
}


// ****************************************************************
// make or remove holes
void cBackground::UpdateHoles(const coreUintW iLine, const coreBool* pbIndex)
{
    constexpr coreUintW iNum  = BACK_BLOCKS_X * BACK_PER_VERTICES;
    constexpr coreUintW iSize = iNum * sizeof(coreFloat);

    // map required area
    coreByte* pData = m_pModel->GetVertexBuffer(1u)->Map(iLine*iSize, iSize, CORE_DATABUFFER_MAP_UNSYNCHRONIZED);
    ASSERT((iLine+1u) * iSize < BACK_TOTAL_INDICES * sizeof(coreFloat))

    // set height values of the selected line
    for(coreUintW i = 0u; i < iNum; ++i)
    {
        r_cast<coreFloat*>(pData)[i] = m_pfHeight[i + iLine*iNum] = pbIndex[i/BACK_PER_VERTICES] ? 100.0f : 0.0f;
    }

    // unmap area
    m_pModel->GetVertexBuffer(1u)->Unmap();

    // reset current model object
    coreModel::Disable(false);
}


// ****************************************************************
// get height value at specific position
coreFloat cBackground::GetHeight(const coreVector2 vPos, const coreVector2 vBackPos)const
{
    // convert real position to block position
    const coreFloat fX = (vPos.x-vBackPos.x) / BACK_DETAIL_X + I_TO_F(BACK_BLOCKS_X)/2.0f;
    const coreFloat fY = (vPos.y-vBackPos.y) / BACK_DETAIL_Y + BACK_OFFSET_Y;

     // retrieve height value of the block
    return m_pfHeight[(F_TO_UI(fX) + F_TO_UI(fY)*BACK_BLOCKS_X) * BACK_PER_VERTICES];
}


// ****************************************************************
// load dance floor geometry
void cBackground::LoadGeometry()
{
    coreList<coreVector4> avColor;     avColor.reserve    (BACK_BLOCKS);
    coreList<sVertex>     pVertexData; pVertexData.reserve(BACK_TOTAL_VERTICES);
    coreList<coreUint16>  pIndexData;  pIndexData.reserve (BACK_TOTAL_INDICES);

    // delete old data
    m_pModel->Unload();
    ALIGNED_DELETE(m_pfHeight)

    // create base geometry
    sVertex aBaseVertex[BACK_WIDTH * BACK_HEIGHT];
    for(coreUintW i = 0u; i < BACK_WIDTH * BACK_HEIGHT; ++i)
    {
        const coreInt32 x = i % BACK_WIDTH;
        const coreInt32 y = i / BACK_WIDTH;

        // set positions and texture coordinates of the grid
        aBaseVertex[i].vPosition = coreVector2(I_TO_F(x - BACK_WIDTH/2) * BACK_DETAIL_X, (I_TO_F(y) - BACK_OFFSET_Y) * BACK_DETAIL_Y);
        aBaseVertex[i].vTexCoord = coreVector2(I_TO_F(x), I_TO_F(-y));
    }

    // create persistent array with mutable height data (the holes are just out of screen)
    m_pfHeight = ALIGNED_NEW(coreFloat, BACK_TOTAL_VERTICES, ALIGNMENT_CACHE);
    for(coreUintW i = 0u; i < BACK_TOTAL_VERTICES; ++i) m_pfHeight[i] = 0.0f;

    // define color values
    coreUintW iCurColor = 0u;
    for(coreUintW i = 0u; i < BACK_BLOCKS; ++i)
    {
        // get random color, but not the same twice (minor error on the border and with separate plates)
        do
        {
            iCurColor = (iCurColor + Core::Rand->Uint(1u, COLOR_NUM-1u)) % COLOR_NUM;
        }
        while(i >= BACK_BLOCKS_X && (m_avColor[iCurColor] == avColor[i-BACK_BLOCKS_X].xyz() ||
                                     m_avColor[iCurColor] == avColor[i-1u]           .xyz()));
        avColor.emplace_back(m_avColor[iCurColor], 1.0f);

        // add additional random parameter for the shader
        avColor.back().w = Core::Rand->Float(0.9f, 1.0f) * COLOR_BRIGHTNESS;
    }

    // sync beginning and ending colors to create an infinite looking grid when resetting the position
    for(coreUintW y = 20u; y < BACK_BLOCKS_Y; ++y)
    {
        for(coreUintW x = 0u; x < BACK_BLOCKS_X; ++x)
        {
            avColor[x + y*BACK_BLOCKS_X] = avColor[x + (y-20u)*BACK_BLOCKS_X];
        }
    }

    // create final vertex and index data
    for(coreUintW y = 0u; y < BACK_BLOCKS_Y; ++y)
    {
        for(coreUintW x = 0u; x < BACK_BLOCKS_X; ++x)
        {
            const coreUintW j = x + y*BACK_WIDTH;

            const coreUint32 iColor      = avColor[x + y*BACK_BLOCKS_X].PackUnorm4x8();
            const coreUint16 iStartIndex = coreUint16(pVertexData.size());

            // copy base vertices to create unique plates and add generated color values
            pVertexData.push_back(aBaseVertex[j]);               pVertexData.back().iColor = iColor;
            pVertexData.push_back(aBaseVertex[j+1u]);            pVertexData.back().iColor = iColor;
            pVertexData.push_back(aBaseVertex[j   +BACK_WIDTH]); pVertexData.back().iColor = iColor;
            pVertexData.push_back(aBaseVertex[j+1u+BACK_WIDTH]); pVertexData.back().iColor = iColor;

            // add indices for the new plate
            pIndexData.push_back(iStartIndex + 0u);
            pIndexData.push_back(iStartIndex + 1u);
            pIndexData.push_back(iStartIndex + 2u);
            pIndexData.push_back(iStartIndex + 1u);
            pIndexData.push_back(iStartIndex + 3u);
            pIndexData.push_back(iStartIndex + 2u);
        }
    }

    coreVertexBuffer* pBuffer;

    // create static vertex buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(sVertex), pVertexData.data(), CORE_DATABUFFER_STORAGE_STATIC);
    pBuffer->DefineAttribute(0u, 2u, GL_FLOAT,         false, 0u);
    pBuffer->DefineAttribute(1u, 2u, GL_FLOAT,         false, 2u*sizeof(coreFloat));
    pBuffer->DefineAttribute(2u, 4u, GL_UNSIGNED_BYTE, false, 4u*sizeof(coreFloat));

    // create dynamic height data buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(coreFloat), m_pfHeight, CORE_DATABUFFER_STORAGE_DYNAMIC);
    pBuffer->DefineAttribute(3u, 1u, GL_FLOAT, false, 0u);

    // create index buffer
    m_pModel->CreateIndexBuffer(BACK_TOTAL_INDICES, sizeof(coreUint16), pIndexData.data(), CORE_DATABUFFER_STORAGE_STATIC);

    // clear memory
    avColor    .clear();
    pVertexData.clear();
    pIndexData .clear();

    // cthulhu fhtagn cheezburger
    Core::Log->Info("Background loaded");
}


// ****************************************************************
// modify colors used for the dance floor
void cBackground::ModifyColor()
{
    // change current hue offset
         if(m_fCurColorHue ==   0.0f/360.0f) m_fCurColorHue = 190.0f/360.0f;
    else if(m_fCurColorHue == 190.0f/360.0f) m_fCurColorHue =         -1.0f;   // gr[e/a]y
                                        else m_fCurColorHue =   0.0f/360.0f;

    // modify default colors
         if(m_fCurColorHue  < 0.0f) for(coreUintW i = 0u; i < COLOR_NUM; ++i) m_avColor[i] = coreVector3(0.0f, 0.0f, 0.2f + g_avColor[i].Min()*1.6f).HsvToRgb();
    else if(m_fCurColorHue == 0.0f) for(coreUintW i = 0u; i < COLOR_NUM; ++i) m_avColor[i] = g_avColor[i];
    else
    {
        for(coreUintW i = 0u; i < COLOR_NUM; ++i)
        {
            // change hue of all default colors
            coreVector3 vNewColor = g_avColor[i].RgbToHsv();
            vNewColor.x  = FRACT(vNewColor.x + m_fCurColorHue);
            vNewColor.z *= 0.95f;
            m_avColor[i] = vNewColor.HsvToRgb();
        }
    }

    // flip light direction
    m_fLightDirection = -m_fLightDirection;
}


// ****************************************************************
// reset with the resource manager
void cBackground::__Reset(const coreResourceReset bInit)
{
    if(bInit) this->LoadGeometry();
    else m_pModel->Unload();
}