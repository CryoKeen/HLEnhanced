#include "extdll.h"
#include "util.h"
#include "Skill.h"
#include "cbase.h"
#include "entities/CBasePlayer.h"
#include "Weapons.h"

#include "CItem.h"

#include "CItemBattery.h"

extern int gmsgItemPickup;

LINK_ENTITY_TO_CLASS( item_battery, CItemBattery );

void CItemBattery::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), "models/w_battery.mdl" );
	CItem::Spawn();
}

void CItemBattery::Precache( void )
{
	PRECACHE_MODEL( "models/w_battery.mdl" );
	PRECACHE_SOUND( "items/gunpickup2.wav" );
}

bool CItemBattery::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->pev->deadflag != DEAD_NO )
	{
		return false;
	}

	if( ( pPlayer->pev->armorvalue < MAX_NORMAL_BATTERY ) &&
		( pPlayer->pev->weapons & ( 1 << WEAPON_SUIT ) ) )
	{
		int pct;
		char szcharge[ 64 ];

		pPlayer->pev->armorvalue += gSkillData.batteryCapacity;
		pPlayer->pev->armorvalue = min( pPlayer->pev->armorvalue, MAX_NORMAL_BATTERY );

		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING( pev->classname ) );
		MESSAGE_END();


		// Suit reports new power level
		// For some reason this wasn't working in release build -- round it.
		pct = ( int ) ( ( float ) ( pPlayer->pev->armorvalue * 100.0 ) * ( 1.0 / MAX_NORMAL_BATTERY ) + 0.5 );
		pct = ( pct / 5 );
		if( pct > 0 )
			pct--;

		sprintf( szcharge, "!HEV_%1dP", pct );

		//EMIT_SOUND_SUIT(ENT(pev), szcharge);
		pPlayer->SetSuitUpdate( szcharge, SUIT_SENTENCE, SUIT_NEXT_IN_30SEC );
		return true;
	}
	return false;
}