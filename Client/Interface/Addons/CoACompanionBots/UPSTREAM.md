# CoA Companion: origin and upstream tracking

CoA Companion – Bots & Dungeon Clear (folder `CoACompanionBots`) joins two addons into one. Both are ported by
hand; there is no shared git history.

| Part | Upstream | Last taken |
|---|---|---|
| SquidBots Lite (`SquidBotsLite.lua`, `Manager.lua`, `Bindings.xml`) | [Zyth45/squidbots-addon](https://github.com/Zyth45/squidbots-addon) `main` | `06992ee8` (2.6, 2026-10-20) |
| Dungeon Clear (`DungeonClear.lua`) | [jrad7/mod-dungeon-clear-addon](https://github.com/jrad7/mod-dungeon-clear-addon) `master` | `997bfbd2` (3.6, 2026-08-30) |

Licences: `LICENSE.SquidBots` (GPL-2.0), `LICENSE.DungeonClear`.

## Porting

1. Compare upstream commits after the ones above with the files here.
2. Port each as its own commit with the trailer `Upstream: Zyth45/squidbots-addon@<sha>` or
   `Upstream: jrad7/mod-dungeon-clear-addon@<sha>`.
3. Record the last taken commit in the table.

Changes made here: one addon (one TOC), the spectator camera removed, a shared minimap button, the Clear button on
the bar, the bot orders in Dungeon Clear's tiny view, Dungeon Clear's texts in English and French.
