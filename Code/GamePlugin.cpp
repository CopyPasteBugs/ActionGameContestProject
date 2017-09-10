#include "StdAfx.h"
#include "GamePlugin.h"

#include "Components/Player.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include "Components/UnifiedCharacterController.h"
#include "Components/PlayerCharacterController.h"
#include "Components/EnemyCharacterController.h"
#include "Components/Player.h"

CGamePlugin::~CGamePlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CGamePlugin::GetCID());
	}
}

bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");
	m_updateFlags = EPluginUpdateType::EUpdateType_Update | EPluginUpdateType::EUpdateType_PrePhysicsUpdate;
	return true;
}

void CGamePlugin::OnPluginUpdate(EPluginUpdateType updateType)
{
	switch (updateType)
	{
	case EPluginUpdateType::EUpdateType_Update:
	{
		// subsystem update call
	}
	break;
	case EPluginUpdateType::EUpdateType_PrePhysicsUpdate:
	{

	}
	break;
	default:
		break;
	}
}

void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
	{
		// Register all components that belong to this plug-in
		if (gEnv->pSchematyc)
		{
			gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
				stl::make_unique<Schematyc::CEnvPackage>(
					CGamePlugin::GetCID(),
					"EntityComponents",
					"Crytek GmbH",
					"Components",
					[this](Schematyc::IEnvRegistrar& registrar) { RegisterComponents(registrar); })
			);
		}
	}
	break;
		// Called when the game framework has initialized and we are ready for game logic to start
	case ESYSTEM_EVENT_GAME_POST_INIT:
	{
		// Don't need to load the map in editor
		if (!gEnv->IsEditor())
		{
			gEnv->pConsole->ExecuteString("e_waterocean=0");
			gEnv->pConsole->ExecuteString("e_waterOceanFFT=0");
			
			//gEnv->pConsole->ExecuteString("e_fog=1");
			//gEnv->pConsole->ExecuteString("e_clouds=1");
			//gEnv->pConsole->ExecuteString("e_terrain = 1");
			//gEnv->pConsole->ExecuteString("e_sun=1");
			//gEnv->pConsole->ExecuteString("e_physOceanCell=0.0");
			//gEnv->pConsole->ExecuteString("e_Wind=0");
			//gEnv->pConsole->ExecuteString("s_DrawAudioDebug=0");
			//gEnv->pConsole->ExecuteString("ai_MNMPathFinderQuota=0.05");
			//gEnv->pConsole->ExecuteString("ai_MNMPathfinderConcurrentRequests=10");

			gEnv->pConsole->ExecuteString("p_draw_helpers=0");
			gEnv->pConsole->ExecuteString("r_RainMaxViewDist_Deferred = 100.0");

			gEnv->pConsole->ExecuteString("map example", false, true);
		}
	}
	break;
	case ESYSTEM_EVENT_LEVEL_GAMEPLAY_START:
	{
		// Setup time
		ITimeOfDay* tod = gEnv->p3DEngine->GetTimeOfDay();
		tod->LoadPreset("Assets\\libs\\environmentpresets\\example.xml");
		tod->SetTime(16.8f);
		tod->Update(true, true);

		// Global Time Scale factor
		gEnv->pConsole->ExecuteString("t_Scale=1.0");
	}
	default:
		break;
	}
}

bool CGamePlugin::RegisterFlowNodes()
{
	CryRegisterFlowNodes();
	return true;
}

bool CGamePlugin::UnregisterFlowNodes()
{
	CryUnregisterFlowNodes();
	return false;
}

void CGamePlugin::RegisterComponents(Schematyc::IEnvRegistrar & registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(IUnifiedCharacterController));
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerCharacterController));
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEnemyCharacterController));
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
	}




}

CRYREGISTER_SINGLETON_CLASS(CGamePlugin)