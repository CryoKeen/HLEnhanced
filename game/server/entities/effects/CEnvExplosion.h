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
#ifndef GAME_SERVER_ENTITIES_EFFECTS_CENVEXPLOSION_H
#define GAME_SERVER_ENTITIES_EFFECTS_CENVEXPLOSION_H

#define	SF_ENVEXPLOSION_NODAMAGE	( 1 << 0 ) // when set, ENV_EXPLOSION will not actually inflict damage
#define	SF_ENVEXPLOSION_REPEATABLE	( 1 << 1 ) // can this entity be refired?
#define SF_ENVEXPLOSION_NOFIREBALL	( 1 << 2 ) // don't draw the fireball
#define SF_ENVEXPLOSION_NOSMOKE		( 1 << 3 ) // don't draw the smoke
#define SF_ENVEXPLOSION_NODECAL		( 1 << 4 ) // don't make a scorch mark
#define SF_ENVEXPLOSION_NOSPARKS	( 1 << 5 ) // don't make a scorch mark

class CEnvExplosion : public CBaseMonster
{
public:
	DECLARE_CLASS( CEnvExplosion, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn() override;
	void EXPORT Smoke( void );
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;

	int m_iMagnitude;// how large is the fireball? how much damage?
	int m_spriteScale; // what's the exact fireball sprite scale? 
};

extern DLL_GLOBAL	short	g_sModelIndexFireball;
extern DLL_GLOBAL	short	g_sModelIndexSmoke;

extern void ExplosionCreate( const Vector &center, const Vector &angles, edict_t *pOwner, int magnitude, const bool doDamage );

#endif //GAME_SERVER_ENTITIES_EFFECTS_CENVEXPLOSION_H
