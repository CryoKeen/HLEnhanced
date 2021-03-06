/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#ifndef GAME_SERVER_ENTITIES_NPCS_CBMORTAR_H
#define GAME_SERVER_ENTITIES_NPCS_CBMORTAR_H

//=========================================================
// Mortar shot entity
//=========================================================
class CBMortar : public CBaseEntity
{
public:
	DECLARE_CLASS( CBMortar, CBaseEntity );
	DECLARE_DATADESC();

	void Spawn( void ) override;

	static CBMortar *Shoot( edict_t *pOwner, Vector vecStart, Vector vecVelocity );
	void Touch( CBaseEntity *pOther ) override;
	void EXPORT Animate( void );

	int  m_maxFrame;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBMORTAR_H