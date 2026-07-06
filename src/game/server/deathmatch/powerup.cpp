//========= Copyright Open Deathmatch, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "powerup.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CPowerup )
	DEFINE_FIELD( m_hGlowSprite, FIELD_EHANDLE ),
	DEFINE_FIELD( m_flRespawnTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bRotating, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flSpriteHeight, FIELD_FLOAT ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Deactivate", InputDeactivate ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetRespawnTime", InputSetRespawnTime ),

	DEFINE_THINKFUNC( RespawnThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup, CPowerup );

CPowerup::CPowerup() :
	m_hGlowSprite( NULL ),
	m_flRespawnTime( 30.0f ),
	m_bActive( true ),
	m_bRotating( true ),
	m_flSpriteHeight( 32.0f )
{
}

CPowerup::~CPowerup()
{
	RemoveGlowSprite();
}

void CPowerup::Precache( void )
{
	BaseClass::Precache();

	PrecacheModel( "models/propper/powerup_stand/powerup_stand.mdl" );
	PrecacheScriptSound( "AlyxEmp.Charge" );
	PrecacheScriptSound( "ODM.CollectPowerup" );
}

void CPowerup::Spawn( void )
{
	Precache();
	SetModel( "models/propper/powerup_stand/powerup_stand.mdl" );
	SetModelScale( 0.5f );

	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_TRIGGER | FSOLID_NOT_SOLID );
	SetCollisionBoundsFromModel();

	AddEffects( EF_NOSHADOW );

	//CollisionProp()->UseTriggerBounds( true, 0 );

	//UTIL_SetSize( this, Vector( -16, -16, -16 ), Vector( 16, 16, 16 ) );

	PickRandomPowerupType();
	CreateGlowSprite();

	m_bActive = true;

	SetTouch( &CPowerup::PowerupTouch );

	BaseClass::Spawn();
}

void CPowerup::CreateGlowSprite( void )
{
	RemoveGlowSprite();

	Vector vecSpritePos = GetAbsOrigin();
	vecSpritePos.z += m_flSpriteHeight;

	CSprite *pSprite = nullptr;
	switch ( m_nPowerupType )
	{
		case POWERUP_HEALTH:
			pSprite = CSprite::SpriteCreate( "sprites/powerup_health.vmt", vecSpritePos, true );
			break;
		case POWERUP_ARMOR:
			pSprite = CSprite::SpriteCreate( "sprites/powerup_suit.vmt", vecSpritePos, true );
			break;
	}

	if ( pSprite )
	{
		pSprite->SetParent( this );

		pSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 200, kRenderFxPulseSlow );
		pSprite->SetScale( 0.4f );
		pSprite->SetGlowProxySize( 0.0f );

		pSprite->SetRenderMode( kRenderTransAdd );
		pSprite->SetRenderColor( 255, 200, 100 );

		m_hGlowSprite = pSprite;
	}
}

void CPowerup::RemoveGlowSprite( void )
{
	if ( m_hGlowSprite )
	{
		UTIL_Remove( m_hGlowSprite );
		m_hGlowSprite = NULL;
	}
}

void CPowerup::PowerupTouch( CBaseEntity *pOther )
{
	if ( !m_bActive )
		return;

	CBasePlayer *pPlayer = ToBasePlayer( pOther );
	if ( !pPlayer || !pPlayer->IsAlive() )
		return;

	ApplyPowerupEffect( pPlayer );

	EmitSound( "ODM.CollectPowerup" );

	m_bActive = false;
	//AddEffects( EF_NODRAW );
	SetTouch( NULL );

	RemoveGlowSprite();

	SetThink( &CPowerup::RespawnThink );
	SetNextThink( gpGlobals->curtime + m_flRespawnTime );

	return;
}

void CPowerup::RespawnThink( void )
{
	m_bActive = true;
	//RemoveEffects( EF_NODRAW );
	SetTouch( &CPowerup::PowerupTouch );

#ifdef HL2MP
	EmitSound( "AlyxEmp.Charge" );
#endif

	PickRandomPowerupType();
	CreateGlowSprite();
}

void CPowerup::ApplyPowerupEffect( CBasePlayer *pPlayer )
{
	switch ( m_nPowerupType )
	{
		case POWERUP_HEALTH:
			UTIL_ScreenFade( pPlayer, { 255, 0, 0, 128 }, 0.75f, 0.05f, FFADE_IN );
			pPlayer->SetHealth( 150 );
			break;

		case POWERUP_ARMOR:
			UTIL_ScreenFade( pPlayer, { 0, 255, 255, 128 }, 0.75f, 0.05f, FFADE_IN );
			pPlayer->SetArmorValue( 200 );
			break;

		default:
			break;
	}
}

void CPowerup::InputActivate( inputdata_t &inputdata )
{
	if ( !m_bActive )
	{
		m_bActive = true;
		//RemoveEffects( EF_NODRAW );
		SetTouch( &CPowerup::PowerupTouch );
		PickRandomPowerupType();
		CreateGlowSprite();
	}
}

void CPowerup::InputDeactivate( inputdata_t &inputdata )
{
	if ( m_bActive )
	{
		m_bActive = false;
		//AddEffects( EF_NODRAW );
		SetTouch( NULL );
		RemoveGlowSprite();
	}
}

void CPowerup::InputSetRespawnTime( inputdata_t &inputdata )
{
	m_flRespawnTime = inputdata.value.Float();
}

void CPowerup::PickRandomPowerupType( void )
{
	m_nPowerupType = ( PowerupType_t ) RandomInt( 0, POWERUP_COUNT - 1 );
}
