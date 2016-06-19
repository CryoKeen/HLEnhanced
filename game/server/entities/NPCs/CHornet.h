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
//=========================================================
// Hornets
//=========================================================

//=========================================================
// Hornet Defines
//=========================================================
#define HORNET_TYPE_RED			0
#define HORNET_TYPE_ORANGE		1
#define HORNET_RED_SPEED		(float)600
#define HORNET_ORANGE_SPEED		(float)800
#define	HORNET_BUZZ_VOLUME		(float)0.8

extern int iHornetPuff;

//=========================================================
// Hornet - this is the projectile that the Alien Grunt fires.
//=========================================================
class CHornet : public CBaseMonster
{
public:
	DECLARE_CLASS( CHornet, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	int	 Classify ( void ) override;
	int  IRelationship ( CBaseEntity *pTarget ) override;

	void IgniteTrail( void );
	void EXPORT StartTrack ( void );
	void EXPORT StartDart ( void );
	void EXPORT TrackTarget ( void );
	void EXPORT TrackTouch ( CBaseEntity *pOther );
	void EXPORT DartTouch( CBaseEntity *pOther );
	void EXPORT DieTouch ( CBaseEntity *pOther );
	
	int TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType ) override;

	float			m_flStopAttack;
	int				m_iHornetType;
	float			m_flFlySpeed;
};
