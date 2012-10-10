/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/runtime/test/runtimeattrstest.H $                     */
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
#ifndef __TEST_RUNTIMETEST_H
#define __TEST_RUNTIMETEST_H

/**
 *  @file runtimetest.H
 *
 */

#include <cxxtest/TestSuite.H>

#include <fapi.H>
#include <fapiAttributeIds.H>
#include <targeting/common/target.H>
#include <targeting/common/targetservice.H>
#include <targeting/common/utilFilter.H>
#include <arch/ppc.H> //for MAGIC
#include <attributeenums.H>
#include <errl/errlmanager.H>
#include <hwpf/plat/fapiPlatHwpInvoker.H>
#include "../common/hsvc_attribute_structs.H"

extern trace_desc_t* g_trac_runtime;

using namespace fapi;

class RuntimeTest: public CxxTest::TestSuite
{
  public:
    void testVerifySystemAttributes(void)
    {
        TRACFCOMP( g_trac_runtime, "testVerifySystemAttributes> start" );
        hsvc_system_data_t* sysdata = getSysPtr();

        //Look for a valid node number
        if( sysdata->nodePresent != 0x8000000000000000 )
        {
            TRACFCOMP( g_trac_runtime, "nodePresent=%.16X", sysdata->nodePresent );
            TS_FAIL("testVerifySystemAttributes> nodePresent is invalid");
        }

        //Make sure we have some attributes out there
        if( sysdata->numAttr == 0 )
        {
            TS_FAIL("testVerifySystemAttributes> numAttr is zero");
        }

        //Make sure that the offset is what we expect
        if( sysdata->offset != 24 ) //3 words from the start
        {
            TRACFCOMP( g_trac_runtime, "offset=%.16X", sysdata->offset );
            TS_FAIL("testVerifySystemAttributes> offset is invalid");
        }

        char* beginning = reinterpret_cast<char*>(sysdata);
        hsvc_attr_header_t* headers = reinterpret_cast<hsvc_attr_header_t*>
          ((beginning+sysdata->offset));

        //Grab a couple of arbitrary attributes        
        ReturnCode l_rc;

        fapi::ATTR_FREQ_PB_Type freq = 0;
        l_rc = FAPI_ATTR_GET(ATTR_FREQ_PB,NULL,freq);
        if( l_rc )
        {
            TS_FAIL("Error getting fapi::ATTR_FREQ_PB");
        }

        fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE_Type crc_gen_enable = 0;
        l_rc = FAPI_ATTR_GET(ATTR_PM_SPIVID_CRC_GEN_ENABLE,NULL,crc_gen_enable);
        if( l_rc )
        {
            TS_FAIL("Error getting fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE");
        }

        uint64_t attr = 0;
        while( headers[attr].id != hsvc_attr_header_t::NO_ATTRIBUTE )
        {
            if( headers[attr].id == fapi::ATTR_FREQ_PB )
            {
                if( headers[attr].sizeBytes !=
                    sizeof(fapi::ATTR_FREQ_PB_Type) )
                {
                    TRACFCOMP( g_trac_runtime, "size=%.16X", headers[attr].sizeBytes );
                    TS_FAIL("Size of fapi::ATTR_FREQ_PB data is wrong");
                }
                else
                {
                    fapi::ATTR_FREQ_PB_Type* freq_act =
                      reinterpret_cast<fapi::ATTR_FREQ_PB_Type*>
                      (beginning+headers[attr].offset);
                    if( *freq_act != freq )
                    {
                        TRACFCOMP( g_trac_runtime, "Expected=%X, Actual=%X", freq, *freq_act );
                        TS_FAIL("fapi::ATTR_FREQ_PB data is wrong");
                    }
                }
            }
            else if( headers[attr].id == fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE )
            {
                if( headers[attr].sizeBytes !=
                    sizeof(fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE_Type) )
                {
                    TRACFCOMP( g_trac_runtime, "size=%.16X", headers[attr].sizeBytes );
                    TS_FAIL("Size of fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE data is wrong");
                }
                else
                {
                    fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE_Type* crc_gen_enable_act =
                      reinterpret_cast<fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE_Type*>
                      (beginning+headers[attr].offset);
                    if( *crc_gen_enable_act != crc_gen_enable )
                    {
                        TRACFCOMP( g_trac_runtime, "Expected=%X, Actual=%X", crc_gen_enable, *crc_gen_enable_act );
                        TS_FAIL("fapi::ATTR_PM_SPIVID_CRC_GEN_ENABLE data is wrong");
                    }
                }
            }
            attr++;
        }

        if( attr != sysdata->numAttr )
        {
            TRACFCOMP( g_trac_runtime, "numAttr=%d, found attr=%d", sysdata->numAttr, attr );
            TS_FAIL("Number of attributes found doesn't match expected value");
        }

        TRACFCOMP( g_trac_runtime, "testVerifySystemAttributes> finish" );
    }

    void testVerifyNodeAttributes(void)
    {
        TRACFCOMP( g_trac_runtime, "testVerifyNodeAttributes> start" );
        hsvc_node_data_t* nodedata = getNodePtr();

        //Make sure we have some targets out there (at least 1 proc and 1 ex)
        if( nodedata->numTargets < 2 )
        {
            TS_FAIL("testVerifyNodeAttributes> numTargets is zero");
        }

        //Make sure that the offsets are reasonable
        if( nodedata->procOffset == 0 ) 
        {
            TS_FAIL("testVerifyNodeAttributes> procOffset is invalid");
        }
        if( nodedata->exOffset == 0 ) 
        {
            TS_FAIL("testVerifyNodeAttributes> exOffset is invalid");
        }
        if( nodedata->procOffset == nodedata->exOffset ) 
        {
            TS_FAIL("testVerifyNodeAttributes> offsets are equal - that is bad");
        }

        char* beginning = reinterpret_cast<char*>(nodedata);

        //Look at the procs
        hsvc_proc_header_t* procs = reinterpret_cast<hsvc_proc_header_t*>
          ((beginning+nodedata->procOffset));

        //Verify the header data is okay
        hsvc_proc_header_t* curproc = procs;
        TRACDCOMP( g_trac_runtime, "procs=%p", procs );
        while( curproc->procid != hsvc_proc_header_t::NO_PROC )
        {
            //Verify we have some attributes
            if( curproc->numAttr == 0 )
            {
                TRACFCOMP(g_trac_runtime,"Proc %d has zero attributes",curproc->procid);
                TS_FAIL("testVerifyNodeAttributes> no attributes for proc");
            }

            //Find a target to match this one
            fapi::Target* proc = getProcTarget( curproc->procid );
            TRACDCOMP( g_trac_runtime, "procid = %d", curproc->procid );

            //Grab a couple of arbitrary attributes        
            hsvc_attr_header_t* headers = reinterpret_cast<hsvc_attr_header_t*>
              ((beginning+curproc->offset));
            ReturnCode l_rc;

            fapi::ATTR_CHIP_ID_Type tmp1 = 0;
            l_rc = FAPI_ATTR_GET(ATTR_CHIP_ID,proc,tmp1);
            if( l_rc )
            {
                TS_FAIL("Error getting fapi::ATTR_CHIP_ID");
            }

            fapi::ATTR_PM_PVSAFE_PSTATE_Type tmp2 = 0;
            l_rc = FAPI_ATTR_GET(ATTR_PM_PVSAFE_PSTATE,proc,tmp2);
            if( l_rc )
            {
                TS_FAIL("Error getting fapi::ATTR_PM_PVSAFE_PSTATE");
            }

            uint64_t attr = 0;
            while( headers[attr].id != hsvc_attr_header_t::NO_ATTRIBUTE )
            {
                if( headers[attr].id == fapi::ATTR_CHIP_ID )
                {
                    if( headers[attr].sizeBytes !=
                        sizeof(fapi::ATTR_CHIP_ID_Type) )
                    {
                        TRACFCOMP( g_trac_runtime, "size=%.16X", headers[attr].sizeBytes );
                        TS_FAIL("Size of fapi::ATTR_CHIP_ID data is wrong");
                    }
                    else
                    {
                        fapi::ATTR_CHIP_ID_Type* tmp1_act =
                          reinterpret_cast<fapi::ATTR_CHIP_ID_Type*>
                          (beginning+headers[attr].offset);
                        if( *tmp1_act != tmp1 )
                        {
                            TRACFCOMP( g_trac_runtime, "Expected=%X, Actual=%X", tmp1, *tmp1_act );
                            TS_FAIL("fapi::ATTR_CHIP_ID data is wrong");
                        }
                    }
                }
                else if( headers[attr].id == fapi::ATTR_PM_PVSAFE_PSTATE )
                {
                    if( headers[attr].sizeBytes !=
                        sizeof(fapi::ATTR_PM_PVSAFE_PSTATE_Type) )
                    {
                        TRACFCOMP( g_trac_runtime, "size=%.16X", headers[attr].sizeBytes );
                        TS_FAIL("Size of fapi::ATTR_PM_PVSAFE_PSTATE data is wrong");
                    }
                    else
                    {
                        fapi::ATTR_PM_PVSAFE_PSTATE_Type* tmp2_act =
                          reinterpret_cast<fapi::ATTR_PM_PVSAFE_PSTATE_Type*>
                          (beginning+headers[attr].offset);
                        if( *tmp2_act != tmp2 )
                        {
                            TRACFCOMP( g_trac_runtime, "Expected=%X, Actual=%X", tmp2, *tmp2_act );
                            TS_FAIL("fapi::ATTR_PM_PVSAFE_PSTATE data is wrong");
                        }
                    }
                }
                attr++;
            }
            if( attr != curproc->numAttr )
            {
                TRACFCOMP( g_trac_runtime, "numAttr=%d, found attr=%d", curproc->numAttr, attr );
                TS_FAIL("Number of attributes found doesn't match expected value");
            }

            delete proc;
            curproc++;
            TRACFCOMP( g_trac_runtime, "curproc=%p", curproc );
        }


        //Look at the EXs
        hsvc_ex_header_t* exs = reinterpret_cast<hsvc_ex_header_t*>
          ((beginning+nodedata->exOffset));

        //Verify the header data is okay
        hsvc_ex_header_t* curex = exs;
        while( curex->parent_procid != hsvc_proc_header_t::NO_PROC )
        {
            //Verify we have some attributes
            if( curex->numAttr == 0 )
            {
                TRACFCOMP(g_trac_runtime,"Proc %d has zero attributes",curproc->procid);
                TS_FAIL("testVerifyNodeAttributes> no attributes for proc");
            }

            //Find a target to match this one
            fapi::Target* ex = getExTarget( curex->parent_procid,
                                            curex->chiplet );
            TRACDCOMP( g_trac_runtime, "procid = %d, chiplet = %d", curex->parent_procid, curex->chiplet );

            //Grab a couple of arbitrary attributes        
            hsvc_attr_header_t* headers = reinterpret_cast<hsvc_attr_header_t*>
              ((beginning+curex->offset));
            ReturnCode l_rc;

            fapi::ATTR_CHIP_UNIT_POS_Type tmp1 = 0;
            l_rc = FAPI_ATTR_GET(ATTR_CHIP_UNIT_POS,ex,tmp1);
            if( l_rc )
            {
                TS_FAIL("Error getting fapi::ATTR_CHIP_UNIT_POS");
                errlHndl_t errhdl = fapiRcToErrl(l_rc);
                errlCommit(errhdl,CXXTEST_COMP_ID);
            }

            fapi::ATTR_FUNCTIONAL_Type tmp2 = 0;
            l_rc = FAPI_ATTR_GET(ATTR_FUNCTIONAL,ex,tmp2);
            if( l_rc )
            {
                TS_FAIL("Error getting fapi::ATTR_FUNCTIONAL");
                errlHndl_t errhdl = fapiRcToErrl(l_rc);
                errlCommit(errhdl,CXXTEST_COMP_ID);
            }

            uint64_t attr = 0;
            while( headers[attr].id != hsvc_attr_header_t::NO_ATTRIBUTE )
            {
                if( headers[attr].id == fapi::ATTR_CHIP_UNIT_POS )
                {
                    if( headers[attr].sizeBytes !=
                        sizeof(fapi::ATTR_CHIP_UNIT_POS_Type) )
                    {
                        TRACFCOMP( g_trac_runtime, "size=%.16X", headers[attr].sizeBytes );
                        TS_FAIL("Size of fapi::ATTR_CHIP_UNIT_POS data is wrong");
                    }
                    else
                    {
                        fapi::ATTR_CHIP_UNIT_POS_Type* tmp1_act =
                          reinterpret_cast<fapi::ATTR_CHIP_UNIT_POS_Type*>
                          (beginning+headers[attr].offset);
                        if( *tmp1_act != tmp1 )
                        {
                            TRACFCOMP( g_trac_runtime, "Expected=%X, Actual=%X", tmp1, *tmp1_act );
                            TS_FAIL("fapi::ATTR_CHIP_UNIT_POS data is wrong");
                        }
                    }
                }
                else if( headers[attr].id == fapi::ATTR_FUNCTIONAL )
                {
                    if( headers[attr].sizeBytes !=
                        sizeof(fapi::ATTR_FUNCTIONAL_Type) )
                    {
                        TRACFCOMP( g_trac_runtime, "size=%.16X", headers[attr].sizeBytes );
                        TS_FAIL("Size of fapi::ATTR_FUNCTIONAL data is wrong");
                    }
                    else
                    {
                        fapi::ATTR_FUNCTIONAL_Type* tmp2_act =
                          reinterpret_cast<fapi::ATTR_FUNCTIONAL_Type*>
                          (beginning+headers[attr].offset);
                        if( *tmp2_act != tmp2 )
                        {
                            TRACFCOMP( g_trac_runtime, "Expected=%X, Actual=%X", tmp2, *tmp2_act );
                            TS_FAIL("fapi::ATTR_FUNCTIONAL data is wrong");
                        }
                    }
                }
                attr++;
            }
            if( attr != curex->numAttr )
            {
                TRACFCOMP( g_trac_runtime, "numAttr=%d, found attr=%d", curex->numAttr, attr );
                TS_FAIL("Number of attributes found doesn't match expected value");
            }

            delete ex;
            curex++;
        }

        TRACFCOMP( g_trac_runtime, "testVerifyNodeAttributes> finish" );
    }

  private:
    //@fixme  RTC:49509
    hsvc_system_data_t* getSysPtr(void)
    {
        return( (hsvc_system_data_t*)(5*MEGABYTE) );
    };
    hsvc_node_data_t* getNodePtr(void)
    {
        return( (hsvc_node_data_t*)(5*MEGABYTE+512*KILOBYTE) );
    };

    //utility to fetch a proc target based on a procid
    TARGETING::Target* _getProcTarget( uint64_t i_procid )
    {
        // Grab a system object to work with
        TARGETING::Target* sys = NULL;
        TARGETING::targetService().getTopLevelTarget(sys);

        // Loop through all of the procs
        TARGETING::TargetHandleList all_procs;
        TARGETING::getAllChips( all_procs, TARGETING::TYPE_PROC, false );
        if( all_procs.empty() )
        {
            TRACDCOMP( g_trac_runtime, "all_procs is empty!!, type=%d", TARGETING::TYPE_PROC );

            TARGETING::PredicateCTM predProc(TARGETING::CLASS_CHIP, TARGETING::TYPE_PROC);

            TARGETING::PredicatePostfixExpr checkExpr;
            checkExpr.push(&predProc);
            TARGETING::targetService().getAssociated( all_procs, sys,
                                     TARGETING::TargetService::CHILD,
                            TARGETING::TargetService::ALL, &checkExpr );
        }

        TARGETING::Target* proc_target = NULL;
        for( size_t p = 0; p < all_procs.size(); p++ )
        {
            uint64_t node_id = 
              all_procs[p]->getAttr<TARGETING::ATTR_FABRIC_NODE_ID>();
            uint64_t CHIP_UNIT_POS =
              all_procs[p]->getAttr<TARGETING::ATTR_FABRIC_CHIP_ID>();
            uint32_t procid = (node_id << 3) | (CHIP_UNIT_POS); //NNNCCC
            if( procid == i_procid )
            {
                proc_target = all_procs[p];
                break;
            }
        }

        return proc_target;
    }

    //utility to fetch a proc target based on a procid
    fapi::Target* getProcTarget( uint64_t i_procid )
    {
        TARGETING::Target* proc = _getProcTarget(i_procid);
        if( proc != NULL )
        {
            return( new fapi::Target( fapi::TARGET_TYPE_PROC_CHIP,
                          reinterpret_cast<void *>
                          (const_cast<TARGETING::Target*>(proc)) ) );
        }
        return NULL;
    };

    //utility to fetch an ex target based on a procid and chiplet number
    fapi::Target* getExTarget( uint64_t i_procid, uint64_t i_chiplet )
    {
        TARGETING::Target* proc = _getProcTarget(i_procid);
        TARGETING::TargetHandleList all_ex;
        TARGETING::getChildChiplets( all_ex, proc,
                                     TARGETING::TYPE_EX, false );
        for( size_t e = 0; e < all_ex.size(); e++ )
        {
            uint32_t chiplet = all_ex[e]->getAttr<TARGETING::ATTR_CHIP_UNIT>();
            if( chiplet == i_chiplet )
            {
                return( new fapi::Target( fapi::TARGET_TYPE_PROC_CHIP,
                                          reinterpret_cast<void *>
                                          (const_cast<TARGETING::Target*>(all_ex[e])) ) );
            }
        }
        return NULL;
    }
};


#endif
