#include "extdll.h"
#include "util.h"
#include "Skill.h"
#include "gamerules/GameRules.h"
#include "cbase.h"
#include "entities/CBasePlayer.h"

#include "CItem.h"

#include "CHealthKit.h"

extern int gmsgItemPickup;

LINK_ENTITY_TO_CLASS( item_healthkit, CHealthKit );

void CHealthKit::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), "models/w_medkit.mdl" );

	CItem::Spawn();
}

void CHealthKit::Precache( void )
{
	PRECACHE_MODEL( "models/w_medkit.mdl" );
	PRECACHE_SOUND( "items/smallmedkit1.wav" );
}

bool CHealthKit::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->pev->deadflag != DEAD_NO )
	{
		return false;
	}

	if( pPlayer->GiveHealth( gSkillData.healthkitCapacity, DMG_GENERIC ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING( pev->classname ) );
		MESSAGE_END();

		EMIT_SOUND( ENT( pPlayer->pev ), CHAN_ITEM, "items/smallmedkit1.wav", 1, ATTN_NORM );

		if( g_pGameRules->ItemShouldRespawn( this ) )
		{
			Respawn();
		}
		else
		{
			UTIL_Remove( this );
		}

		return true;
	}

	return false;
}