//
//  LandscapeView.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeView_hpp
#define LandscapeView_hpp

#include <stdio.h>

#include "LandscapeGenerator.h"
#include "LandscapeNode.h"
#include "LandscapeSky.h"
#include "LandscapeLand.h"
#include "LandscapeTerrain.h"
#include "LandscapeDebug.h"
#include "LandscapeColour.h"



class LandscapeOptions : Ref
{
public:
    LandscapeGenerator*  generator;
    LandscapeColour*     colour;
    
    bool            showWater;
    bool            showLand;
    bool            showTerrain;
    int             debugMode;
    bool            debugLand;
    f32             landScaleX;
    f32             landScaleY;
    tme::loc_t	    here ;
    tme::loc_t      currentLocation;
    tme::loc_t      aheadLocation;
    mxdir_t         currentDirection;
    bool            isMoving;
    bool            isLooking;
    f32             movementAmount;
    f32             lookAmount;
};


class LandscapeView : public LandscapeNode
{
public:
    virtual void Init(LandscapeOptions* options);
    
public:
    GLProgramState* programState;

};


#endif /* LandscapeView_hpp */