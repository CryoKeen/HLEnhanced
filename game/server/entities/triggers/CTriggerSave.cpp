#include "extdll.h"
#include "util.h"
#include "gamerules/GameRules.h"
#include "cbase.h"

#include "CTriggerSave.h"

LINK_ENTITY_TO_CLASS( trigger_autosave, CTriggerSave );

void CTriggerSave::Spawn( void )
{
	if( g_pGameRules->IsDeathmatch() )
	{
		REMOVE_ENTITY( ENT( pev ) );
		return;
	}

	InitTrigger();
	SetTouch( &CTriggerSave::SaveTouch );
}

void CTriggerSave::SaveTouch( CBaseEntity *pOther )
{
	if( !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
		return;

	// Only save on clients
	if( !pOther->IsPlayer() )
		return;

	SetTouch( NULL );
	UTIL_Remove( this );
	SERVER_COMMAND( "autosave\n" );
}