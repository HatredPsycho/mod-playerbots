# mod-playerbots-auctions' loot filter lives in that optional module, so the two actions that ask it only call
# it when this build links the module. Included from modules/CMakeLists.txt, where the sources are set up.
ModuleNameToVariable(mod-playerbots-auctions PLAYERBOTS_AUCTIONS_VARIABLE)
if ("mod-playerbots-auctions" IN_LIST MODULES_MODULE_LIST AND
    NOT "${${PLAYERBOTS_AUCTIONS_VARIABLE}}" STREQUAL "disabled")
    set_property(
        SOURCE
            "${CMAKE_SOURCE_DIR}/modules/mod-playerbots/src/Ai/Base/Actions/LootAction.cpp"
            "${CMAKE_SOURCE_DIR}/modules/mod-playerbots/src/Ai/Base/Actions/LootRollAction.cpp"
        APPEND PROPERTY COMPILE_DEFINITIONS PLAYERBOTS_WITH_AUCTIONS)
endif()
