#pragma once

#include <CryGame/IGameFramework.h>
#include <ICryMannequin.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryCore/Containers/CryArray.h>

struct SDefaultScopeSettings
{
	inline bool operator==(const SDefaultScopeSettings& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
	static void ReflectType(Schematyc::CTypeDesc<SDefaultScopeSettings>& desc)
	{
		desc.SetGUID("{8BAF1902-9BBE-4F24-9175-9A4F31F6E3DC}"_cry_guid);
		desc.SetLabel("Animation Component Default Scope");
		desc.SetDescription("Settings for the default Mannequin scope");

	}

	string  m_controllerDefinitionPath;
	string  m_contextName;
	string  m_fragmentName;
};

class IUnifiedCharacterAnimation : public IEntityComponent
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{0374505D-634A-4763-A7F0-91A46A9DC047}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<IUnifiedCharacterAnimation>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("UnifiedCharacterAnimation");
		desc.SetDescription("Exposes playback the Mannequin systems");
		desc.SetIcon("icons:General/Mannequin.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&IUnifiedCharacterAnimation::m_characterFile, 'file', "Character", "Character", "Determines the character to load", "");
		desc.AddMember(&IUnifiedCharacterAnimation::m_databasePath, 'dbpa', "DatabasePath", "Animation Database", "Path to the Mannequin .adb file", "");
		//desc.AddMember(&IUnifiedCharacterAnimation::m_defaultScopeSettings, 'defs', "DefaultScope", "Default Scope Context Name", "Default Mannequin scope settings", SDefaultScopeSettings());
		desc.AddMember(&IUnifiedCharacterAnimation::m_bAnimationDrivenMotion, 'andr', "AnimDriven", "Animation Driven Motion", "Whether or not to use root motion in the animations", true);
		desc.AddMember(&IUnifiedCharacterAnimation::m_bGroundAlignment, 'grou', "GroundAlign", "Use Ground Alignment", "Enables adjustment of leg positions to align to the ground surface", false);

		desc.AddMember(&IUnifiedCharacterAnimation::m_controllerDefinitionPath, 'path', "ControllerDefinitionPath", "ControllerDefinitionPath", "controllerDefinitionPath", "");
		desc.AddMember(&IUnifiedCharacterAnimation::m_contextName, 'cntx', "ContextName", "ContextName", "ContextName", "ThirdPersonCharacter");
		desc.AddMember(&IUnifiedCharacterAnimation::m_fragmentName, 'frag', "FragmentName", "FragmentName", "FragmentName", "Fragment1");

	}


	IUnifiedCharacterAnimation() = default;
	virtual ~IUnifiedCharacterAnimation();

	// IEntityComponent
	virtual void   Initialize() override;
	virtual void   ProcessEvent(SEntityEvent& event) override;
	virtual uint64 GetEventMask() const override;
	// ~IEntityComponent

public:
	virtual void ActivateContext(const Schematyc::CSharedString& contextName);
	virtual void QueueFragment(const Schematyc::CSharedString& fragmentName);
	virtual void QueueFragmentWithId(const FragmentID& fragmentId);
	virtual void SetTag(const Schematyc::CSharedString& tagName, bool bSet);
	virtual void SetMotionParameter(EMotionParamID motionParam, float value);
	TagID GetTagId(const char* szTagName) const;
	FragmentID GetFragmentId(const char* szFragmentName) const;
	virtual void SetTagWithId(TagID id, bool bSet);
	ICharacterInstance* GetCharacter() const { return m_pCachedCharacter; };
	virtual void LoadFromDisk();
	virtual void ResetCharacter();
	virtual void SetAnimationDrivenMotion(bool bSet);


	virtual void SetCharacterFile(const char* szPath);
	const char*  SetCharacterFile() const { return m_characterFile.value.c_str(); }
	virtual void SetMannequinAnimationDatabaseFile(const char* szPath);
	const char*  GetMannequinAnimationDatabaseFile() const { return m_databasePath.value.c_str(); }

	virtual void SetControllerDefinitionFile(const char* szPath);
	const char*  GetControllerDefinitionFile() const { return m_controllerDefinitionPath.c_str(); }

	virtual void SetDefaultScopeContextName(const char* szName);
	const char*  GetDefaultScopeContextName() const { return m_fragmentName.c_str(); }
	virtual void SetDefaultFragmentName(const char* szName);
	const char*  GetDefaultFragmentName() const { return m_fragmentName.c_str(); }

	virtual void EnableGroundAlignment(bool bEnable) { m_bGroundAlignment = bEnable; }
	bool IsGroundAlignmentEnabled() const { return m_bGroundAlignment; }

	virtual bool IsTurning() const { return fabsf(m_turnAngle) > 0; }

	void ManualUpdate(float frametime);

protected:

	bool m_bAnimationDrivenMotion = true;
	Schematyc::CharacterFileName              m_characterFile;
	Schematyc::MannequinAnimationDatabasePath m_databasePath;

	//SDefaultScopeSettings m_defaultScopeSettings;

	IActionController*                 m_pActionController = nullptr;
	std::unique_ptr<SAnimationContext> m_pAnimationContext;
	const IAnimationDatabase*          m_pDatabase = nullptr;

	_smart_ptr<IAction>                m_pActiveAction;
	std::bitset<eMotionParamID_COUNT>  m_overriddenMotionParams;

	const SControllerDef*              m_pControllerDefinition = nullptr;
	_smart_ptr<ICharacterInstance>     m_pCachedCharacter = nullptr;

	IAnimationPoseAlignerPtr m_pPoseAligner;
	Vec3 m_prevForwardDir = ZERO;
	float m_turnAngle = 0.f;

	bool m_bGroundAlignment = false;

	Schematyc::CSharedString  m_controllerDefinitionPath;
	Schematyc::CSharedString  m_contextName;
	Schematyc::CSharedString  m_fragmentName;
};