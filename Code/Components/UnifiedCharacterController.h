#pragma once

struct SUnifiedPhysics
{
	inline bool operator==(const SUnifiedPhysics &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

	static void ReflectType(Schematyc::CTypeDesc<SUnifiedPhysics>& desc)
	{

		desc.SetGUID("{7DD698B6-E85A-4CFA-A556-5B7F5BDAD3B9}"_cry_guid);
		desc.AddMember(&SUnifiedPhysics::m_mass, 'mass', "Mass", "Mass", "Mass of the character in kg", 80.f);
		desc.AddMember(&SUnifiedPhysics::m_radius, 'radi', "Radius", "Collider Radius", "Radius of the capsule or cylinder", 0.45f);
		desc.AddMember(&SUnifiedPhysics::m_height, 'heig', "Height", "Collider Height", "Height of the capsule or cylinder", 0.935f);
		desc.AddMember(&SUnifiedPhysics::m_bCapsule, 'caps', "Capsule", "Use Capsule", "Whether or not to use a capsule as the main collider, otherwise cylinder", true);
		desc.AddMember(&SUnifiedPhysics::m_offset, 'offs', "Offset", "Offset", "Offset from Model's origin by Y axis", 0.0f);
		desc.AddMember(&SUnifiedPhysics::m_bSendCollisionSignal, 'send', "SendCollisionSignal", "SendCollisionSignal", "SendCollisionSignal", true);
	}

	Schematyc::PositiveFloat m_mass = 80.f;
	float m_radius = 0.45f;
	float m_height = 0.935f;
	bool m_bCapsule = true;
	float m_offset = 0.0f;
	bool m_bSendCollisionSignal = true;
};

struct SUnifiedMovement
{
	inline bool operator==(const SUnifiedMovement &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

	static void ReflectType(Schematyc::CTypeDesc<SUnifiedMovement>& desc)
	{
		desc.SetGUID("{32564947-08DF-4F2B-B747-129F94EEDA7D}"_cry_guid);
		desc.AddMember(&SUnifiedMovement::m_airControlRatio, 'airc', "AirControl", "Air Control Ratio", "Indicates how much the character can move in the air, 0 means no movement while 1 means full control.", 1.f);
		desc.AddMember(&SUnifiedMovement::m_airResistance, 'airr', "AirResistance", "Air Resistance", nullptr, 0.2f);
		desc.AddMember(&SUnifiedMovement::m_inertia, 'iner', "Inertia", "Inertia Coefficient", "More amount gives less inertia, 0 being none", 8.f);
		desc.AddMember(&SUnifiedMovement::m_inertiaAcceleration, 'inea', "InertiaAcc", "Inertia Acceleration Coefficient", "More amount gives less inertia on acceleration, 0 being none", 8.f);
		desc.AddMember(&SUnifiedMovement::m_maxClimbAngle, 'maxc', "MaxClimb", "Maximum Climb Angle", "Maximum angle the character can climb", 50.0_degrees);
		desc.AddMember(&SUnifiedMovement::m_maxJumpAngle, 'maxj', "MaxJump", "Maximum Jump Angle", "Maximum angle the character can jump at", 50.0_degrees);
		desc.AddMember(&SUnifiedMovement::m_minSlideAngle, 'mins', "MinSlide", "Minimum Angle For Slide", "Minimum angle before the player starts sliding", 70.0_degrees);
		desc.AddMember(&SUnifiedMovement::m_minFallAngle, 'minf', "MinFall", "Minimum Angle For Fall", "Minimum angle before the character starts falling", 80.0_degrees);
		desc.AddMember(&SUnifiedMovement::m_maxGroundVelocity, 'maxg', "MaxGroundVelocity", "Maximum Surface Velocity", "Maximum velocity of the surface the character is on before they are considered airborne and slide off", DEG2RAD(50.f));
	}

	Schematyc::Range<0, 1> m_airControlRatio = 0.f;
	Schematyc::Range<0, 10000> m_airResistance = 0.2f;
	Schematyc::Range<0, 10000> m_inertia = 8.f;
	Schematyc::Range<0, 10000> m_inertiaAcceleration = 8.f;

	CryTransform::CClampedAngle<0, 90> m_maxClimbAngle = 50.0_degrees;
	CryTransform::CClampedAngle<0, 90> m_maxJumpAngle = 50.0_degrees;
	CryTransform::CClampedAngle<0, 90> m_minFallAngle = 80.0_degrees;
	CryTransform::CClampedAngle<0, 90> m_minSlideAngle = 70.0_degrees;

	Schematyc::Range<0, 10000> m_maxGroundVelocity = 16.f;
};

class IUnifiedCharacterController : 
	public IEntityComponent
#ifndef RELEASE
	,public IEntityComponentPreviewer
#endif
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{8F590B2E-D13A-463B-954F-6227EAC7D48D}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<IUnifiedCharacterController>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("UnifiedCharacterController");
		desc.SetDescription("");
		desc.SetIcon("icons:General/Core.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&IUnifiedCharacterController::m_unifiedPhysics, 'phys', "UnifiedPhysics", "UnifiedPhysics", "UnifiedPhysical properties for the characters", SUnifiedPhysics());
		desc.AddMember(&IUnifiedCharacterController::m_unifiedMovement, 'move', "UnifiedMovement", "UnifiedMovement", "UnifiedMovement properties for the character", SUnifiedMovement());

	}

	IUnifiedCharacterController() = default;
	virtual ~IUnifiedCharacterController();

	// IEntityComponent
	virtual void Initialize();
	virtual void ProcessEvent(SEntityEvent& event);
	virtual uint64 GetEventMask() const;

#ifndef RELEASE
	virtual IEntityComponentPreviewer* GetPreviewer() final { return this; }
	// ~IEntityComponent
	// IEntityComponentPreviewer
	virtual void SerializeProperties(Serialization::IArchive& archive) final {}
	virtual void Render(const IEntity& entity, const IEntityComponent& component, SEntityPreviewContext &context) const final;
	// ~IEntityComponentPreviewer
#endif

public: // extended functionality of component
	bool IsOnGround() const { return m_bOnGround; }
	const Schematyc::UnitLength<Vec3>& GetGroundNormal() const { return m_groundNormal; }
	virtual SUnifiedPhysics& GetUnifiedPhysics() { return m_unifiedPhysics; };
	virtual SUnifiedMovement& GetUnifiedMovement() { return m_unifiedMovement; };
	const SUnifiedPhysics& GetUnifiedPhysics() const { return m_unifiedPhysics; };
	const SUnifiedMovement& GetUnifiedMovement() const { return m_unifiedMovement; };

	virtual void AddVelocity(const Vec3& velocity)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
		{
			pe_action_move moveAction;

			// Apply movement request directly to velocity
			moveAction.iJump = 2;
			moveAction.dir = velocity;

			// Dispatch the movement request
			pPhysicalEntity->Action(&moveAction);
		}
	}

	virtual void SetVelocity(const Vec3& velocity)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
		{
			pe_action_move moveAction;

			// Override velocity
			moveAction.iJump = 1;
			moveAction.dir = velocity;

			// Dispatch the movement request
			pPhysicalEntity->Action(&moveAction);
		}
	}

	const Vec3& GetVelocity() const { return m_velocity; }
	Vec3 GetMoveDirection() const { return m_velocity.GetNormalized(); }
	Vec3 GetMoveDirection2D() const { Vec3 unifiedVelocity2D(m_velocity.x, m_velocity.y, 0.0f); return unifiedVelocity2D.GetNormalized(); }

	bool IsWalking() const { return m_velocity.GetLength2D() > 0.2f && m_bOnGround; }
	virtual void Physicalize();
	virtual void PhysicalizeWithNone();
	virtual void Ragdollize();

protected:
	SUnifiedPhysics m_unifiedPhysics;
	SUnifiedMovement m_unifiedMovement;
	bool m_bOnGround = false;
	Schematyc::UnitLength<Vec3> m_groundNormal = Vec3(0, 0, 1);
	Vec3 m_velocity = ZERO;
};