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

===== weapons.cpp ========================================================

  functions governing the selection/use of weapons for players

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/CBasePlayer.h"
#include "entities/NPCs/Monsters.h"
#include "Weapons.h"
#include "nodes/Nodes.h"
#include "entities/CSoundEnt.h"
#include "Decals.h"
#include "gamerules/GameRules.h"

DLL_GLOBAL	short	g_sModelIndexLaser;// holds the index for the laser beam
DLL_GLOBAL  const char *g_pModelNameLaser = "sprites/laserbeam.spr";
DLL_GLOBAL	short	g_sModelIndexLaserDot;// holds the index for the laser beam dot
DLL_GLOBAL	short	g_sModelIndexFireball;// holds the index for the fireball
DLL_GLOBAL	short	g_sModelIndexSmoke;// holds the index for the smoke cloud
DLL_GLOBAL	short	g_sModelIndexWExplosion;// holds the index for the underwater explosion
DLL_GLOBAL	short	g_sModelIndexBubbles;// holds the index for the bubbles model
DLL_GLOBAL	short	g_sModelIndexBloodDrop;// holds the sprite index for the initial blood
DLL_GLOBAL	short	g_sModelIndexBloodSpray;// holds the sprite index for splattered blood

ItemInfo CBasePlayerItem::ItemInfoArray[MAX_WEAPONS];

CMultiDamage g_MultiDamage;

/*
================
SpawnBlood
================
*/
void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage)
{
	UTIL_BloodDrips( vecSpot, g_vecAttackDir, bloodColor, (int)flDamage );
}


int DamageDecal( CBaseEntity *pEntity, int bitsDamageType )
{
	if ( !pEntity )
		return (DECAL_GUNSHOT1 + RANDOM_LONG(0,4));
	
	return pEntity->DamageDecal( bitsDamageType );
}

void DecalGunshot( TraceResult *pTrace, int iBulletType )
{
	// Is the entity valid
	if ( !UTIL_IsValidEntity( pTrace->pHit ) )
		return;

	if ( VARS(pTrace->pHit)->solid == SOLID_BSP || VARS(pTrace->pHit)->movetype == MOVETYPE_PUSHSTEP )
	{
		CBaseEntity *pEntity = NULL;
		// Decal the wall with a gunshot
		if ( !FNullEnt(pTrace->pHit) )
			pEntity = CBaseEntity::Instance(pTrace->pHit);

		switch( iBulletType )
		{
		case BULLET_PLAYER_9MM:
		case BULLET_MONSTER_9MM:
		case BULLET_PLAYER_MP5:
		case BULLET_MONSTER_MP5:
		case BULLET_PLAYER_BUCKSHOT:
		case BULLET_PLAYER_357:
		case BULLET_PLAYER_762:
		default:
			// smoke and decal
			UTIL_GunshotDecalTrace( pTrace, DamageDecal( pEntity, DMG_BULLET ) );
			break;
		case BULLET_MONSTER_12MM:
			// smoke and decal
			UTIL_GunshotDecalTrace( pTrace, DamageDecal( pEntity, DMG_BULLET ) );
			break;
		case BULLET_PLAYER_CROWBAR:
			// wall decal
			UTIL_DecalTrace( pTrace, DamageDecal( pEntity, DMG_CLUB ) );
			break;
		}
	}
}



//
// EjectBrass - tosses a brass shell from passed origin at passed velocity
//
void EjectBrass ( const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype )
{
	// FIX: when the player shoots, their gun isn't in the same position as it is on the model other players see.

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecOrigin );
		WRITE_BYTE( TE_MODEL);
		WRITE_COORD( vecOrigin.x);
		WRITE_COORD( vecOrigin.y);
		WRITE_COORD( vecOrigin.z);
		WRITE_COORD( vecVelocity.x);
		WRITE_COORD( vecVelocity.y);
		WRITE_COORD( vecVelocity.z);
		WRITE_ANGLE( rotation );
		WRITE_SHORT( model );
		WRITE_BYTE ( soundtype);
		WRITE_BYTE ( 25 );// 2.5 seconds
	MESSAGE_END();
}


#if 0
// UNDONE: This is no longer used?
void ExplodeModel( const Vector &vecOrigin, float speed, int model, int count )
{
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecOrigin );
		WRITE_BYTE ( TE_EXPLODEMODEL );
		WRITE_COORD( vecOrigin.x );
		WRITE_COORD( vecOrigin.y );
		WRITE_COORD( vecOrigin.z );
		WRITE_COORD( speed );
		WRITE_SHORT( model );
		WRITE_SHORT( count );
		WRITE_BYTE ( 15 );// 1.5 seconds
	MESSAGE_END();
}
#endif

// Precaches the weapon and queues the weapon info for sending to clients
void UTIL_PrecacheOtherWeapon( const char *szClassname )
{
	edict_t	*pent;

	pent = CREATE_NAMED_ENTITY( MAKE_STRING( szClassname ) );
	if ( FNullEnt( pent ) )
	{
		ALERT ( at_console, "NULL Ent in UTIL_PrecacheOtherWeapon\n" );
		return;
	}
	
	CBaseEntity *pEntity = CBaseEntity::Instance (VARS( pent ));

	if (pEntity)
	{
		ItemInfo II;
		pEntity->Precache( );
		memset( &II, 0, sizeof II );
		if ( ((CBasePlayerItem*)pEntity)->GetItemInfo( &II ) )
		{
			CBasePlayerItem::ItemInfoArray[II.iId] = II;
		}
	}

	REMOVE_ENTITY(pent);
}

void RegisterAmmoTypes()
{
	g_AmmoTypes.Clear();

	g_AmmoTypes.SetCanAddAmmoTypes( true );

	// Precaches the ammo and queues the ammo info for sending to clients
	g_AmmoTypes.AddAmmoType( "buckshot", BUCKSHOT_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "9mm", _9MM_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "ARgrenades", M203_GRENADE_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "357", _357_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "uranium", URANIUM_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "rockets", ROCKET_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "bolts", BOLT_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "Trip Mine", TRIPMINE_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "Satchel Charge", SATCHEL_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "Hand Grenade", HANDGRENADE_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "Snarks", SNARK_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "Hornets", HORNET_MAX_CARRY );
	g_AmmoTypes.AddAmmoType( "762", NATO762_MAX_CARRY );

	//No more letting weapons define ammo types. - Solokiller
	g_AmmoTypes.SetCanAddAmmoTypes( false );
}

// called by worldspawn
void W_Precache(void)
{
	memset( CBasePlayerItem::ItemInfoArray, 0, sizeof(CBasePlayerItem::ItemInfoArray) );

	// custom items...

	// common world objects
	UTIL_PrecacheOther( "item_suit" );
	UTIL_PrecacheOther( "item_battery" );
	UTIL_PrecacheOther( "item_antidote" );
	UTIL_PrecacheOther( "item_security" );
	UTIL_PrecacheOther( "item_longjump" );

	//TODO: weapons and ammo could be added to a list that is enumerated. Would make things like cheats and map config parsers easier to write - Solokiller

	// shotgun
	UTIL_PrecacheOtherWeapon( "weapon_shotgun" );
	UTIL_PrecacheOther( "ammo_buckshot" );

	// crowbar
	UTIL_PrecacheOtherWeapon( "weapon_crowbar" );

	// glock
	UTIL_PrecacheOtherWeapon( "weapon_9mmhandgun" );
	UTIL_PrecacheOther( "ammo_9mmclip" );

	// mp5
	UTIL_PrecacheOtherWeapon( "weapon_9mmAR" );
	UTIL_PrecacheOther( "ammo_9mmAR" );
	UTIL_PrecacheOther( "ammo_ARgrenades" );

	// python
	UTIL_PrecacheOtherWeapon( "weapon_357" );
	UTIL_PrecacheOther( "ammo_357" );
	
	// gauss
	UTIL_PrecacheOtherWeapon( "weapon_gauss" );
	UTIL_PrecacheOther( "ammo_gaussclip" );

	// rpg
	UTIL_PrecacheOtherWeapon( "weapon_rpg" );
	UTIL_PrecacheOther( "ammo_rpgclip" );

	// crossbow
	UTIL_PrecacheOtherWeapon( "weapon_crossbow" );
	UTIL_PrecacheOther( "ammo_crossbow" );

	// egon
	UTIL_PrecacheOtherWeapon( "weapon_egon" );

	// tripmine
	UTIL_PrecacheOtherWeapon( "weapon_tripmine" );

	// satchel charge
	UTIL_PrecacheOtherWeapon( "weapon_satchel" );

	// hand grenade
	UTIL_PrecacheOtherWeapon("weapon_handgrenade");

	// squeak grenade
	UTIL_PrecacheOtherWeapon( "weapon_snark" );

	// hornetgun
	UTIL_PrecacheOtherWeapon( "weapon_hornetgun" );

	//Sniper rifle
	UTIL_PrecacheOtherWeapon( "weapon_sniperrifle" );
	UTIL_PrecacheOther( "ammo_762" );

	if ( g_pGameRules->IsDeathmatch() )
	{
		UTIL_PrecacheOther( "weaponbox" );// container for dropped deathmatch weapons
	}

	g_sModelIndexFireball = PRECACHE_MODEL ("sprites/zerogxplode.spr");// fireball
	g_sModelIndexWExplosion = PRECACHE_MODEL ("sprites/WXplo1.spr");// underwater fireball
	g_sModelIndexSmoke = PRECACHE_MODEL ("sprites/steam1.spr");// smoke
	g_sModelIndexBubbles = PRECACHE_MODEL ("sprites/bubble.spr");//bubbles
	g_sModelIndexBloodSpray = PRECACHE_MODEL ("sprites/bloodspray.spr"); // initial blood
	g_sModelIndexBloodDrop = PRECACHE_MODEL ("sprites/blood.spr"); // splattered blood 

	g_sModelIndexLaser = PRECACHE_MODEL( (char *)g_pModelNameLaser );
	g_sModelIndexLaserDot = PRECACHE_MODEL("sprites/laserdot.spr");


	// used by explosions
	PRECACHE_MODEL ("models/grenade.mdl");
	PRECACHE_MODEL ("sprites/explode1.spr");

	PRECACHE_SOUND ("weapons/debris1.wav");// explosion aftermaths
	PRECACHE_SOUND ("weapons/debris2.wav");// explosion aftermaths
	PRECACHE_SOUND ("weapons/debris3.wav");// explosion aftermaths

	PRECACHE_SOUND ("weapons/grenade_hit1.wav");//grenade
	PRECACHE_SOUND ("weapons/grenade_hit2.wav");//grenade
	PRECACHE_SOUND ("weapons/grenade_hit3.wav");//grenade

	PRECACHE_SOUND ("weapons/bullet_hit1.wav");	// hit by bullet
	PRECACHE_SOUND ("weapons/bullet_hit2.wav");	// hit by bullet
	
	PRECACHE_SOUND ("items/weapondrop1.wav");// weapon falls to the ground

}