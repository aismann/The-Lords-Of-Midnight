//
//  savegamemapper_tests.cpp
//  revenge
//
//  Created by Chris Wild on 01/09/2023.
//
#include "catch2/catch.hpp"
#include "../../src/tme/baseinc/tme_internal.h"
#include "../../src/tme/utils/savegamemapping.h"

using oldflags = tme::utils::OLD_DDR_CHARACTERFLAGS ;
using newflags = tme::flags::CHARACTERFLAGS ;

TEST_CASE("DDR Flags are remapped")
{
    flags32 old_flags(
        oldflags::cf_resting |
        oldflags::cf_tunnel |
        oldflags::cf_approaching
    );
    
    flags32 expected_flags(
        newflags::cf_resting |
        newflags::cf_tunnel |
        newflags::cf_approaching
    );

    flags32 new_flags = tme::utils::UpdateDDRCharacterFlags(old_flags);
    
    REQUIRE (new_flags == expected_flags);
}

TEST_CASE("DDR All Flags are remapped")
{
    flags32 old_flags(
        oldflags::cf_all
    );
    
    flags32 expected_flags(
        newflags::cf_resting |
        newflags::cf_inbattle |
        newflags::cf_wonbattle |
        newflags::cf_tunnel |
        newflags::cf_usedobject |
        newflags::cf_followers |
        newflags::cf_preparesbattle |
        newflags::cf_approaching
     );
    
    flags32 new_flags = tme::utils::UpdateDDRCharacterFlags(old_flags);
    
    REQUIRE (new_flags == expected_flags);
}


TEST_CASE("Other Flags are not moved")
{
    flags32 old_flags(
        MXBIT(0) |
        MXBIT(13) |
        oldflags::cf_tunnel |
        MXBIT(24) |
        MXBIT(31)
    );
    
    flags32 expected_flags(
        MXBIT(0) |
        MXBIT(13) |
        newflags::cf_tunnel |
        MXBIT(24) |
        MXBIT(31)
    );
    
    flags32 new_flags = tme::utils::UpdateDDRCharacterFlags(old_flags);
    
    REQUIRE (new_flags == expected_flags);
}
