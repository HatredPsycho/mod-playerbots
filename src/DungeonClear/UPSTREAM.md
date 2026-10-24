# Dungeon Clear: origin and upstream tracking

Dungeon Clear was integrated from the standalone module mod-dungeon-clear on 2026-10-20. It has no shared git
history with this repository; upstream changes are ported by hand.

## Source

| | |
|---|---|
| Integrated from | HatredPsycho/mod-dungeon-clear, branch `coa`, commit `beb8d7189fe76c992d0f652381823fce9b139047` |
| Original | jrad7/mod-dungeon-clear `master`, last merged commit `5c858d6` (2026-10-19) |
| Further upstream | buildthehomelab/wow-mod-dungeon-clear `evermore`, fully merged as of 2026-10-17 |

## Layout

| mod-dungeon-clear | mod-playerbots |
|---|---|
| `src/Ai/Dungeon/DungeonClear/` | `src/Ai/Dungeon/DungeonClear/` |
| `src/BgQueueFill/`, `src/DungeonQueueFill/` | `src/QueueFill/BgQueueFill/`, `src/QueueFill/DungeonQueueFill/` |
| everything else under `src/` (`Lab/`, `TestRun/`, `Util/`, scripts, command) | `src/DungeonClear/` |
| `src/AiObjectContextAccess.h` | removed: the contexts are registered in `Bot/Engine/BuildShared*Contexts.cpp` |
| `src/mod_dungeon_clear_loader.cpp` | `src/DungeonClear/DungeonClearLoader.cpp`, called from `src/Script/playerbots_loader.cpp` |
| `t/` | `test/dungeon_clear/` (target `dungeon_clear_tests`) |
| `conf/mod_dungeon_clear.conf.dist` | section "DUNGEON CLEAR" of `conf/playerbots.conf.dist` (the old file is a deprecated stub) |
| `mod-dungeon-clear.cmake` | `mod-playerbots.cmake` |
| `lab/`, `testdeck/`, `tools/`, `CLAUDE.md` | not taken |

## Conquest of Azeroth changes carried over

Our commits on top of jrad7/evermore in the `coa` branch, all part of the integrated state:

- CoA tank pull openers, CoA healers' resurrection and lock-free clickable doors (`Util/DcCoaCompat.h`,
  `DungeonClear.LockFreeClickableDoors`).
- `coa auto pull` and `coa catch up` held during a run (`Multiplier/DungeonClearMultiplier.cpp`).
- `MoveKnockbackFrom` with four arguments; Windows build of the test target.
- Registration into `CoaAiObjectContext` (`DungeonClearModule.cpp`).
- Spell distance through `DC_PB_CONFIG` in the Iron Hall.
- Since the integration: pull openers and resurrection read from playerbots' CoA class kit
  (`Ai/Class/Coa/CoaAbilities.h`); `coa auto pull` / `coa catch up` recognised by `Action::GetAutonomy()`
  instead of by name.

## Porting upstream changes

1. Fetch jrad7 (and evermore) in the archived clone `D:\Dev\mod-dungeon-clear-upstream`.
2. List the commits after the last ported one below; sort each into take / adapt / drop.
3. Port each as its own commit on `coa-original` with the trailer `Upstream: jrad7/mod-dungeon-clear@<sha>`,
   then carry it to `coa-custom-races` and `coa-scaling`.
4. Record the last ported commit here.

| Upstream | Last ported |
|---|---|
| jrad7/mod-dungeon-clear `master` | `fcb680f` (2026-10-23, #61) |
| buildthehomelab/wow-mod-dungeon-clear `evermore` | `5d49ee7` (2026-10-24, #24) |
