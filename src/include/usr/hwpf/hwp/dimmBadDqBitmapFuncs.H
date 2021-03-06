/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/hwpf/hwp/dimmBadDqBitmapFuncs.H $             */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2012,2014              */
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
// $Id: dimmBadDqBitmapFuncs.H,v 1.2 2013/08/13 20:30:25 mjjones Exp $
/**
 *  @file dimmBadDqBitmapFuncs.H
 *
 *  @brief FW Team Utility functions that accesses the Bad DQ Bitmap.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     02/17/2012  Created.
 */

#ifndef DIMMBADDQBITMAPFUNCS_H_
#define DIMMBADDQBITMAPFUNCS_H_

#include <fapi.H>
#include <dimmConsts.H>

extern "C"
{

/**
 * @brief FW Team Utility function that gets the Bad DQ Bitmap.
 *
 * This utility functon should be called by a HWP needing to get the Bad DQ
 * bitmap for a particular mba, port, dimm and rank.
 *
 * This function finds the corresponding DIMM Target, calls
 * dimmBadDqBitmapAccessHwp to get the DQ bitmap and returns the data
 * for the specified rank.
 *
 * @param[in]  i_mba  Reference to MBA Chiplet
 * @param[in]  i_port MBA port number (0-(DIMM_DQ_MAX_MBA_PORTS - 1))
 * @param[in]  i_dimm MBA port DIMM number (0-(DIMM_DQ_MAX_MBAPORT_DIMMS - 1))
 * @param[in]  i_rank DIMM rank number (0-(DIMM_DQ_MAX_DIMM_RANKS -1))
 * @param[out] o_data Reference to data where Bad DQ bitmap is copied to
 *
 * @return ReturnCode
 */
fapi::ReturnCode dimmGetBadDqBitmap(const fapi::Target & i_mba,
                                    const uint8_t i_port,
                                    const uint8_t i_dimm,
                                    const uint8_t i_rank,
                                    uint8_t (&o_data)[DIMM_DQ_RANK_BITMAP_SIZE]);

/**
 * @brief FW Team Utility function that sets the Bad DQ Bitmap.
 *
 * This utility functon should be called by a HWP needing to set the Bad DQ
 * bitmap for a particular mba, port, dimm and rank.
 *
 * This utility function finds the corresponding DIMM Target, calls
 * dimmBadDqBitmapAccessHwp to get the DQ bitmap, fills in the data for the
 * specified rank and calls dimmBadDqBitmapAccessHwp to set the DQ bitmap
 *
 * @param[in] i_mba  Reference to MBA Chiplet
 * @param[in] i_port MBA port number (0-(DIMM_DQ_MAX_MBA_PORTS - 1))
 * @param[in] i_dimm MBA port DIMM number (0-(DIMM_DQ_MAX_MBAPORT_DIMMS - 1))
 * @param[in] i_rank DIMM rank number (0-(DIMM_DQ_MAX_DIMM_RANKS -1))
 * @param[in] i_data Reference to data where Bad DQ bitmap is copied from
 *
 * @return ReturnCode
 */
fapi::ReturnCode dimmSetBadDqBitmap(const fapi::Target & i_mba,
                                    const uint8_t i_port,
                                    const uint8_t i_dimm,
                                    const uint8_t i_rank,
                                    const uint8_t (&i_data)[DIMM_DQ_RANK_BITMAP_SIZE]);
}

#endif
