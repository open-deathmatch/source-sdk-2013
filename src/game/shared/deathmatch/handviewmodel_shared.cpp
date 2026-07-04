//========== Copyright (C) 2026, Open Deathmatch, All rights reserved. ===========//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "baseviewmodel_shared.h"
#ifdef CLIENT_DLL
#include "materialsystem/imaterialvar.h"
#include "proxyentity.h"
#include "c_hl2mp_player.h"
#include "c_baseanimating.h"
#endif

#if defined( CLIENT_DLL )
	#define CHandViewModel C_HandViewModel
#endif

class CHandViewModel : public CBaseViewModel
{
	DECLARE_CLASS( CHandViewModel, CBaseViewModel );

public:
	DECLARE_NETWORKCLASS();

private:
};

LINK_ENTITY_TO_CLASS( hand_viewmodel, CHandViewModel );
IMPLEMENT_NETWORKCLASS_ALIASED( HandViewModel, DT_HandViewModel )

// For whatever reason the parent doesn't get sent 
// And I don't really want to mess with BaseViewModel
// so now it does
BEGIN_NETWORK_TABLE( CHandViewModel, DT_HandViewModel )
#ifndef CLIENT_DLL
	SendPropEHandle( SENDINFO_NAME( m_hMoveParent, moveparent ) ),
#else
	RecvPropInt( RECVINFO_NAME( m_hNetworkMoveParent, moveparent ), 0, RecvProxy_IntToMoveParent ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
class PlayerColorProxy : public CEntityMaterialProxy
{
public:
	virtual bool	   Init( IMaterial *pMaterial, KeyValues *pKeyValues );
	virtual void	   OnBind( C_BaseEntity *pBaseEntity );
	virtual void	   Release();
	virtual IMaterial *GetMaterial();

private:
	IMaterial *m_pMaterial;
	IMaterialVar *m_pResultVar;
};

bool PlayerColorProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	bool foundVar;
	m_pResultVar = pMaterial->FindVar( "$color2", &foundVar, false );
	m_pMaterial = pMaterial;
	return foundVar;
}

void PlayerColorProxy::OnBind( C_BaseEntity *pBaseEntity )
{
	if ( !m_pResultVar || !pBaseEntity )
		return;

	CBaseAnimating *pAnimating = dynamic_cast< CBaseAnimating * >( pBaseEntity );
	if ( !pAnimating )
		return;

	float			r, g, b;
	C_HL2MP_Player *pPlayer = nullptr;

	// If entity is a ragdoll try to convert it into the player
	// ( this applies to their corpses )
	if ( pAnimating->IsRagdoll() )
	{
		C_HL2MPRagdoll *pRagdoll = dynamic_cast< C_HL2MPRagdoll * >( pAnimating );
		if ( pRagdoll )
			pPlayer = dynamic_cast< C_HL2MP_Player * >( pRagdoll->m_hPlayer.Get() );
	}

	if ( !pPlayer )
		pPlayer = dynamic_cast< C_HL2MP_Player * >( pAnimating );

	if ( !pPlayer )
	{
		C_BaseViewModel *pVM = dynamic_cast< C_BaseViewModel * >( pAnimating );
		if ( pVM )
			pPlayer = ToHL2MPPlayer( pVM->GetOwner() );
	}

	if ( pPlayer )
	{
		// Dirty hack, however we're only using this proxy when
		// we are using any of the rebel models so its okay.
		r = 79 / 255.0f;
		g = 92 / 255.0f;
		b = 75 / 255.0f;
	}

	m_pResultVar->SetVecValue( r, g, b );
}

void PlayerColorProxy::Release()
{
}

IMaterial *PlayerColorProxy::GetMaterial()
{
	return m_pMaterial;
}

EXPOSE_INTERFACE( PlayerColorProxy, IMaterialProxy, "PlayerColor" IMATERIAL_PROXY_INTERFACE_VERSION );
#endif