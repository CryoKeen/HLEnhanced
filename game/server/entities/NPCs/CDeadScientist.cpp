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
#include "cbase.h"
#include "CTalkMonster.h"

#include "CScientist.h"

#include "CDeadScientist.h"

char *CDeadScientist::m_szPoses[] = { "lying_on_back", "lying_on_stomach", "dead_sitting", "dead_hang", "dead_table1", "dead_table2", "dead_table3" };

LINK_ENTITY_TO_CLASS( monster_scientist_dead, CDeadScientist );

//
// ********** DeadScientist SPAWN **********
//
void CDeadScientist::Spawn()
{
	PRECACHE_MODEL( "models/scientist.mdl" );
	SET_MODEL( ENT( pev ), "models/scientist.mdl" );

	pev->effects = 0;
	pev->sequence = 0;
	// Corpses have less health
	pev->health = 8;//gSkillData.scientistHealth;

	m_bloodColor = BLOOD_COLOR_RED;

	if( pev->body == -1 )
	{// -1 chooses a random head
		pev->body = RANDOM_LONG( 0, NUM_SCIENTIST_HEADS - 1 );// pick a head, any head
	}
	// Luther is black, make his hands black
	if( pev->body == HEAD_LUTHER )
		pev->skin = 1;
	else
		pev->skin = 0;

	pev->sequence = LookupSequence( m_szPoses[ m_iPose ] );
	if( pev->sequence == -1 )
	{
		ALERT( at_console, "Dead scientist with bad pose\n" );
	}

	//	pev->skin += 2; // use bloody skin -- UNDONE: Turn this back on when we have a bloody skin again!
	MonsterInitDead();
}

void CDeadScientist::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}