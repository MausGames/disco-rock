/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
cBackground::cBackground()noexcept
: m_fPositionTime   (0.0f)
, m_fFloorTime      (0.0f)
, m_fFillTime       (0.0f)
, m_pfHeight        (NULL)
, m_iOffset         (0)
, m_fLightStrength  (0.0f)
, m_iLightTick      (0)
, m_fLightTime      (0.0f)
, m_fCurColorHue    (-1.0f)
, m_fLightDirection (-1.0f)
{
    std::memset(m_avColor, 0, sizeof(m_avColor));

    // load dance floor geometry
    m_pModel = Core::Manager::Resource->LoadNew<coreModel>();
    this->ModifyColor();
    this->LoadGeometry();

    // load object resources
    this->DefineTexture(0, "background.png");
    this->DefineTexture(1, "background_norm.png");
    this->DefineProgram("floor_program");

    // create filling background
    m_Fill.DefineTexture(0, "background.png");
    m_Fill.DefineProgram("fill_program");
    m_Fill.FitToScreen();
    m_Fill.SetTexSize(m_Fill.GetSize()*7.2f);
}


// ****************************************************************
cBackground::~cBackground()
{
    // free model
    Core::Manager::Resource->Free(&m_pModel);

    // delete height data
    SAFE_DELETE_ARRAY(m_pfHeight)
}


// ****************************************************************
void cBackground::Render()
{
    glDisable(GL_BLEND);
    {
        // light flash (made in render-function to animate even in pause-mode)
        {
            // get music speed (yes this is currently hardcoded, needs to be improved)
            float fSpeed = 0.0f;
                 if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(0)) fSpeed = 1.9166667f;
            else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(1)) fSpeed = 2.00f;
            else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(2)) fSpeed = 1.95f;

            // update light time
            m_fLightTime += Core::System->GetTime() * fSpeed * (1.0f + MAX((g_fCurSpeed - 1.5f) * 0.16667f, 0.0f));

            // check for new tick
            const int iNewTick = (int)FLOOR(m_fLightTime);
            if(m_iLightTick < iNewTick)
            {
                // create flash
                m_iLightTick = iNewTick;
                m_fLightStrength = 0.3f;
            }

            // reduce light strength (flash) over time
            m_fLightStrength = MAX(m_fLightStrength * (1.0f - Core::System->GetTime() * 4.0f), 0.0f);

            if(!g_bPause)
            {
                // calculate final light power
                const float fPower = MAX(g_fCurSpeed-1.0f, 0.5f) * 1.5f * m_fLightStrength;

                // set alpha values used as light power in the shader
                m_Fill.SetAlpha((1.0f + 2.0f*fPower) * 0.375f);
                this->SetAlpha(1.0f + fPower);
            }
        }

        // enable all resources
        if(this->Enable())
        {
            // draw the model
            glDrawRangeElements(m_pModel->GetPrimitiveType(), 0, BACK_BLOCKS * BACK_PER_VERTICES, BACK_RANGE, m_pModel->GetIndexType(), r_cast<const GLvoid*>(m_iOffset));
        }

        // render the filling background
        m_Fill.Render();
    }
    glEnable(GL_BLEND);
}


// ****************************************************************
void cBackground::Move()
{
    // update dance floor position
    m_fPositionTime.Update(2.5f, 0);
    if(m_fPositionTime < 0.0f)        m_fPositionTime += BACK_REPEAT;
    if(m_fPositionTime > BACK_REPEAT) m_fPositionTime -= BACK_REPEAT;

    // calculate drawing offset and background position
    m_iOffset = coreUint(FLOOR(m_fPositionTime)) * BACK_BLOCKS_X * BACK_PER_INDICES * sizeof(coreUshort);
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
    coreObject3D::Move();
}


// ****************************************************************
void cBackground::UpdateHoles(const coreUint& iLine, const bool* pbIndex)
{
    constexpr_var coreUint iNum  = BACK_BLOCKS_X * BACK_PER_VERTICES;
    constexpr_var coreUint iSize = iNum * sizeof(float);

    // map required area
    float* pfData = m_pModel->GetVertexBuffer(1)->Map<float>(iLine*iSize, iSize, CORE_DATABUFFER_MAP_INVALIDATE_RANGE);
    ASSERT((iLine+1) * iSize < BACK_TOTAL_INDICES * sizeof(float));

    // set height values of the selected line
    for(coreUint i = 0; i < iNum; ++i)
    {
        pfData[i] = m_pfHeight[i + iLine*iNum] = pbIndex[i/BACK_PER_VERTICES] ? 100.0f : 0.0f;
    }

    // unmap area
    m_pModel->GetVertexBuffer(1)->Unmap(pfData);  

    // reset current model object
    coreModel::Disable(false);
}


// ****************************************************************
float cBackground::GetHeight(const coreVector2& vPos, const coreVector2& vBackPos)const
{
    // convert real position to block position
    const float fX = (vPos.x-vBackPos.x) / BACK_DETAIL_X + float(BACK_BLOCKS_X)/2.0f;
    const float fY = (vPos.y-vBackPos.y) / BACK_DETAIL_Y + BACK_OFFSET_Y;

     // retrieve height value of the block
    return m_pfHeight[(int(FLOOR(fX)) + int(FLOOR(fY))*BACK_BLOCKS_X) * BACK_PER_VERTICES];

/*
    // retrieve all four corners of the block
    const int iI00 = (int(FLOOR(fX)) + int(FLOOR(fY))*BACK_BLOCKS_X) * BACK_PER_VERTICES;
    const int iI01 = iI00 + 1;
    const int iI10 = iI00 + 2;
    const int iI11 = iI00 + 3;
    ASSERT(iI11 < BACK_BLOCKS * BACK_PER_VERTICES);
    
    // retrieve height values of the corners
    const float& fH00 = m_pfHeight[iI00];
    const float& fH01 = m_pfHeight[iI01];
    const float& fH10 = m_pfHeight[iI10];
    const float& fH11 = m_pfHeight[iI11];

    // interpolate between all height values
    const float fFractX = FRACT(fX);
    const float fFractY = FRACT(fY);
    return LERP(LERP(fH00, fH10, fFractX), LERP(fH01, fH11, fFractX), fFractY);
*/
}


// ****************************************************************
void cBackground::LoadGeometry()
{
    std::vector<coreVector4> avColor;       avColor.reserve      (BACK_BLOCKS);
    std::vector<sVertex>     m_pVertexData; m_pVertexData.reserve(BACK_TOTAL_VERTICES);
    std::vector<coreUshort>  m_pIndexData;  m_pIndexData.reserve (BACK_TOTAL_INDICES);

    // delete old data
    m_pModel->Unload();
    SAFE_DELETE_ARRAY(m_pfHeight)

    // create base geometry
    sVertex* pBaseVertex = new sVertex[BACK_WIDTH * BACK_HEIGHT];
    for(int i = 0; i < BACK_WIDTH * BACK_HEIGHT; i++)
    {
        const int x = i % BACK_WIDTH;
        const int y = i / BACK_WIDTH;

        // set positions and texture coordinates of the grid
        pBaseVertex[i].vPosition = coreVector2(float(x - BACK_WIDTH/2) * BACK_DETAIL_X, (float(y) - BACK_OFFSET_Y) * BACK_DETAIL_Y);
        pBaseVertex[i].vTexture  = coreVector2(float(x), float(-y));
    }

    // create persistent array with mutable height data (the holes are just out of screen)
    m_pfHeight = new float[BACK_TOTAL_VERTICES];
    for(int i = 0; i < BACK_TOTAL_VERTICES; ++i) m_pfHeight[i] = 0.0f;

    // define color values
    int iCurColor = 0;
    for(int i = 0; i < BACK_BLOCKS; ++i)
    {
        // get random color, but not the same twice (minor error on the border and with separate plates)
        do
        {
            iCurColor = (iCurColor + Core::Rand->Int(1, COLOR_NUM-1)) % COLOR_NUM;
        }
        while(i >= BACK_BLOCKS_X && (m_avColor[iCurColor] == avColor[i-BACK_BLOCKS_X].xyz() ||
                                     m_avColor[iCurColor] == avColor[i-1].xyz()));
        avColor.push_back(coreVector4(m_avColor[iCurColor], 1.0f));

        // add additional random parameter for the shader
        avColor.back().a = Core::Rand->Float(0.9f, 1.0f) * COLOR_BRIGHTNESS;
    }

    // sync beginning and ending colors to create an infinit looking grid when resetting the position
    for(int y = 20; y < BACK_BLOCKS_Y; ++y)
    {
        for(int x = 0; x < BACK_BLOCKS_X; ++x)
        {
            avColor[x + y*BACK_BLOCKS_X] = avColor[x + (y-20)*BACK_BLOCKS_X];
        }
    }

    // create final vertex and index data
    for(int y = 0; y < BACK_BLOCKS_Y; ++y)
    {
        for(int x = 0; x < BACK_BLOCKS_X; ++x)
        {
            const coreUint j = x + y*BACK_WIDTH;

            const coreVector4& vColor    = avColor[x + y*BACK_BLOCKS_X];
            const coreUshort iStartIndex = m_pVertexData.size();

            // copy base vertices to create unique plates and add generated color values
            m_pVertexData.push_back(pBaseVertex[j]);              m_pVertexData.back().vColor = vColor;
            m_pVertexData.push_back(pBaseVertex[j+1]);            m_pVertexData.back().vColor = vColor;
            m_pVertexData.push_back(pBaseVertex[j  +BACK_WIDTH]); m_pVertexData.back().vColor = vColor;
            m_pVertexData.push_back(pBaseVertex[j+1+BACK_WIDTH]); m_pVertexData.back().vColor = vColor;

            // add indices for the new plate
            m_pIndexData.push_back(iStartIndex + 0);
            m_pIndexData.push_back(iStartIndex + 1);
            m_pIndexData.push_back(iStartIndex + 2);
            m_pIndexData.push_back(iStartIndex + 1);
            m_pIndexData.push_back(iStartIndex + 3);
            m_pIndexData.push_back(iStartIndex + 2);
        }
    }

    coreVertexBuffer* pBuffer;

    // create static vertex buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(sVertex), m_pVertexData.data(), CORE_DATABUFFER_STORAGE_STATIC);
    pBuffer->DefineAttribute(0, 2, GL_FLOAT, 0);
    pBuffer->DefineAttribute(1, 2, GL_FLOAT, 2*sizeof(float));
    pBuffer->DefineAttribute(2, 4, GL_FLOAT, 4*sizeof(float));

    // create dynamic height data buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(float), m_pfHeight, CORE_DATABUFFER_STORAGE_PERSISTENT);
    pBuffer->DefineAttribute(3, 1, GL_FLOAT, 0);

    // create index buffer
    m_pModel->CreateIndexBuffer(BACK_TOTAL_INDICES, sizeof(coreUshort), m_pIndexData.data(), CORE_DATABUFFER_STORAGE_STATIC);

    // clear memory
    avColor.clear();
    m_pVertexData.clear();
    m_pIndexData.clear();
    SAFE_DELETE_ARRAY(pBaseVertex)

    // cthulhu fhtagn cheezburger
    Core::Log->Info("Background loaded");
}


// ****************************************************************
void cBackground::ModifyColor()
{
    // change current hue offset
         if(m_fCurColorHue ==   0.0f/360.0f) m_fCurColorHue = 190.0f/360.0f;
    else if(m_fCurColorHue == 190.0f/360.0f) m_fCurColorHue =         -1.0f;   // gr[e/a]y
                                        else m_fCurColorHue =   0.0f/360.0f;

    // modify default colors
         if(m_fCurColorHue  < 0.0f) for(int i = 0; i < 6; ++i) m_avColor[i] = coreVector3(0.0f, 0.0f, 0.2f + g_avColor[i].Min()*1.6f).HSVtoRGB();
    else if(m_fCurColorHue == 0.0f) for(int i = 0; i < 6; ++i) m_avColor[i] = g_avColor[i];
    else
    {
        for(int i = 0; i < 6; ++i)
        {
            // change hue of all default colors
            coreVector3 vNewColor = g_avColor[i].RGBtoHSV();
            vNewColor.x  = FRACT(vNewColor.x + m_fCurColorHue);
            vNewColor.z *= 0.95f;
            m_avColor[i] = vNewColor.HSVtoRGB();
        }
    }

    // flip light direction
    m_fLightDirection = -m_fLightDirection;
}