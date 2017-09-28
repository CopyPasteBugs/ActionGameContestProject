#include "StdAfx.h"
#include "FootstepQueue.h"

CFootstepQueue::~CFootstepQueue()
{

}

void CFootstepQueue::Initialize()
{
	m_SpawnVec.reserve(50);
}

void CFootstepQueue::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE) 
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		for (int i = 0; i < m_SpawnVec.size(); ++i) 
		{
			
			if (m_SpawnVec[i].active)
			{
				if (m_SpawnVec[i].elapsed > 3.0f)
				{
					m_SpawnVec[i].active = false;

					if (m_SpawnVec[i].entity)
					{
						IParticleEmitter* pPE = m_SpawnVec[i].entity->GetParticleEmitter(PE_SLOT);

						if (pPE)
							gEnv->pParticleManager->DeleteEmitter(pPE);
						
						gEnv->pEntitySystem->RemoveEntity(m_SpawnVec[i].entity->GetId());
						
						m_SpawnVec[i].entity = nullptr;
					}
				}

				m_SpawnVec[i].elapsed += pCtx->fFrameTime;
			}
		}

	}
}

uint64 CFootstepQueue::GetEventMask() const
{
	return eventMask;
}

void CFootstepQueue::ClearEventMask()
{
	eventMask = 0;
}

void CFootstepQueue::QueueNewFootstep(Vec3 pos)
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = pos;
	spawnParams.qRotation = IDENTITY;

	// Spawn the entity
	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		if(IParticleEffect* pEffect = gEnv->pParticleManager->FindEffect("Assets/libs/p1.pfx", "ParticleEmitter"))
		{
			pEntity->LoadParticleEmitter(PE_SLOT, pEffect);
		}

		SSpawnedFootstep fs;

		fs.active = true;
		fs.elapsed = 0.0f;
		fs.entity = pEntity;

		int index = GetFirstFreeFootstep();

		if (index != NFOOTSTEP)
			m_SpawnVec[index] = fs;
		else
			m_SpawnVec.push_back(fs);
	}
}

int CFootstepQueue::GetFirstFreeFootstep()
{
	for (int i = 0; i < m_SpawnVec.size(); ++i)
	{
		if (!m_SpawnVec[i].active)
		{
			return i;
		}
	}

	return NFOOTSTEP;
}
