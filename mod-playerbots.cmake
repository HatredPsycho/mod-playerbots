# Dungeon Clear (from mod-dungeon-clear): compile settings for the module library and its test target.
# Sources live in src/Ai/Dungeon/DungeonClear (the AI), src/QueueFill (dungeon and battleground queue fill)
# and src/DungeonClear (scripts, command, test runs); tests in test/dungeon_clear.
# MSVC: force _USE_MATH_DEFINES onto the compiler command line.
#
# MSVC's <math.h> only defines M_PI (and the rest of the M_* family) when
# _USE_MATH_DEFINES is defined BEFORE math.h is first included; it is #pragma
# once, so setting the macro later has no effect. AzerothCore sets it in
# src/common/Define.h, which works for core translation units because they reach
# Define.h before any standard math header — but a module TU that includes
# <cmath> (directly, or via <algorithm>/<vector>/G3D) before the first core
# header does not, and then every M_PI in that TU is undeclared. That breaks the
# CORE's own headers, not just ours: Position.h::NormalizeOrientation calls
#   std::fmod(o, 2.0f * static_cast<float>(M_PI))
# so the reported "error C2065: 'M_PI': undeclared identifier" is immediately
# followed by the cascade "error C2661: 'fmod': no overloaded function takes 1
# arguments" (the second argument failed to compile, so the call is seen with
# one). Nothing we can do inside our own sources fixes a core header — a
# command-line define is the only ordering-proof place for it.
#
# This file is included from modules/CMakeLists.txt AFTER the targets are
# created, so both linkage modes can be handled here. PRIVATE: it changes how
# these sources compile, nothing downstream. Our own sources additionally avoid
# M_PI entirely (DC_PI in Util/DungeonClearTuning.h), so this is only needed for
# the core headers we include.
# Spelled as a raw /D option rather than target_compile_definitions, and with a
# trailing '=', for one reason: src/common/Define.h line 38 ALSO does
#   #define _USE_MATH_DEFINES
# with an empty replacement list. A bare -D gives the macro the value 1, which is
# a NON-identical redefinition, and MSVC then emits
#   warning C4005: '_USE_MATH_DEFINES': macro redefinition
# once per translation unit — hundreds of lines of noise that bury real
# diagnostics. '/D_USE_MATH_DEFINES=' defines it EMPTY, identical to Define.h's,
# so the redefinition is legal and silent. target_compile_definitions cannot
# express this: CMake escapes "NAME=" into -DNAME="" (verified), which is a value
# of "" and warns just the same.
if (MSVC)
    foreach (DC_MATH_TARGET modules mod_mod-playerbots)
        if (TARGET ${DC_MATH_TARGET})
            target_compile_options(${DC_MATH_TARGET} PRIVATE /D_USE_MATH_DEFINES=)
        endif()
    endforeach()
endif()

# Core-shape probes. See src/Ai/Dungeon/DungeonClear/Util/DcCoreCompat.h for
# what each one gates and why it cannot be a plain #if on a core macro. Each
# greps the exact token in the core header rather than a proxy (a file's
# existence), so a fork that half-syncs upstream still reads correctly.
set(DC_CORE_COMPAT_DEFS "")
file(READ "${CMAKE_SOURCE_DIR}/src/server/game/Scripting/ScriptDefines/ServerScript.h" _dc_server_script_h)
string(FIND "${_dc_server_script_h}" "SERVERHOOK_ON_PACKET_SENT" _dc_pos)
if (NOT _dc_pos EQUAL -1)
    list(APPEND DC_CORE_COMPAT_DEFS DC_CORE_HAS_ON_PACKET_SENT=1)
endif()
file(READ "${CMAKE_SOURCE_DIR}/src/server/game/World/IWorld.h" _dc_iworld_h)
string(FIND "${_dc_iworld_h}" "AddQueryHolderCallback" _dc_pos)
if (NOT _dc_pos EQUAL -1)
    list(APPEND DC_CORE_COMPAT_DEFS DC_CORE_IWORLD_HAS_QUERYHOLDER=1)
endif()
if (DC_CORE_COMPAT_DEFS)
    foreach (DC_COMPAT_TARGET modules mod_mod-playerbots)
        if (TARGET ${DC_COMPAT_TARGET})
            target_compile_definitions(${DC_COMPAT_TARGET} PRIVATE ${DC_CORE_COMPAT_DEFS})
        endif()
    endforeach()
endif()
# The test target is defined in a deferred call that runs in the ROOT
# directory's scope, where a variable set here is not visible.
set_property(GLOBAL PROPERTY DC_CORE_COMPAT_DEFS "${DC_CORE_COMPAT_DEFS}")

if (BUILD_TESTING)
    function(define_dungeon_clear_tests)
        set(MOD_PATH "${CMAKE_SOURCE_DIR}/modules/mod-playerbots")
        get_property(DC_CORE_COMPAT_DEFS GLOBAL PROPERTY DC_CORE_COMPAT_DEFS)

        # Define our standalone test target
        add_executable(dungeon_clear_tests
            "${MOD_PATH}/test/dungeon_clear/TestDoorPolicy.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDungeonClearMath.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestHealReposition.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestCombatRegroup.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestEscapeLeap.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSmartRest.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestPostCombatRez.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestStrandedRecovery.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestFightInPlace.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBossPullback.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestScriptedPull.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSocialQuarantine.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSealedEncounter.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestWaitAtBoss.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDungeonClearUtil.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDungeonClearApproach.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestApproachDecisions.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDcProgressWatchdog.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRecoveryEscalation.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestPullDecisions.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestScenarioDriver.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRoomAggro.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestNavPenalty.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRunWing.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestNeverTarget.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestCombatPurge.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestFactionEntrySwap.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDcHazard.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDcZoneLine.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBossRoster.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBossOrdering.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDifficultyGate.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRaidCore.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRaidScale.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRaidMuster.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestMoltenCore.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestPartyHoldDecision.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestKarazhan.cpp"
    "${MOD_PATH}/test/dungeon_clear/TestKarazhanChess.cpp"
    "${MOD_PATH}/test/dungeon_clear/TestKarazhanChessSim.cpp"
    "${MOD_PATH}/test/dungeon_clear/TestKarazhanChessProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestEventRegistry.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestVioletHold.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestHallsOfStone.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestHallsOfLightning.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestUtgardePinnacle.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestPitOfSaron.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestHallsOfReflection.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestCullingOfStratholme.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTrialOfTheChampion.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestOculus.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestGundrak.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackwingLair.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackfathomDeeps.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestStratTrapAlley.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDungeonEvent.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestNavGeometry.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSplineWindow.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestMechanarElevatorProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRampartsLedgeProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestAzjolNerubRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestHallsOfLightningRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestUtgardePinnacleRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestPitOfSaronRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestHallsOfReflectionRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestCullingOfStratholmeRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackrockDepthsRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackrockSpireEvents.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackrockSpireRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTrialOfTheChampionRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestOculusRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestKarazhanRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSuppressionTransit.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackwingLairSuppressionRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBlackMorassRouteProbe.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestStrategyGate.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestModuleEnable.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestRelevanceLadder.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSettingsRegistry.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestRunVerdict.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestDungeonRegistry.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestGearTiers.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestRunRecord.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestRunSelect.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestWatchHop.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestSpectatorAccess.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestRoster.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestRunLiveJson.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestWipeContext.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDcDiagSnapshot.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestComp.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDungeonQueueFill.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestBgQueueFill.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestPlanSchedule.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestTestPlanSummary.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestDcTestPlan.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestLabOracles.cpp"
            "${MOD_PATH}/test/dungeon_clear/TestLabScenario.cpp"
            "${MOD_PATH}/test/dungeon_clear/NavHarness.cpp"
            "${MOD_PATH}/test/dungeon_clear/replay_decisions.cpp"
            "${MOD_PATH}/test/dungeon_clear/replay_pull.cpp"
            "${CMAKE_SOURCE_DIR}/src/test/mocks/TestMap.cpp"
        )

        # The replay runner reads the captured-decision fixtures from the source
        # tree (the test binary runs from the build dir). Pass the path in.
        # DC_MAPDATA_DIR points the Tier-2 navmesh geometry suite at a sliced
        # mmaps directory (produced by tools/slice_mapdata.py); the suite
        # GTEST_SKIPs every case when it is absent, so a checkout WITHOUT
        # client-derived map data (which is never committed) still builds and
        # passes the rest of the suite.
        target_compile_definitions(dungeon_clear_tests PRIVATE
            DC_FIXTURE_DIR="${MOD_PATH}/test/dungeon_clear/fixtures"
            DC_MAPDATA_DIR="${MOD_PATH}/test/dungeon_clear/fixtures/mapdata"
            ${DC_CORE_COMPAT_DEFS}
        )

        # Same MSVC math-macro ordering trap as the module sources above — the
        # test TUs include the core's Position.h too. Same empty-value spelling,
        # same reason (see the C4005 note above).
        if (MSVC)
            target_compile_options(dungeon_clear_tests PRIVATE /D_USE_MATH_DEFINES=)
            # windows.h's min/max macros break std::numeric_limits<...>::max() in the suite.
            target_compile_definitions(dungeon_clear_tests PRIVATE NOMINMAX)
        endif()

        # Link the necessary targets
        target_link_libraries(dungeon_clear_tests
            acore-core-interface
            game
            gtest_main
            gmock_main
            game-interface
            modules
            scripts
        )

        # Include directories
        target_include_directories(dungeon_clear_tests PRIVATE
            "${MOD_PATH}/src"
            "${MOD_PATH}/src/DungeonClear"
            "${MOD_PATH}/src/QueueFill"
            "${MOD_PATH}/src/Ai/Dungeon/DungeonClear/Util"
            "${CMAKE_SOURCE_DIR}/src/test/mocks"
        )

        # Place executable directly in the main build folder for easy execution
        set_target_properties(dungeon_clear_tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endfunction()

    # Defer execution to the root directory's configuration end, after googletest has been fetched by the core
    cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL define_dungeon_clear_tests)
endif()
