# SquidBots Lite: origin and upstream tracking

`SquidBotsLite.lua`, `Missions.lua`, `Alts.lua`, `Dashboard.lua` and `Bindings.xml` follow
[Zyth45/squidbots-addon](https://github.com/Zyth45/squidbots-addon), last taken `a037f7a` (2.9, 2026-11-02).
The Dungeon clear panel is the upstream one and talks to the separate module `mod-dungeon-clear`.

Our deviations:

- `BotQuests.lua`: quests near you that only some of your bots can take, `/sbl quests`.
- `BotTalents.lua`: the CoA talent window for an alt bot.
- `Manager.lua`: bot sheet with spells, trainer, marked items sold together, soulbound frames.
- `SquidBotsLite.toc`: both files above and `OptionalDeps: CoATalentPath`.

Port upstream commits by hand, each with the trailer `Upstream: Zyth45/squidbots-addon@<sha>`.
