#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CTentacleMaw.h"

LINK_ENTITY_TO_CLASS( monster_tentaclemaw, CTentacleMaw );

//
// Tentacle Spawn
//
void CTentacleMaw::Spawn()
{
	Precache();
	SET_MODEL( ENT( pev ), "models/maw.mdl" );
	UTIL_SetSize( pev, Vector( -32, -32, 0 ), Vector( 32, 32, 64 ) );

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 75;
	pev->yaw_speed = 8;
	pev->sequence = 0;

	pev->angles.x = 90;
	// ResetSequenceInfo( );
}

void CTentacleMaw::Precache()
{
	PRECACHE_MODEL( "models/maw.mdl" );
}