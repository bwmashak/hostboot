/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/hwpf/hwp/mc_config/mss_eff_config/opt_memmap.C $      */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2012                   */
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
//------------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2012
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
//------------------------------------------------------------------------------
// *! TITLE       : opt_memmap.C
// *! DESCRIPTION : see additional comments below
//------------------------------------------------------------------------------
// Don't forget to create CVS comments when you check in your changes!
//------------------------------------------------------------------------------
//Owner :- Girisankar paulraj
//Back-up owner :- Mark bellows
//
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version:|  Author: |  Date:  | Comment:
//---------|----------|---------|-----------------------------------------------
//  1.1    | vanlee   | 12/01/12| First drop
//------------------------------------------------------------------------------
// Design flow
//
// opt_memmap() is run in between two mss_eff_grouping() calls.
//
// 1) Before First mss_eff_grouping() call
//    - Each proc's ATTR_PROC_MEM_BASE attribute is set to 0
//    - Each proc's ATTR_PROC_MIRROR_BASE attribute is set to 512TB
// 2) First mss_eff_grouping() call
//    - The HWP updates each proc's ATTR_PROC_MEM_BASES and ATTR_PROC_MEM_SIZES
//      attributes based on installed memory behind each proc
// 3) Call opt_memmap()
//    - Get "effective stackable" size (EffSize) of each proc. Due to (1), 
//        (a) EffSize = highest ATTR_PROC_MEM_BASES +
//                      its corresponding ATTR_PROC_MEM_SIZES
//        (b) Round up EffSize to a power of 2
//        (c) Save (proc,EffSize) pair to a vector
//        Repeat (a) - (c) for all procs
//    - Sort all (proc,EffSize) pairs of the above vector based on EffSize from
//      smallest to largest
//    - Set the ATTR_PROC_MEM_BASE and ATTR_PROC_MIRROR_BASE attributes for
//      each proc:
//        cur_mem_base = 0
//        cur_mirror_base = 512TB
//        Starting from the last (proc,EffSize) pair of the vector
//        while this (proc,EffSize) pair is not yet processed
//        Begin
//          set this proc ATTR_PROC_MEM_BASE = cur_mem_base
//          set this proc ATTR_PROC_MIRROR_BASE = cur_mirrow_base
//          cur_mem_base += "This proc EffSize"
//          cur_mirrow_base += "This proc EffSize" / 2
//          move backward to preceeding (proc,EffSize) pair
//        End
// 4) Second mss_eff_grouping() call
//    - The HWP adjusts each proc's ATTR_PROC_MEM_BASES using the updated
//      ATTR_PROC_MEM_BASE value from (3).
//------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------

#include <opt_memmap.H>

extern "C" {

    using namespace fapi;

    class MemRegion
    {
    public:
        uint64_t iv_base;
        uint64_t iv_size;
        bool operator<(MemRegion rhs)
        { 
            bool l_lt = true;
            if (iv_base > rhs.iv_base || 
                (iv_base == rhs.iv_base && iv_size != 0))
            {
                l_lt = false;
            }
            return l_lt;
        }
        MemRegion(uint64_t b, uint64_t s) : iv_base(b), iv_size(s) {}
    };

    class ProcBase
    {
    public:
        fapi::Target *iv_tgt;
        uint64_t iv_size;
        bool operator<(ProcBase rhs) { return iv_size < rhs.iv_size; }
        ProcBase(fapi::Target* t, uint64_t s) : iv_tgt(t), iv_size(s) {}
    };

    inline uint64_t PowerOf2Roundedup( uint64_t i_number )
    {
        if (i_number)
        {
            uint64_t leading0s = 0;
            asm volatile("cntlzd %0, %1" : "=r"(leading0s) : "r"(i_number));
            uint64_t mask = ( 1ULL << (63 - leading0s) );
            i_number = mask << ((mask ^ i_number) ? 1 : 0);
        }
        return i_number;
    }
        
    ReturnCode opt_memmap(std::vector<fapi::Target> & i_procs)
    {
        ReturnCode rc;
        std::vector<ProcBase> l_procBases;
        const size_t l_MCS_per_proc = 8;
        uint64_t l_bases[l_MCS_per_proc];
        uint64_t l_sizes[l_MCS_per_proc];
        uint32_t l_pos = 0;

        for (std::vector<fapi::Target>::iterator l_iter = i_procs.begin();
             l_iter != i_procs.end(); ++l_iter)
        {
            rc = FAPI_ATTR_GET(ATTR_POS, &(*l_iter), l_pos);
            if (rc)
            {
                FAPI_ERR("Error reading ATTR_POS");
                break;
            }
            else
            {
                FAPI_INF("Proc %d :", l_pos);
            }

            // retrieve bases and sizes
            rc = FAPI_ATTR_GET(ATTR_PROC_MEM_BASES, &(*l_iter), l_bases);
            if (rc)
            {
                FAPI_ERR("Error reading ATTR_PROC_MEM_BASES");
                break;
            }
            else
            {
                for(size_t i = 0; i < l_MCS_per_proc; i++)
                {
                    FAPI_INF("  l_bases[%d] = %016X", i, l_bases[i]);
                }
            }

            rc = FAPI_ATTR_GET(ATTR_PROC_MEM_SIZES, &(*l_iter), l_sizes);
            if (rc)
            {
                FAPI_ERR("Error reading ATTR_PROC_MEM_SIZES");
                break;
            }
            else
            {
                for(size_t i = 0; i < l_MCS_per_proc; i++)
                {
                    FAPI_INF("  l_sizes[%d] = %016X", i, l_sizes[i]);
                }
            }

            // create the l_regions vector and sort it
            std::vector<MemRegion> l_regions;
            for (size_t i = 0; i < l_MCS_per_proc; i++)
            {
                MemRegion l_region(l_bases[i], l_sizes[i]);
                l_regions.push_back(l_region);
            }

            // compute effective size and round up to power of 2
            std::sort( l_regions.begin(), l_regions.end() );
            uint64_t round_size = l_regions[l_regions.size()-1].iv_base;
            round_size += l_regions[l_regions.size()-1].iv_size;
            round_size = PowerOf2Roundedup( round_size );

            FAPI_INF("  round_size = %016X", round_size);

            // save the proc's target and effective size
            ProcBase l_procBase(&(*l_iter), round_size);
            l_procBases.push_back(l_procBase);
        }

        while (rc.ok())
        {
            std::sort(l_procBases.begin(), l_procBases.end());
            uint64_t cur_mem_base = 0;
            uint64_t cur_mir_base = 0x0002000000000000;  // 512TB

            for (size_t i = l_procBases.size(); i != 0; --i)
            {
                fapi::Target * l_tgt = l_procBases[i-1].iv_tgt;
                uint64_t size = l_procBases[i-1].iv_size;

                uint32_t l_pos = 0;
                rc = FAPI_ATTR_GET(ATTR_POS, l_tgt, l_pos);
                if (rc.ok())
                {
                    FAPI_INF("proc%d MEM_BASE = %016X", l_pos, cur_mem_base);
                    FAPI_INF("proc%d MIRROR_BASE = %016X", l_pos, cur_mir_base);
                }

                rc = FAPI_ATTR_SET(ATTR_PROC_MEM_BASE, l_tgt, cur_mem_base);
                if (rc)
                {
                    FAPI_ERR("Error reading ATTR_PROC_MEM_BASE");
                    break;
                }

                rc = FAPI_ATTR_SET(ATTR_PROC_MIRROR_BASE, l_tgt, cur_mir_base);
                if (rc)
                {
                    FAPI_ERR("Error reading ATTR_PROC_MIRROR_BASE");
                    break;
                }
                cur_mem_base += size;
                cur_mir_base += size / 2;
            }

            break;

        }

        return rc;
    }

} //end extern C