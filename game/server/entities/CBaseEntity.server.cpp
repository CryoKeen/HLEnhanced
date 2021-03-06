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
#include "extdll.h"
#include "util.h"
#include "ServerInterface.h"

#include "Decals.h"

#include "cbase.h"

extern DLL_GLOBAL Vector		g_vecAttackDir;

void SetObjectCollisionBox( entvars_t *pev );

// give health
float CBaseEntity::GiveHealth( float flHealth, int bitsDamageType )
{
	if( !pev->takedamage )
		return 0;

	// heal
	if( pev->health >= pev->max_health )
		return 0;

	const float flOldHealth = pev->health;

	pev->health += flHealth;

	//TODO: if the entity's health drops below 1, kill it. - Solokiller

	if( pev->health > pev->max_health )
		pev->health = pev->max_health;

	return pev->health - flOldHealth;
}

// inflict damage on this entity.  bitsDamageType indicates type of damage inflicted, ie: DMG_CRUSH

int CBaseEntity::TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType )
{
	Vector			vecTemp;

	if( !pev->takedamage )
		return 0;

	// UNDONE: some entity types may be immune or resistant to some bitsDamageType

	// if Attacker == Inflictor, the attack was a melee or other instant-hit attack.
	// (that is, no actual entity projectile was involved in the attack so use the shooter's origin). 
	if( pInflictor == pAttacker )
	{
		vecTemp = pInflictor->pev->origin - ( VecBModelOrigin( pev ) );
	}
	else
		// an actual missile was involved.
	{
		vecTemp = pInflictor->pev->origin - ( VecBModelOrigin( pev ) );
	}

	// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();

	// save damage based on the target's armor level

	// figure momentum add (don't let hurt brushes or other triggers move player)
	if( ( !FNullEnt( pInflictor ) ) && ( pev->movetype == MOVETYPE_WALK || pev->movetype == MOVETYPE_STEP ) && ( pAttacker->pev->solid != SOLID_TRIGGER ) )
	{
		Vector vecDir = pev->origin - ( pInflictor->pev->absmin + pInflictor->pev->absmax ) * 0.5;
		vecDir = vecDir.Normalize();

		float flForce = flDamage * ( ( 32 * 32 * 72.0 ) / ( pev->size.x * pev->size.y * pev->size.z ) ) * 5;

		if( flForce > 1000.0 )
			flForce = 1000.0;
		pev->velocity = pev->velocity + vecDir * flForce;
	}

	// do the damage
	pev->health -= flDamage;
	if( pev->health <= 0 )
	{
		Killed( pAttacker->pev, GIB_NORMAL );
		return 0;
	}

	return 1;
}

void CBaseEntity::Killed( entvars_t *pevAttacker, GibAction gibAction )
{
	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;
	UTIL_Remove( this );
}


CBaseEntity *CBaseEntity::GetNextTarget( void )
{
	if( FStringNull( pev->target ) )
		return NULL;
	edict_t *pTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING( pev->target ) );
	if( FNullEnt( pTarget ) )
		return NULL;

	return Instance( pTarget );
}

bool CBaseEntity::Save( CSave& save )
{
	if( save.WriteEntVars( "ENTVARS", pev ) )
	{
		const DataMap_t* pDataMap = GetDataMap();

		bool bResult = true;

		while( pDataMap )
		{
			bResult = save.WriteFields( pDataMap->pszClassName, this, pDataMap->pTypeDesc, pDataMap->uiNumDescriptors );

			if( !bResult )
				return false;

			pDataMap = pDataMap->pParent;
		}

		return bResult;
	}

	return false;
}

bool CBaseEntity::Restore( CRestore &restore )
{
	bool bResult = restore.ReadEntVars( "ENTVARS", pev );

	if( bResult )
	{
		const DataMap_t* pDataMap = GetDataMap();

		while( pDataMap )
		{
			bResult = restore.ReadFields( pDataMap->pszClassName, this, pDataMap->pTypeDesc, pDataMap->uiNumDescriptors );

			if( !bResult )
				break;

			pDataMap = pDataMap->pParent;
		}
	}

	if( pev->modelindex != 0 && !FStringNull( pev->model ) )
	{
		Vector mins, maxs;
		mins = pev->mins;	// Set model is about to destroy these
		maxs = pev->maxs;


		PRECACHE_MODEL( ( char * ) STRING( pev->model ) );
		SET_MODEL( ENT( pev ), STRING( pev->model ) );
		UTIL_SetSize( pev, mins, maxs );	// Reset them
	}

	return bResult;
}

void CBaseEntity::SetObjectCollisionBox( void )
{
	::SetObjectCollisionBox( pev );
}

bool CBaseEntity::Intersects( const CBaseEntity* const pOther ) const
{
	if( pOther->pev->absmin.x > pev->absmax.x ||
		pOther->pev->absmin.y > pev->absmax.y ||
		pOther->pev->absmin.z > pev->absmax.z ||
		pOther->pev->absmax.x < pev->absmin.x ||
		pOther->pev->absmax.y < pev->absmin.y ||
		pOther->pev->absmax.z < pev->absmin.z )
		return false;
	return true;
}

void CBaseEntity::MakeDormant( void )
{
	SetBits( pev->flags, FL_DORMANT );

	// Don't touch
	pev->solid = SOLID_NOT;
	// Don't move
	pev->movetype = MOVETYPE_NONE;
	// Don't draw
	SetBits( pev->effects, EF_NODRAW );
	// Don't think
	pev->nextthink = 0;
	// Relink
	UTIL_SetOrigin( pev, pev->origin );
}

bool CBaseEntity::IsDormant() const
{
	return FBitSet( pev->flags, FL_DORMANT ) != 0;
}

bool CBaseEntity::IsInWorld() const
{
	//TODO: use constants. - Solokiller
	// position 
	if( pev->origin.x >= 4096 ) return false;
	if( pev->origin.y >= 4096 ) return false;
	if( pev->origin.z >= 4096 ) return false;
	if( pev->origin.x <= -4096 ) return false;
	if( pev->origin.y <= -4096 ) return false;
	if( pev->origin.z <= -4096 ) return false;
	// speed
	if( pev->velocity.x >= 2000 ) return false;
	if( pev->velocity.y >= 2000 ) return false;
	if( pev->velocity.z >= 2000 ) return false;
	if( pev->velocity.x <= -2000 ) return false;
	if( pev->velocity.y <= -2000 ) return false;
	if( pev->velocity.z <= -2000 ) return false;

	return true;
}

bool CBaseEntity::ShouldToggle( USE_TYPE useType, const bool currentState ) const
{
	if( useType != USE_TOGGLE && useType != USE_SET )
	{
		if( ( currentState && useType == USE_ON ) || ( !currentState && useType == USE_OFF ) )
			return false;
	}
	return true;
}


int	CBaseEntity::DamageDecal( int bitsDamageType ) const
{
	if( pev->rendermode == kRenderTransAlpha )
		return -1;

	if( pev->rendermode != kRenderNormal )
		return DECAL_BPROOF1;

	return DECAL_GUNSHOT1 + RANDOM_LONG( 0, 4 );
}



// NOTE: szName must be a pointer to constant memory, e.g. "monster_class" because the entity
// will keep a pointer to it after this call.
CBaseEntity * CBaseEntity::Create( char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner )
{
	edict_t	*pent;
	CBaseEntity *pEntity;

	pent = CREATE_NAMED_ENTITY( MAKE_STRING( szName ) );
	if( FNullEnt( pent ) )
	{
		ALERT( at_console, "NULL Ent in Create!\n" );
		return NULL;
	}
	pEntity = Instance( pent );
	pEntity->pev->owner = pentOwner;
	pEntity->pev->origin = vecOrigin;
	pEntity->pev->angles = vecAngles;
	DispatchSpawn( pEntity->edict() );
	return pEntity;
}


