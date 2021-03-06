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
#ifndef GAME_SERVER_ENTITIES_ITEMS_CAIRTANK_H
#define GAME_SERVER_ENTITIES_ITEMS_CAIRTANK_H

class CAirtank : public CGrenade
{
public:
	DECLARE_CLASS( CAirtank, CGrenade );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void EXPORT TankThink( void );
	void EXPORT TankTouch( CBaseEntity *pOther );
	int	 BloodColor() const override { return DONT_BLEED; }
	void Killed( entvars_t *pevAttacker, GibAction gibAction ) override;

	int	 m_state;
};

#endif //GAME_SERVER_ENTITIES_ITEMS_CAIRTANK_H