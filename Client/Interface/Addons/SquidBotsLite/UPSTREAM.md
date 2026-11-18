# SquidBots Lite: origin and upstream tracking

`SquidBotsLite.lua`, `Missions.lua`, `Alts.lua`, `Dashboard.lua`, `MainTank.lua`, `World.lua`, `TalentTree.lua`,
`TalentLayouts.lua`, `KeySpells.lua`, `GroupQuests.lua`, `AltsList.lua`, `Minimap.lua` and `Bindings.xml` follow
[Zyth45/squidbots-addon](https://github.com/Zyth45/squidbots-addon), last taken `e21db09` (3.1.4.1, 2026-11-18).
The Dungeon clear panel is the upstream one and talks to the separate module `mod-dungeon-clear`.

Our deviations:

- `BotQuests.lua`: quests near you that only some of your bots can take, `/sbl botquests`
  (`/sbl quests` is upstream's group quests since 3.1.3).
- `BotTalents.lua`: the CoA talent window for an alt bot.
- `BotTalentPath.lua`: CoA Talent Path's leveling path on the bot sheet's tree view (Talents tab).
- `Manager.lua`: bot sheet with spells, trainer, soulbound frames, specialization menu
  on the talents tab (served by `BotTalents.lua`).
- `SquidBotsLite.toc`: the three files above and `OptionalDeps: CoATalentPath`.

Port upstream commits by hand, each with the trailer `Upstream: Zyth45/squidbots-addon@<sha>`.
