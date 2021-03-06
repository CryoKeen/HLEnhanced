#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/CBasePlayer.h"

#include "CItem.h"

#include "CItemLongJump.h"

extern int gmsgItemPickup;

LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );

void CItemLongJump::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), "models/w_longjump.mdl" );
	CItem::Spawn();
}

void CItemLongJump::Precache( void )
{
	PRECACHE_MODEL( "models/w_longjump.mdl" );
}

bool CItemLongJump::MyTouch( CBasePlayer *pPlayer )
{
	if( pPlayer->m_fLongJump )
	{
		return false;
	}

	if( ( pPlayer->pev->weapons & ( 1 << WEAPON_SUIT ) ) )
	{
		pPlayer->m_fLongJump = true;// player now has longjump module

		g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "1" );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING( pev->classname ) );
		MESSAGE_END();

		EMIT_SOUND_SUIT( pPlayer->edict(), "!HEV_A1" );	// Play the longjump sound UNDONE: Kelly? correct sound?
		return true;
	}
	return false;
}