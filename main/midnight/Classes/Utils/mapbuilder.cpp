//
//  mapbuilder.cpp
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "mapbuilder.h"
#include "../system/tmemanager.h"
#include "../system/resolutionmanager.h"
#include "../Extensions/TMXTiledMap.h"
#include "../tme_interface.h"

USING_NS_CC;
USING_NS_CC_UI;

static int density_mapping[] = { 10, 6, 11, 7, 14, 2, 15, 3, 9, 5, 8, 4, 13, 1, 12, 0 };

const s32 CELL_FIRST_TUNNEL             = (0*16) + 13;
const s32 CELL_FIRST_OBJECT             = (8*16) + 4;
const s32 CELL_FIRST_INCOMPLETE_TUNNEL  = (1*16) + 9;
const s32 CELL_FIRST_BLANK_TERRAIN      = (8*16) + 0;
const s32 CELL_FIRST_TUNNEL_END         = (0*16) + 9;
const s32 CELL_BLANK                    = (3*16) + 1;

#define INDEX(x1,y1)        (((y1)*mapsize.cx)+(x1))
#define CELL(x1,y1)         (mapdata+INDEX(x1,y1))
#define REMAP_DENSITY(x)    ((x/4) * 16 ) + (x%4)


mapbuilder::mapbuilder() :
    tunnels(nullptr),
    terrain(nullptr),
    terrain_discovery(nullptr),
    critters(nullptr),
    mapdata(nullptr),
    obj_characters(0),
    obj_regiments(0),
    obj_strongholds(0),
    mapsize(0,0)
{
    //setFlags(mapflags::debug_map);
    //setFlags(mapflags::show_all_characters);
    //setFlags(mapflags::show_all_critters);
    
    setFlags(mapflags::show_characters);
    setFlags(mapflags::show_critters);
    setFlags(mapflags::show_tunnels);
}

mapbuilder::~mapbuilder()
{
    //SAFEFREE(tunnels);
    //SAFEFREE(terrain);
    //SAFEFREE(terrain_discovery)
    //SAFEFREE(critters);
    SAFEFREE(mapdata);
    objects.clear();
}

mapbuilder* mapbuilder::build ( void )
{
    mapsize = TME_MapSize();
    
    if ( !TME_MapInfo(&info) )
        return nullptr;
    
    if ( checkFlags(mapflags::debug_map) ) {
        info.top = loc_t(0,0) ;
        info.bottom = loc_t(mapsize.cx,mapsize.cy);
        info.size = mapsize ;
    }
    
    loc_start = info.top;
    
    mapsize.cx = info.bottom.x - loc_start.x;
    mapsize.cy = info.bottom.y - loc_start.y;
    
    // we need some memory to store the map
    SAFEFREE(mapdata);
    SAFEFREE(terrain);
    SAFEFREE(terrain_discovery);
    SAFEFREE(tunnels);
    SAFEFREE(critters);
    
    u32 size = mapsize.cx * mapsize.cy;
    
    mapdata = static_cast<maplocation*>(malloc(size*sizeof(maplocation)));
    terrain = static_cast<u32*>(malloc(size*sizeof(u32)));
    terrain_discovery = static_cast<u32*>(malloc(size*sizeof(u32)));
    critters = static_cast<u32*>(malloc(size*sizeof(u32)));
    tunnels = static_cast<u32*>(malloc(size*sizeof(u32)));
    
    updateTerrain();
    
    updateDensityMap();
    
    updateLayers();
    
    updateObjects();

#if defined(_DDR_)
    updateSpecialObjects();
#
    
    return this;
}

mapbuilder* mapbuilder::updateTerrain()
{
    maplocation m;
    maplocation* dst = mapdata ;
    
    /*
     Discover map uses the following flags
        lf_seen             // 5
        lf_visited          // 16
        lf_looked_at        // 17
        lf_tunnel_looked_at // 18
        lf_tunnel_visited   // 19
     */
    
    bool debug_map = checkFlags(mapflags::debug_map);
    
    u32 reset_discover_mask = (lf_seen|lf_visited|lf_looked_at);
#if defined(_DDR_)
    reset_discover_mask |= lf_tunnel_looked_at|lf_tunnel_visited;
#endif
    
    // grab terrain
    for  ( int y=0; y<mapsize.cy; y++ ) {
        for ( int x=0; x<mapsize.cx; x++ ) {
            TME_GetLocation(m,mxgridref(loc_start.x+x,loc_start.y+y));
            
            dst->terrain = m.terrain ;
            dst->density=0;
            dst->tunnel=0;
            dst->flags=m.flags;
            dst->object=GET_ID(m.object);
            dst->object_tunnel = GET_ID(m.object_tunnel);
            dst->discovery_flags = m.discovery_flags ;
            dst->discovery_flags.Reset(~reset_discover_mask);
            dst->area = m.area ;
            
            if ( dst->flags.Is(lf_seen) || dst->discovery_flags.Is(lf_seen) || debug_map )
                dst->discovery_flags.Set(cf_visible);
            
            dst++;
        }
    }
    
    return this;
}


mapbuilder* mapbuilder::updateDensityMap()
{
    maplocation* dst = mapdata ;
    maplocation* src=NULL;

    u8 density=0;
    
    bool debug_map = checkFlags(mapflags::debug_map);
    
    // calculate gfx type
    for  ( int y=0; y<mapsize.cy; y++ ) {
        for ( int x=0; x<mapsize.cx; x++ ) {
            
            dst = CELL(x,y) ;
            
            density = 0;
        
            //
            // Check GFX type of terrain
            //
            
            if ( dst->flags.Is(lf_seen) || dst->discovery_flags.Is(lf_seen) || debug_map ) {
                
#define COMPARE(xx,yy,v) \
    src = CELL(xx,yy); \
    if ( src->terrain!=dst->terrain || (!src->flags.Is(lf_seen) && !src->discovery_flags.Is(lf_seen) && !debug_map) ) \
                density += v;
                
                // check above
                if ( y>0 ) {
                    COMPARE(x,y-1,1);
                }else
                    density+=1;
                
                // check down
                if ( y<mapsize.cy-1 ) {
                    COMPARE(x,y+1,4);
                }else
                    density+=4;
                
                // check left
                if ( x>0 ) {
                    COMPARE(x-1,y,8);
                }else
                    density+=8;
                
                // check right
                if ( x<mapsize.cx-1 ) {
                    COMPARE(x+1,y,2);
                }else
                    density+=2;
                
                dst->density = density_mapping[ density ];
                dst->density = REMAP_DENSITY(dst->density);
                
#undef COMPARE
                
            }else{
        
#define COMPARE(xx,yy,v) \
src = CELL(xx,yy); \
if ( src->flags.Is(lf_seen) || src->discovery_flags.Is(lf_seen) || debug_map ) \
    density += v;
                
                // check above
                if ( y>0 ) {
                    COMPARE(x,y-1,1);
                }
                // check down
                if ( y<mapsize.cy-1 ) {
                    COMPARE(x,y+1,4);
                }
                // check left
                if ( x>0 ) {
                    COMPARE(x-1,y,8);
                }
                // check right
                if ( x<mapsize.cx-1 ) {
                    COMPARE(x+1,y,2);
                }
                
                dst->density = density_mapping[ density ];
                dst->density = REMAP_DENSITY(dst->density);

#undef COMPARE
            }
            
            
#if defined(_DDR_)
            //
            // Check GFX type of tunnel
            //
            density=15;
            
            if ( dst->flags.Is(lf_tunnel_looked_at) || ( dst->flags.Is(lf_tunnel) && debug_map ) ) {
                
                int flags=lf_tunnel_visited;
                if ( dst->flags.Is(lf_tunnel_visited))
                    flags = lf_tunnel_looked_at;
                
#define COMPARE(xx,yy,v) \
src = CELL(xx,yy); \
if ( src->flags.Is(flags) || (src->flags.Is(lf_tunnel) && debug_map) ) \
    density -= v;
                
                // check above
                if ( y>0 ) {
                    COMPARE(x,y-1,1);
                }
                // check down
                if ( y<mapsize.cy-1 ) {
                    COMPARE(x,y+1,4);
                }
                // check left
                if ( x>0 ) {
                    COMPARE(x-1,y,8);
                }
                // check right
                if ( x<mapsize.cx-1 ) {
                    COMPARE(x+1,y,2);
                }
                
                dst->tunnel = density_mapping[ density ];

#undef COMPARE
            }
#endif
        }
        
    }
    
    return this;
}


mapbuilder* mapbuilder::updateLayers()
{
 
    
    int px,py=0;
    int x,y=0;
    
    loc_first.x = 0 ;
    loc_first.y = 0;
    
    int lastx = mapsize.cx+1;
    int lasty = mapsize.cy+1;
    
    u32 size = mapsize.cx * mapsize.cy;

    memset(terrain, 0, size);
    memset(terrain_discovery,0,size);
    memset(tunnels, 0, size);
    memset(critters, 0, size);
    
    bool debug_map = checkFlags(mapflags::debug_map);
    bool show_tunnels = checkFlags(mapflags::show_tunnels);
    bool show_critters = checkFlags(mapflags::show_critters);
    bool show_all_critters = checkFlags(mapflags::show_all_critters);
    
    maplocation*    m = mapdata;
    terraininfo t;
    object o;
    
    for ( int ii=0; ii<size; ii++ ) {
        terrain[ii] = 0;
        critters[ii] = 0;
        tunnels[ii]=0;
        terrain_discovery[ii]=0;
        
    
        bool seen = m->flags.Is(lf_seen) || debug_map ;
        bool domain = m->flags.Is(lf_domain);
        bool visited = m->flags.Is(lf_visited);
        bool looked_at = m->flags.Is(lf_looked_at);
        bool displayed_character=false;
        bool mist = m->flags.Is(lf_mist);
        //bool showarea = m->area == currentarea ;
        
        bool discovery_seen = m->discovery_flags.Is(lf_seen) ;
        bool discovery_visited = m->discovery_flags.Is(lf_visited);
        bool discovery_looked_at = m->discovery_flags.Is(lf_looked_at);
        
#if defined(_DDR_)
        bool tunnelseen = m->flags.Is(lf_tunnel_looked_at) || debug_map;
        bool tunnelvisited = m->flags.Is(lf_tunnel_visited) || debug_map;
        
        bool discovery_tunnelseen =  m->discovery_flags.Is(lf_tunnel_looked_at);
        bool discovery_tunnelvisited = m->discovery_flags.Is(lf_tunnel_visited) ;
        
        bool passageway = m->flags.Is(lf_tunnel_passageway);
        bool object = m->flags.Is(lf_object);
        bool object_special = m->flags.Is(lf_object_special);
#else
        bool tunnelseen = false;
        bool tunnelvisited = false ;
        bool discovery_tunnelseen = false;
        bool discovery_tunnelvisited = false ;
        bool passageway = false;
        bool object = false;
        bool object_special = false;
#endif
        bool visible = seen || visited || discovery_seen || discovery_visited || tunnelvisited || tunnelseen || discovery_tunnelseen || discovery_tunnelvisited ;
        
        bool locationTextAvailable = looked_at || discovery_looked_at || visited || discovery_visited || debug_map ;
        
        mxthing_t thing = (mxthing_t)m->object ;
        
#if defined(_DDR_)
        if ( m->object_tunnel != OB_NONE && show_tunnels )
            thing = (mxthing_t)m->object_tunnel ;
#endif

        if ( visible ) {
            
            // Terrain
            TME_GetTerrainInfo(t,MAKE_ID(IDT_TERRAININFO,m->terrain));
            terrain_data_t* d = (terrain_data_t*)t.userdata ;
     
            u32 cell = d->mapdensity ? d->mapcell + m->density : d->mapcell ;
            
            
            if ( seen )
                terrain[ii] = cell ;
            else {
                terrain_discovery[ii] = cell;
                terrain[ii] = CELL_BLANK;
            }
                
            
        } else {
            terrain[ii] = CELL_FIRST_BLANK_TERRAIN + m->density + 1 ;
        }
        
        mxthing_t show_thing = OB_NONE;
        
        // critters
        if (!displayed_character ) {
#if defined(_LOM_)
            if ( ((seen && (visited || looked_at)) || show_all_critters ) && show_critters )  {
                    show_thing = thing;
            }
#endif
#if defined(_DDR_)
            bool show_critter = ( passageway && tunnelseen ) || (!passageway && (seen && (visited || looked_at))) ;
            if ( (show_critter || show_all_critters ) && show_critters  )  {
                    //if ( thing == (mxthing_t)m->object_tunnel )
                    show_thing = thing;
            }
#endif
        }
        
        if ( show_thing && seen ) {
            TME_GetObject(o,MAKE_ID(IDT_OBJECT,show_thing));
            obj_data_t* d = (obj_data_t*)o.userdata ;
            if ( d )
                critters[ii] = d->mapcell ;
        }
        
        
#if defined(_DDR_)
        BOOL tunnel_visited = m->flags.Is(lf_tunnel_visited) || ( m->flags.Is(lf_tunnel) && debug_map ) ;
        BOOL tunnel_looked_at = m->flags.Is(lf_tunnel_looked_at)  ;
        
        if ( (tunnel_visited||tunnel_looked_at) && show_tunnels ) {
            int d=m->tunnel;
            int tunnel_end = -1;
            
            // top = 0 / 1
            // left= 1 / 0
            // right=2 / 2
            // bottom=7 / 3
            if ( d == 3 ) // right
                tunnel_end=2;
            else if ( d == 1) // left
                tunnel_end=0;
            else if ( d == 4) // top
                tunnel_end=1;
            else if ( d == 12) // bottom
                tunnel_end=3;
            
            if ( m->flags.Is(lf_tunnel_entrance|lf_tunnel_exit)) {
                // tunnel ends
                if ( tunnel_end != -1 )
                    tunnels[ii] = CELL_FIRST_TUNNEL_END + tunnel_end;
            } else {
                if ( tunnel_visited ) {
                    tunnels[ii] = CELL_FIRST_TUNNEL + REMAP_DENSITY(d);
                } else
                    if ( tunnel_end!=-1 )
                        tunnels[ii] = CELL_FIRST_INCOMPLETE_TUNNEL + tunnel_end;
            }
            
        }
#endif
        
        m++;
    }
    
    return this;
}

loc_t mapbuilder::normaliseLocation( loc_t loc )
{
    return loc_t( loc.x-loc_start.x, loc.y - loc_start.y);
}

Vec2 mapbuilder::convertToPosition( loc_t loc )
{
    loc = normaliseLocation(loc);
    return Vec2( loc.x*RES(64), loc.y*RES(64) );
}


#if defined(_DDR_)
mapbuilder* mapbuilder::updateSpecialObjects()
{
    character c;
    object o;
    
    c_mxid objects;
    TME_GetAllObjects(objects);
    
    for( auto id : objects ) {
        TME_GetObject(o, id);

        CONTINUE_IF( !(o.flags.Is(of_unique)));

        loc_t loc = o.location;
        
        if ( o.carriedby != IDT_NONE ) {
            TME_GetCharacter(c, o.carriedby);
            loc = c.location;
        }
        
        CONTINUE_IF ( loc.x == 0 && loc.y == 0 );
        
        loc = normaliseLocation(loc);
        
        auto userdata = static_cast<obj_data_t*>(o.userdata) ;
        u32 cell = IS_NOT_NULL(userdata, userdata->mapcell, CELL_FIRST_OBJECT + o.type);
        
        int index = INDEX(loc.x,loc.y);
        
        if ( mapdata[index].flags & lf_seen )
            critters[ index ] = cell ;
    }
    
    
    return this;
}
#endif


mapbuilder* mapbuilder::updateObjects()
{
    character c;
    regiment r;
    stronghold s;
    maplocation m;
    
    rect empty_rect(0,0,0,0);
    
    bool show_all_characters = checkFlags(mapflags::show_all_characters);
    bool show_enemy_armies = checkFlags(mapflags::show_enemy_armies);
    
    // get lords
    c_mxid characters;
    TME_GetAllCharacters(characters);
    
    // get armies
    c_mxid regiments;
    TME_GetAllRegiments(regiments);
    
    // get armies
    c_mxid strongholds;
    TME_GetAllStrongholds(strongholds);

    objects.clear();
    obj_characters = objects.size();
    
    for ( auto id : characters ) {
        TME_GetCharacter(c, id );
        
        CONTINUE_IF(!Character_IsAlive(c));
        
        TME_GetLocation(m,c.location);
        
        CONTINUE_IF ( !show_all_characters && !Character_IsControllable(c.id));
        
#if defined(_LOM_)
        bool visited = (m.flags & ( lf_looked_at|lf_visited))
#endif
#if defined(_DDR_)
        bool seen = show_all_characters || m.flags&lf_seen ;
        CONTINUE_IF ( !seen && !Character_IsInTunnel(c));
        bool visited = (m.flags & ( lf_looked_at|lf_visited|lf_tunnel_visited|lf_tunnel_looked_at));
#endif
        
        CONTINUE_IF ( !(show_all_characters || Character_IsRecruited(c) || visited) );

        auto object = new map_object();
        objects.pushBack(object);
        
        object->selectable=FALSE;
        object->id = c.id ;
        object->location = c.location ;
        //object->image = GetCharacterFace(c);
        object->soldiers = c.warriors+c.riders;
        //object->name = c.shortname ;
        object->r = empty_rect ;
        object->selected = false ;
        object->selectable=Character_IsRecruited(c) || show_all_characters;
        object->multiple=false;
#if defined(_DDR_)
        object->tunnel=Character_IsInTunnel(c);
#endif
        object->lastlocation = c.lastlocation ;
        object->targetlocation = c.targetlocation;
        object->homelocation = c.homelocation;
        
        loc_t loc = normaliseLocation(c.location);
        critters[ INDEX(loc.x,loc.y) ] = 7 ;
        
    }
    
    if ( show_enemy_armies ) {
        obj_regiments = objects.size();
        for ( auto id : regiments ) {
            TME_GetRegiment(r, id );
            
            CONTINUE_IF( r.total == 0 );

            auto object = new map_object();
            objects.pushBack(object);
            
            object->id = r.id ;
            object->location = r.location ;
            object->soldiers = r.total ;
            //if ( r.type == UT_WARRIORS )
            //    object->image = map_warrior ;
            //else if ( r.type == UT_RIDERS )
            //    object->image = map_rider ;
            object->lastlocation = r.lastlocation ;
            object->selectable = false;
            object->r = empty_rect ;
            object->targetlocation = r.targetlocation ;
    
        }
    }
    
    if ( 1 ) {
        obj_strongholds = objects.size();
        for ( auto id : strongholds ) {
            TME_GetStronghold(s, id );
            CONTINUE_IF ( s.loyalty != RA_MOONPRINCE );
            
            auto object = new map_object();
            objects.pushBack(object);
            
            object->id = s.id ;
            object->location = s.location ;
            object->selectable = false;
            object->r = empty_rect ;
            //object->image = map_token_blue ;
        }
    }
 
    return this;
}

void mapbuilder::clearLayers()
{
    critters = nullptr;
    tunnels = nullptr;
    terrain = nullptr;
    terrain_discovery = nullptr;
}
