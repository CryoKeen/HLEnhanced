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

// Lightning target, just alias landmark
LINK_ENTITY_TO_CLASS( info_target, CPointEntity );

// Landmark class
void CPointEntity::Spawn( void )
{
	pev->solid = SOLID_NOT;
	//	UTIL_SetSize(pev, g_vecZero, g_vecZero);
}