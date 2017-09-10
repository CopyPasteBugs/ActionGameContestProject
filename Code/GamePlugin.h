#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>

class CPlayerComponent;

// The entry-point of the application
// An instance of CGamePlugin is automatically created when the library is loaded
// We then construct the local player entity and CPlayerComponent instance when OnClientConnectionReceived is first called.
class CGamePlugin 
	: public ICryPlugin
	, public ISystemEventListener
{
public:
	CRYINTERFACE_SIMPLE(ICryPlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CGamePlugin, "ContestProject", "54ED7F4E-45B5-4072-A12D-A4584E827BFC"_cry_guid)

	virtual ~CGamePlugin();
	
	// ICryPlugin
	virtual const char* GetName() const override { return "ContestProject"; }
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	virtual void OnPluginUpdate(EPluginUpdateType updateType) override;
	// ~ICryPlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	virtual bool RegisterFlowNodes() override;
	virtual bool UnregisterFlowNodes() override;

	static CryGUID GetSchematycPackageGUID() { return "{4BE140EE-D740-48BA-918B-1BE12F047713}"_cry_guid; }
	void RegisterComponents(Schematyc::IEnvRegistrar& registrar);

};