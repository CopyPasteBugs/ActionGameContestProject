// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_Game
#define GAME_API   DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryNetwork/ISerialize.h>

#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/CoreAPI.h>
#include <CryAudio/IAudioSystem.h>
#include <CryAudio/IAudioInterfacesCommonData.h>

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <Cry3DEngine/ITimeOfDay.h>
#include <CryMath/Cry_Camera.h>
#include <ICryMannequin.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <CryMath/Random.h>

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>


#include <CryFlowGraph/IFlowBaseNode.h>
#include <CryMath/Random.h>