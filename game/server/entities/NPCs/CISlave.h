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
#ifndef GAME_SERVER_ENTITIES_NPCS_CISLAVE_H
#define GAME_SERVER_ENTITIES_NPCS_CISLAVE_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		ISLAVE_AE_CLAW		( 1 )
#define		ISLAVE_AE_CLAWRAKE	( 2 )
#define		ISLAVE_AE_ZAP_POWERUP	( 3 )
#define		ISLAVE_AE_ZAP_SHOOT		( 4 )
#define		ISLAVE_AE_ZAP_DONE		( 5 )

#define		ISLAVE_MAX_BEAMS	8

class CISlave : public CSquadMonster
{
public:
	DECLARE_CLASS( CISlave, CSquadMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	int	 ISoundMask( void ) override;
	int  Classify( void ) override;
	int  IRelationship( CBaseEntity *pTarget ) override;
	void HandleAnimEvent( MonsterEvent_t *pEvent ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;
	bool CheckRangeAttack2( float flDot, float flDist ) override;
	void CallForHelp( char *szClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) override;
	int TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType ) override;

	void DeathSound( void ) override;
	void PainSound( void ) override;
	void AlertSound( void ) override;
	void IdleSound( void ) override;

	void Killed( entvars_t *pevAttacker, GibAction gibAction ) override;

	void StartTask( Task_t *pTask ) override;
	Schedule_t *GetSchedule( void ) override;
	Schedule_t *GetScheduleOfType( int Type ) override;
	CUSTOM_SCHEDULES;

	void ClearBeams();
	void ArmBeam( int side );
	void WackBeam( int side, CBaseEntity *pEntity );
	void ZapBeam( int side );
	void BeamGlow( void );

	int m_iBravery;

	CBeam *m_pBeam[ ISLAVE_MAX_BEAMS ];

	int m_iBeams;
	float m_flNextAttack;

	int	m_voicePitch;

	EHANDLE m_hDead;

	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];
	static const char *pPainSounds[];
	static const char *pDeathSounds[];
};

#endif //GAME_SERVER_ENTITIES_NPCS_CISLAVE_H