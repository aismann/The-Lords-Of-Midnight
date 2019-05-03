//
//  mapmodel.h
//  midnight
//
//  Created by Chris Wild on 03/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "panelmodel.h"

#include "../tme_interface.h"


enum class map_filters : u32 {
    show_critters    = MXBIT(0),
    centre_char      = MXBIT(1),
    overview_map     = MXBIT(2),
//#if defined(_DDR_)
    show_tunnels     = MXBIT(3),
//#endif
    all              = show_critters
                        | centre_char
                        | overview_map
                        | show_tunnels
};


class mapmodel : public panelmodel
{
public:
    void serialize( u32 version, lib::archive& ar ) override;
    virtual void setDefaults() override;
    
public:

    // game specific
    flags<map_filters> filters;
    point   oldoffset;
    f32     mapscale;
    f32     lastmapscale;
    
};
