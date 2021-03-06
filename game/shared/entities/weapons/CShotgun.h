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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CSHOTGUN_H
#define GAME_SHARED_ENTITIES_WEAPONS_CSHOTGUN_H

enum shotgun_e
{
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

class CShotgun : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CShotgun, CBasePlayerWeapon );
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

	void Spawn( void ) override;
	void Precache( void ) override;
	int iItemSlot() override { return 3; }
	bool GetItemInfo( ItemInfo* p ) override;
	bool AddToPlayer( CBasePlayer *pPlayer ) override;

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	void Reload( void ) override;
	void WeaponIdle( void ) override;
	float m_flNextReload;
	int m_iShell;

	virtual bool UseDecrement() const override
	{
#if defined( CLIENT_WEAPONS )
		return true;
#else
		return false;
#endif
	}

	void GetWeaponData( weapon_data_t& data ) override
	{
		BaseClass::GetWeaponData( data );

		data.m_fInSpecialReload = m_fInSpecialReload;

		//data.m_flPumpTime		= max( m_flPumpTime, -0.001 );
	}

	void SetWeaponData( const weapon_data_t& data ) override
	{
		BaseClass::SetWeaponData( data );

		m_fInSpecialReload = data.m_fInSpecialReload;
		//m_flPumpTime		= data.m_flPumpTime;
	}

private:
	unsigned short m_usDoubleFire;
	unsigned short m_usSingleFire;

	int m_fInSpecialReload;		// Are we in the middle of a reload for the shotguns
	float m_flPumpTime;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CSHOTGUN_H