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

#include "CControllerHeadBall.h"

LINK_ENTITY_TO_CLASS( controller_head_ball, CControllerHeadBall );

void CControllerHeadBall::Spawn( void )
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
	pev->scale = 2.0;

	UTIL_SetSize( pev, Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );
	UTIL_SetOrigin( pev, pev->origin );

	SetThink( &CControllerHeadBall::HuntThink );
	SetTouch( &CControllerHeadBall::BounceTouch );

	m_vecIdeal = Vector( 0, 0, 0 );

	pev->nextthink = gpGlobals->time + 0.1;

	m_hOwner = Instance( pev->owner );
	pev->dmgtime = gpGlobals->time;
}

void CControllerHeadBall::Precache( void )
{
	PRECACHE_MODEL( "sprites/xspark1.spr" );
	PRECACHE_SOUND( "debris/zap4.wav" );
	PRECACHE_SOUND( "weapons/electro4.wav" );
}

void CControllerHeadBall::HuntThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	pev->renderamt -= 5;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_ELIGHT );
	WRITE_SHORT( entindex() );		// entity, attachment
	WRITE_COORD( pev->origin.x );		// origin
	WRITE_COORD( pev->origin.y );
	WRITE_COORD( pev->origin.z );
	WRITE_COORD( pev->renderamt / 16 );	// radius
	WRITE_BYTE( 255 );	// R
	WRITE_BYTE( 255 );	// G
	WRITE_BYTE( 255 );	// B
	WRITE_BYTE( 2 );	// life * 10
	WRITE_COORD( 0 ); // decay
	MESSAGE_END();

	// check world boundaries
	if( gpGlobals->time - pev->dmgtime > 5 || pev->renderamt < 64 || m_hEnemy == NULL || m_hOwner == NULL || pev->origin.x < -4096 || pev->origin.x > 4096 || pev->origin.y < -4096 || pev->origin.y > 4096 || pev->origin.z < -4096 || pev->origin.z > 4096 )
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}

	MovetoTarget( m_hEnemy->Center() );

	if( ( m_hEnemy->Center() - pev->origin ).Length() < 64 )
	{
		TraceResult tr;

		UTIL_TraceLine( pev->origin, m_hEnemy->Center(), dont_ignore_monsters, ENT( pev ), &tr );

		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if( pEntity != NULL && pEntity->pev->takedamage )
		{
			g_MultiDamage.Clear();
			pEntity->TraceAttack( m_hOwner->pev, gSkillData.controllerDmgZap, pev->velocity, &tr, DMG_SHOCK );
			g_MultiDamage.ApplyMultiDamage( this, m_hOwner );
		}

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMENTPOINT );
		WRITE_SHORT( entindex() );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_SHORT( g_sModelIndexLaser );
		WRITE_BYTE( 0 ); // frame start
		WRITE_BYTE( 10 ); // framerate
		WRITE_BYTE( 3 ); // life
		WRITE_BYTE( 20 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 10 );		// speed
		MESSAGE_END();

		UTIL_EmitAmbientSound( ENT( pev ), tr.vecEndPos, "weapons/electro4.wav", 0.5, ATTN_NORM, 0, RANDOM_LONG( 140, 160 ) );

		m_flNextAttack = gpGlobals->time + 3.0;

		SetThink( &CControllerHeadBall::DieThink );
		pev->nextthink = gpGlobals->time + 0.3;
	}

	// Crawl( );
}

void CControllerHeadBall::DieThink( void )
{
	UTIL_Remove( this );
}

void CControllerHeadBall::BounceTouch( CBaseEntity *pOther )
{
	Vector vecDir = m_vecIdeal.Normalize();

	TraceResult tr = UTIL_GetGlobalTrace();

	float n = -DotProduct( tr.vecPlaneNormal, vecDir );

	vecDir = 2.0 * tr.vecPlaneNormal * n + vecDir;

	m_vecIdeal = vecDir * m_vecIdeal.Length();
}

void CControllerHeadBall::MovetoTarget( Vector vecTarget )
{
	// accelerate
	float flSpeed = m_vecIdeal.Length();
	if( flSpeed == 0 )
	{
		m_vecIdeal = pev->velocity;
		flSpeed = m_vecIdeal.Length();
	}

	if( flSpeed > 400 )
	{
		m_vecIdeal = m_vecIdeal.Normalize() * 400;
	}
	m_vecIdeal = m_vecIdeal + ( vecTarget - pev->origin ).Normalize() * 100;
	pev->velocity = m_vecIdeal;
}

void CControllerHeadBall::Crawl( void )
{

	Vector vecAim = Vector( RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ), RANDOM_FLOAT( -1, 1 ) ).Normalize();
	Vector vecPnt = pev->origin + pev->velocity * 0.3 + vecAim * 64;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BEAMENTPOINT );
	WRITE_SHORT( entindex() );
	WRITE_COORD( vecPnt.x );
	WRITE_COORD( vecPnt.y );
	WRITE_COORD( vecPnt.z );
	WRITE_SHORT( g_sModelIndexLaser );
	WRITE_BYTE( 0 ); // frame start
	WRITE_BYTE( 10 ); // framerate
	WRITE_BYTE( 3 ); // life
	WRITE_BYTE( 20 );  // width
	WRITE_BYTE( 0 );   // noise
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );   // r, g, b
	WRITE_BYTE( 255 );	// brightness
	WRITE_BYTE( 10 );		// speed
	MESSAGE_END();
}