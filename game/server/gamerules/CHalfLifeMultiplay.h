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
#ifndef GAME_SERVER_GAMERULES_CHALFLIFEMULTIPLAY_H
#define GAME_SERVER_GAMERULES_CHALFLIFEMULTIPLAY_H

#include "CGameRules.h"

//=========================================================
// CHalfLifeMultiplay - rules for the basic half life multiplayer
// competition
//=========================================================
class CHalfLifeMultiplay : public CGameRules
{
public:
	CHalfLifeMultiplay();

	// GR_Think
	virtual void Think() override;
	virtual void RefreshSkillData() override;
	virtual bool IsAllowedToSpawn( CBaseEntity *pEntity ) override;
	virtual bool FAllowFlashlight() const override;

	virtual bool FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon ) override;
	virtual bool GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon ) override;

	// Functions to verify the single/multiplayer status of a game
	virtual bool IsMultiplayer() const override;
	virtual bool IsDeathmatch() const override;
	virtual bool IsCoOp() const override;

	// Client connection/disconnection
	// If ClientConnected returns false, the connection is rejected and the user is provided the reason specified in
	//  szRejectReason
	// Only the client's name and remote address are provided to the dll for verification.
	virtual bool ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ) override;
	virtual void InitHUD( CBasePlayer *pl ) override;		// the client dll is ready for updating
	virtual void ClientDisconnected( edict_t *pClient ) override;
	virtual void UpdateGameMode( CBasePlayer *pPlayer ) override;  // the client needs to be informed of the current game mode

																   // Client damage rules
	virtual float FlPlayerFallDamage( CBasePlayer *pPlayer ) override;
	virtual bool  FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker ) override;

	// Client spawn/respawn control
	virtual void PlayerSpawn( CBasePlayer *pPlayer ) override;
	virtual void PlayerThink( CBasePlayer *pPlayer ) override;
	virtual bool FPlayerCanRespawn( CBasePlayer *pPlayer ) override;
	virtual float FlPlayerSpawnTime( CBasePlayer *pPlayer ) override;
	virtual edict_t *GetPlayerSpawnSpot( CBasePlayer *pPlayer ) override;

	virtual bool AllowAutoTargetCrosshair() override;
	virtual bool ClientCommand( CBasePlayer *pPlayer, const char *pcmd ) override;

	// Client kills/scoring
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled ) override;
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor ) override;
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor ) override;

	// Weapon retrieval
	virtual void PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon ) override;
	virtual bool CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon ) override;// The player is touching an CBasePlayerItem, do I give it to him?

																							  // Weapon spawn/respawn control
	virtual int WeaponShouldRespawn( CBasePlayerItem *pWeapon ) override;
	virtual float FlWeaponRespawnTime( CBasePlayerItem *pWeapon ) override;
	virtual float FlWeaponTryRespawn( CBasePlayerItem *pWeapon ) override;
	virtual Vector VecWeaponRespawnSpot( CBasePlayerItem *pWeapon ) override;

	// Item retrieval
	virtual bool CanHaveItem( CBasePlayer *pPlayer, CItem *pItem ) override;
	virtual void PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem ) override;

	// Item spawn/respawn control
	virtual int ItemShouldRespawn( CItem *pItem ) override;
	virtual float FlItemRespawnTime( CItem *pItem ) override;
	virtual Vector VecItemRespawnSpot( CItem *pItem ) override;

	// Ammo retrieval
	virtual void PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount ) override;

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn( CBasePlayerAmmo *pAmmo ) override;
	virtual float FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo ) override;
	virtual Vector VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo ) override;

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime() override;
	virtual float FlHEVChargerRechargeTime() override;

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer ) override;

	// What happens to a dead player's ammo	
	virtual int DeadPlayerAmmo( CBasePlayer *pPlayer ) override;

	// Teamplay stuff	
	virtual const char *GetTeamID( CBaseEntity *pEntity ) override { return ""; }
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget ) override;

	virtual bool PlayTextureSounds() override { return false; }
	virtual bool PlayFootstepSounds( CBasePlayer *pl, float fvol ) override;

	// Monsters
	virtual bool FAllowMonsters() const override;

	// Immediately end a multiplayer game
	virtual void EndMultiplayerGame() override { GoToIntermission(); }

protected:
	virtual void ChangeLevel();
	virtual void GoToIntermission();
	float m_flIntermissionEndTime;
	bool m_bEndIntermissionButtonHit;
	void SendMOTDToClient( edict_t *client );
};

#endif //GAME_SERVER_GAMERULES_CHALFLIFEMULTIPLAY_H