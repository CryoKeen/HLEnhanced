#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"
#include "entities/CBasePlayer.h"

// Global Savedata for Delay
BEGIN_DATADESC_NOBASE( CBaseEntity )
	DEFINE_FIELD( m_pGoalEnt, FIELD_CLASSPTR ),
	
	DEFINE_FIELD( m_pfnThink, FIELD_FUNCTION ),		// UNDONE: Build table of these!!!
	DEFINE_FIELD( m_pfnTouch, FIELD_FUNCTION ),
	DEFINE_FIELD( m_pfnUse, FIELD_FUNCTION ),
	DEFINE_FIELD( m_pfnBlocked, FIELD_FUNCTION ),
END_DATADESC()

// Global Savedata for Delay
BEGIN_DATADESC( CBaseDelay )
	DEFINE_FIELD( m_flDelay, FIELD_FLOAT ),
	DEFINE_FIELD( m_iszKillTarget, FIELD_STRING ),
END_DATADESC()

BEGIN_DATADESC( CBaseAnimating )
	DEFINE_FIELD( m_flFrameRate, FIELD_FLOAT ),
	DEFINE_FIELD( m_flGroundSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_flLastEventCheck, FIELD_TIME ),
	DEFINE_FIELD( m_fSequenceFinished, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fSequenceLoops, FIELD_BOOLEAN ),
END_DATADESC()

// Global Savedata for Toggle
BEGIN_DATADESC( CBaseToggle )
	DEFINE_FIELD( m_toggle_state, FIELD_INTEGER ),
	DEFINE_FIELD( m_flActivateFinished, FIELD_TIME ),
	DEFINE_FIELD( m_flMoveDistance, FIELD_FLOAT ),
	DEFINE_FIELD( m_flWait, FIELD_FLOAT ),
	DEFINE_FIELD( m_flLip, FIELD_FLOAT ),
	DEFINE_FIELD( m_flTWidth, FIELD_FLOAT ),
	DEFINE_FIELD( m_flTLength, FIELD_FLOAT ),
	DEFINE_FIELD( m_vecPosition1, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecPosition2, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecAngle1, FIELD_VECTOR ),		// UNDONE: Position could go through transition, but also angle?
	DEFINE_FIELD( m_vecAngle2, FIELD_VECTOR ),		// UNDONE: Position could go through transition, but also angle?
	DEFINE_FIELD( m_cTriggersLeft, FIELD_INTEGER ),
	DEFINE_FIELD( m_flHeight, FIELD_FLOAT ),
	DEFINE_FIELD( m_hActivator, FIELD_EHANDLE ),
	DEFINE_FIELD( m_pfnCallWhenMoveDone, FIELD_FUNCTION ),
	DEFINE_FIELD( m_vecFinalDest, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecFinalAngle, FIELD_VECTOR ),
	DEFINE_FIELD( m_sMaster, FIELD_STRING ),
	DEFINE_FIELD( m_bitsDamageInflict, FIELD_INTEGER ),	// damage type inflicted
END_DATADESC()

// Global Savedata for monster
// UNDONE: Save schedule data?  Can this be done?  We may
// lose our enemy pointer or other data (goal ent, target, etc)
// that make the current schedule invalid, perhaps it's best
// to just pick a new one when we start up again.
BEGIN_DATADESC( CBaseMonster )
	DEFINE_FIELD( m_hEnemy, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hTargetEnt, FIELD_EHANDLE ),
	DEFINE_ARRAY( m_hOldEnemy, FIELD_EHANDLE, MAX_OLD_ENEMIES ),
	DEFINE_ARRAY( m_vecOldEnemy, FIELD_POSITION_VECTOR, MAX_OLD_ENEMIES ),
	DEFINE_FIELD( m_flFieldOfView, FIELD_FLOAT ),
	DEFINE_FIELD( m_flWaitFinished, FIELD_TIME ),
	DEFINE_FIELD( m_flMoveWaitFinished, FIELD_TIME ),
	
	DEFINE_FIELD( m_Activity, FIELD_INTEGER ),
	DEFINE_FIELD( m_IdealActivity, FIELD_INTEGER ),
	DEFINE_FIELD( m_LastHitGroup, FIELD_INTEGER ),
	DEFINE_FIELD( m_MonsterState, FIELD_INTEGER ),
	DEFINE_FIELD( m_IdealMonsterState, FIELD_INTEGER ),
	DEFINE_FIELD( m_iTaskStatus, FIELD_INTEGER ),
	
	//Schedule_t			*m_pSchedule;
	
	DEFINE_FIELD( m_iScheduleIndex, FIELD_INTEGER ),
	DEFINE_FIELD( m_afConditions, FIELD_INTEGER ),
	//WayPoint_t			m_Route[ ROUTE_SIZE ];
	//	DEFINE_FIELD( m_movementGoal, FIELD_INTEGER ),
	//	DEFINE_FIELD( m_iRouteIndex, FIELD_INTEGER ),
	//	DEFINE_FIELD( m_moveWaitTime, FIELD_FLOAT ),
	
	DEFINE_FIELD( m_vecMoveGoal, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_movementActivity, FIELD_INTEGER ),
	
	//		int					m_iAudibleList; // first index of a linked list of sounds that the monster can hear.
	//	DEFINE_FIELD( m_afSoundTypes, FIELD_INTEGER ),
	DEFINE_FIELD( m_vecLastPosition, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_iHintNode, FIELD_INTEGER ),
	DEFINE_FIELD( m_afMemory, FIELD_INTEGER ),
	DEFINE_FIELD( m_iMaxHealth, FIELD_INTEGER ),
	
	DEFINE_FIELD( m_vecEnemyLKP, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_cAmmoLoaded, FIELD_INTEGER ),
	DEFINE_FIELD( m_afCapability, FIELD_INTEGER ),
	
	DEFINE_FIELD( m_flNextAttack, FIELD_TIME ),
	DEFINE_FIELD( m_bitsDamageType, FIELD_INTEGER ),
	DEFINE_ARRAY( m_rgbTimeBasedDamage, FIELD_CHARACTER, CDMG_TIMEBASED ),
	DEFINE_FIELD( m_bloodColor, FIELD_INTEGER ),
	DEFINE_FIELD( m_failSchedule, FIELD_INTEGER ),
	
	DEFINE_FIELD( m_flHungryTime, FIELD_TIME ),
	DEFINE_FIELD( m_flDistTooFar, FIELD_FLOAT ),
	DEFINE_FIELD( m_flDistLook, FIELD_FLOAT ),
	DEFINE_FIELD( m_iTriggerCondition, FIELD_INTEGER ),
	DEFINE_FIELD( m_iszTriggerTarget, FIELD_STRING ),
	
	DEFINE_FIELD( m_HackedGunPos, FIELD_VECTOR ),
	
	DEFINE_FIELD( m_scriptState, FIELD_INTEGER ),
	DEFINE_FIELD( m_pCine, FIELD_CLASSPTR ),
END_DATADESC()

BEGIN_DATADESC( CBasePlayerItem )
	DEFINE_FIELD( m_pPlayer, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_pNext, FIELD_CLASSPTR ),
	//DEFINE_FIELD( m_fKnown, FIELD_INTEGER ),Reset to zero on load
	DEFINE_FIELD( m_iId, FIELD_INTEGER ),
	// DEFINE_FIELD( m_iIdPrimary, FIELD_INTEGER ),
	// DEFINE_FIELD( m_iIdSecondary, FIELD_INTEGER ),
END_DATADESC()

BEGIN_DATADESC( CBasePlayerWeapon )
#if defined( CLIENT_WEAPONS )
	DEFINE_FIELD( m_flNextPrimaryAttack, FIELD_FLOAT ),
	DEFINE_FIELD( m_flNextSecondaryAttack, FIELD_FLOAT ),
	DEFINE_FIELD( m_flTimeWeaponIdle, FIELD_FLOAT ),
#else	// CLIENT_WEAPONS
	DEFINE_FIELD( m_flNextPrimaryAttack, FIELD_TIME ),
	DEFINE_FIELD( m_flNextSecondaryAttack, FIELD_TIME ),
	DEFINE_FIELD( m_flTimeWeaponIdle, FIELD_TIME ),
#endif	// CLIENT_WEAPONS
	DEFINE_FIELD( m_iPrimaryAmmoType, FIELD_INTEGER ),
	DEFINE_FIELD( m_iSecondaryAmmoType, FIELD_INTEGER ),
	DEFINE_FIELD( m_iClip, FIELD_INTEGER ),
	DEFINE_FIELD( m_iDefaultAmmo, FIELD_INTEGER ),
	//	DEFINE_FIELD( m_iClientClip, FIELD_INTEGER )	 , reset to zero on load so hud gets updated correctly
	//  DEFINE_FIELD( m_iClientWeaponState, FIELD_INTEGER ), reset to zero on load so hud gets updated correctly
END_DATADESC()

// Global Savedata for player
BEGIN_DATADESC( CBasePlayer )
	DEFINE_FIELD( m_flFlashLightTime, FIELD_TIME ),
	DEFINE_FIELD( m_iFlashBattery, FIELD_INTEGER ),
	
	DEFINE_FIELD( m_afButtonLast, FIELD_INTEGER ),
	DEFINE_FIELD( m_afButtonPressed, FIELD_INTEGER ),
	DEFINE_FIELD( m_afButtonReleased, FIELD_INTEGER ),
	
	DEFINE_ARRAY( m_rgItems, FIELD_INTEGER, MAX_ITEMS ),
	DEFINE_FIELD( m_afPhysicsFlags, FIELD_INTEGER ),
	
	DEFINE_FIELD( m_flTimeStepSound, FIELD_TIME ),
	DEFINE_FIELD( m_flTimeWeaponIdle, FIELD_TIME ),
	DEFINE_FIELD( m_flSwimTime, FIELD_TIME ),
	DEFINE_FIELD( m_flDuckTime, FIELD_TIME ),
	DEFINE_FIELD( m_flWallJumpTime, FIELD_TIME ),
	
	DEFINE_FIELD( m_flSuitUpdate, FIELD_TIME ),
	DEFINE_ARRAY( m_rgSuitPlayList, FIELD_INTEGER, CSUITPLAYLIST ),
	DEFINE_FIELD( m_iSuitPlayNext, FIELD_INTEGER ),
	DEFINE_ARRAY( m_rgiSuitNoRepeat, FIELD_INTEGER, CSUITNOREPEAT ),
	DEFINE_ARRAY( m_rgflSuitNoRepeatTime, FIELD_TIME, CSUITNOREPEAT ),
	DEFINE_FIELD( m_lastDamageAmount, FIELD_INTEGER ),
	
	DEFINE_ARRAY( m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES ),
	DEFINE_FIELD( m_pActiveItem, FIELD_CLASSPTR ),
	DEFINE_FIELD( m_pLastItem, FIELD_CLASSPTR ),
	
	DEFINE_ARRAY( m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS ),
	DEFINE_FIELD( m_idrowndmg, FIELD_INTEGER ),
	DEFINE_FIELD( m_idrownrestored, FIELD_INTEGER ),
	
	DEFINE_FIELD( m_iTrain, FIELD_INTEGER ),
	DEFINE_FIELD( m_bitsHUDDamage, FIELD_INTEGER ),
	DEFINE_FIELD( m_flFallVelocity, FIELD_FLOAT ),
	DEFINE_FIELD( m_iTargetVolume, FIELD_INTEGER ),
	DEFINE_FIELD( m_iWeaponVolume, FIELD_INTEGER ),
	DEFINE_FIELD( m_iExtraSoundTypes, FIELD_INTEGER ),
	DEFINE_FIELD( m_iWeaponFlash, FIELD_INTEGER ),
	DEFINE_FIELD( m_fLongJump, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fInitHUD, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_tbdPrev, FIELD_TIME ),
	
	DEFINE_FIELD( m_pTank, FIELD_EHANDLE ),
	DEFINE_FIELD( m_iHideHUD, FIELD_INTEGER ),
	DEFINE_FIELD( m_iFOV, FIELD_INTEGER ),
	
	//DEFINE_FIELD( m_fDeadTime, FIELD_FLOAT ), // only used in multiplayer games
	//DEFINE_FIELD( m_fGameHUDInitialized, FIELD_INTEGER ), // only used in multiplayer games
	//DEFINE_FIELD( m_flStopExtraSoundTime, FIELD_TIME ),
	//DEFINE_FIELD( m_bKnownItem, FIELD_BOOLEAN ), // reset to zero on load
	//DEFINE_FIELD( m_iPlayerSound, FIELD_INTEGER ),	// Don't restore, set in Precache()
	//DEFINE_FIELD( m_pentSndLast, FIELD_EDICT ),	// Don't restore, client needs reset
	//DEFINE_FIELD( m_flSndRoomtype, FIELD_FLOAT ),	// Don't restore, client needs reset
	//DEFINE_FIELD( m_flSndRange, FIELD_FLOAT ),	// Don't restore, client needs reset
	//DEFINE_FIELD( m_fNewAmmo, FIELD_INTEGER ), // Don't restore, client needs reset
	//DEFINE_FIELD( m_flgeigerRange, FIELD_FLOAT ),	// Don't restore, reset in Precache()
	//DEFINE_FIELD( m_flgeigerDelay, FIELD_FLOAT ),	// Don't restore, reset in Precache()
	//DEFINE_FIELD( m_igeigerRangePrev, FIELD_FLOAT ),	// Don't restore, reset in Precache()
	//DEFINE_FIELD( m_iStepLeft, FIELD_INTEGER ), // Don't need to restore
	//DEFINE_ARRAY( m_szTextureName, FIELD_CHARACTER, CBTEXTURENAMEMAX ), // Don't need to restore
	//DEFINE_FIELD( m_chTextureType, FIELD_CHARACTER ), // Don't need to restore
	//DEFINE_FIELD( m_fNoPlayerSound, FIELD_BOOLEAN ), // Don't need to restore, debug
	//DEFINE_FIELD( m_iUpdateTime, FIELD_INTEGER ), // Don't need to restore
	//DEFINE_FIELD( m_iClientHealth, FIELD_INTEGER ), // Don't restore, client needs reset
	//DEFINE_FIELD( m_iClientBattery, FIELD_INTEGER ), // Don't restore, client needs reset
	//DEFINE_FIELD( m_iClientHideHUD, FIELD_INTEGER ), // Don't restore, client needs reset
	//DEFINE_FIELD( m_fWeapon, FIELD_BOOLEAN ),  // Don't restore, client needs reset
	//DEFINE_FIELD( m_nCustomSprayFrames, FIELD_INTEGER ), // Don't restore, depends on server message after spawning and only matters in multiplayer
	//DEFINE_FIELD( m_vecAutoAim, FIELD_VECTOR ), // Don't save/restore - this is recomputed
	//DEFINE_ARRAY( m_rgAmmoLast, FIELD_INTEGER, MAX_AMMO_SLOTS ), // Don't need to restore
	//DEFINE_FIELD( m_fOnTarget, FIELD_BOOLEAN ), // Don't need to restore
	//DEFINE_FIELD( m_nCustomSprayFrames, FIELD_INTEGER ), // Don't need to restore
END_DATADESC()