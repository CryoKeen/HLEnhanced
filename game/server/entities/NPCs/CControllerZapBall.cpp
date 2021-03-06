/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#include "extdll.h"
#include "util.h"
#include "Skill.h"
#include "cbase.h"
#include "Weapons.h"

#include "CControllerZapBall.h"

LINK_ENTITY_TO_CLASS( controller_energy_ball, CControllerZapBall );

void CControllerZapBall::Spawn( void )
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL( ENT( pev ), "sprites/xspark4.spr" );
	pev->rendermode = kRenderTransAdd;
	pev->rendercolor.x = 255;
	pev->rendercolor.y = 255;
	pev->rendercolor.z = 255;
	pev->renderamt = 255;
	pev->scale = 0.5;

	UTIL_SetSize( pev, Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	UTIL_SetOrigin( pev, pev->origin );

	SetThink( &CControllerZapBall::AnimateThink );
	SetTouch( &CControllerZapBall::ExplodeTouch );

	m_hOwner = Instance( pev->owner );
	pev->dmgtime = gpGlobals->time; // keep track of when ball spawned
	pev->nextthink = gpGlobals->time + 0.1;
}

void CControllerZapBall::Precache( void )
{
	PRECACHE_MODEL( "sprites/xspark4.spr" );
	// PRECACHE_SOUND("debris/zap4.wav");
	// PRECACHE_SOUND("weapons/electro4.wav");
}

void CControllerZapBall::AnimateThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	pev->frame = ( ( int ) pev->frame + 1 ) % 11;

	if( gpGlobals->time - pev->dmgtime > 5 || pev->velocity.Length() < 10 )
	{
		SetTouch( NULL );
		UTIL_Remove( this );
	}
}

void CControllerZapBall::ExplodeTouch( CBaseEntity *pOther )
{
	if( pOther->pev->takedamage )
	{
		TraceResult tr = UTIL_GetGlobalTrace();

		CBaseEntity* pOwner;
		if( m_hOwner != nullptr )
		{
			pOwner = m_hOwner;
		}
		else
		{
			pOwner = this;
		}

		g_MultiDamage.Clear();
		pOther->TraceAttack( pOwner->pev, gSkillData.controllerDmgBall, pev->velocity.Normalize(), &tr, DMG_ENERGYBEAM );
		g_MultiDamage.ApplyMultiDamage( pOwner, pOwner );

		UTIL_EmitAmbientSound( ENT( pev ), tr.vecEndPos, "weapons/electro4.wav", 0.3, ATTN_NORM, 0, RANDOM_LONG( 90, 99 ) );

	}

	UTIL_Remove( this );
}