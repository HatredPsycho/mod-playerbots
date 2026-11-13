# SquidBots Lite: origin and upstream tracking

`SquidBotsLite.lua`, `Missions.lua`, `Alts.lua`, `Dashboard.lua`, `MainTank.lua`, `World.lua`, `TalentTree.lua`,
`TalentLayouts.lua` and `Bindings.xml` follow
[Zyth45/squidbots-addon](https://github.com/Zyth45/squidbots-addon), last taken `ea38912` (3.0, 2026-11-12).
The Dungeon clear panel is the upstream one and talks to the separate module `mod-dungeon-clear`.

Our deviations:

- `BotQuests.lua`: quests near you that only some of your bots can take, `/sbl quests`.
- `BotTalents.lua`: the CoA talent window for an alt bot.
- `Manager.lua`: bot sheet with spells, trainer, marked items sold together, soulbound frames, specialization menu
  on the talents tab (served by `BotTalents.lua`).
- `SquidBotsLite.toc`: both files above and `OptionalDeps: CoATalentPath`.

Port upstream commits by hand, each with the trailer `Upstream: Zyth45/squidbots-addon@<sha>`.
