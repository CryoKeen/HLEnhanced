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
#ifndef GAME_SERVER_ENTITIES_NPCS_CLEECH_H
#define GAME_SERVER_ENTITIES_NPCS_CLEECH_H

// Animation events
#define LEECH_AE_ATTACK		1
#define LEECH_AE_FLOP		2

// Movement constants
#define LEECH_ACCELERATE		10
#define LEECH_CHECK_DIST		45
#define LEECH_SWIM_SPEED		50
#define LEECH_SWIM_ACCEL		80
#define LEECH_SWIM_DECEL		10
#define LEECH_TURN_RATE			90
#define LEECH_SIZEX				10
#define LEECH_FRAMETIME			0.1

class CLeech : public CBaseMonster
{
public:
	DECLARE_CLASS( CLeech, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;

	void EXPORT SwimThink( void );
	void EXPORT DeadThink( void );
	void Touch( CBaseEntity *pOther ) override
	{
		if( pOther->IsPlayer() )
		{
			// If the client is pushing me, give me some base velocity
			if( gpGlobals->trace_ent && gpGlobals->trace_ent == edict() )
			{
				pev->basevelocity = pOther->pev->velocity;
				pev->flags |= FL_BASEVELOCITY;
			}
		}
	}

	void SetObjectCollisionBox( void ) override
	{
		pev->absmin = pev->origin + Vector( -8, -8, 0 );
		pev->absmax = pev->origin + Vector( 8, 8, 2 );
	}

	void AttackSound( void );
	void AlertSound( void ) override;
	void UpdateMotion( void );
	float ObstacleDistance( CBaseEntity *pTarget );
	void MakeVectors( void );
	void RecalculateWaterlevel( void );
	void SwitchLeechState( void );

	// Base entity functions
	void HandleAnimEvent( MonsterEvent_t *pEvent ) override;
	int	BloodColor() const override { return DONT_BLEED; }
	void Killed( entvars_t *pevAttacker, GibAction gibAction ) override;
	void Activate( void ) override;
	int TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType ) override;
	int	Classify( void ) override { return CLASS_INSECT; }
	int IRelationship( CBaseEntity *pTarget ) override;

	static const char *pAttackSounds[];
	static const char *pAlertSounds[];

private:
	// UNDONE: Remove unused boid vars, do group behavior
	float	m_flTurning;// is this boid turning?
	bool	m_fPathBlocked;// true if there is an obstacle ahead
	float	m_flAccelerate;
	float	m_obstacle;
	float	m_top;
	float	m_bottom;
	float	m_height;
	float	m_waterTime;
	float	m_sideTime;		// Timer to randomly check clearance on sides
	float	m_zTime;
	float	m_stateTime;
	float	m_attackSoundTime;

#if DEBUG_BEAMS
	CBeam	*m_pb;
	CBeam	*m_pt;
#endif
};

#endif //GAME_SERVER_ENTITIES_NPCS_CLEECH_H