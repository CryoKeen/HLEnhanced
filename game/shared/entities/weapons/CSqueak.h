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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CSQUEAK_H
#define GAME_SHARED_ENTITIES_WEAPONS_CSQUEAK_H

enum w_squeak_e
{
	WSQUEAK_IDLE1 = 0,
	WSQUEAK_FIDGET,
	WSQUEAK_JUMP,
	WSQUEAK_RUN,
};

enum squeak_e
{
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};

class CSqueak : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CSqueak, CBasePlayerWeapon );

	void Spawn( void ) override;
	void Precache( void ) override;
	int iItemSlot( void ) override { return 5; }
	bool GetItemInfo( ItemInfo* p ) override;

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	void Holster( int skiplocal = 0 ) override;
	void WeaponIdle( void ) override;
	int m_fJustThrown;

	virtual bool UseDecrement() const override
	{
#if defined( CLIENT_WEAPONS )
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usSnarkFire;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CSQUEAK_H