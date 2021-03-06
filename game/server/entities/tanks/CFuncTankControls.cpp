#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFuncTank.h"

#include "CFuncTankControls.h"

BEGIN_DATADESC( CFuncTankControls )
	DEFINE_FIELD( m_pTank, FIELD_CLASSPTR ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_tankcontrols, CFuncTankControls );

int	CFuncTankControls::ObjectCaps() const
{
	return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | FCAP_IMPULSE_USE;
}

void CFuncTankControls::Spawn( void )
{
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	pev->effects |= EF_NODRAW;
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );

	pev->nextthink = gpGlobals->time + 0.3;	// After all the func_tank's have spawned

	CBaseEntity::Spawn();
}

void CFuncTankControls::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ // pass the Use command onto the controls
	if( m_pTank )
		m_pTank->Use( pActivator, pCaller, useType, value );

	ASSERT( m_pTank != NULL );	// if this fails,  most likely means save/restore hasn't worked properly
}

void CFuncTankControls::Think( void )
{
	edict_t *pTarget = NULL;

	do
	{
		pTarget = FIND_ENTITY_BY_TARGETNAME( pTarget, STRING( pev->target ) );
	}
	while( !FNullEnt( pTarget ) && strncmp( STRING( pTarget->v.classname ), "func_tank", 9 ) );

	if( FNullEnt( pTarget ) )
	{
		ALERT( at_console, "No tank %s\n", STRING( pev->target ) );
		return;
	}

	m_pTank = ( CFuncTank* ) Instance( pTarget );
}