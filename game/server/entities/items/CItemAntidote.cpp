#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/CBasePlayer.h"
#include "Weapons.h"

#include "CItem.h"

#include "CItemAntidote.h"

LINK_ENTITY_TO_CLASS( item_antidote, CItemAntidote );

void CItemAntidote::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), "models/w_antidote.mdl" );
	CItem::Spawn();
}

void CItemAntidote::Precache( void )
{
	PRECACHE_MODEL( "models/w_antidote.mdl" );
}

bool CItemAntidote::MyTouch( CBasePlayer *pPlayer )
{
	pPlayer->SetSuitUpdate( "!HEV_DET4", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN );

	pPlayer->m_rgItems[ ITEM_ANTIDOTE ] += 1;
	return true;
}