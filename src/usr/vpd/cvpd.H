/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/vpd/cvpd.H $                                          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2013,2014              */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#ifndef __CVPD_H
#define __CVPD_H


/**
 * @file cvpd.H
 *
 * @brief Provides the interfaces for the CVPD device driver
 *
 */

// ----------------------------------------------
// Includes
// ----------------------------------------------
#include <errl/errlentry.H>
#include <vpd/cvpdenums.H>

#include "ipvpd.H"

namespace CVPD
{

    enum
    {
        SECTION_SIZE       = 0x1000,
        MAX_SECTIONS       = 64,
    };


    /**
     * @brief Conversion of CVPD Records to corresponding character
     *        representation.
     */
    const IpVpdFacade::recordInfo cvpdRecords[] =
    {
        // -------------------------------------------------------------------
        // NOTE: This list must remain an ordered list!  There will be a
        //      testcase that checks this.  When adding new entries to the
        //      list, be sure that the keyword in each entry (value 0)
        //      are in ascending order.
        // -------------------------------------------------------------------
        { VEIR, "VEIR" },
        { VER0, "VER0" },  //Not currently used, but keep support
        { MER0, "MER0" },
        { VSPD, "VSPD" },
        { VINI, "VINI" },
        // -------------------------------------------------------------------
        // DO NOT USE!!  This is for test purposes ONLY!
        { CVPD_TEST_RECORD, "TEST" },
        // -------------------------------------------------------------------
    };

    /**
     * @brief Conversion of CVPD Keywords to corresponding character
     *        representation.
     */
    const IpVpdFacade::keywordInfo cvpdKeywords[] =
    {
        // -------------------------------------------------------------------
        // NOTE: This list must remain an ordered list!  There will be a
        //      testcase that checks this.  When adding new entries to
        //      the list, be sure that the keyword in each entry (value 0)
        //      are in ascending order.
        // -------------------------------------------------------------------
        { pdI, "#I" },
        { PF,  "PF" },
        { MT,  "MT" },
        { MR,  "MR" },
        { pdA, "#A" },
        { EL,  "EL" },
        { LM,  "LM" },
        { MW,  "MW" },
        { MV,  "MV" },
        { AM,  "AM" },
        { VZ,  "VZ" },
        { pdD, "#D" },
        { MX,  "MX" },
        // -------------------------------------------------------------------
        // DO NOT USE!!  This is for test purposes ONLY!
        { CVPD_TEST_KEYWORD, "XX" },
        // -------------------------------------------------------------------
    };

}; // end CVPD namespace

class CvpdFacade: public IpVpdFacade
{
  public:

    /**
     * @brief Constructor
     */
    CvpdFacade( );

};
#endif  // __CVPD_H
