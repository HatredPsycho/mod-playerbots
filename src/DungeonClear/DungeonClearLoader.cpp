/*
 * Dungeon Clear (from mod-dungeon-clear) — entry point, called from playerbots' own
 * module loader (Script/playerbots_loader.cpp).
 */

void AddSC_dungeon_clear_module();
void AddSC_dungeon_clear_command();
void AddSC_dungeon_clear_addon_hook();
void AddSC_dungeon_clear_queue_fill();
void AddSC_dungeon_clear_bg_queue_fill();
void AddSC_dungeon_clear_lab();

void AddDungeonClearScripts()
{
    AddSC_dungeon_clear_module();
    AddSC_dungeon_clear_command();
    AddSC_dungeon_clear_addon_hook();
    AddSC_dungeon_clear_queue_fill();
    AddSC_dungeon_clear_bg_queue_fill();
    AddSC_dungeon_clear_lab();
}
