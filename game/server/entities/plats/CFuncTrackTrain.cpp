#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "mathlib.h"

#include "CPathTrack.h"

#include "CFuncTrackTrain.h"

BEGIN_DATADESC( CFuncTrackTrain )
	DEFINE_FIELD( m_ppath, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_length, FIELD_FLOAT ),
	DEFINE_FIELD( m_height, FIELD_FLOAT ),
	DEFINE_FIELD( m_speed, FIELD_FLOAT ),
	DEFINE_FIELD( m_dir, FIELD_FLOAT ),
	DEFINE_FIELD( m_startSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_controlMins, FIELD_VECTOR ),
	DEFINE_FIELD( m_controlMaxs, FIELD_VECTOR ),
	DEFINE_FIELD( m_sounds, FIELD_INTEGER ),
	DEFINE_FIELD( m_flVolume, FIELD_FLOAT ),
	DEFINE_FIELD( m_flBank, FIELD_FLOAT ),
	DEFINE_FIELD( m_oldSpeed, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_tracktrain, CFuncTrackTrain );

void CFuncTrackTrain::Spawn( void )
{
	if( pev->speed == 0 )
		m_speed = 100;
	else
		m_speed = pev->speed;

	pev->speed = 0;
	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;
	pev->impulse = m_speed;

	m_dir = 1;

	if( FStringNull( pev->target ) )
		ALERT( at_console, "FuncTrain with no target" );

	if( pev->spawnflags & SF_TRACKTRAIN_PASSABLE )
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );

	// Cache off placed origin for train controls
	pev->oldorigin = pev->origin;

	m_controlMins = pev->mins;
	m_controlMaxs = pev->maxs;
	m_controlMaxs.z += 72;
	// start trains on the next frame, to make sure their targets have had
	// a chance to spawn/activate
	NextThink( pev->ltime + 0.1, false );
	SetThink( &CFuncTrackTrain::Find );
	Precache();
}

void CFuncTrackTrain::Precache( void )
{
	if( m_flVolume == 0.0 )
		m_flVolume = 1.0;

	switch( m_sounds )
	{
	default:
		// no sound
		pev->noise = 0;
		break;
	case 1: PRECACHE_SOUND( "plats/ttrain1.wav" ); pev->noise = MAKE_STRING( "plats/ttrain1.wav" ); break;
	case 2: PRECACHE_SOUND( "plats/ttrain2.wav" ); pev->noise = MAKE_STRING( "plats/ttrain2.wav" ); break;
	case 3: PRECACHE_SOUND( "plats/ttrain3.wav" ); pev->noise = MAKE_STRING( "plats/ttrain3.wav" ); break;
	case 4: PRECACHE_SOUND( "plats/ttrain4.wav" ); pev->noise = MAKE_STRING( "plats/ttrain4.wav" ); break;
	case 5: PRECACHE_SOUND( "plats/ttrain6.wav" ); pev->noise = MAKE_STRING( "plats/ttrain6.wav" ); break;
	case 6: PRECACHE_SOUND( "plats/ttrain7.wav" ); pev->noise = MAKE_STRING( "plats/ttrain7.wav" ); break;
	}

	PRECACHE_SOUND( "plats/ttrain_brake1.wav" );
	PRECACHE_SOUND( "plats/ttrain_start1.wav" );

	m_usAdjustPitch = PRECACHE_EVENT( 1, "events/train.sc" );
}

void CFuncTrackTrain::Blocked( CBaseEntity *pOther )
{
	entvars_t	*pevOther = pOther->pev;

	// Blocker is on-ground on the train
	if( FBitSet( pevOther->flags, FL_ONGROUND ) && VARS( pevOther->groundentity ) == pev )
	{
		float deltaSpeed = fabs( pev->speed );
		if( deltaSpeed > 50 )
			deltaSpeed = 50;
		if( !pevOther->velocity.z )
			pevOther->velocity.z += deltaSpeed;
		return;
	}
	else
		pevOther->velocity = ( pevOther->origin - pev->origin ).Normalize() * pev->dmg;

	ALERT( at_aiconsole, "TRAIN(%s): Blocked by %s (dmg:%.2f)\n", STRING( pev->targetname ), STRING( pOther->pev->classname ), pev->dmg );
	if( pev->dmg <= 0 )
		return;
	// we can't hurt this thing, so we're not concerned with it
	pOther->TakeDamage( this, this, pev->dmg, DMG_CRUSH );
}

void CFuncTrackTrain::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( useType != USE_SET )
	{
		if( !ShouldToggle( useType, ( pev->speed != 0 ) ) )
			return;

		if( pev->speed == 0 )
		{
			pev->speed = m_speed * m_dir;

			Next();
		}
		else
		{
			pev->speed = 0;
			pev->velocity = g_vecZero;
			pev->avelocity = g_vecZero;
			StopSound();
			SetThink( NULL );
		}
	}
	else
	{
		float delta = value;

		delta = ( ( int ) ( pev->speed * 4 ) / ( int ) m_speed )*0.25 + 0.25 * delta;
		if( delta > 1 )
			delta = 1;
		else if( delta < -1 )
			delta = -1;
		if( pev->spawnflags & SF_TRACKTRAIN_FORWARDONLY )
		{
			if( delta < 0 )
				delta = 0;
		}
		pev->speed = m_speed * delta;
		Next();
		ALERT( at_aiconsole, "TRAIN(%s), speed to %.2f\n", STRING( pev->targetname ), pev->speed );
	}
}

void CFuncTrackTrain::KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "wheels" ) )
	{
		m_length = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "height" ) )
	{
		m_height = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "startspeed" ) )
	{
		m_startSpeed = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "volume" ) )
	{
		m_flVolume = ( float ) ( atoi( pkvd->szValue ) );
		m_flVolume *= 0.1;
		pkvd->fHandled = true;
	}
	else if( FStrEq( pkvd->szKeyName, "bank" ) )
	{
		m_flBank = atof( pkvd->szValue );
		pkvd->fHandled = true;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CFuncTrackTrain::Next( void )
{
	float time = 0.5;

	if( !pev->speed )
	{
		ALERT( at_aiconsole, "TRAIN(%s): Speed is 0\n", STRING( pev->targetname ) );
		StopSound();
		return;
	}

	//	if ( !m_ppath )
	//		m_ppath = CPathTrack::Instance(FIND_ENTITY_BY_TARGETNAME( NULL, STRING(pev->target) ));
	if( !m_ppath )
	{
		ALERT( at_aiconsole, "TRAIN(%s): Lost path\n", STRING( pev->targetname ) );
		StopSound();
		return;
	}

	UpdateSound();

	Vector nextPos = pev->origin;

	nextPos.z -= m_height;
	CPathTrack *pnext = m_ppath->LookAhead( &nextPos, pev->speed * 0.1, true );
	nextPos.z += m_height;

	pev->velocity = ( nextPos - pev->origin ) * 10;
	Vector nextFront = pev->origin;

	nextFront.z -= m_height;
	if( m_length > 0 )
		m_ppath->LookAhead( &nextFront, m_length, false );
	else
		m_ppath->LookAhead( &nextFront, 100, false );
	nextFront.z += m_height;

	Vector delta = nextFront - pev->origin;
	Vector angles = UTIL_VecToAngles( delta );
	// The train actually points west
	angles.y += 180;

	// !!!  All of this crap has to be done to make the angles not wrap around, revisit this.
	FixupAngles( angles );
	FixupAngles( pev->angles );

	if( !pnext || ( delta.x == 0 && delta.y == 0 ) )
		angles = pev->angles;

	float vy, vx;
	if( !( pev->spawnflags & SF_TRACKTRAIN_NOPITCH ) )
		vx = UTIL_AngleDistance( angles.x, pev->angles.x );
	else
		vx = 0;
	vy = UTIL_AngleDistance( angles.y, pev->angles.y );

	pev->avelocity.y = vy * 10;
	pev->avelocity.x = vx * 10;

	if( m_flBank != 0 )
	{
		if( pev->avelocity.y < -5 )
			pev->avelocity.z = UTIL_AngleDistance( UTIL_ApproachAngle( -m_flBank, pev->angles.z, m_flBank * 2 ), pev->angles.z );
		else if( pev->avelocity.y > 5 )
			pev->avelocity.z = UTIL_AngleDistance( UTIL_ApproachAngle( m_flBank, pev->angles.z, m_flBank * 2 ), pev->angles.z );
		else
			pev->avelocity.z = UTIL_AngleDistance( UTIL_ApproachAngle( 0, pev->angles.z, m_flBank * 4 ), pev->angles.z ) * 4;
	}

	if( pnext )
	{
		if( pnext != m_ppath )
		{
			CPathTrack *pFire;
			if( pev->speed >= 0 )
				pFire = pnext;
			else
				pFire = m_ppath;

			m_ppath = pnext;
			// Fire the pass target if there is one
			if( pFire->pev->message )
			{
				FireTargets( STRING( pFire->pev->message ), this, this, USE_TOGGLE, 0 );
				if( FBitSet( pFire->pev->spawnflags, SF_PATH_FIREONCE ) )
					pFire->pev->message = 0;
			}

			if( pFire->pev->spawnflags & SF_PATH_DISABLE_TRAIN )
				pev->spawnflags |= SF_TRACKTRAIN_NOCONTROL;

			// Don't override speed if under user control
			if( pev->spawnflags & SF_TRACKTRAIN_NOCONTROL )
			{
				if( pFire->pev->speed != 0 )
				{// don't copy speed from target if it is 0 (uninitialized)
					pev->speed = pFire->pev->speed;
					ALERT( at_aiconsole, "TrackTrain %s speed to %4.2f\n", STRING( pev->targetname ), pev->speed );
				}
			}

		}
		SetThink( &CFuncTrackTrain::Next );
		NextThink( pev->ltime + time, true );
	}
	else	// end of path, stop
	{
		StopSound();
		pev->velocity = ( nextPos - pev->origin );
		pev->avelocity = g_vecZero;
		float distance = pev->velocity.Length();
		m_oldSpeed = pev->speed;


		pev->speed = 0;

		// Move to the dead end

		// Are we there yet?
		if( distance > 0 )
		{
			// no, how long to get there?
			time = distance / m_oldSpeed;
			pev->velocity = pev->velocity * ( m_oldSpeed / distance );
			SetThink( &CFuncTrackTrain::DeadEnd );
			NextThink( pev->ltime + time, false );
		}
		else
		{
			DeadEnd();
		}
	}
}

void CFuncTrackTrain::Find( void )
{
	m_ppath = CPathTrack::Instance( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( pev->target ) ) );
	if( !m_ppath )
		return;

	entvars_t *pevTarget = m_ppath->pev;
	if( !FClassnameIs( pevTarget, "path_track" ) )
	{
		ALERT( at_error, "func_track_train must be on a path of path_track\n" );
		m_ppath = NULL;
		return;
	}

	Vector nextPos = pevTarget->origin;
	nextPos.z += m_height;

	Vector look = nextPos;
	look.z -= m_height;
	m_ppath->LookAhead( &look, m_length, false );
	look.z += m_height;

	pev->angles = UTIL_VecToAngles( look - nextPos );
	// The train actually points west
	pev->angles.y += 180;

	if( pev->spawnflags & SF_TRACKTRAIN_NOPITCH )
		pev->angles.x = 0;
	UTIL_SetOrigin( pev, nextPos );
	NextThink( pev->ltime + 0.1, false );
	SetThink( &CFuncTrackTrain::Next );
	pev->speed = m_startSpeed;

	UpdateSound();
}

void CFuncTrackTrain::NearestPath( void )
{
	CBaseEntity *pTrack = NULL;
	CBaseEntity *pNearest = NULL;
	float dist, closest;

	closest = 1024;

	while( ( pTrack = UTIL_FindEntityInSphere( pTrack, pev->origin, 1024 ) ) != NULL )
	{
		// filter out non-tracks
		if( !( pTrack->pev->flags & ( FL_CLIENT | FL_MONSTER ) ) && FClassnameIs( pTrack->pev, "path_track" ) )
		{
			dist = ( pev->origin - pTrack->pev->origin ).Length();
			if( dist < closest )
			{
				closest = dist;
				pNearest = pTrack;
			}
		}
	}

	if( !pNearest )
	{
		ALERT( at_console, "Can't find a nearby track !!!\n" );
		SetThink( NULL );
		return;
	}

	ALERT( at_aiconsole, "TRAIN: %s, Nearest track is %s\n", STRING( pev->targetname ), STRING( pNearest->pev->targetname ) );
	// If I'm closer to the next path_track on this path, then it's my real path
	pTrack = ( ( CPathTrack * ) pNearest )->GetNext();
	if( pTrack )
	{
		if( ( pev->origin - pTrack->pev->origin ).Length() < ( pev->origin - pNearest->pev->origin ).Length() )
			pNearest = pTrack;
	}

	m_ppath = ( CPathTrack * ) pNearest;

	if( pev->speed != 0 )
	{
		NextThink( pev->ltime + 0.1, false );
		SetThink( &CFuncTrackTrain::Next );
	}
}

void CFuncTrackTrain::DeadEnd( void )
{
	// Fire the dead-end target if there is one
	CPathTrack *pTrack, *pNext;

	pTrack = m_ppath;

	ALERT( at_aiconsole, "TRAIN(%s): Dead end ", STRING( pev->targetname ) );
	// Find the dead end path node
	// HACKHACK -- This is bugly, but the train can actually stop moving at a different node depending on it's speed
	// so we have to traverse the list to it's end.
	if( pTrack )
	{
		if( m_oldSpeed < 0 )
		{
			do
			{
				pNext = pTrack->ValidPath( pTrack->GetPrevious(), true );
				if( pNext )
					pTrack = pNext;
			}
			while( pNext );
		}
		else
		{
			do
			{
				pNext = pTrack->ValidPath( pTrack->GetNext(), true );
				if( pNext )
					pTrack = pNext;
			}
			while( pNext );
		}
	}

	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;
	if( pTrack )
	{
		ALERT( at_aiconsole, "at %s\n", STRING( pTrack->pev->targetname ) );
		if( pTrack->pev->netname )
			FireTargets( STRING( pTrack->pev->netname ), this, this, USE_TOGGLE, 0 );
	}
	else
		ALERT( at_aiconsole, "\n" );
}

void CFuncTrackTrain::NextThink( float thinkTime, const bool alwaysThink )
{
	if( alwaysThink )
		pev->flags |= FL_ALWAYSTHINK;
	else
		pev->flags &= ~FL_ALWAYSTHINK;

	pev->nextthink = thinkTime;
}

void CFuncTrackTrain::SetTrack( CPathTrack *track )
{
	m_ppath = track->Nearest( pev->origin );
}

void CFuncTrackTrain::SetControls( entvars_t *pevControls )
{
	Vector offset = pevControls->origin - pev->oldorigin;

	m_controlMins = pevControls->mins + offset;
	m_controlMaxs = pevControls->maxs + offset;
}

bool CFuncTrackTrain::OnControls( const CBaseEntity* const pTest ) const
{
	const Vector offset = pTest->pev->origin - pev->origin;

	if( pev->spawnflags & SF_TRACKTRAIN_NOCONTROL )
		return false;

	// Transform offset into local coordinates
	UTIL_MakeVectors( pev->angles );
	Vector local;
	local.x = DotProduct( offset, gpGlobals->v_forward );
	local.y = -DotProduct( offset, gpGlobals->v_right );
	local.z = DotProduct( offset, gpGlobals->v_up );

	if( local.x >= m_controlMins.x && local.y >= m_controlMins.y && local.z >= m_controlMins.z &&
		local.x <= m_controlMaxs.x && local.y <= m_controlMaxs.y && local.z <= m_controlMaxs.z )
		return true;

	return false;
}

void CFuncTrackTrain::StopSound( void )
{
	// if sound playing, stop it
	if( m_soundPlaying && pev->noise )
	{
		unsigned short us_encode;
		unsigned short us_sound = ( ( unsigned short ) ( m_sounds ) & 0x0007 ) << 12;

		us_encode = us_sound;

		PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0.0,
			( float * ) &g_vecZero, ( float * ) &g_vecZero, 0.0, 0.0, us_encode, 0, 1, 0 );

		/*
		STOP_SOUND(ENT(pev), CHAN_STATIC, (char*)STRING(pev->noise));
		*/
		EMIT_SOUND_DYN( ENT( pev ), CHAN_ITEM, "plats/ttrain_brake1.wav", m_flVolume, ATTN_NORM, 0, 100 );
	}

	m_soundPlaying = 0;
}

// update pitch based on speed, start sound if not playing
// NOTE: when train goes through transition, m_soundPlaying should go to 0, 
// which will cause the looped sound to restart.

void CFuncTrackTrain::UpdateSound( void )
{
	float flpitch;

	if( !pev->noise )
		return;

	flpitch = TRAIN_STARTPITCH + ( fabs( pev->speed ) * ( TRAIN_MAXPITCH - TRAIN_STARTPITCH ) / TRAIN_MAXSPEED );

	if( !m_soundPlaying )
	{
		// play startup sound for train
		EMIT_SOUND_DYN( ENT( pev ), CHAN_ITEM, "plats/ttrain_start1.wav", m_flVolume, ATTN_NORM, 0, 100 );
		EMIT_SOUND_DYN( ENT( pev ), CHAN_STATIC, ( char* ) STRING( pev->noise ), m_flVolume, ATTN_NORM, 0, ( int ) flpitch );
		m_soundPlaying = 1;
	}
	else
	{
		/*
		// update pitch
		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, (char*)STRING(pev->noise), m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, (int) flpitch);
		*/
		// volume 0.0 - 1.0 - 6 bits
		// m_sounds 3 bits
		// flpitch = 6 bits
		// 15 bits total

		unsigned short us_encode;
		unsigned short us_sound = ( ( unsigned short ) ( m_sounds ) & 0x0007 ) << 12;
		unsigned short us_pitch = ( ( unsigned short ) ( flpitch / 10.0 ) & 0x003f ) << 6;
		unsigned short us_volume = ( ( unsigned short ) ( m_flVolume * 40.0 ) & 0x003f );

		us_encode = us_sound | us_pitch | us_volume;

		PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0.0,
			( float * ) &g_vecZero, ( float * ) &g_vecZero, 0.0, 0.0, us_encode, 0, 0, 0 );
	}
}

CFuncTrackTrain *CFuncTrackTrain::Instance( edict_t *pent )
{
	if( FClassnameIs( pent, "func_tracktrain" ) )
		return ( CFuncTrackTrain * ) GET_PRIVATE( pent );
	return NULL;
}

void CFuncTrackTrain::OverrideReset( void )
{
	NextThink( pev->ltime + 0.1, false );
	SetThink( &CFuncTrackTrain::NearestPath );
}