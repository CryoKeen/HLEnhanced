#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "CFrictionModifier.h"

// Global Savedata for changelevel friction modifier
BEGIN_DATADESC( CFrictionModifier )
	DEFINE_FIELD( m_frictionFraction, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_friction, CFrictionModifier );

// Modify an entity's friction
void CFrictionModifier::Spawn( void )
{
	pev->solid = SOLID_TRIGGER;
	SET_MODEL( ENT( pev ), STRING( pev->model ) );    // set size and link into world
	pev->movetype = MOVETYPE_NONE;
	SetTouch( &CFrictionModifier::ChangeFriction );
}

// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "modifier" ) )
	{
		m_frictionFraction = atof( pkvd->szValue ) / 100.0;
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier::ChangeFriction( CBaseEntity *pOther )
{
	if( pOther->pev->movetype != MOVETYPE_BOUNCEMISSILE && pOther->pev->movetype != MOVETYPE_BOUNCE )
		pOther->pev->friction = m_frictionFraction;
}