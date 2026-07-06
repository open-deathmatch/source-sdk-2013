//========= Copyright Open Deathmatch, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef POWERUP_H
#define POWERUP_H
#ifdef _WIN32
#pragma once
#endif

#include "player.h"
#include "gamerules.h"
#include "Sprite.h"

enum PowerupType_t
{
	POWERUP_HEALTH = 0,
	POWERUP_ARMOR,

	POWERUP_COUNT
};

class CPowerup : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPowerup, CBaseAnimating );
	DECLARE_DATADESC();

	CPowerup();
	virtual ~CPowerup();

	virtual void Spawn( void ) OVERRIDE;
	virtual void Precache( void ) OVERRIDE;
	void PowerupTouch( CBaseEntity *pOther );

	void InputActivate( inputdata_t &inputdata );
	void InputDeactivate( inputdata_t &inputdata );
	void InputSetRespawnTime( inputdata_t &inputdata );

private:
	void CreateGlowSprite( void );
	void RemoveGlowSprite( void );
	void RespawnThink( void );

	void PickRandomPowerupType( void );
	void ApplyPowerupEffect( CBasePlayer *pPlayer );

	CHandle<CSprite> m_hGlowSprite;
	float m_flRespawnTime;
	bool m_bActive;
	bool m_bRotating;
	float m_flSpriteHeight;

	PowerupType_t m_nPowerupType;
};

#endif