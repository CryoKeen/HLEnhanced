/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"

// CBaseButton
BEGIN_DATADESC( CBaseButton )
	DEFINE_FIELD( m_fStayPushed, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fRotating, FIELD_BOOLEAN ),
	
	DEFINE_FIELD( m_sounds, FIELD_INTEGER ),
	DEFINE_FIELD( m_bLockedSound, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bLockedSentence, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bUnlockedSound, FIELD_CHARACTER ),
	DEFINE_FIELD( m_bUnlockedSentence, FIELD_CHARACTER ),
	DEFINE_FIELD( m_strChangeTarget, FIELD_STRING ),
	//DEFINE_FIELD( m_ls, FIELD_??? ),   // This is restored in Precache()
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_button, CBaseButton );

void CBaseButton::Spawn()
{
	const char* pszSound;

	//----------------------------------------------------
	//determine sounds for buttons
	//a sound of 0 should not make a sound
	//----------------------------------------------------
	pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND( pszSound );
	pev->noise = ALLOC_STRING( pszSound );

	Precache();

	if( FBitSet( pev->spawnflags, SF_BUTTON_SPARK_IF_OFF ) )// this button should spark in OFF state
	{
		SetThink( &CBaseButton::ButtonSpark );
		pev->nextthink = gpGlobals->time + 0.5;// no hurry, make sure everything else spawns
	}

	SetMovedir( pev );

	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if( pev->speed == 0 )
		pev->speed = 40;

	if( pev->health > 0 )
	{
		pev->takedamage = DAMAGE_YES;
	}

	if( m_flWait == 0 )
		m_flWait = 1;
	if( m_flLip == 0 )
		m_flLip = 4;

	m_toggle_state = TS_AT_BOTTOM;
	m_vecPosition1 = pev->origin;
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * ( fabs( pev->movedir.x * ( pev->size.x - 2 ) ) + fabs( pev->movedir.y * ( pev->size.y - 2 ) ) + fabs( pev->movedir.z * ( pev->size.z - 2 ) ) - m_flLip ) );


	// Is this a non-moving button?
	if( ( ( m_vecPosition2 - m_vecPosition1 ).Length() < 1 ) || ( pev->spawnflags & SF_BUTTON_DONTMOVE ) )
		m_vecPosition2 = m_vecPosition1;

	m_fStayPushed = m_flWait == -1;
	m_fRotating = false;

	// if the button is flagged for USE button activation only, take away it's touch function and add a use function

	if( FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) ) // touchable button
	{
		SetTouch( &CBaseButton::ButtonTouch );
	}
	else
	{
		SetTouch( NULL );
		SetUse( &CBaseButton::ButtonUse );
	}
}

void CBaseButton::Precache( void )
{
	const char* pszSound;

	if( FBitSet( pev->spawnflags, SF_BUTTON_SPARK_IF_OFF ) )// this button should spark in OFF state
	{
		PRECACHE_SOUND( "buttons/spark1.wav" );
		PRECACHE_SOUND( "buttons/spark2.wav" );
		PRECACHE_SOUND( "buttons/spark3.wav" );
		PRECACHE_SOUND( "buttons/spark4.wav" );
		PRECACHE_SOUND( "buttons/spark5.wav" );
		PRECACHE_SOUND( "buttons/spark6.wav" );
	}

	// get door button sounds, for doors which require buttons to open

	if( m_bLockedSound )
	{
		pszSound = ButtonSound( ( int ) m_bLockedSound );
		PRECACHE_SOUND( pszSound );
		m_ls.sLockedSound = ALLOC_STRING( pszSound );
	}

	if( m_bUnlockedSound )
	{
		pszSound = ButtonSound( ( int ) m_bUnlockedSound );
		PRECACHE_SOUND( pszSound );
		m_ls.sUnlockedSound = ALLOC_STRING( pszSound );
	}

	// get sentence group names, for doors which are directly 'touched' to open
	pszSound = LockedSentence( m_bLockedSentence );

	if( pszSound )
		m_ls.sLockedSentence = MAKE_STRING( pszSound );
	else
		m_ls.sLockedSentence = iStringNull;

	pszSound = UnlockedSentence( m_bUnlockedSentence );

	if( pszSound )
		m_ls.sUnlockedSentence = MAKE_STRING( pszSound );
	else
		m_ls.sUnlockedSentence = iStringNull;
}

//
// Cache user-entity-field values until spawn is called.
//

void CBaseButton::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "changetarget" ) )
	{
		m_strChangeTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "locked_sound" ) )
	{
		m_bLockedSound = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "locked_sentence" ) )
	{
		m_bLockedSentence = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "unlocked_sound" ) )
	{
		m_bUnlockedSound = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "unlocked_sentence" ) )
	{
		m_bUnlockedSentence = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

//
// Starts the button moving "in/up".
//
void CBaseButton::ButtonActivate()
{
	EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char* ) STRING( pev->noise ), 1, ATTN_NORM );

	if( !UTIL_IsMasterTriggered( m_sMaster, m_hActivator ) )
	{
		// button is locked, play locked sound
		PlayLockSounds( pev, &m_ls, true, true );
		return;
	}
	else
	{
		// button is unlocked, play unlocked sound
		PlayLockSounds( pev, &m_ls, false, true );
	}

	ASSERT( m_toggle_state == TS_AT_BOTTOM );
	m_toggle_state = TS_GOING_UP;

	SetMoveDone( &CBaseButton::TriggerAndWait );
	if( !m_fRotating )
		LinearMove( m_vecPosition2, pev->speed );
	else
		AngularMove( m_vecAngle2, pev->speed );
}

//
// Touching a button simply "activates" it.
//
void CBaseButton::ButtonTouch( CBaseEntity *pOther )
{
	// Ignore touches by anything but players
	if( !FClassnameIs( pOther->pev, "player" ) )
		return;

	m_hActivator = pOther;

	BUTTON_CODE code = ButtonResponseToTouch();

	if( code == BUTTON_NOTHING )
		return;

	if( !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
	{
		// play button locked sound
		PlayLockSounds( pev, &m_ls, true, true );
		return;
	}

	// Temporarily disable the touch function, until movement is finished.
	SetTouch( NULL );

	if( code == BUTTON_RETURN )
	{
		EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char* ) STRING( pev->noise ), 1, ATTN_NORM );
		SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
		ButtonReturn();
	}
	else	// code == BUTTON_ACTIVATE
		ButtonActivate();
}

void CBaseButton::ButtonSpark( void )
{
	SetThink( &CBaseButton::ButtonSpark );
	pev->nextthink = gpGlobals->time + ( 0.1 + RANDOM_FLOAT( 0, 1.5 ) );// spark again at random interval

	DoSpark( pev, pev->mins );
}

//
// Button has reached the "in/up" position.  Activate its "targets", and pause before "popping out".
//
void CBaseButton::TriggerAndWait( void )
{
	ASSERT( m_toggle_state == TS_GOING_UP );

	if( !UTIL_IsMasterTriggered( m_sMaster, m_hActivator ) )
		return;

	m_toggle_state = TS_AT_TOP;

	// If button automatically comes back out, start it moving out.
	// Else re-instate touch method
	if( m_fStayPushed || FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) )
	{
		if( !FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) ) // this button only works if USED, not touched!
		{
			// ALL buttons are now use only
			SetTouch( NULL );
		}
		else
			SetTouch( &CBaseButton::ButtonTouch );
	}
	else
	{
		pev->nextthink = pev->ltime + m_flWait;
		SetThink( &CBaseButton::ButtonReturn );
	}

	pev->frame = 1;			// use alternate textures


	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
}

//
// Starts the button moving "out/down".
//
void CBaseButton::ButtonReturn( void )
{
	ASSERT( m_toggle_state == TS_AT_TOP );
	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone( &CBaseButton::ButtonBackHome );
	if( !m_fRotating )
		LinearMove( m_vecPosition1, pev->speed );
	else
		AngularMove( m_vecAngle1, pev->speed );

	pev->frame = 0;			// use normal textures
}

//
// Button has returned to start state.  Quiesce it.
//
void CBaseButton::ButtonBackHome( void )
{
	ASSERT( m_toggle_state == TS_GOING_DOWN );
	m_toggle_state = TS_AT_BOTTOM;

	if( FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) )
	{
		//EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), 1, ATTN_NORM);

		SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
	}


	if( !FStringNull( pev->target ) )
	{
		edict_t* pentTarget = NULL;
		for( ;;)
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );

			if( FNullEnt( pentTarget ) )
				break;

			if( !FClassnameIs( pentTarget, "multisource" ) )
				continue;
			CBaseEntity *pTarget = CBaseEntity::Instance( pentTarget );

			if( pTarget )
				pTarget->Use( m_hActivator, this, USE_TOGGLE, 0 );
		}
	}

	// Re-instate touch method, movement cycle is complete.
	if( !FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) ) // this button only works if USED, not touched!
	{
		// All buttons are now use only	
		SetTouch( NULL );
	}
	else
		SetTouch( &CBaseButton::ButtonTouch );

	// reset think for a sparking button
	if( FBitSet( pev->spawnflags, SF_BUTTON_SPARK_IF_OFF ) )
	{
		SetThink( &CBaseButton::ButtonSpark );
		pev->nextthink = gpGlobals->time + 0.5;// no hurry.
	}
}

//
// Button's Use function
//
void CBaseButton::ButtonUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Ignore touches if button is moving, or pushed-in and waiting to auto-come-out.
	// UNDONE: Should this use ButtonResponseToTouch() too?
	if( m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN )
		return;

	m_hActivator = pActivator;
	if( m_toggle_state == TS_AT_TOP )
	{
		if( !m_fStayPushed && FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) )
		{
			EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char* ) STRING( pev->noise ), 1, ATTN_NORM );

			//SUB_UseTargets( m_eoActivator );
			ButtonReturn();
		}
	}
	else
		ButtonActivate();
}

//
// ButtonShot
//
int CBaseButton::TakeDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, int bitsDamageType )
{
	BUTTON_CODE code = ButtonResponseToTouch();

	if( code == BUTTON_NOTHING )
		return 0;
	// Temporarily disable the touch function, until movement is finished.
	SetTouch( NULL );

	m_hActivator = pAttacker;
	if( m_hActivator == NULL )
		return 0;

	if( code == BUTTON_RETURN )
	{
		EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char* ) STRING( pev->noise ), 1, ATTN_NORM );

		// Toggle buttons fire when they get back to their "home" position
		if( !( pev->spawnflags & SF_BUTTON_TOGGLE ) )
			SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
		ButtonReturn();
	}
	else // code == BUTTON_ACTIVATE
		ButtonActivate();

	return 0;
}

CBaseButton::BUTTON_CODE CBaseButton::ButtonResponseToTouch( void )
{
	// Ignore touches if button is moving, or pushed-in and waiting to auto-come-out.
	if( m_toggle_state == TS_GOING_UP ||
		m_toggle_state == TS_GOING_DOWN ||
		( m_toggle_state == TS_AT_TOP && !m_fStayPushed && !FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) ) )
		return BUTTON_NOTHING;

	if( m_toggle_state == TS_AT_TOP )
	{
		if( ( FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) ) && !m_fStayPushed )
		{
			return BUTTON_RETURN;
		}
	}
	else
		return BUTTON_ACTIVATE;

	return BUTTON_NOTHING;
}