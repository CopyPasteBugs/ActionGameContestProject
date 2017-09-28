#pragma once

#include <Cry3DEngine\IIndexedMesh.h>

class CSimpleGeom :public IEntityComponent
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{68987D5E-02DA-411A-8731-C422FE1BFFD3}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CSimpleGeom>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("SimpleGeom");
		desc.SetDescription("Simple Geom");
		desc.SetIcon("icons:General/Core.ico");
		desc.SetComponentFlags({IEntityComponent::EFlags::ClientOnly });

		desc.AddMember(&CSimpleGeom::m_value, 'valu', "Value", "Value", nullptr, 0.0f);
	}

	CSimpleGeom() = default;
	virtual ~CSimpleGeom();

	// IEntityComponent
	virtual void Initialize();
	virtual void ProcessEvent(SEntityEvent& event);
	virtual uint64 GetEventMask() const;
	
protected:

	void BuildGeom();
	float m_value = 0.0f;
	bool bInited = false;

	_smart_ptr<IStatObj> pStaticObject = nullptr;
	_smart_ptr<IMaterial> material = nullptr;
	Matrix34 matrix;
	Vec3 pos[3];
	Vec2 uv[3];
	Vec3 tng[3];
	vtx_idx ind[3];
	SMeshSubset subset;
	SMeshFace face;

};