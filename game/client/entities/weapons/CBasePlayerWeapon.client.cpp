#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "entities/CBasePlayer.h"
#include "Weapons.h"

#include "com_weapons.h"

extern bool g_brunninggausspred;

bool CBasePlayerWeapon::AddToPlayer( CBasePlayer* pPlayer )
{
	return false;
}

bool CBasePlayerWeapon::AddDuplicate( CBasePlayerItem* pOriginal )
{
	return false;
}

bool CBasePlayerWeapon::ExtractAmmo( CBasePlayerWeapon* pWeapon )
{
	return false;
}

bool CBasePlayerWeapon::ExtractClipAmmo( CBasePlayerWeapon* pWeapon )
{
	return false;
}

bool CBasePlayerWeapon::AddPrimaryAmmo( int iCount, const char *szName, int iMaxClip )
{
	return true;
}

bool CBasePlayerWeapon::AddSecondaryAmmo( int iCount, const char *szName )
{
	return true;
}

/*
=====================
CBasePlayerWeapon :: PlayEmptySound

=====================
*/
//TODO: these 2 functions are virtually identical to the server side copy - Solokiller
bool CBasePlayerWeapon::PlayEmptySound()
{
	if( m_bPlayEmptySound )
	{
		HUD_PlaySound( "weapons/357_cock1.wav", 0.8 );
		m_bPlayEmptySound = false;
		return false;
	}
	return false;
}

/*
=====================
CBasePlayerWeapon :: ResetEmptySound

=====================
*/
void CBasePlayerWeapon::ResetEmptySound()
{
	m_bPlayEmptySound = true;
}

/*
=====================
CBasePlayerWeapon::SendWeaponAnim

Animate weapon model
=====================
*/
void CBasePlayerWeapon::SendWeaponAnim( int iAnim, int skiplocal, int body )
{
	m_pPlayer->pev->weaponanim = iAnim;

	HUD_SendWeaponAnim( iAnim, body, 0 );
}

/*
=====================
CBasePlayerWeapon :: CanDeploy
=====================
*/
//TODO: identical to the server version - Solokiller
bool CBasePlayerWeapon::CanDeploy() const
{
	bool bHasAmmo = false;

	if( !pszAmmo1() )
	{
		// this weapon doesn't use ammo, can always deploy.
		return true;
	}

	if( pszAmmo1() )
	{
		bHasAmmo |= ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] != 0 );
	}
	if( pszAmmo2() )
	{
		bHasAmmo |= ( m_pPlayer->m_rgAmmo[ m_iSecondaryAmmoType ] != 0 );
	}
	if( m_iClip > 0 )
	{
		bHasAmmo |= 1;
	}
	if( !bHasAmmo )
	{
		return false;
	}

	return true;
}

bool CBasePlayerWeapon::IsUseable()
{
	return true;
}

/*
=====================
CBasePlayerWeapon :: DefaultDeploy

=====================
*/
bool CBasePlayerWeapon::DefaultDeploy( char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal, int	body )
{
	if( !CanDeploy() )
		return false;

	gEngfuncs.CL_LoadModel( szViewModel, &m_pPlayer->pev->viewmodel );

	SendWeaponAnim( iAnim, skiplocal, body );

	g_brunninggausspred = false;
	m_pPlayer->m_flNextAttack = 0.5;
	m_flTimeWeaponIdle = 1.0;
	return true;
}

/*
=====================
CBasePlayerWeapon :: DefaultReload
=====================
*/
bool CBasePlayerWeapon::DefaultReload( int iClipSize, int iAnim, float fDelay, int body )
{

	if( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
		return false;

	int j = min( iClipSize - m_iClip, m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] );

	if( j == 0 )
		return false;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

	//!!UNDONE -- reload sound goes here !!!
	SendWeaponAnim( iAnim, UseDecrement(), body );

	m_fInReload = true;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	return true;
}

/*
=====================
CBasePlayerWeapon::ItemPostFrame

Handles weapon firing, reloading, etc.
=====================
*/
void CBasePlayerWeapon::ItemPostFrame( void )
{
	if( ( m_fInReload ) && ( m_pPlayer->m_flNextAttack <= 0.0 ) )
	{
#if 0 // FIXME, need ammo on client to make this work right
		// complete the reload. 
		int j = min( iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] );

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] -= j;
#else	
		m_iClip += 10;
#endif
		m_fInReload = false;
	}

	if( ( m_pPlayer->pev->button & IN_ATTACK2 ) && ( m_flNextSecondaryAttack <= 0.0 ) )
	{
		if( pszAmmo2() && !m_pPlayer->m_rgAmmo[ SecondaryAmmoIndex() ] )
		{
			m_bFireOnEmpty = true;
		}

		SecondaryAttack();
		m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if( ( m_pPlayer->pev->button & IN_ATTACK ) && ( m_flNextPrimaryAttack <= 0.0 ) )
	{
		if( ( m_iClip == 0 && pszAmmo1() ) || ( iMaxClip() == -1 && !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] ) )
		{
			m_bFireOnEmpty = true;
		}

		PrimaryAttack();
	}
	else if( m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != WEAPON_NOCLIP && !m_fInReload )
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}
	else if( !( m_pPlayer->pev->button & ( IN_ATTACK | IN_ATTACK2 ) ) )
	{
		// no fire buttons down

		m_bFireOnEmpty = false;

		// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
		if( m_iClip == 0 && !( iFlags() & ITEM_FLAG_NOAUTORELOAD ) && m_flNextPrimaryAttack < 0.0 )
		{
			Reload();
			return;
		}

		WeaponIdle();
		return;
	}

	// catch all
	if( ShouldWeaponIdle() )
	{
		WeaponIdle();
	}
}

bool CBasePlayerWeapon::UpdateClientData( CBasePlayer* pPlayer )
{
	return false;
}

void CBasePlayerWeapon::RetireWeapon( void )
{
}

/*
=====================
CBasePlayerWeapon::Holster

Put away weapon
=====================
*/
void CBasePlayerWeapon::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false; // cancel any reload in progress.
	g_brunninggausspred = false;
	m_pPlayer->pev->viewmodel = 0;
}

int CBasePlayerWeapon::PrimaryAmmoIndex() const
{
	return -1;
}

int CBasePlayerWeapon::SecondaryAmmoIndex() const
{
	return -1;
}

float CBasePlayerWeapon::GetNextAttackDelay( float flTime )
{
	return flTime;
}