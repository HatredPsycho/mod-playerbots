# CoA Companion: origin and upstream tracking

CoA Companion – Bots & Dungeon Clear (folder `CoACompanionBots`) joins two addons into one. Both are ported by
hand; there is no shared git history.

| Part | Upstream | Last taken |
|---|---|---|
| SquidBots Lite (`SquidBotsLite.lua`, `Manager.lua`, `Missions.lua`, `Bindings.xml`) | [Zyth45/squidbots-addon](https://github.com/Zyth45/squidbots-addon) `main` | `3ddcb33` (2.8.2, 2026-10-29) |
| Dungeon Clear (`DungeonClear.lua`) | [jrad7/mod-dungeon-clear-addon](https://github.com/jrad7/mod-dungeon-clear-addon) `master` | `997bfbd2` (3.6, 2026-08-30) |

Licences: `LICENSE.SquidBots` (GPL-2.0), `LICENSE.DungeonClear`.

## Porting

1. Compare upstream commits after the ones above with the files here.
2. Port each as its own commit with the trailer `Upstream: Zyth45/squidbots-addon@<sha>` or
   `Upstream: jrad7/mod-dungeon-clear-addon@<sha>`.
3. Record the last taken commit in the table.

Changes made here: one addon (one TOC), the spectator camera removed, a shared minimap button, the Clear button on
the bar, the bot orders in Dungeon Clear's tiny view, Dungeon Clear's texts in English and French.

## Not taken from SquidBots Lite 2.8

2.8 is one feature: SquidBots Lite grew its own Dungeon clear panel (state and reason, On / Off / Pause / Skip,
pull mode, the boss list with Go) fed by its own `DC` addon-channel handler, plus the bar's Dungeon button opening
it. This addon already has that panel, richer, in `DungeonClear.lua` (jrad7's), so taking it would put two panels
and two protocol handlers in one addon. Skipped: `608cba6`, `b6d12ff`, `f039d8f`, `25dd4ef`, `df1dc5b`, `7f4eddf`,
`5f7bf86`, `7bff2b7`, `619adb7`, `860c06a`, and `9ceeca4` (a locals-budget refactor only its panel needs).
`2f70069` (`^CHAT\t` instead of a literal tab) and `99134a6` (swallow the difficulty refusal) sit inside that
handler and do not apply: our relay splits the line before it reads it, and the refusal reaches the player through
`DcRefuse` / `TellError`, not the addon channel.

Taken: `f3e962d`, the Auto pull button greyed out while a clear runs, implemented against our own state
(`DungeonClear.lua` calls `CoACompanionBots_DcRunning`), and the version marker from `dd41b80`.

## Taken from SquidBots Lite 2.8.2

`Missions.lua` whole (the missions panel, the alts cards with class icons, the progress bar, the badge over the
bar and the watch menu), the 2.8.2 changes to `SquidBotsLite.lua` and `Manager.lua`, and `bar.last` so the badge
lines up on the bar's last button. Ported with a 3-way merge of `a50fdff` (base), our files and `3ddcb33`.

Three conflicts, all our own deviations kept: the bot sheet's Spells tab stays and takes an icon like the other
tabs; the bar keeps our `order` instead of their `BAR_ORDER`; and their Dungeon clear panel, which 2.8.2 restyles
with the flat buttons, stays out for the reason in the 2.8 section above.
