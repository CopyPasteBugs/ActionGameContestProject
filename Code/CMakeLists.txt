cmake_minimum_required (VERSION 3.6.2)
set(CRYENGINE_DIR "C:/Program Files (x86)/Crytek/CRYENGINE Launcher/Crytek/CRYENGINE_5.4")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "C:/CryProjects/ContestProject")

include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS ContestProject
    SOURCE_GROUP "Root"
		"GamePlugin.cpp"
		"StdAfx.cpp"
		"GamePlugin.h"
		"StdAfx.h"
)
add_sources("Components_uber.cpp"
    PROJECTS ContestProject
    SOURCE_GROUP "Components"
		"Components/Player.cpp"
		"Components/Player.h"
    "Components/UnifiedCharacterController.h"
    "Components/UnifiedCharacterController.cpp"
    "Components/PlayerCharacterController.h"
    "Components/PlayerCharacterController.cpp"
    "Components/EnemyCharacterController.h"
    "Components/EnemyCharacterController.cpp"
    "Components/UnifiedCamera.h"
    "Components/UnifiedCamera.cpp"
    "Components/ThirdPersonCamera.h"
    "Components/ThirdPersonCamera.cpp"
    "Components/GameCommon.h"
    "Components/GameCommon.cpp"
    "Components/UnifiedCharacterAnimation.h"
    "Components/UnifiedCharacterAnimation.cpp"
    "Components/FootstepQueue.h"
    "Components/FootstepQueue.cpp"
    "Components/SimpleGeom.h"
    "Components/SimpleGeom.cpp"
    "Components/ProximityTrigger.h"
    "Components/ProximityTrigger.cpp"
    "Components/SplineComponent.h"
    "Components/SplineComponent.cpp"
    
)

end_sources()

CryEngineModule(ContestProject PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE 
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
	"${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
	"${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

# Set StartUp project in Visual Studio

add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"C:/CryProjects/ContestProject/ContestProject.cryproject\"")
endif()

add_library(Sandbox STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(Sandbox "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"C:/CryProjects/ContestProject/ContestProject.cryproject\"")
endif()

add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"C:/CryProjects/ContestProject/ContestProject.cryproject\"")
endif()

set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"C:/CryProjects/ContestProject/ContestProject.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
#END-CUSTOM