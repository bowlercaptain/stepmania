#include "global.h"
/*
-----------------------------------------------------------------------------
 Class: LightsManager

 Desc: See header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "LightsManager.h"
#include "GameState.h"
#include "RageTimer.h"
#include "arch/Lights/LightsDriver.h"
#include "arch/arch.h"
#include "RageUtil.h"
#include "GameInput.h"	// for GameController
#include "InputMapper.h"


LightsManager*	LIGHTSMAN = NULL;	// global and accessable from anywhere in our program

LightsManager::LightsManager(CString sDriver)
{
	m_LightsMode = LIGHTSMODE_JOINING;

	m_pDriver = MakeLightsDriver(sDriver);
}

LightsManager::~LightsManager()
{
	SAFE_DELETE( m_pDriver );
}

void LightsManager::Update( float fDeltaTime )
{
	//
	// Update cabinet lights
	//
	switch( m_LightsMode )
	{
	case LIGHTSMODE_JOINING:
		{
			switch( GAMESTATE->GetNumSidesJoined() )
			{
			case 0:
				{
					int iSec = (int)(RageTimer::GetTimeSinceStart()*2);
//					float fBeatPercentage = GAMESTATE->m_fSongBeat - (int)GAMESTATE->m_fSongBeat;
					bool bOn = (iSec%2)==0; 
					for( int i=0; i<8; i++ )
						m_LightsState.m_bCabinetLights[i] = bOn;
				}
				break;
			default:
				for( int i=0; i<8; i++ )
				{
					bool bOn = GAMESTATE->m_bSideIsJoined[i%2];
					m_LightsState.m_bCabinetLights[i] = bOn;
				}
			}
		}
		break;
	case LIGHTSMODE_ATTRACT:
		{
			int iSec = (int)RageTimer::GetTimeSinceStart();
			int i;
			for( i=0; i<4; i++ )
			{
				bool bOn = (iSec%4)==i;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
			for( i=4; i<6; i++ )
			{
				bool bOn = false;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
			for( i=6; i<8; i++ )
			{
				bool bOn = (iSec%3)==0;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
		}
		break;
	case LIGHTSMODE_MENU:
		{
			int iSec = (int)RageTimer::GetTimeSinceStart();
			int i;
			for( i=0; i<2; i++ )
			{
				bool bOn = (iSec%2)==0;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
			for( i=2; i<4; i++ )
			{
				bool bOn = (iSec%2)==1;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
			for( i=4; i<6; i++ )
			{
				bool bOn = (iSec%2)==0;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
			for( i=6; i<8; i++ )
			{
				bool bOn = (iSec%2)==1;
				m_LightsState.m_bCabinetLights[i] = bOn;
			}
		}
		break;
	case LIGHTSMODE_DEMONSTRATION:
	case LIGHTSMODE_GAMEPLAY_READY:
	case LIGHTSMODE_GAMEPLAY_GO:
		{
			int i;

			bool bMarqueeLightsOn = m_LightsMode == LIGHTSMODE_GAMEPLAY_READY;

			for( i=0; i<4; i++ )
				m_LightsState.m_bCabinetLights[i] = bMarqueeLightsOn;

			// menu lights are always off
			for( i=4; i<6; i++ )
				m_LightsState.m_bCabinetLights[i] = false;

			// bass lights
			float fBeatPercentage = GAMESTATE->m_fSongBeat - (int)GAMESTATE->m_fSongBeat;
			bool bOn = fBeatPercentage < 0.1 || fBeatPercentage > 0.9; 
			for( i=7; i<8; i++ )
				m_LightsState.m_bCabinetLights[i] = bOn;
		}
		break;
	case LIGHTSMODE_STAGE:
	case LIGHTSMODE_ALL_CLEARED:
		{
			FOREACH_CabinetLight( cl )
				m_LightsState.m_bCabinetLights[cl] = true;
		}
		break;
	default:
		ASSERT(0);
	}

	//
	// Update game controller lights
	//
	// FIXME:  Works only with Game==dance
	// FIXME:  lights pads for players who aren't playing
	switch( m_LightsMode )
	{
	case LIGHTSMODE_ATTRACT:
	case LIGHTSMODE_MENU:
	case LIGHTSMODE_DEMONSTRATION:
		{
			ZERO( m_LightsState.m_bGameButtonLights );
		}
		break;
	case LIGHTSMODE_ALL_CLEARED:
	case LIGHTSMODE_STAGE:
	case LIGHTSMODE_JOINING:
		{
			FOREACH_GameController( gc )
			{
				bool bOn = GAMESTATE->m_bSideIsJoined[gc];

				FOREACH_GameButton( gb )
					m_LightsState.m_bGameButtonLights[gc][gb] = bOn;
			}
		}
		break;
	case LIGHTSMODE_GAMEPLAY_READY:
	case LIGHTSMODE_GAMEPLAY_GO:
		{
			FOREACH_GameController( gc )
			{
				FOREACH_GameButton( gb )
				{
					bool bOn = INPUTMAPPER->IsButtonDown( GameInput(gc,gb) );
					m_LightsState.m_bGameButtonLights[gc][gb] = bOn;
				}
			}
		}
		break;
	default:
		ASSERT(0);
	}

	// apply new light values we set above
	m_pDriver->Set( &m_LightsState );
}

void LightsManager::SetLightsMode( LightsMode lm )
{
	m_LightsMode = lm;
}
