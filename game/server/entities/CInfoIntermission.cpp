#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CInfoIntermission.h"

LINK_ENTITY_TO_CLASS( info_intermission, CInfoIntermission );

void CInfoIntermission::Spawn( void )
{
	UTIL_SetOrigin( pev, pev->origin );
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->v_angle = g_vecZero;

	pev->nextthink = gpGlobals->time + 2;// let targets spawn!

}

void CInfoIntermission::Think( void )
{
	edict_t *pTarget;

	// find my target
	pTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING( pev->target ) );

	if( !FNullEnt( pTarget ) )
	{
		pev->v_angle = UTIL_VecToAngles( ( pTarget->v.origin - pev->origin ).Normalize() );
		pev->v_angle.x = -pev->v_angle.x;
	}
}