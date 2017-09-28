#include "StdAfx.h"
#include "SimpleGeom.h"

CSimpleGeom::~CSimpleGeom()
{
}

void CSimpleGeom::Initialize()
{
	BuildGeom();
	material = gEnv->p3DEngine->GetMaterialManager()->GetDefaultHelperMaterial();
}

void CSimpleGeom::ProcessEvent(SEntityEvent & event)
{
	//m_pEntity->GetProxy(EEntityProxy::ENTITY_PROXY_TRIGGER);
}

uint64 CSimpleGeom::GetEventMask() const
{
	return uint64();
}

void CSimpleGeom::BuildGeom()
{
	if (!bInited)
	{
		if (!pStaticObject)
		{
			pStaticObject = gEnv->p3DEngine->CreateStatObj();
			IIndexedMesh* idxMesh = pStaticObject->GetIndexedMesh();
			CMesh* mesh = idxMesh->GetMesh();
			int count = 3;
			mesh->SetIndexCount(count);
			mesh->SetVertexCount(count);
			mesh->SetTexCoordsCount(count);
			int gotElements = 0;

			//Vec3* pos = mesh->GetStreamPtr<Vec3>(CMesh::POSITIONS, &gotElements);

			pos[0] = Vec3(0, 0, 0);
			pos[1] = Vec3(0, 100, 0);
			pos[2] = Vec3(0, 100, 100);

			mesh->SetSharedStream(CMesh::POSITIONS, &pos[0], count);


			tng[0] = Vec3(1, 0, 0);
			tng[1] = Vec3(1, 0, 0);
			tng[2] = Vec3(1, 0, 0);

			mesh->SetSharedStream(CMesh::NORMALS, &tng[0], count);

			//uv = mesh->GetStreamPtr<Vec2>(CMesh::TEXCOORDS, &gotElements);

			uv[0] = Vec2(0, 0);
			uv[1] = Vec2(1, 0);
			uv[2] = Vec2(1, 1);

			mesh->SetSharedStream(CMesh::TEXCOORDS, &uv[0], count);

			//ind = mesh->GetStreamPtr<vtx_idx>(CMesh::INDICES, &gotElements);

			ind[0] = 0;
			ind[1] = 1;
			ind[2] = 2;

			mesh->SetSharedStream(CMesh::INDICES, &ind[0], count);

			subset.nNumIndices = count;
			subset.nNumVerts = count;
			subset.nMatID = 0;
			//subset.FixRanges(&ind[0]);
			subset.nFirstVertId = 0;
			subset.nFirstIndexId = 0;

			mesh->m_subsets.clear();
			mesh->m_subsets.push_back(subset);

			face.v[0] = 0;
			face.v[1] = 1;
			face.v[2] = 2;
			face.nSubset = 1;

			mesh->SetSharedStream(CMesh::FACES, &face, 1);

			mesh->m_bbox = AABB(Vec3(-100, -100, -100), Vec3(100, 100, 100));

			bool ret = mesh->Validate(nullptr);

			// make the static object update
			pStaticObject->SetFlags(STATIC_OBJECT_GENERATED | STATIC_OBJECT_DYNAMIC);
			pStaticObject->Invalidate();
			//pStaticObject->SetMaterial(material); // 0 reaction
			m_pEntity->SetMaterial(material); // this works

			uint32 flags = ENTITY_SLOT_RENDER | ENTITY_SLOT_CAST_SHADOW;
			m_pEntity->SetSlotFlags(1,  flags);
			m_pEntity->SetStatObj(pStaticObject, 1, false);
		}

		bInited = true;
	}
}
