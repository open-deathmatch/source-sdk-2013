//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "gameinterface.h"
#include "mapentities.h"
#include "hl2mp_gameinterface.h"
#include "tier0/icommandline.h"
#ifdef DEATHMATCH
#include "filesystem.h"
#include "tier1/utlbuffer.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// -------------------------------------------------------------------------------------------- //
// Mod-specific CServerGameClients implementation.
// -------------------------------------------------------------------------------------------- //

void CServerGameClients::GetPlayerLimits( int& minplayers, int& maxplayers, int &defaultMaxPlayers ) const
{
	minplayers = 2;
#ifdef PLATFORM_64BITS
	maxplayers = MAX_PLAYERS;
#else
	if ( CommandLine()->HasParm("-unrestricted_maxplayers") )
	{
		static bool s_bWarned = false;
		if ( !s_bWarned )
		{
			Warning( "The use of -unrestricted_maxplayers is NOT supported and definitely NOT recommended and may be unstable.\n" );
			s_bWarned = true;
		}
		maxplayers = MAX_PLAYERS;
	}
	else
		maxplayers = 33;
#endif
	defaultMaxPlayers = 16; // misyl: Was 2... but why would the default be 2?! Is there some very intimate HL2DM going on?
}

// -------------------------------------------------------------------------------------------- //
// Mod-specific CServerGameDLL implementation.
// -------------------------------------------------------------------------------------------- //

void CServerGameDLL::LevelInit_ParseAllEntities( const char *pMapEntities )
{
#ifdef DEATHMATCH
	char szMapAddPath [ MAX_PATH ];
	Q_snprintf( szMapAddPath, sizeof( szMapAddPath ), "maps/%s_mapadd.txt", STRING( gpGlobals->mapname ) );

	if ( filesystem->FileExists( szMapAddPath, "MOD" ) )
	{
		CUtlBuffer buf;
		if ( filesystem->ReadFile( szMapAddPath, "MOD", buf ) )
		{
			buf.PutChar( '\0' );

			int newLen = Q_strlen( pMapEntities ) + buf.TellPut() + 1;
			char *pCombined = new char [ newLen ];
			Q_strncpy( pCombined, pMapEntities, newLen );
			Q_strncat( pCombined, ( const char * ) buf.Base(), newLen );

			MapEntity_ParseAllEntities( pCombined );
			delete[] pCombined;
		}
	}
#endif
}

