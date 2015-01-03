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
// constructor
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
// destructor
cBackground::~cBackground()
{
    // free model
    Core::Manager::Resource->Free(&m_pModel);

    // delete height data
    SAFE_DELETE_ARRAY(m_pfHeight)
}


// ****************************************************************
// render the background
void cBackground::Render()
{
    glDisable(GL_BLEND);
    {
        // light flash (in render-function to animate even in pause-mode)
        {
            // get music speed (yes this is currently hardcoded, needs to be improved)
            float fSpeed = 0.0f;
                 if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(0)) fSpeed = 1.9166667f;
            else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(1)) fSpeed = 2.00f;
            else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(2)) fSpeed = 1.95f;

            // update light time
            m_fLightTime += Core::System->GetTime() * fSpeed * g_fMusicSpeed;

            // check for new tick
            const int iNewTick = F_TO_SI(m_fLightTime);
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
            glDrawRangeElements(m_pModel->GetPrimitiveType(), 0, BACK_TOTAL_VERTICES, BACK_RANGE, m_pModel->GetIndexType(), r_cast<const GLvoid*>(m_iOffset));
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
    m_iOffset = F_TO_UI(m_fPositionTime) * BACK_BLOCKS_X * BACK_PER_INDICES * sizeof(coreUshort);
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
// make or remove holes
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
// get height value at specific position
float cBackground::GetHeight(const coreVector2& vPos, const coreVector2& vBackPos)const
{
    // convert real position to block position
    const float fX = (vPos.x-vBackPos.x) / BACK_DETAIL_X + I_TO_F(BACK_BLOCKS_X)/2.0f;
    const float fY = (vPos.y-vBackPos.y) / BACK_DETAIL_Y + BACK_OFFSET_Y;

     // retrieve height value of the block
    return m_pfHeight[(F_TO_SI(fX) + F_TO_SI(fY)*BACK_BLOCKS_X) * BACK_PER_VERTICES];

    /*
        // retrieve all four corners of the block
        const int iI00 = (F_TO_SI(fX) + F_TO_SI(fY)*BACK_BLOCKS_X) * BACK_PER_VERTICES;
        const int iI01 = iI00 + 1;
        const int iI10 = iI00 + 2;
        const int iI11 = iI00 + 3;
        ASSERT(iI11 < BACK_TOTAL_VERTICES);

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
// load dance floor geometry
void cBackground::LoadGeometry()
{
    std::vector<coreVector4> avColor;     avColor.reserve    (BACK_BLOCKS);
    std::vector<sVertex>     pVertexData; pVertexData.reserve(BACK_TOTAL_VERTICES);
    std::vector<coreUshort>  pIndexData;  pIndexData.reserve (BACK_TOTAL_INDICES);

    // delete old data
    m_pModel->Unload();
    SAFE_DELETE_ARRAY(m_pfHeight)

    // create base geometry
    sVertex aBaseVertex[BACK_WIDTH * BACK_HEIGHT];
    for(int i = 0; i < BACK_WIDTH * BACK_HEIGHT; i++)
    {
        const int x = i % BACK_WIDTH;
        const int y = i / BACK_WIDTH;

        // set positions and texture coordinates of the grid
        aBaseVertex[i].vPosition = coreVector2(I_TO_F(x - BACK_WIDTH/2) * BACK_DETAIL_X, (I_TO_F(y) - BACK_OFFSET_Y) * BACK_DETAIL_Y);
        aBaseVertex[i].vTexCoord = coreVector2(I_TO_F(x), I_TO_F(-y));
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

    // sync beginning and ending colors to create an infinite looking grid when resetting the position
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

            const coreUint   iColor      = avColor[x + y*BACK_BLOCKS_X].PackUnorm4x8();
            const coreUshort iStartIndex = coreUshort(pVertexData.size());

            // copy base vertices to create unique plates and add generated color values
            pVertexData.push_back(aBaseVertex[j]);              pVertexData.back().iColor = iColor;
            pVertexData.push_back(aBaseVertex[j+1]);            pVertexData.back().iColor = iColor;
            pVertexData.push_back(aBaseVertex[j  +BACK_WIDTH]); pVertexData.back().iColor = iColor;
            pVertexData.push_back(aBaseVertex[j+1+BACK_WIDTH]); pVertexData.back().iColor = iColor;

            // add indices for the new plate
            pIndexData.push_back(iStartIndex + 0);
            pIndexData.push_back(iStartIndex + 1);
            pIndexData.push_back(iStartIndex + 2);
            pIndexData.push_back(iStartIndex + 1);
            pIndexData.push_back(iStartIndex + 3);
            pIndexData.push_back(iStartIndex + 2);
        }
    }

    coreVertexBuffer* pBuffer;

    // create static vertex buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(sVertex), pVertexData.data(), CORE_DATABUFFER_STORAGE_STATIC);
    pBuffer->DefineAttribute(0, 2, GL_FLOAT,         0);
    pBuffer->DefineAttribute(1, 2, GL_FLOAT,         2*sizeof(float));
    pBuffer->DefineAttribute(2, 4, GL_UNSIGNED_BYTE, 4*sizeof(float));

    // create dynamic height data buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(float), m_pfHeight, CORE_DATABUFFER_STORAGE_DYNAMIC);
    pBuffer->DefineAttribute(3, 1, GL_FLOAT, 0);

    // create index buffer
    m_pModel->CreateIndexBuffer(BACK_TOTAL_INDICES, sizeof(coreUshort), pIndexData.data(), CORE_DATABUFFER_STORAGE_STATIC);

    // clear memory
    avColor.clear();
    pVertexData.clear();
    pIndexData.clear();

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
         if(m_fCurColorHue  < 0.0f) for(coreUint i = 0; i < COLOR_NUM; ++i) m_avColor[i] = coreVector3(0.0f, 0.0f, 0.2f + g_avColor[i].Min()*1.6f).HSVtoRGB();
    else if(m_fCurColorHue == 0.0f) for(coreUint i = 0; i < COLOR_NUM; ++i) m_avColor[i] = g_avColor[i];
    else
    {
        for(coreUint i = 0; i < COLOR_NUM; ++i)
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


// ****************************************************************
// reset with the resource manager
void cBackground::__Reset(const coreResourceReset& bInit)
{
    if(bInit) this->LoadGeometry();
    else m_pModel->Unload();
}