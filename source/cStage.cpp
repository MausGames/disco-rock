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
// apply stage algorithms to the game
void cGame::ProcessStage(const float &fSpawnY, bool* OUTPUT pbHole)
{
#if defined(_CORE_DEBUG_)
    m_aiAlgo[m_iAlgoCurIndex] = STAGE_TRAP;
#endif

    // prevent double-empty-lines
    auto nHasEmptyLines = [this](const bool& bEmptyLines)
    {
        if((m_iAlgoCurCount == 0) && m_bAlgoEmptyLines && bEmptyLines)
            ++m_iAlgoCurCount;
        m_bAlgoEmptyLines = bEmptyLines;
    };

    // select algorithm
    coreUint iMaxCount = 0;
    switch(m_aiAlgo[m_iAlgoCurIndex])
    {

    // big diamond shaped platforms
    case STAGE_DIAMOND: nHasEmptyLines(true); iMaxCount = 61; // 6*n+1
    {
        //  #   //
        // ###  //
        //##### //
        // ###  //
        //  #   //
        //      //
        //   #  //
        //  ### //
        // #####//

        const int iCol = (m_iAlgoCurCount / 6) % 2 + 2;
        const int iAlt =  m_iAlgoCurCount % 6;
        const int iNum = (iAlt > 3) ? (5 - iAlt) : (iAlt - 1);

        if(iNum >= 0) this->AddStreet(iCol, true, iNum, iNum, pbHole);
        if(iNum >= 1)
        {
            this->AddBeverage(fSpawnY, iCol-1, pbHole);
            this->AddBeverage(fSpawnY, iCol,   pbHole);
            this->AddBeverage(fSpawnY, iCol+1, pbHole);
        }

        break;
    }

    // small quadratic platforms
    case STAGE_QUAD: nHasEmptyLines(true); iMaxCount = 60; // 5*n
    {
        //###   //
        //###   //
        //###   //
        //      //
        //   ###//
        //   ###//
        //   ###//
        //      //
        // ###  //
        // ###  //

        const int iAlt =  m_iAlgoCurCount % 5;
        const int iNum = (iAlt > 1) ? (3 - iAlt) : (iAlt - 1);

        if(!iAlt) m_aiAlgoStatus[0] = (m_aiAlgoStatus[0] + Core::Rand->Int(1,3)) % 4;
        const int iCol = (m_aiAlgoStatus[0] > 1) ? (2*m_aiAlgoStatus[0] - 2) : (2*m_aiAlgoStatus[0] + 1);

        if(iNum >= 0)
        {
            this->AddStreet  (iCol, true, 1, 1, pbHole);
            this->AddBeverage(fSpawnY, iCol, pbHole);
            if(iNum >= 1)
            {
                this->AddBeverage(fSpawnY, iCol-1, pbHole);
                this->AddBeverage(fSpawnY, iCol+1, pbHole);
            }
        }

        break;
    }

    // doubled zig-zag course
    case STAGE_ZIGZAG: nHasEmptyLines(true); iMaxCount = 60; // 8*n+4
    {
        //##  ##//
        //##  ##//
        // #### //
        // #### //
        //  ##  //
        //  ##  //
        // ##   //
        // ##   //
        //##    //
        //##    //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            const int iHalf = (m_iAlgoCurCount-1) / 2 + 2;
            const int iCol  = ((iHalf % 8) >= 4) ? (4 - (iHalf-4) % 4) : (iHalf % 4);

            if((iCol == 2) && (m_iAlgoCurCount % 2 == 1))
            {
                const int iNew = Core::Rand->Int(2);
                if((!!iNew) != (!!m_aiAlgoStatus[0])) m_aiAlgoStatus[0] = iNew;
            }

            if(!(m_aiAlgoStatus[0] & 0x1))
            {
                this->AddBeverage(fSpawnY, iCol, pbHole);
                this->AddStreet  (iCol, true, 0, 1, pbHole);
            }
            if(!(m_aiAlgoStatus[0] & 0x2))
            {
                this->AddBeverage(fSpawnY, 4-iCol+1, pbHole);
                this->AddStreet  (4-iCol, true, 0, 1, pbHole);
            }
        }

        break;
    }

    // concave course with jump-passages
    case STAGE_CONCAVE: nHasEmptyLines(true); iMaxCount = 62; // 5*n+2
    {
        //    ##//
        //   ## //
        //  ##  //
        //   ## //
        //    ##//
        //##    //
        // ##   //
        //  ##  //
        // ##   //
        //##    //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            if(m_iAlgoCurCount == 1) m_aiAlgoStatus[0] = Core::Rand->Int(1) * 5;

            const int iHalf = (m_iAlgoCurCount + m_aiAlgoStatus[0] + 1) % 10;
            const int iCol  = (iHalf >= 5) ? (1 - (iHalf-5) % 5) : (-2 + iHalf % 5);

            if((iHalf == 1) || (iHalf == 6)) m_aiAlgoStatus[0] = Core::Rand->Int(1) * 5;

            if(iCol < 0)
            {
                if((iHalf == 9) || (iHalf <= 1))
                {
                    this->AddBeverage(fSpawnY, iCol+5, pbHole);
                    this->AddBeverage(fSpawnY, iCol+6, pbHole);
                }
                else this->AddStreet(iCol+5, true, 0, 1, pbHole);
            }
            else
            {
                if((iHalf >= 4) && (iHalf <= 6))
                {
                    this->AddBeverage(fSpawnY, iCol,   pbHole);
                    this->AddBeverage(fSpawnY, iCol+1, pbHole);
                }
                else this->AddStreet(iCol, true, 0, 1, pbHole);
            }
        }

        break;
    }

    // alternating lines (old one)
    case STAGE_NET: nHasEmptyLines(true); iMaxCount = 62; // 6*n+2
    {
        //# # # //
        //# # # //
        //# # # //
        // # # #//
        // # # #//
        // # # #//

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            if(!m_aiAlgoStatus[0]) m_aiAlgoStatus[0] = Core::Rand->Int(1,2) * 3;

            const int iHalf = (m_iAlgoCurCount - 1) / 6 + 1 + m_aiAlgoStatus[0];
            const int iAlt  = (iHalf + 1) % 2;

            if(m_aiAlgoStatus[1] != iAlt)
            {
                m_aiAlgoStatus[1] = iAlt;
                m_aiAlgoStatus[2] = CLAMP(m_aiAlgoStatus[2] + Core::Rand->Int(-1,1), 0, 2);
            }
            if(m_iAlgoCurCount <= 1) m_aiAlgoStatus[2] = 1;

            this->AddBeverage(fSpawnY, m_aiAlgoStatus[2]*2 + iAlt, pbHole);

            this->AddStreet(0+iAlt, true, 0, 0, pbHole);
            this->AddStreet(2+iAlt, true, 0, 0, pbHole);
            this->AddStreet(4+iAlt, true, 0, 0, pbHole);
        }

        break;
    }

    // multiple canyon jumps (more often)
    case STAGE_JUMP: nHasEmptyLines(false); iMaxCount = GAME_JUMP_WIDTH*4+3; // J*n+3 (shorter!)
    {
        //   #*#//
        //   ###//
        //      //
        //#*#   //
        //###   //
        //      //
        //  **  //
        //  ##  //

        if((m_iAlgoCurCount <= 2) || (m_iAlgoCurCount >= iMaxCount-2))
        {
            this->AddStreet(2, true, 0, 1, pbHole);
            if(m_iAlgoCurCount == 2)
            {
                this->AddTrap(fSpawnY, 2, pbHole);
                this->AddTrap(fSpawnY, 3, pbHole);
            }
            else
            {
                this->AddBeverage(fSpawnY, 2, pbHole);
                this->AddBeverage(fSpawnY, 3, pbHole);
            }
        }
        else
        {
            const int iRed  = m_iAlgoCurCount - 3;
            const int iFill = MAX((iRed % GAME_JUMP_WIDTH) - (GAME_JUMP_WIDTH-3), 0);

            if(iFill)
            {
                const int iCol = ((iRed / GAME_JUMP_WIDTH) % 2) * 3 + 1;

                this->AddStreet(iCol, true, 1, 1, pbHole);
                if(iFill == 1)
                {
                    this->AddBeverage(fSpawnY, iCol-1, pbHole);
                    this->AddBeverage(fSpawnY, iCol,   pbHole);
                    this->AddBeverage(fSpawnY, iCol+1, pbHole);
                }
                else if(iFill == 2) this->AddTrap(fSpawnY, iCol, pbHole);
            }
        }

        break;
    }

    // jump left or right
    case STAGE_CHOICE: nHasEmptyLines(false); iMaxCount = GAME_JUMP_WIDTH*4+3; // J*n+3 (shorter!)
    {
        //  **  //
        //  ##  //
        //      //
        //*    *//
        //#    #//
        //      //
        //  **  //
        //  ##  //

        const int iAlt = m_iAlgoCurCount % (GAME_JUMP_WIDTH*2);

        if(((iAlt <= 2) || (iAlt >= GAME_JUMP_WIDTH*2+1)) && (iAlt || !m_iAlgoCurCount))
        {
            this->AddStreet(2, true, 0, 1, pbHole);
            if((iAlt == 2) && (m_iAlgoCurCount != iMaxCount-1))
            {
                m_aiAlgoStatus[0] = Core::Rand->Int(1) ? 6 : 1;

                this->AddTrap(fSpawnY, 2, pbHole);
                this->AddTrap(fSpawnY, 3, pbHole);
            }
            else
            {
                this->AddBeverage(fSpawnY, 2, pbHole);
                this->AddBeverage(fSpawnY, 3, pbHole);
            }
        }
        else
        {
            const int iFill = MAX(((iAlt-3) % GAME_JUMP_WIDTH) - (GAME_JUMP_WIDTH-3), 0);

            if(iFill)
            {
                for(int i = 0; i < 6; i+=5)
                {
                    this->AddStreet(i, true, 0, 0, pbHole);
                         if((iFill == 1) && (m_aiAlgoStatus[0] == i+1)) this->AddBeverage(fSpawnY, i, pbHole);
                    else if( iFill == 2) this->AddTrap(fSpawnY, i, pbHole);
                }
            }
        }

        break;
    }

    // fast side movement, like shading (old one)
    case STAGE_SHADING: nHasEmptyLines(true); iMaxCount = 62; //6*n+2
    {
        //  ### //
        // ###  //
        //##    //
        //    ##//
        //   ###//
        //  ### //
        // ###  //
        //###   //
        //##    //

        if(m_iAlgoCurCount && m_iAlgoCurCount != iMaxCount-1)
        {
            if(!m_aiAlgoStatus[0]) m_aiAlgoStatus[0] = Core::Rand->Int(1,2);

            const int iCol = ((m_iAlgoCurCount-1) % 6);
            const int iAlt = (m_aiAlgoStatus[0] == 1) ? (5 - iCol) : iCol;

            this->AddBeverage(fSpawnY, iAlt, pbHole);
            this->AddStreet  (iAlt, true, 1, 1, pbHole);
        }

        break;
    }

    // slow side movement (old one)
    case STAGE_SIDE: nHasEmptyLines(true); iMaxCount = 62; //12*n+2
    {
        //    ##//
        //    ##//
        //   ###//
        //   ###//
        //  ### //
        //  ### //
        // ###  //
        // ###  //
        //###   //
        //###   //
        //##    //
        //##    //

        if(m_iAlgoCurCount && m_iAlgoCurCount != iMaxCount-1)
        {
            if(!m_aiAlgoStatus[0]) m_aiAlgoStatus[0] = Core::Rand->Int(1,2);

            const int iCol = (((m_iAlgoCurCount-1) / 2) % 6);
            const int iAlt = (m_aiAlgoStatus[0] == 1) ? (5 - iCol) : iCol;

            this->AddBeverage(fSpawnY, iAlt, pbHole);
            this->AddStreet  (iAlt, true, 1, 1, pbHole);
        }

        break;
    }

    // real traps, but with save-spots
    case STAGE_TRAP: nHasEmptyLines(true); iMaxCount = 65; // 10*n+5
    {
        // #### //
        //   #  //
        // #### //
        // **** //
        // #### //
        //  #   //
        // #### //
        // **** //
        // #### //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            const int iNum = (m_iAlgoCurCount-1) % (GAME_JUMP_WIDTH+4);

            if(!m_aiAlgoStatus[0])
                m_aiAlgoStatus[0] = Core::Rand->Int(2,3);
            else if((iNum == 0) || (iNum == 3) || (iNum == GAME_JUMP_WIDTH+2))
                m_aiAlgoStatus[0] = Core::Rand->Int(1,4);

            if(iNum == 3)
            {
                for(int i = 1; i < 5; ++i)
                    this->AddTrap(fSpawnY, i, pbHole);
            }
            else if((iNum != GAME_JUMP_WIDTH+2) && (iNum != GAME_JUMP_WIDTH+3))
            {
                this->AddBeverage(fSpawnY, m_aiAlgoStatus[0], pbHole);
                this->AddStreet  (1, true, 0, 3, pbHole);
            }
            else this->AddStreet(m_aiAlgoStatus[0], true, 0, 0, pbHole);
        }

        break;
    }

    // double-lines with preventing traps
    case STAGE_PREVENT: nHasEmptyLines(true); iMaxCount = 57; // 10*n+7
    {
        //    **//
        //    ##//
        //    ##//
        //  **##//
        //  ##  //
        //  ##  //
        //**##  //
        //##    //
        //##    //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            const int iAlt = (m_iAlgoCurCount + 3) % 5;

            if(iAlt == 4)
            {
                m_aiAlgoStatus[1] =  m_aiAlgoStatus[0];
                m_aiAlgoStatus[0] = (m_aiAlgoStatus[0] == 2) ? (Core::Rand->Int(1) * 4) : 2;

                if(m_iAlgoCurCount >= 2)
                {
                    this->AddTrap(fSpawnY, m_aiAlgoStatus[1],   pbHole);
                    this->AddTrap(fSpawnY, m_aiAlgoStatus[1]+1, pbHole);
                }
            }
            else if(iAlt != 3)
            {
                this->AddBeverage(fSpawnY, m_aiAlgoStatus[0],   pbHole);
                this->AddBeverage(fSpawnY, m_aiAlgoStatus[0]+1, pbHole);
            }

            this->AddStreet(m_aiAlgoStatus[0], true, 0, 1, pbHole);
        }

        break;
    }

    // only middle-platforms, sometimes jump over traps for bonus
    case STAGE_BONUS: nHasEmptyLines(true); iMaxCount = 63; // 9*n
    {
        //  **  //
        //  ##  //
        //      //
        //  **  //
        //  ##  //
        //      //
        //      //
        //  **  //
        //  ##  //

        const int iAlt = m_iAlgoCurCount % 9;

        if(!m_aiAlgoStatus[1])
        {
            m_aiAlgoStatus[0] = 0;
            m_aiAlgoStatus[1] = Core::Rand->Int(2,3);
        }
        else if(iAlt == 0)
        {
            m_aiAlgoStatus[0] = m_aiAlgoStatus[0] ? (Core::Rand->Int(1) * 3) : 3;
            m_aiAlgoStatus[1] = Core::Rand->Int(2,3);
        }
        else if(iAlt == 4)
        {
            m_aiAlgoStatus[1] = (m_aiAlgoStatus[1] == 2) ? 3 : 2;
        }

        if((iAlt > m_aiAlgoStatus[0]) && (m_iAlgoCurCount < iMaxCount-1))
        {
            this->AddStreet(2, true, 0, 1, pbHole);

            if(iAlt == 8)
            {
                this->AddTrap(fSpawnY, 2, pbHole);
                this->AddTrap(fSpawnY, 3, pbHole);
            }
            else this->AddBeverage(fSpawnY, m_aiAlgoStatus[1], pbHole);
        }

        break;
    }

    // nothing in the middle, platforms on the side
    case STAGE_MIDDLE: nHasEmptyLines(true); iMaxCount = 61; // 6*n+1
    {
        //##  ##//
        //##  ##//
        //##  ##//
        //      //
        //##  ##//
        //##  ##//
        //##  ##//

        if(m_iAlgoCurCount % 6)
        {
            this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] ? 1 : 4, pbHole);
            this->AddStreet  (0, true, 0, 1, pbHole);
            this->AddStreet  (5, true, 1, 0, pbHole);
        }
        else m_aiAlgoStatus[0] = Core::Rand->Int(1);

        break;
    }

    // side-lines with middle-platforms
    case STAGE_LINES: nHasEmptyLines(false); iMaxCount = 62; // 12*n+2
    {
        //     #//
        //#    #//
        //# ## #//
        //# ## #//
        //#    #//
        //#     //

        if(!m_iAlgoCurCount) m_aiAlgoStatus[0] = Core::Rand->Int(1);

        const int iFill = (m_iAlgoCurCount + 10) % 12;

        if(iFill >= 10)
        {
            this->AddBeverage(fSpawnY, 2, pbHole);
            this->AddBeverage(fSpawnY, 3, pbHole);
        }
        else
        {
            if(iFill == 0) m_aiAlgoStatus[0] = 1 - m_aiAlgoStatus[0];
            this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] ? 0 : 5, pbHole);
        }

        if( m_aiAlgoStatus[0] || ((iFill != 4) && (iFill != 5))) this->AddStreet(0, true, 0, 0, pbHole);
        if(!m_aiAlgoStatus[0] || ((iFill != 4) && (iFill != 5))) this->AddStreet(5, true, 0, 0, pbHole);

        break;
    }

    // side-lines with gateway-jumps
    case STAGE_GATEWAY: nHasEmptyLines(false); iMaxCount = 63; // 12*n+3
    {
        //#    #//
        //#   *#//
        //#   ##//
        //#    #//
        //      //
        //#    #//
        //#*   #//
        //##   #//
        //#    #//

        if(!m_iAlgoCurCount) m_aiAlgoStatus[0] = Core::Rand->Int(1);

        const int iFill = (m_iAlgoCurCount + 9) % 12;

        if((m_iAlgoCurCount <= 1) || (m_iAlgoCurCount >= iMaxCount-2))
        {
            this->AddBeverage(fSpawnY, 2, pbHole);
            this->AddBeverage(fSpawnY, 3, pbHole);
        }
        else if(m_iAlgoCurCount == 2)
        {
            this->AddTrap(fSpawnY, 2, pbHole);
            this->AddTrap(fSpawnY, 3, pbHole);
        }
        else if(iFill == 10) this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] ? 1 : 4, pbHole);
        else if(iFill == 11) this->AddTrap    (fSpawnY, m_aiAlgoStatus[0] ? 1 : 4, pbHole);
        else if(iFill ==  0) m_aiAlgoStatus[0] = Core::Rand->Int(1);
        else if(iFill >=  4) this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] ? 0 : 5, pbHole);

        if((iFill != 2) && (iFill != 3))
        {
            this->AddStreet(0, true, 0, 0, pbHole);
            this->AddStreet(5, true, 0, 0, pbHole);
        }

        break;
    }

    // risky fast movement or safe jumping
    case STAGE_CHEESE: nHasEmptyLines(true); iMaxCount = 63; // 2*n+3
    {
        // #### //
        //  # # //
        // #### //
        // # #  //
        // #### //
        //  # # //
        // #### //
        // # #  //
        // #### //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            if((m_iAlgoCurCount) % 4)
            {
                this->AddStreet(1, true, 0, 0, pbHole);
                this->AddStreet(3, true, 0, 0, pbHole);
            }
            else m_aiAlgoStatus[0] = CLAMP(m_aiAlgoStatus[0] + (Core::Rand->Int(1) ? -1 : 1), 2, 4);

            if((m_iAlgoCurCount + 2) % 4)
            {
                this->AddStreet(2, true, 0, 0, pbHole);
                this->AddStreet(4, true, 0, 0, pbHole);
            }
            else m_aiAlgoStatus[0] = CLAMP(m_aiAlgoStatus[0] + (Core::Rand->Int(1) ? -1 : 1), 1, 3);

            if(m_iAlgoCurCount <= 2) m_aiAlgoStatus[0] = 3;
            this->AddBeverage(fSpawnY, m_aiAlgoStatus[0], pbHole);
        }

        break;
    }

    // use broken bridges to jump across
    case STAGE_CANYON: nHasEmptyLines(true); iMaxCount = 60; // 8*n+4
    {
        // #### //
        // #### //
        // #    //
        //      //
        //      //
        //    # //
        // #### //
        // #### //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            const int iFill = (m_iAlgoCurCount-1) % 8;

            if( m_iAlgoCurCount == 1) m_aiAlgoStatus[0] = Core::Rand->Int(1,2);
            if((m_iAlgoCurCount >= 3) && (m_iAlgoCurCount <= iMaxCount-3))
            {
                if((iFill == 1) || (iFill == 6))
                    m_aiAlgoStatus[0] = (m_aiAlgoStatus[0] + Core::Rand->Int((m_aiAlgoStatus[0] == 3) ? 2 : 1,
                                                                             (m_aiAlgoStatus[0] == 0) ? 2 : 3)) % 4;
            }

            if((iFill <= 3) || (iFill >= 6))
            {
                this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] + 1, pbHole);
                if(iFill <= 1) this->AddStreet(1, true, 0, 3, pbHole);
            }
        }

        break;
    }

    // risky double-jump with save side
    case STAGE_DOUBLE: nHasEmptyLines(true); iMaxCount = 60; // 9*n+6
    {
        // #### //
        // #### //
        //      //
        //#    #//
        //######//
        //#    #//
        //      //
        // #### //
        // #### //

        const int iFill = m_iAlgoCurCount % 9;

        if(m_iAlgoCurCount == 1) m_aiAlgoStatus[0] = Core::Rand->Int(1,2);

        if(iFill == 7)
        {
            this->AddBeverage(fSpawnY, 2, pbHole);
            this->AddBeverage(fSpawnY, 3, pbHole);
            this->AddStreet  (0, true, 0, 5, pbHole);

            m_aiAlgoStatus[0] = (m_aiAlgoStatus[0] + Core::Rand->Int(1,3)) % 4;
        }
        else if((iFill >= 1) && (iFill <= 4))
        {
            this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] + 1, pbHole);
            this->AddStreet  (1, true, 0, 3, pbHole);
        }
        else if((iFill == 6) || (iFill == 8))
        {
            this->AddStreet(0, true, 0, 0, pbHole);
            this->AddStreet(5, true, 0, 0, pbHole);
        }

        break;
    }

    // 2x2 rails with alternating status
    case STAGE_RAILS: nHasEmptyLines(true); iMaxCount = 62; // 5*n+2
    {
        //    # //
        //    # //
        // # #  //
        // # #  //
        //  # # //
        //  # # //
        //  #   //
        //  #   //
        // #  # //
        // #  # //

        if((0 < m_iAlgoCurCount) && (m_iAlgoCurCount < iMaxCount-1))
        {
            const int iFill = (m_iAlgoCurCount + 4) % 5;

            if(!iFill)
            {
                if(!m_aiAlgoStatus[0])
                {
                    m_aiAlgoStatus[1] = Core::Rand->Int(1) * 2;
                    m_aiAlgoStatus[2] = 2;
                    m_aiAlgoStatus[3] = m_aiAlgoStatus[1] ? 0 : 1;
                }
                else
                {
                    m_aiAlgoStatus[1] = (m_aiAlgoStatus[1] + Core::Rand->Int(1,2)) % 3;
                    m_aiAlgoStatus[2] = (m_aiAlgoStatus[2] + Core::Rand->Int(1,2)) % 3;
                    m_aiAlgoStatus[3] = Core::Rand->Int(1);
                }

                switch(m_aiAlgoStatus[1])
                {
                default: WARN_IF(true) {}
                case 0: m_aiAlgoStatus[0] = 0x05; break; // 01 01
                case 1: m_aiAlgoStatus[0] = 0x09; break; // 10 01
                case 2: m_aiAlgoStatus[0] = 0x0A; break; // 10 10
                }
            }

            for(int i = 0; i < 4; ++i)
            {
                if((m_aiAlgoStatus[0] & BIT(i)) && ((m_aiAlgoStatus[2]+1) & BIT(i/2)))
                {
                    if((m_aiAlgoStatus[2] < 2) || (m_aiAlgoStatus[3] == i/2))
                        this->AddBeverage(fSpawnY, i+1, pbHole);
                    else this->AddStreet(i+1, true, 0, 0, pbHole);
                }
            }
        }

        break;
    }

    // slalom between traps with wide jumps
    case STAGE_SLALOM: nHasEmptyLines(true); iMaxCount = (8+GAME_JUMP_WIDTH)*4+4; // (8+J)*n+4
    {
        //#*#   //
        //###   //
        //      //
        //   #*#//
        //   ###//
        //   #*#//
        //   ###//
        //   #*#//

        const int iFill = (m_iAlgoCurCount + 7+GAME_JUMP_WIDTH) % (8+GAME_JUMP_WIDTH);

        if(m_iAlgoCurCount >= iMaxCount-3)
        {
            if(m_iAlgoCurCount <= iMaxCount-2)
            {
                this->AddBeverage(fSpawnY, 2, pbHole);
                this->AddBeverage(fSpawnY, 3, pbHole);
            }
        }
        else if(iFill <= 9)
        {
            if(!iFill)
            {
                m_aiAlgoStatus[0] = (m_aiAlgoStatus[0] + Core::Rand->Int(1, (m_iAlgoCurCount <= 1) ? 2 : 3)) % 4;
                m_aiAlgoStatus[1] = Core::Rand->Int(1)*2;
            }

            if(iFill % 3) this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] + 1, pbHole);
            else
            {
                m_aiAlgoStatus[1] = 2 - m_aiAlgoStatus[1];
                this->AddBeverage(fSpawnY, m_aiAlgoStatus[0] + m_aiAlgoStatus[1], pbHole);

                if(iFill) this->AddTrap(fSpawnY, m_aiAlgoStatus[0] + 1, pbHole);
            }

            this->AddStreet(m_aiAlgoStatus[0], true, 0, 2, pbHole);
        }

        break;
    }

#if defined(_CORE_DEBUG_)

    // 
    case -1: //nHasEmptyLines(true); iMaxCount = 2000; // 
    {
        ////
        ////
        ////
        ////
        ////
        ////

        break;
    }

#endif

    default:
        ASSERT(false)
        break;
    }

    // increment line number and switch algorithm
    if(++m_iAlgoCurCount >= iMaxCount)
    {
        if(++m_iAlgoCurIndex >= m_aiAlgo.size())
        {
            m_iAlgoCurIndex = 0;

            // further increase speed (and difficulty) with shock-wave
            g_fTargetSpeed += GAME_SPEED_FAST * 0.15f;
            g_fCurSpeed     = g_fTargetSpeed  + 0.2f;
            m_Rock.CreateShockWave(0);

            // create speed-up message
            g_pCombatText->AddTextTransformed("+BOOST", m_Rock.GetPosition(), coreVector4(COLOR_RED_F, 1.0f));

            // shuffle everything again (don't let the last stage come twice, put at least two others in-between)
            auto oEngine = std::default_random_engine(int(std::time(NULL)));
            std::shuffle(    m_aiAlgo.begin(), --m_aiAlgo.end(), oEngine);
            std::shuffle(++++m_aiAlgo.begin(),   m_aiAlgo.end(), oEngine);

            // achieve speed-trophy
            if(!this->GetStatus()) this->AchieveTrophy(GJ_TROPHY_13, 12);
        }

        // reset count and status
        m_iAlgoCurCount = 0;
        std::memset(m_aiAlgoStatus, 0, sizeof(m_aiAlgoStatus));
    }
}