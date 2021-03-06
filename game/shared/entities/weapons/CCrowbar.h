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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CCROWBAR_H
#define GAME_SHARED_ENTITIES_WEAPONS_CCROWBAR_H

enum crowbar_e
{
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW,
	CROWBAR_HOLSTER,
	CROWBAR_ATTACK1HIT,
	CROWBAR_ATTACK1MISS,
	CROWBAR_ATTACK2MISS,
	CROWBAR_ATTACK2HIT,
	CROWBAR_ATTACK3MISS,
	CROWBAR_ATTACK3HIT
};

class CCrowbar : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CCrowbar, CBasePlayerWeapon );

	void Spawn( void ) override;
	void Precache( void ) override;
	int iItemSlot( void ) override { return 1; }
	void EXPORT SwingAgain( void );
	void EXPORT Smack( void );
	bool GetItemInfo( ItemInfo* p ) override;

	void PrimaryAttack( void ) override;
	bool Swing( const bool bFirst );
	bool Deploy() override;
	void Holster( int skiplocal = 0 ) override;
	int m_iSwing;
	TraceResult m_trHit;

	virtual bool UseDecrement() const override
	{
#if defined( CLIENT_WEAPONS )
		return true;
#else
		return false;
#endif
	}
private:
	unsigned short m_usCrowbar;
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CCROWBAR_H