/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/common/plat/pegasus/prdfCenConst.H $        */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2004,2013              */
/*                                                                        */
/* p1                                                                     */
/*                                                                        */
/* Object Code Only (OCO) source materials                                */
/* Licensed Internal Code Source Materials                                */
/* IBM HostBoot Licensed Internal Code                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* Origin: 30                                                             */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

#ifndef PRDF_CEN_MBA_CONST_H
#define PRDF_CEN_MBA_CONST_H

/**
 *   @file prdfCenConst.H
 *   @brief Defines Centaur related constants
 */
//------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------

namespace PRDF
{

enum
{
    PORT_SLCT_PER_MBA = 2,
    MAX_DIMM_SLCT_PER_MBA = 2,
    MAX_RANKS_PER_DIMM_SLCT = 4,
    MAX_RANKS_PER_MBA = MAX_DIMM_SLCT_PER_MBA * MAX_RANKS_PER_DIMM_SLCT,

    SYMBOLS_PER_RANK = 72,
    DQS_PER_DIMM     = 72,
};

} // end namespace PRDF

#endif // PRDF_CEN_MBA_CAPTURE_DATA_H
