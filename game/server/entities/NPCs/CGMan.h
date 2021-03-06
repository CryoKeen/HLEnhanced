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
#ifndef GAME_SERVER_ENTITIES_NPCS_CGMAN_H
#define GAME_SERVER_ENTITIES_NPCS_CGMAN_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CGMan : public CBaseMonster
{
public:
	DECLARE_CLASS( CGMan, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void SetYawSpeed( void ) override;
	int  Classify( void ) override;
	void HandleAnimEvent( MonsterEvent_t *pEvent ) override;
	int ISoundMask( void ) override;

	void StartTask( Task_t *pTask ) override;
	void RunTask( Task_t *pTask ) override;
	int  TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType ) override;
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) override;

	void PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, const bool bConcurrent, CBaseEntity *pListener ) override;

	EHANDLE m_hPlayer;
	EHANDLE m_hTalkTarget;
	float m_flTalkTime;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CGMAN_H