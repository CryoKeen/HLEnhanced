/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "SaveRestore.h"
#include <time.h>
#include "shake.h"
#include "Decals.h"
#include "entities/CBasePlayer.h"
#include "Weapons.h"
#include "gamerules/GameRules.h"

float UTIL_WeaponTimeBase( void )
{
#if defined( CLIENT_WEAPONS )
	return 0.0;
#else
	return gpGlobals->time;
#endif
}

void UTIL_ParametricRocket( entvars_t *pev, Vector vecOrigin, Vector vecAngles, edict_t *owner )
{	
	pev->startpos = vecOrigin;
	// Trace out line to end pos
	TraceResult tr;
	UTIL_MakeVectors( vecAngles );
	UTIL_TraceLine( pev->startpos, pev->startpos + gpGlobals->v_forward * 8192, ignore_monsters, owner, &tr);
	pev->endpos = tr.vecEndPos;

	// Now compute how long it will take based on current velocity
	Vector vecTravel = pev->endpos - pev->startpos;
	float travelTime = 0.0;
	if ( pev->velocity.Length() > 0 )
	{
		travelTime = vecTravel.Length() / pev->velocity.Length();
	}
	pev->starttime = gpGlobals->time;
	pev->impacttime = gpGlobals->time + travelTime;
}

int g_groupmask = 0;
int g_groupop = 0;

// Normal overrides
void UTIL_SetGroupTrace( int groupmask, int op )
{
	g_groupmask		= groupmask;
	g_groupop		= op;

	ENGINE_SETGROUPMASK( g_groupmask, g_groupop );
}

void UTIL_UnsetGroupTrace( void )
{
	g_groupmask		= 0;
	g_groupop		= 0;

	ENGINE_SETGROUPMASK( 0, 0 );
}

// Smart version, it'll clean itself up when it pops off stack
UTIL_GroupTrace::UTIL_GroupTrace( int groupmask, int op )
{
	m_oldgroupmask	= g_groupmask;
	m_oldgroupop	= g_groupop;

	g_groupmask		= groupmask;
	g_groupop		= op;

	ENGINE_SETGROUPMASK( g_groupmask, g_groupop );
}

UTIL_GroupTrace::~UTIL_GroupTrace( void )
{
	g_groupmask		=	m_oldgroupmask;
	g_groupop		=	m_oldgroupop;

	ENGINE_SETGROUPMASK( g_groupmask, g_groupop );
}

#ifdef	DEBUG
edict_t *DBG_EntOfVars( const entvars_t *pev )
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;
	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine");
	edict_t* pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t*)pev);
	if (pent == NULL)
		ALERT(at_console, "DAMN!  Even the engine couldn't FindEntityByVars!");
	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif //DEBUG

bool UTIL_GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon )
{
	return g_pGameRules->GetNextBestWeapon( pPlayer, pCurrentWeapon );
}

Vector UTIL_VecToAngles( const Vector &vec )
{
	Vector rgflVecOut;
	VEC_TO_ANGLES(vec, rgflVecOut);
	return rgflVecOut;
}
	
//	float UTIL_MoveToOrigin( edict_t *pent, const Vector vecGoal, float flDist, int iMoveType )
void UTIL_MoveToOrigin( edict_t *pent, const Vector &vecGoal, float flDist, int iMoveType )
{
//		return MOVE_TO_ORIGIN ( pent, rgfl, flDist, iMoveType ); 
	MOVE_TO_ORIGIN ( pent, vecGoal, flDist, iMoveType );
}

int UTIL_EntitiesInBox( CBaseEntity **pList, int listMax, const Vector &mins, const Vector &maxs, int flagMask )
{
	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBaseEntity *pEntity;
	int			count;

	count = 0;

	if ( !pEdict )
		return count;

	for ( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		if ( pEdict->free )	// Not in use
			continue;
		
		if ( flagMask && !(pEdict->v.flags & flagMask) )	// Does it meet the criteria?
			continue;

		if ( mins.x > pEdict->v.absmax.x ||
			 mins.y > pEdict->v.absmax.y ||
			 mins.z > pEdict->v.absmax.z ||
			 maxs.x < pEdict->v.absmin.x ||
			 maxs.y < pEdict->v.absmin.y ||
			 maxs.z < pEdict->v.absmin.z )
			 continue;

		pEntity = CBaseEntity::Instance(pEdict);
		if ( !pEntity )
			continue;

		pList[ count ] = pEntity;
		count++;

		if ( count >= listMax )
			return count;
	}

	return count;
}


int UTIL_MonstersInSphere( CBaseEntity **pList, int listMax, const Vector &center, float radius )
{
	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBaseEntity *pEntity;
	int			count;
	float		distance, delta;

	count = 0;
	float radiusSquared = radius * radius;

	if ( !pEdict )
		return count;

	for ( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		if ( pEdict->free )	// Not in use
			continue;
		
		if ( !(pEdict->v.flags & (FL_CLIENT|FL_MONSTER)) )	// Not a client/monster ?
			continue;

		// Use origin for X & Y since they are centered for all monsters
		// Now X
		delta = center.x - pEdict->v.origin.x;//(pEdict->v.absmin.x + pEdict->v.absmax.x)*0.5;
		delta *= delta;

		if ( delta > radiusSquared )
			continue;
		distance = delta;
		
		// Now Y
		delta = center.y - pEdict->v.origin.y;//(pEdict->v.absmin.y + pEdict->v.absmax.y)*0.5;
		delta *= delta;

		distance += delta;
		if ( distance > radiusSquared )
			continue;

		// Now Z
		delta = center.z - (pEdict->v.absmin.z + pEdict->v.absmax.z)*0.5;
		delta *= delta;

		distance += delta;
		if ( distance > radiusSquared )
			continue;

		pEntity = CBaseEntity::Instance(pEdict);
		if ( !pEntity )
			continue;

		pList[ count ] = pEntity;
		count++;

		if ( count >= listMax )
			return count;
	}


	return count;
}


CBaseEntity *UTIL_FindEntityInSphere( CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius )
{
	edict_t	*pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_IN_SPHERE( pentEntity, vecCenter, flRadius);

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);
	return NULL;
}


CBaseEntity *UTIL_FindEntityByString( CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue )
{
	edict_t	*pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_BY_STRING( pentEntity, szKeyword, szValue );

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);
	return NULL;
}

CBaseEntity *UTIL_FindEntityByClassname( CBaseEntity *pStartEntity, const char *szName )
{
	return UTIL_FindEntityByString( pStartEntity, "classname", szName );
}

CBaseEntity *UTIL_FindEntityByTargetname( CBaseEntity *pStartEntity, const char *szName )
{
	return UTIL_FindEntityByString( pStartEntity, "targetname", szName );
}


CBaseEntity *UTIL_FindEntityGeneric( const char *szWhatever, Vector &vecSrc, float flRadius )
{
	CBaseEntity *pEntity = NULL;

	pEntity = UTIL_FindEntityByTargetname( NULL, szWhatever );
	if (pEntity)
		return pEntity;

	CBaseEntity *pSearch = NULL;
	float flMaxDist2 = flRadius * flRadius;
	while ((pSearch = UTIL_FindEntityByClassname( pSearch, szWhatever )) != NULL)
	{
		float flDist2 = (pSearch->pev->origin - vecSrc).Length();
		flDist2 = flDist2 * flDist2;
		if (flMaxDist2 > flDist2)
		{
			pEntity = pSearch;
			flMaxDist2 = flDist2;
		}
	}
	return pEntity;
}


// returns a CBasePlayer pointer to a player by index.  Only returns if the player is spawned and connected
// otherwise returns nullptr
// Index is 1 based
CBasePlayer	*UTIL_PlayerByIndex( int playerIndex )
{
	CBasePlayer *pPlayer = nullptr;

	if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients )
	{
		edict_t *pPlayerEdict = INDEXENT( playerIndex );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{
			pPlayer = static_cast<CBasePlayer*>( CBaseEntity::Instance( pPlayerEdict ) );
		}
	}
	
	return pPlayer;
}

CBasePlayer* UTIL_FindPlayerByName( const char *pszTestName )
{
	for( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		edict_t *pEdict = g_engfuncs.pfnPEntityOfEntIndex( i );
		if( pEdict )
		{
			CBaseEntity *pEnt = CBaseEntity::Instance( pEdict );
			if( pEnt && pEnt->IsPlayer() )
			{
				const char *pNetName = STRING( pEnt->pev->netname );
				if( stricmp( pNetName, pszTestName ) == 0 )
				{
					return ( CBasePlayer* ) pEnt;
				}
			}
		}
	}

	return nullptr;
}

void UTIL_MakeVectors( const Vector &vecAngles )
{
	MAKE_VECTORS( vecAngles );
}


void UTIL_MakeAimVectors( const Vector &vecAngles )
{
	Vector rgflVec = vecAngles;
	rgflVec[0] = -rgflVec[0];
	MAKE_VECTORS(rgflVec);
}


#define SWAP(a,b,temp)	((temp)=(a),(a)=(b),(b)=(temp))

void UTIL_MakeInvVectors( const Vector &vec, globalvars_t *pgv )
{
	MAKE_VECTORS(vec);

	float tmp;
	pgv->v_right = pgv->v_right * -1;

	SWAP(pgv->v_forward.y, pgv->v_right.x, tmp);
	SWAP(pgv->v_forward.z, pgv->v_up.x, tmp);
	SWAP(pgv->v_right.z, pgv->v_up.y, tmp);
}

static unsigned short FixedUnsigned16( float value, float scale )
{
	int output;

	output = value * scale;
	if ( output < 0 )
		output = 0;
	if ( output > 0xFFFF )
		output = 0xFFFF;

	return (unsigned short)output;
}

static short FixedSigned16( float value, float scale )
{
	int output;

	output = value * scale;

	if ( output > 32767 )
		output = 32767;

	if ( output < -32768 )
		output = -32768;

	return (short)output;
}

// Shake the screen of all clients within radius
// radius == 0, shake all clients
// UNDONE: Allow caller to shake clients not ONGROUND?
// UNDONE: Fix falloff model (disabled)?
// UNDONE: Affect user controls?
void UTIL_ScreenShake( const Vector &center, float amplitude, float frequency, float duration, float radius )
{
	int			i;
	float		localAmplitude;
	ScreenShake	shake;

	shake.duration = FixedUnsigned16( duration, 1<<12 );		// 4.12 fixed
	shake.frequency = FixedUnsigned16( frequency, 1<<8 );	// 8.8 fixed

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

		if ( !pPlayer || !(pPlayer->pev->flags & FL_ONGROUND) )	// Don't shake if not onground
			continue;

		localAmplitude = 0;

		if ( radius <= 0 )
			localAmplitude = amplitude;
		else
		{
			Vector delta = center - pPlayer->pev->origin;
			float distance = delta.Length();
	
			// Had to get rid of this falloff - it didn't work well
			if ( distance < radius )
				localAmplitude = amplitude;//radius - distance;
		}
		if ( localAmplitude )
		{
			shake.amplitude = FixedUnsigned16( localAmplitude, 1<<12 );		// 4.12 fixed
			
			MESSAGE_BEGIN( MSG_ONE, gmsgShake, NULL, pPlayer->edict() );		// use the magic #1 for "one client"
				
				WRITE_SHORT( shake.amplitude );				// shake amount
				WRITE_SHORT( shake.duration );				// shake lasts this long
				WRITE_SHORT( shake.frequency );				// shake noise frequency

			MESSAGE_END();
		}
	}
}



void UTIL_ScreenShakeAll( const Vector &center, float amplitude, float frequency, float duration )
{
	UTIL_ScreenShake( center, amplitude, frequency, duration, 0 );
}


void UTIL_ScreenFadeBuild( ScreenFade &fade, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags )
{
	fade.duration = FixedUnsigned16( fadeTime, 1<<12 );		// 4.12 fixed
	fade.holdTime = FixedUnsigned16( fadeHold, 1<<12 );		// 4.12 fixed
	fade.r = (int)color.x;
	fade.g = (int)color.y;
	fade.b = (int)color.z;
	fade.a = alpha;
	fade.fadeFlags = flags;
}


void UTIL_ScreenFadeWrite( const ScreenFade &fade, CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsNetClient() )
		return;

	MESSAGE_BEGIN( MSG_ONE, gmsgFade, NULL, pEntity->edict() );		// use the magic #1 for "one client"
		
		WRITE_SHORT( fade.duration );		// fade lasts this long
		WRITE_SHORT( fade.holdTime );		// fade lasts this long
		WRITE_SHORT( fade.fadeFlags );		// fade type (in / out)
		WRITE_BYTE( fade.r );				// fade red
		WRITE_BYTE( fade.g );				// fade green
		WRITE_BYTE( fade.b );				// fade blue
		WRITE_BYTE( fade.a );				// fade blue

	MESSAGE_END();
}


void UTIL_ScreenFadeAll( const Vector &color, float fadeTime, float fadeHold, int alpha, int flags )
{
	int			i;
	ScreenFade	fade;


	UTIL_ScreenFadeBuild( fade, color, fadeTime, fadeHold, alpha, flags );

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
	
		UTIL_ScreenFadeWrite( fade, pPlayer );
	}
}


void UTIL_ScreenFade( CBaseEntity *pEntity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags )
{
	ScreenFade	fade;

	UTIL_ScreenFadeBuild( fade, color, fadeTime, fadeHold, alpha, flags );
	UTIL_ScreenFadeWrite( fade, pEntity );
}


void UTIL_HudMessage( CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage )
{
	if ( !pEntity || !pEntity->IsNetClient() )
		return;

	MESSAGE_BEGIN( MSG_ONE, SVC_TEMPENTITY, NULL, pEntity->edict() );
		WRITE_BYTE( TE_TEXTMESSAGE );
		WRITE_BYTE( textparms.channel & 0xFF );

		WRITE_SHORT( FixedSigned16( textparms.x, 1<<13 ) );
		WRITE_SHORT( FixedSigned16( textparms.y, 1<<13 ) );
		WRITE_BYTE( textparms.effect );

		WRITE_BYTE( textparms.r1 );
		WRITE_BYTE( textparms.g1 );
		WRITE_BYTE( textparms.b1 );
		WRITE_BYTE( textparms.a1 );

		WRITE_BYTE( textparms.r2 );
		WRITE_BYTE( textparms.g2 );
		WRITE_BYTE( textparms.b2 );
		WRITE_BYTE( textparms.a2 );

		WRITE_SHORT( FixedUnsigned16( textparms.fadeinTime, 1<<8 ) );
		WRITE_SHORT( FixedUnsigned16( textparms.fadeoutTime, 1<<8 ) );
		WRITE_SHORT( FixedUnsigned16( textparms.holdTime, 1<<8 ) );

		if ( textparms.effect == 2 )
			WRITE_SHORT( FixedUnsigned16( textparms.fxTime, 1<<8 ) );
		
		if ( strlen( pMessage ) < 512 )
		{
			WRITE_STRING( pMessage );
		}
		else
		{
			char tmp[512];
			strncpy( tmp, pMessage, 511 );
			tmp[511] = 0;
			WRITE_STRING( tmp );
		}
	MESSAGE_END();
}

void UTIL_HudMessageAll( const hudtextparms_t &textparms, const char *pMessage )
{
	int			i;

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
		if ( pPlayer )
			UTIL_HudMessage( pPlayer, textparms, pMessage );
	}
}

					 
extern int gmsgTextMsg, gmsgSayText;
void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgTextMsg );
		WRITE_BYTE( msg_dest );
		WRITE_STRING( msg_name );

		if ( param1 )
			WRITE_STRING( param1 );
		if ( param2 )
			WRITE_STRING( param2 );
		if ( param3 )
			WRITE_STRING( param3 );
		if ( param4 )
			WRITE_STRING( param4 );

	MESSAGE_END();
}

void ClientPrint( entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgTextMsg, NULL, client );
		WRITE_BYTE( msg_dest );
		WRITE_STRING( msg_name );

		if ( param1 )
			WRITE_STRING( param1 );
		if ( param2 )
			WRITE_STRING( param2 );
		if ( param3 )
			WRITE_STRING( param3 );
		if ( param4 )
			WRITE_STRING( param4 );

	MESSAGE_END();
}

void UTIL_SayText( const char *pText, CBaseEntity *pEntity )
{
	if ( !pEntity->IsNetClient() )
		return;

	MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, pEntity->edict() );
		WRITE_BYTE( pEntity->entindex() );
		WRITE_STRING( pText );
	MESSAGE_END();
}

void UTIL_SayTextAll( const char *pText, CBaseEntity *pEntity )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgSayText, NULL );
		WRITE_BYTE( pEntity->entindex() );
		WRITE_STRING( pText );
	MESSAGE_END();
}

void UTIL_ShowMessage( const char *pString, CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsNetClient() )
		return;

	MESSAGE_BEGIN( MSG_ONE, gmsgHudText, NULL, pEntity->edict() );
	WRITE_STRING( pString );
	MESSAGE_END();
}


void UTIL_ShowMessageAll( const char *pString )
{
	int		i;

	// loop through all players

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
		if ( pPlayer )
			UTIL_ShowMessage( pString, pPlayer );
	}
}

// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, ( igmon == ignore_monsters ? TRF_IGNORE_MONSTERS : TRF_NONE ) | ( ignoreGlass ? TRF_IGNORE_GLASS : 0 ), pentIgnore, ptr );
}


void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, ( igmon == ignore_monsters ? TRF_IGNORE_MONSTERS : TRF_NONE ), pentIgnore, ptr );
}


void UTIL_TraceHull( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_HULL( vecStart, vecEnd, ( igmon == ignore_monsters ? TRF_IGNORE_MONSTERS : TRF_NONE ), hullNumber, pentIgnore, ptr );
}

void UTIL_TraceModel( const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr )
{
	g_engfuncs.pfnTraceModel( vecStart, vecEnd, hullNumber, pentModel, ptr );
}


TraceResult UTIL_GetGlobalTrace( )
{
	TraceResult tr;

	tr.fAllSolid		= gpGlobals->trace_allsolid;
	tr.fStartSolid		= gpGlobals->trace_startsolid;
	tr.fInOpen			= gpGlobals->trace_inopen;
	tr.fInWater			= gpGlobals->trace_inwater;
	tr.flFraction		= gpGlobals->trace_fraction;
	tr.flPlaneDist		= gpGlobals->trace_plane_dist;
	tr.pHit			= gpGlobals->trace_ent;
	tr.vecEndPos		= gpGlobals->trace_endpos;
	tr.vecPlaneNormal	= gpGlobals->trace_plane_normal;
	tr.iHitgroup		= gpGlobals->trace_hitgroup;
	return tr;
}

	
void UTIL_SetSize( entvars_t *pev, const Vector &vecMin, const Vector &vecMax )
{
	SET_SIZE( ENT(pev), vecMin, vecMax );
}
	
	
float UTIL_VecToYaw( const Vector &vec )
{
	return VEC_TO_YAW(vec);
}


void UTIL_SetOrigin( entvars_t *pev, const Vector &vecOrigin )
{
	edict_t *ent = ENT(pev);
	if ( ent )
		SET_ORIGIN( ent, vecOrigin );
}

void UTIL_ParticleEffect( const Vector &vecOrigin, const Vector &vecDirection, const unsigned int ulColor, const unsigned int ulCount )
{
	PARTICLE_EFFECT( vecOrigin, vecDirection, (float)ulColor, (float)ulCount );
}
	
Vector UTIL_GetAimVector( edict_t *pent, float flSpeed )
{
	Vector tmp;
	GET_AIM_VECTOR(pent, flSpeed, tmp);
	return tmp;
}

bool UTIL_IsMasterTriggered( string_t sMaster, const CBaseEntity* const pActivator )
{
	if (sMaster)
	{
		edict_t *pentTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(sMaster));
	
		if ( !FNullEnt(pentTarget) )
		{
			CBaseEntity *pMaster = CBaseEntity::Instance(pentTarget);
			if ( pMaster && (pMaster->ObjectCaps() & FCAP_MASTER) )
				return pMaster->IsTriggered( pActivator );
		}

		ALERT(at_console, "Master was null or not a master!\n");
	}

	// if this isn't a master entity, just say yes.
	return true;
}

bool UTIL_ShouldShowBlood( int color )
{
	if ( color != DONT_BLEED )
	{
		if ( color == BLOOD_COLOR_RED )
		{
			if ( CVAR_GET_FLOAT("violence_hblood") != 0 )
				return true;
		}
		else
		{
			if ( CVAR_GET_FLOAT("violence_ablood") != 0 )
				return true;
		}
	}
	return false;
}

int UTIL_PointContents(	const Vector &vec )
{
	return POINT_CONTENTS(vec);
}

void UTIL_BloodStream( const Vector &origin, const Vector &direction, int color, int amount )
{
	if ( !UTIL_ShouldShowBlood( color ) )
		return;

	if ( g_Language == LANGUAGE_GERMAN && color == BLOOD_COLOR_RED )
		color = 0;

	
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, origin );
		WRITE_BYTE( TE_BLOODSTREAM );
		WRITE_COORD( origin.x );
		WRITE_COORD( origin.y );
		WRITE_COORD( origin.z );
		WRITE_COORD( direction.x );
		WRITE_COORD( direction.y );
		WRITE_COORD( direction.z );
		WRITE_BYTE( color );
		WRITE_BYTE( min( amount, 255 ) );
	MESSAGE_END();
}				

void UTIL_BloodDrips( const Vector &origin, const Vector &direction, int color, int amount )
{
	if ( !UTIL_ShouldShowBlood( color ) )
		return;

	if ( color == DONT_BLEED || amount == 0 )
		return;

	if ( g_Language == LANGUAGE_GERMAN && color == BLOOD_COLOR_RED )
		color = 0;

	if ( g_pGameRules->IsMultiplayer() )
	{
		// scale up blood effect in multiplayer for better visibility
		amount *= 2;
	}

	if ( amount > 255 )
		amount = 255;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, origin );
		WRITE_BYTE( TE_BLOODSPRITE );
		WRITE_COORD( origin.x);								// pos
		WRITE_COORD( origin.y);
		WRITE_COORD( origin.z);
		WRITE_SHORT( g_sModelIndexBloodSpray );				// initial sprite model
		WRITE_SHORT( g_sModelIndexBloodDrop );				// droplet sprite models
		WRITE_BYTE( color );								// color index into host_basepal
		WRITE_BYTE( min( max( 3, amount / 10 ), 16 ) );		// size
	MESSAGE_END();
}				

Vector UTIL_RandomBloodVector( void )
{
	Vector direction;

	direction.x = RANDOM_FLOAT ( -1, 1 );
	direction.y = RANDOM_FLOAT ( -1, 1 );
	direction.z = RANDOM_FLOAT ( 0, 1 );

	return direction;
}


void UTIL_BloodDecalTrace( TraceResult *pTrace, int bloodColor )
{
	if ( UTIL_ShouldShowBlood( bloodColor ) )
	{
		if ( bloodColor == BLOOD_COLOR_RED )
			UTIL_DecalTrace( pTrace, DECAL_BLOOD1 + RANDOM_LONG(0,5) );
		else
			UTIL_DecalTrace( pTrace, DECAL_YBLOOD1 + RANDOM_LONG(0,5) );
	}
}


void UTIL_DecalTrace( TraceResult *pTrace, int decalNumber )
{
	short entityIndex;
	int index;
	int message;

	if ( decalNumber < 0 )
		return;

	index = gDecals[ decalNumber ].index;

	if ( index < 0 )
		return;

	if (pTrace->flFraction == 1.0)
		return;

	// Only decal BSP models
	if ( pTrace->pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( pTrace->pHit );
		if ( pEntity && !pEntity->IsBSPModel() )
			return;
		entityIndex = ENTINDEX( pTrace->pHit );
	}
	else 
		entityIndex = 0;

	message = TE_DECAL;
	if ( entityIndex != 0 )
	{
		if ( index > 255 )
		{
			message = TE_DECALHIGH;
			index -= 256;
		}
	}
	else
	{
		message = TE_WORLDDECAL;
		if ( index > 255 )
		{
			message = TE_WORLDDECALHIGH;
			index -= 256;
		}
	}
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( message );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_BYTE( index );
		if ( entityIndex )
			WRITE_SHORT( entityIndex );
	MESSAGE_END();
}

/*
==============
UTIL_PlayerDecalTrace

A player is trying to apply his custom decal for the spray can.
Tell connected clients to display it, or use the default spray can decal
if the custom can't be loaded.
==============
*/
void UTIL_PlayerDecalTrace( TraceResult *pTrace, int playernum, int decalNumber, const bool bIsCustom )
{
	int index;
	
	if (!bIsCustom)
	{
		if ( decalNumber < 0 )
			return;

		index = gDecals[ decalNumber ].index;
		if ( index < 0 )
			return;
	}
	else
		index = decalNumber;

	if (pTrace->flFraction == 1.0)
		return;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_PLAYERDECAL );
		WRITE_BYTE ( playernum );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_SHORT( (short)ENTINDEX(pTrace->pHit) );
		WRITE_BYTE( index );
	MESSAGE_END();
}

void UTIL_GunshotDecalTrace( TraceResult *pTrace, int decalNumber )
{
	//Prevent array index out of bounds here - Solokiller
	if ( decalNumber < 0 || static_cast<size_t>( decalNumber ) >= gDecalsSize )
		return;

	int index = gDecals[ decalNumber ].index;
	if ( index < 0 )
		return;

	if (pTrace->flFraction == 1.0)
		return;

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pTrace->vecEndPos );
		WRITE_BYTE( TE_GUNSHOTDECAL );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_SHORT( (short)ENTINDEX(pTrace->pHit) );
		WRITE_BYTE( index );
	MESSAGE_END();
}


void UTIL_Sparks( const Vector &position )
{
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, position );
		WRITE_BYTE( TE_SPARKS );
		WRITE_COORD( position.x );
		WRITE_COORD( position.y );
		WRITE_COORD( position.z );
	MESSAGE_END();
}

//
// Makes flagged buttons spark when turned off
//
void DoSpark( entvars_t *pev, const Vector &location )
{
	Vector tmp = location + pev->size * 0.5;
	UTIL_Sparks( tmp );

	float flVolume = RANDOM_FLOAT( 0.25, 0.75 ) * 0.4;//random volume range
	switch( ( int ) ( RANDOM_FLOAT( 0, 1 ) * 6 ) )
	{
	case 0: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark1.wav", flVolume, ATTN_NORM );	break;
	case 1: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark2.wav", flVolume, ATTN_NORM );	break;
	case 2: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark3.wav", flVolume, ATTN_NORM );	break;
	case 3: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark4.wav", flVolume, ATTN_NORM );	break;
	case 4: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM );	break;
	case 5: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM );	break;
	}
}

void UTIL_Ricochet( const Vector &position, float scale )
{
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, position );
		WRITE_BYTE( TE_ARMOR_RICOCHET );
		WRITE_COORD( position.x );
		WRITE_COORD( position.y );
		WRITE_COORD( position.z );
		WRITE_BYTE( (int)(scale*10) );
	MESSAGE_END();
}


bool UTIL_TeamsMatch( const char *pTeamName1, const char *pTeamName2 )
{
	// Everyone matches unless it's teamplay
	if ( !g_pGameRules->IsTeamplay() )
		return true;

	// Both on a team?
	if ( *pTeamName1 != 0 && *pTeamName2 != 0 )
	{
		if ( !stricmp( pTeamName1, pTeamName2 ) )	// Same Team?
			return true;
	}

	return false;
}

float UTIL_WaterLevel( const Vector &position, float minz, float maxz )
{
	Vector midUp = position;
	midUp.z = minz;

	if (UTIL_PointContents(midUp) != CONTENTS_WATER)
		return minz;

	midUp.z = maxz;
	if (UTIL_PointContents(midUp) == CONTENTS_WATER)
		return maxz;

	float diff = maxz - minz;
	while (diff > 1.0)
	{
		midUp.z = minz + diff/2.0;
		if (UTIL_PointContents(midUp) == CONTENTS_WATER)
		{
			minz = midUp.z;
		}
		else
		{
			maxz = midUp.z;
		}
		diff = maxz - minz;
	}

	return midUp.z;
}


extern DLL_GLOBAL	short	g_sModelIndexBubbles;// holds the index for the bubbles model

void UTIL_Bubbles( Vector mins, Vector maxs, int count )
{
	Vector mid =  (mins + maxs) * 0.5;

	float flHeight = UTIL_WaterLevel( mid,  mid.z, mid.z + 1024 );
	flHeight = flHeight - mins.z;

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, mid );
		WRITE_BYTE( TE_BUBBLES );
		WRITE_COORD( mins.x );	// mins
		WRITE_COORD( mins.y );
		WRITE_COORD( mins.z );
		WRITE_COORD( maxs.x );	// maxz
		WRITE_COORD( maxs.y );
		WRITE_COORD( maxs.z );
		WRITE_COORD( flHeight );			// height
		WRITE_SHORT( g_sModelIndexBubbles );
		WRITE_BYTE( count ); // count
		WRITE_COORD( 8 ); // speed
	MESSAGE_END();
}

void UTIL_BubbleTrail( Vector from, Vector to, int count )
{
	float flHeight = UTIL_WaterLevel( from,  from.z, from.z + 256 );
	flHeight = flHeight - from.z;

	if (flHeight < 8)
	{
		flHeight = UTIL_WaterLevel( to,  to.z, to.z + 256 );
		flHeight = flHeight - to.z;
		if (flHeight < 8)
			return;

		// UNDONE: do a ploink sound
		flHeight = flHeight + to.z - from.z;
	}

	if (count > 255) 
		count = 255;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BUBBLETRAIL );
		WRITE_COORD( from.x );	// mins
		WRITE_COORD( from.y );
		WRITE_COORD( from.z );
		WRITE_COORD( to.x );	// maxz
		WRITE_COORD( to.y );
		WRITE_COORD( to.z );
		WRITE_COORD( flHeight );			// height
		WRITE_SHORT( g_sModelIndexBubbles );
		WRITE_BYTE( count ); // count
		WRITE_COORD( 8 ); // speed
	MESSAGE_END();
}


void UTIL_Remove( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return;

	pEntity->UpdateOnRemove();
	pEntity->pev->flags |= FL_KILLME;
	pEntity->pev->targetname = 0;
}


bool UTIL_IsValidEntity( edict_t *pent )
{
	if ( !pent || pent->free || (pent->v.flags & FL_KILLME) )
		return false;
	return true;
}


void UTIL_PrecacheOther( const char *szClassname )
{
	edict_t	*pent;

	pent = CREATE_NAMED_ENTITY( MAKE_STRING( szClassname ) );
	if ( FNullEnt( pent ) )
	{
		ALERT ( at_console, "NULL Ent in UTIL_PrecacheOther\n" );
		return;
	}
	
	CBaseEntity *pEntity = CBaseEntity::Instance (VARS( pent ));
	if (pEntity)
		pEntity->Precache( );
	REMOVE_ENTITY(pent);
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( char *fmt, ... )
{
	va_list			argptr;
	static char		string[1024];
	
	va_start ( argptr, fmt );
	vsprintf ( string, fmt, argptr );
	va_end   ( argptr );

	// Print to server console
	ALERT( at_logged, "%s", string );
}

#ifndef _WIN32
extern "C" {
unsigned _rotr ( unsigned val, int shift)
{
        register unsigned lobit;        /* non-zero means lo bit set */
        register unsigned num = val;    /* number to rotate */

        shift &= 0x1f;                  /* modulo 32 -- this will also make
                                           negative shifts work */

        while (shift--) {
                lobit = num & 1;        /* get high bit */
                num >>= 1;              /* shift right one bit */
                if (lobit)
                        num |= 0x80000000;  /* set hi bit if lo bit was set */
        }

        return num;
}
}
#endif

void EntvarsKeyvalue( entvars_t *pev, KeyValueData *pkvd )
{
	const TYPEDESCRIPTION* pField;

	for ( size_t i = 0; i < gEntvarsCount; i++ )
	{
		pField = &gEntvarsDescription[i];

		if ( !stricmp( pField->fieldName, pkvd->szKeyName ) )
		{
			switch( pField->fieldType )
			{
			case FIELD_MODELNAME:
			case FIELD_SOUNDNAME:
			case FIELD_STRING:
				(*(int *)((char *)pev + pField->fieldOffset)) = ALLOC_STRING( pkvd->szValue );
				break;

			case FIELD_TIME:
			case FIELD_FLOAT:
				(*(float *)((char *)pev + pField->fieldOffset)) = atof( pkvd->szValue );
				break;

			case FIELD_INTEGER:
				(*(int *)((char *)pev + pField->fieldOffset)) = atoi( pkvd->szValue );
				break;

			case FIELD_POSITION_VECTOR:
			case FIELD_VECTOR:
				UTIL_StringToVector( (float *)((char *)pev + pField->fieldOffset), pkvd->szValue );
				break;

			default:
			case FIELD_EVARS:
			case FIELD_CLASSPTR:
			case FIELD_EDICT:
			case FIELD_ENTITY:
			case FIELD_POINTER:
				ALERT( at_error, "Bad field in entity!!\n" );
				break;
			}
			pkvd->fHandled = true;
			return;
		}
	}
}

CBaseEntity* UTIL_RandomTargetname( const char* const pszName )
{
	int total = 0;

	CBaseEntity *pEntity = nullptr;
	CBaseEntity *pNewEntity = nullptr;

	//Enumerate all entities with the given name and assign it to pEntity if a random check succeeds.
	while( ( pNewEntity = UTIL_FindEntityByTargetname( pNewEntity, pszName ) ) != nullptr )
	{
		++total;

		if( RANDOM_LONG( 0, total - 1 ) < 1 )
			pEntity = pNewEntity;
	}

	return pEntity;
}

CBaseEntity* UTIL_RandomClassname( const char* pszName )
{
	int total = 0;

	CBaseEntity *pEntity = nullptr;
	CBaseEntity *pNewEntity = nullptr;

	while( ( pNewEntity = UTIL_FindEntityByClassname( pNewEntity, pszName ) ) != nullptr )
	{
		++total;

		if( RANDOM_LONG( 0, total - 1 ) < 1 )
			pEntity = pNewEntity;
	}

	return pEntity;
}

//
// BModelOrigin - calculates origin of a bmodel from absmin/size because all bmodel origins are 0 0 0
//
Vector VecBModelOrigin( entvars_t* pevBModel )
{
	return pevBModel->absmin + ( pevBModel->size * 0.5 );
}

// Initialize absmin & absmax to the appropriate box
void SetObjectCollisionBox( entvars_t *pev )
{
	if( ( pev->solid == SOLID_BSP ) &&
		( pev->angles.x || pev->angles.y || pev->angles.z ) )
	{	// expand for rotation
		float		max, v;
		int			i;

		max = 0;
		for( i = 0; i<3; i++ )
		{
			v = fabs( ( ( float * ) pev->mins )[ i ] );
			if( v > max )
				max = v;
			v = fabs( ( ( float * ) pev->maxs )[ i ] );
			if( v > max )
				max = v;
		}
		for( i = 0; i<3; i++ )
		{
			( ( float * ) pev->absmin )[ i ] = ( ( float * ) pev->origin )[ i ] - max;
			( ( float * ) pev->absmax )[ i ] = ( ( float * ) pev->origin )[ i ] + max;
		}
	}
	else
	{
		pev->absmin = pev->origin + pev->mins;
		pev->absmax = pev->origin + pev->maxs;
	}

	pev->absmin.x -= 1;
	pev->absmin.y -= 1;
	pev->absmin.z -= 1;
	pev->absmax.x += 1;
	pev->absmax.y += 1;
	pev->absmax.z += 1;
}