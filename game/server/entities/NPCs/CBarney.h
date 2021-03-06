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
#ifndef GAME_SERVER_ENTITIES_NPCS_CBARNEY_H
#define GAME_SERVER_ENTITIES_NPCS_CBARNEY_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
// first flag is barney dying for scripted sequences?
#define		BARNEY_AE_DRAW		( 2 )
#define		BARNEY_AE_SHOOT		( 3 )
#define		BARNEY_AE_HOLSTER	( 4 )

#define	BARNEY_BODY_GUNHOLSTERED	0
#define	BARNEY_BODY_GUNDRAWN		1
#define BARNEY_BODY_GUNGONE			2

class CBarney : public CTalkMonster
{
public:
	DECLARE_CLASS( CBarney, CTalkMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	int  ISoundMask( void ) override;
	void BarneyFirePistol( void );
	void AlertSound( void ) override;
	int  Classify( void ) override;
	void HandleAnimEvent( MonsterEvent_t *pEvent ) override;

	void RunTask( Task_t *pTask ) override;
	void StartTask( Task_t *pTask ) override;
	virtual int	ObjectCaps() const override { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	int TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;

	void DeclineFollowing( void ) override;

	// Override these to set behavior
	Schedule_t *GetScheduleOfType( int Type ) override;
	Schedule_t *GetSchedule( void ) override;
	MONSTERSTATE GetIdealState( void ) override;

	void DeathSound( void ) override;
	void PainSound( void ) override;

	void TalkInit( void );

	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) override;
	void Killed( entvars_t *pevAttacker, GibAction gibAction ) override;

	bool	m_fGunDrawn;
	float	m_painTime;
	float	m_checkAttackTime;
	bool	m_lastAttackCheck;

	// UNDONE: What is this for?  It isn't used?
	float	m_flPlayerDamage;// how much pain has the player inflicted on me?

	CUSTOM_SCHEDULES;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBARNEY_H