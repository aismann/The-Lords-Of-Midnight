/*
 *  ILandscape.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 ___ORGANIZATIONNAME___. All rights reserved.
 *
 */

#pragma once

#include "LandscapeNode.h"

FORWARD_REFERENCE(LandscapeGenerator);
FORWARD_REFERENCE(LandscapeColour);


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
    tme::loc_t        here ;
    tme::loc_t      currentLocation;
    tme::loc_t      aheadLocation;
    mxdir_t         currentDirection;
    bool            isMoving;
    bool            isLooking;
    f32             movementAmount;
    f32             lookAmount;
};

class ILandscape : public LandscapeNode
{
public:
	ILandscape();
    virtual ~ILandscape() {};
    
    virtual void Init(LandscapeOptions* options);
    
public:
    GLProgramState* programState;
    
};