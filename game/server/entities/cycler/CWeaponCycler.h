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
#ifndef GAME_SERVER_ENTITIES_CYCLER_CWEAPONCYCLER_H
#define GAME_SERVER_ENTITIES_CYCLER_CWEAPONCYCLER_H

class CWeaponCycler : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CWeaponCycler, CBasePlayerWeapon );

	void Spawn( void ) override;
	int iItemSlot( void ) override { return 1; }
	bool GetItemInfo( ItemInfo* p ) override { return false; }

	void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	bool Deploy() override;
	void Holster( int skiplocal = 0 ) override;
	int m_iszModel;
	int m_iModel;
};

#endif //GAME_SERVER_ENTITIES_CYCLER_CWEAPONCYCLER_H