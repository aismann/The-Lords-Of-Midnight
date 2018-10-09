/*
 *  tme_interface.h
 *  tme
 *
 *  Created by Chris Wild on 10/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#ifndef _TME_INTERFACE_H_INCLUDED
#define _TME_INTERFACE_H_INCLUDED

#include "tme/tme.h"
#include "library/libinc/collections.h"
#include "tme/baseinc/variables.h"

#ifdef _DDR_
#include "tme/scenarios/ddr/scenario_ddr.h"
#define ddrexport	tme::scenarios::ddr
#endif

#ifdef _LOM_
#include "tme/scenarios/lom/scenario_lom.h"
#define lomexport	tme::scenarios::lom
#endif


#define character	defaultexport::character_t
#define regiment	defaultexport::regiment_t
#define stronghold	defaultexport::stronghold_t
#define anobject	defaultexport::object_t
#define unitinfo	defaultexport::unitinfo_t
#define maplocation	defaultexport::location_t 
#define terraininfo	defaultexport::terraininfo_t 
#define raceinfo	defaultexport::raceinfo_t 
//#define army		defaultexport::army_t


typedef struct loc_armyinfo_t {
    chilli::collections::c_mxid	armies;
	u32		friends_warriors;
	u32		friends_riders;
	u32		friends_armies;
	u32		foe_warriors;
	u32		foe_riders;
	u32		foes_armies;
	u32		regiment_warriors;
	u32		regiment_riders;
} loc_armyinfo_t ;

class NightNotificationDelegate
{
public:
	virtual void OnNightNotification( tme::callback_t* event ) = 0 ;
};

using namespace tme::flags;


LPCSTR		TME_ScenarioDirectory ( void );
LPCSTR		TME_ScenarioName ( void );

bool		TME_Init ( void );
bool		TME_DeInit ( void );
void		TME_PurgeTextCache();
LPCSTR		TME_GetCharacterText ( const character& c, const c_str& command ) ;
LPCSTR		TME_GetText ( const c_str& command ) ;
LPCSTR		TME_GetText ( const c_str& command, variant args[], int count ) ;
LPCSTR		TME_GetSystemString ( const character& c, int msg ) ;
LPCSTR		TME_LastActionMsg();
LPCSTR		TME_GetNumber(int number);
LPCSTR		TME_GetLocationText ( tme::mxgridref loc );

character&	TME_CurrentCharacter ( void );
character&	TME_CurrentCharacter ( mxid characterid );
character&	TME_RefreshCurrentCharacter ( void );

bool		TME_GetCharacter( character& out, mxid id );
bool		TME_GetStronghold( stronghold& out, mxid id );
bool		TME_GetTerrainInfo( terraininfo& out, mxid id );
bool		TME_GetRaceInfo( raceinfo& out, mxid id );
bool		TME_GetObject( anobject& out, mxid id );
bool		TME_GetUnitInfo( unitinfo& out, mxid id );
bool		TME_GetLocation( maplocation& out, mxid id );
bool		TME_GetLocation( maplocation& out, tme::loc_t loc );

bool		TME_GetMapLocation( maplocation& out, mxid id );
bool		TME_GetMapLocation( maplocation& out, tme::loc_t loc );

bool        TME_GetRegiment( regiment& out, mxid id );

bool		TME_GetLocationInfo( tme::loc_t loc );
bool		TME_GetCharacterLocationInfo ( const character& c );
LPCSTR		TME_GetUnitTypeName(mxunit_t type );

m_gameover_t TME_CheckWinLose (void);
bool		TME_Night ( NightNotificationDelegate* delegate );
bool		TME_Save ( LPSTR filespec, tme::PFNSERIALIZE function );
bool		TME_Load ( LPSTR filespec, tme::PFNSERIALIZE function );
bool        TME_SaveDescription ( LPSTR filespec, c_str& description );

bool		TME_SaveDiscoveryMap ( LPCSTR filespec );
bool		TME_LoadDiscoveryMap ( LPCSTR filespec );


void		TME_SelectChar ( mxid newid );

size		TME_MapSize ( void );
void		TME_GetArmies ( mxid loc, loc_armyinfo_t* army );
s32         TME_GetAllStrongholds (chilli::collections::c_mxid& collection ) ;
s32         TME_GetAllRegiments (chilli::collections::c_mxid& collection );
s32			TME_GetCharacters ( mxid id, chilli::collections::c_mxid& collection, u32& recruited );
s32         TME_GetAllCharacters (chilli::collections::c_mxid& collection );
s32         TME_GetFollowers ( mxid id, chilli::collections::c_mxid& collection );
s32			TME_GetCharactersAtLocation ( mxid id, chilli::collections::c_mxid& collection, bool showall, bool showtunnel  );
s32			TME_GetCharactersAtLocation ( tme::loc_t loc, chilli::collections::c_mxid& collection, bool showall  );
MXRESULT	TME_GetArmiesAtLocation( mxid loc, u32& enemies, u32& friends );
MXRESULT	TME_GetArmiesAtLocation( tme::loc_t loc, u32& enemies, u32& friends );
LPCSTR      TME_GetSymbol( mxid id );
mxid		TME_LinkData ( LPCSTR symbol, void* data );
void*		TME_GetEntityUserData ( mxid );
mxid		TME_GetId( LPCSTR symbol, id_type_t type );

typedef struct MapInfo_t {
    tme::loc_t   top;
    tme::loc_t   bottom;
    chilli::types::size    size;
} MapInfo_t ;


bool        TME_MapInfo ( MapInfo_t* info );





// character helpers
inline bool Character_IsResting(const character& c)			{ return c.flags.Is(cf_resting); }
inline mxtime_t Character_Time(const character& c)		{ return Character_IsResting(c) ? tme::variables::sv_time_night : c.time ; }
inline bool Character_IsNight(const character& c)			{ return Character_Time(c)==(mxtime_t)tme::variables::sv_time_night; }
inline bool Character_IsDawn(const character& c)			{ return Character_Time(c)==(mxtime_t)tme::variables::sv_time_dawn; }
inline bool Character_IsWaiting(const character& c)			{ return c.wait!=WM_NONE; }
inline bool Character_IsHidden(const character& c)			{ return c.flags.Is(cf_hidden); }
inline bool Character_IsRiding(const character& c)			{ return c.flags.Is(cf_riding); }
inline bool Character_IsAlive(const character& c)			{ return c.flags.Is(cf_alive); }
inline bool Character_IsDead(const character& c)			{ return !c.flags.Is(cf_alive); }
inline bool Character_IsRecruited(const character& c)		{ return c.flags.Is(cf_recruited); }
inline bool Character_IsAllowedArmy(const character& c)		{ return c.flags.Is(cf_army); }
inline bool Character_IsAllowedHide(const character& c)		{ return c.flags.Is(cf_hide); }
inline bool Character_IsAllowedHorse(const character& c)	{ return c.flags.Is(cf_horse); }
inline bool Character_IsAllowedMoonring(const character& c)	{ return c.flags.Is(cf_moonring); }
inline bool Character_IsAllowedIcecrown(const character& c)	{ return c.flags.Is(cf_icecrown); }
inline bool Character_CanDestroyIcecrown(const character& c){ return c.flags.Is(cf_destroyicecrown); }
inline bool Character_IsInBattle(const character& c)		{ return c.flags.Is(cf_inbattle); }
inline bool Character_HasWonBattle(const character& c)		{ return c.flags.Is(cf_wonbattle); }

#if defined(_DDR_)
inline bool Character_HasUsedObject(const character& c)		{ return c.flags.Is(cf_usedobject); }
inline bool Character_IsInTunnel(const character& c)		{ return c.flags.Is(cf_tunnel); }
inline bool Character_InTunnel(const character& c)		{ return c.flags.Is(cf_tunnel); }
inline bool Character_IsPreparingForBattle(const character& c)		{ return c.flags.Is(cf_preparesbattle); }
bool Character_EnterTunnel ( const character& c );
//bool Character_ExitTunnel ( const character& c );
bool Character_Use ( const character& c );
bool Character_Give ( const character& c, mxid to );
bool Character_Take ( const character& c );
#endif

inline bool Character_IsFollowing(const character& c)		{ return c.following!=IDT_NONE; }
inline bool Character_IsLeading(const character& c)		{ return c.followers!=0; }

bool Character_IsControllable( mxid id );
void Character_Lookat ( const character& c, tme::loc_t location );
void Character_Wait ( const character& c );
void Character_Rest ( const character& c );
void Character_Look ( const character& c, mxdir_t dir );
void Character_LookLeft ( const character& c );
void Character_LookRight ( const character& c );
bool Character_Approach ( const character& c );
bool Character_Hide ( const character& c );
bool Character_HasBattleInfo ( const character& c );
bool Character_Move ( const character& c );
bool Character_Attack ( const character& c );
bool Character_RecruitMen ( const character& c );
bool Character_PostMen ( const character& c );
mxid Character_LocationObject (  const character& c );
mxid Character_Fight ( const character& c );
mxid Character_Seek ( const character& c );

bool Character_Place ( const character& c, tme::loc_t location );


bool Character_Follow ( const character& c, mxid id );
bool Character_UnFollow ( const character& c, mxid id );
bool Character_CanFollow( const character& c, mxid id );
bool Character_Disband( const character& c );
bool Character_SwapGroupLeader( const character& c, mxid id );

bool Character_Army ( mxid id, tme::scenarios::exports::army_t& out );

#if defined(_DDR_)
inline bool Location_IsVisible(const maplocation& l)			{ return l.flags.Is(lf_seen); }
inline bool Location_HasCharacters(const maplocation& l)		{ return l.flags.Is(lf_character); }
inline bool Location_HasTunnel(const maplocation& l)		{ return l.flags.Is(lf_tunnel); }
inline bool Location_HasTunnelExit(const maplocation& l)		{ return l.flags.Is(lf_tunnel_exit); }
inline bool Location_HasTunnelEntrance(const maplocation& l)		{ return l.flags.Is(lf_tunnel_entrance); }
#endif


// global_tme_vars

void Vars_Init ( void );
void Vars_DeInit ( void );
void Vars_EmptyCache ( void );
tme::info_t* Vars_GetVar( mxid id, const c_str& varname, tme::variables::CVar var );


// Debug!!
bool TME_DebugInstallMap( void* map );


extern tme::mxinterface*	mxi ;
extern chilli::collections::c_mxid			default_characters ;
extern chilli::collections::c_mxid			recruitable_characters;
extern chilli::collections::c_mxid			location_characters;
extern u32                  location_recruited;
extern chilli::collections::c_mxid			location_strongholds;
extern mxid                 location_infrontid;
extern mxid                 location_lookingatid;
extern tme::loc_t			location_infront;
extern tme::loc_t			location_lookingat;
extern flags32              location_flags;
extern mxid                 location_fightthing;
extern stronghold           location_stronghold;
extern mxid                 location_object;
extern mxid                 location_stubborn_lord_attack;
extern mxid                 location_stubborn_lord_move;

#if defined(_DDR_)
extern mxid                 location_object_tunnel;
extern mxid                 location_someone_to_give_to;
#endif

extern chilli::collections::c_mxid			location_infront_strongholds;
extern mxid                 location_infront_object;

extern	loc_armyinfo_t	location_infront_armies;
extern	loc_armyinfo_t	location_armies;


#endif // _TME_INTERFACE_H_INCLUDED

