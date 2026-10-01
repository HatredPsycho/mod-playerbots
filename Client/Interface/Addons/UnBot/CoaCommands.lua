-- Buttons added for the CoA server: group 7 (items), group 11 (Dungeon) and group 12 (CoA and profiles).
-- Loaded after Tooltips.lua: completes the command tables without rewriting them.

local function InGroup()
	if (GetNumPartyMembers() == 0 and GetNumRaidMembers() == 0) then
		DisplayInfomation("You are not in a group with bots.");
		return false;
	end
	return true;
end

local function SendToGroup(...)
	if (not InGroup()) then
		return false;
	end
	for i=1, select("#", ...) do
		SendChatMessage(select(i, ...), "PARTY");
	end
	return true;
end

local function WhisperTarget(...)
	local targetName = UnitName("target");
	if (targetName == nil or targetName == "" or not UnitIsPlayer("target")) then
		DisplayInfomation("You must target a player bot.");
		return;
	end
	for i=1, select("#", ...) do
		SendChatMessage(select(i, ...), "WHISPER", nil, targetName);
	end
end

-- Instance names (enUS client, French names as a fallback) -> mod-playerbots strategy.
local UnBotInstanceStrategies = {
	["auchenai crypts"] = "tbc-ac", ["cryptes auchenaï"] = "tbc-ac",
	["sethekk halls"] = "tbc-seth", ["salles des sethekk"] = "tbc-seth",
	["the mechanar"] = "tbc-mech", ["le méchanar"] = "tbc-mech",
	["the underbog"] = "tbc-ub", ["la basse-tourbière"] = "tbc-ub",
	["magisters' terrace"] = "tbc-mgt", ["terrasse des magistères"] = "tbc-mgt",
	["utgarde keep"] = "wotlk-uk", ["donjon d'utgarde"] = "wotlk-uk",
	["the nexus"] = "wotlk-nex", ["le nexus"] = "wotlk-nex",
	["azjol-nerub"] = "wotlk-an", ["azjol-nérub"] = "wotlk-an",
	["ahn'kahet: the old kingdom"] = "wotlk-ok", ["ahn'kahet : l'ancien royaume"] = "wotlk-ok",
	["drak'tharon keep"] = "wotlk-dtk", ["donjon de drak'tharon"] = "wotlk-dtk",
	["the violet hold"] = "wotlk-vh", ["le fort pourpre"] = "wotlk-vh",
	["gundrak"] = "wotlk-gd",
	["halls of stone"] = "wotlk-hos", ["les salles de pierre"] = "wotlk-hos",
	["halls of lightning"] = "wotlk-hol", ["les salles de foudre"] = "wotlk-hol",
	["the oculus"] = "wotlk-occ", ["l'oculus"] = "wotlk-occ",
	["utgarde pinnacle"] = "wotlk-up", ["cime d'utgarde"] = "wotlk-up",
	["the culling of stratholme"] = "wotlk-cos", ["l'épuration de stratholme"] = "wotlk-cos",
	["trial of the champion"] = "wotlk-toc", ["l'épreuve du champion"] = "wotlk-toc",
	["pit of saron"] = "wotlk-pos", ["fosse de saron"] = "wotlk-pos",
	["the forge of souls"] = "wotlk-fos", ["la forge des âmes"] = "wotlk-fos",
	["molten core"] = "moltencore", ["cœur du magma"] = "moltencore",
	["onyxia's lair"] = "onyxia", ["repaire d'onyxia"] = "onyxia",
	["blackwing lair"] = "bwl", ["repaire de l'aile noire"] = "bwl",
	["ruins of ahn'qiraj"] = "aq20", ["ruines d'ahn'qiraj"] = "aq20",
	["karazhan"] = "karazhan",
	["gruul's lair"] = "gruulslair", ["repaire de gruul"] = "gruulslair",
	["magtheridon's lair"] = "magtheridon", ["le repaire de magtheridon"] = "magtheridon",
	["serpentshrine cavern"] = "ssc", ["caverne du sanctuaire du serpent"] = "ssc",
	["tempest keep"] = "tempestkeep", ["the eye"] = "tempestkeep", ["l'œil"] = "tempestkeep",
	["hyjal summit"] = "hyjal", ["sommet d'hyjal"] = "hyjal",
	["black temple"] = "blacktemple", ["temple noir"] = "blacktemple",
	["zul'aman"] = "zulaman",
	["naxxramas"] = "naxx",
	["the obsidian sanctum"] = "wotlk-os", ["le sanctum obsidien"] = "wotlk-os",
	["the eye of eternity"] = "wotlk-eoe", ["l'œil de l'éternité"] = "wotlk-eoe",
	["vault of archavon"] = "voa", ["caveau d'archavon"] = "voa",
	["ulduar"] = "ulduar",
	["icecrown citadel"] = "icc", ["citadelle de la couronne de glace"] = "icc",
	["the ruby sanctum"] = "rs", ["le sanctum rubis"] = "rs",
};

-- A strategy is created under the key below but stored under its own getName(), and for the
-- dungeons the two differ (Engine::addStrategy does strategies[strategy->getName()] = strategy).
-- "co -wotlk-dtk" therefore removes nothing: the engine looks for a key that is not there. Only
-- the dungeons need this; every raid strategy reports its key as its name, which is why leaving a
-- raid always worked and leaving a dungeon never did.
UnBotStrategyRemovalName = {
	["vanilla-rfc"] = "ragefire chasm",
	["vanilla-sfk"] = "shadowfang keep",
	["wotlk-uk"] = "utgarde keep",
	["wotlk-nex"] = "nexus",
	["wotlk-an"] = "azjol'nerub",
	["wotlk-ok"] = "old kingdom",
	["wotlk-dtk"] = "drak'tharon keep",
	["wotlk-vh"] = "violet hold",
	["wotlk-gd"] = "gundrak",
	["wotlk-hos"] = "halls of stone",
	["wotlk-hol"] = "halls of lightning",
	["wotlk-occ"] = "oculus",
	["wotlk-up"] = "utgarde pinnacle",
	["wotlk-cos"] = "culling of stratholme",
	["wotlk-toc"] = "trial of the champion",
	["wotlk-pos"] = "pit of saron",
	["wotlk-fos"] = "forge of souls",
};

-- The name "co -<x>" has to use, which is the removal name where one is listed.
local function StrategyRemovalName(strategy)
	return UnBotStrategyRemovalName[strategy] or strategy;
end

local function CurrentInstanceStrategy()
	local inInstance = IsInInstance();
	if (not inInstance) then
		return nil, nil;
	end
	local name = GetInstanceInfo();
	if (name == nil or name == "") then
		name = GetRealZoneText();
	end
	if (name == nil) then
		return nil, nil;
	end
	return UnBotInstanceStrategies[string.lower(name)], name;
end

local function CommandInstanceStrategy(index)
	local strategy, name = CurrentInstanceStrategy();
	if (name == nil) then
		DisplayInfomation("You are not in a dungeon or raid.");
		return;
	end
	if (strategy == nil) then
		DisplayInfomation(name..": bots know no boss strategy for this place.");
		return;
	end
	if (SendToGroup("co +"..strategy)) then
		DisplayInfomation(name..": strategy \""..strategy.."\" enabled for the group.");
	end
end

local function CommandDungeonProfile(index)
	-- Without "wait for attack": tested in a dungeon on 15/09, that delay stops CoA bots
	-- from engaging; without it the tank tanks and the healer heals.
	if (not SendToGroup("co -wait for attack,+avoid aoe",
		"nc +follow,-grind,-rpg,-move random", "follow")) then
		return;
	end
	local strategy, name = CurrentInstanceStrategy();
	if (strategy ~= nil) then
		SendChatMessage("co +"..strategy, "PARTY");
		DisplayInfomation("Dungeon mode enabled, with the \""..strategy.."\" strategy ("..name..").");
	else
		DisplayInfomation("Dungeon mode enabled.");
	end
end

local function CommandLeaveDungeonProfile(index)
	local strategy = CurrentInstanceStrategy();
	local removal = "co -wait for attack,-avoid aoe,-mark rti";
	if (strategy ~= nil) then
		removal = removal..",-"..StrategyRemovalName(strategy);
	end
	if (SendToGroup(removal)) then
		DisplayInfomation("Dungeon settings removed.");
	end
end

local function CommandWaitForAttack(index)
	SendToGroup("co +wait for attack", "wait for attack time 3");
end

local function CommandFarmProfile(index)
	if (SendToGroup("nc +grind,+loot,+food", "co +save mana")) then
		DisplayInfomation("Farm profile: bots hunt, loot and eat.");
	end
end

local function CommandStrictFollowProfile(index)
	if (SendToGroup("nc +follow,-grind,-rpg,-move random", "follow")) then
		DisplayInfomation("Strict follow profile: bots follow you without wandering off.");
	end
end

local function CommandShowTargetStrategies(index)
	WhisperTarget("co ?", "nc ?");
end

local function CommandGroupComposition(index)
	local units = {};
	if (GetNumRaidMembers() > 0) then
		for i=1, GetNumRaidMembers() do table.insert(units, "raid"..i); end
	elseif (GetNumPartyMembers() > 0) then
		for i=1, GetNumPartyMembers() do table.insert(units, "party"..i); end
	else
		DisplayInfomation("You are not in a group.");
		return;
	end
	DisplayInfomation("Group composition ("..#units.." members):");
	for i=1, #units do
		local unit = units[i];
		local name = UnitName(unit);
		if (name ~= nil) then
			local className = UnitClass(unit) or "?";
			local state = UnitIsConnected(unit) and (UnitIsDeadOrGhost(unit) and "|cffff3333dead|r" or "") or "|cff888888offline|r";
			DisplayInfomation(string.format("  %s  lvl %d  %s %s", name, UnitLevel(unit) or 0, className, state));
		end
	end
end

-- "init=auto" is refused on random bots ("only addclass bots"): raise them with the GM
-- level command instead. A real level gain also runs the bot's level-up maintenance,
-- which is where a CoA bot picks its specialization at level 10.
local function CommandRaiseGroupToMyLevel(index)
	local units = {};
	if (GetNumRaidMembers() > 0) then
		for i=1, GetNumRaidMembers() do table.insert(units, "raid"..i); end
	else
		for i=1, GetNumPartyMembers() do table.insert(units, "party"..i); end
	end
	if (#units == 0) then
		DisplayInfomation("You are not in a group with bots.");
		return;
	end
	local myLevel = UnitLevel("player");
	local raised = 0;
	for i=1, #units do
		local name = UnitName(units[i]);
		local level = UnitLevel(units[i]);
		if (name ~= nil and name ~= UnitName("player") and level ~= nil and level > 0 and level < myLevel) then
			SendChatMessage(".character level "..name.." "..myLevel, "SAY");
			raised = raised + 1;
		end
	end
	DisplayInfomation(raised.." bot(s) raised to level "..myLevel..". Then click Reset AI (9) if their role did not change.");
end


-- Share gear moves the items, equip upgrade puts them on. The second message waits, because a bot
-- only receives what another bot hands over on one of the next server ticks: sent at once it would
-- run before the gear has arrived.
local ShareThenEquipFrame = CreateFrame("Frame");
local ShareThenEquipTimer = 0;
ShareThenEquipFrame:Hide();
ShareThenEquipFrame:SetScript("OnUpdate", function(self, elapsed)
	ShareThenEquipTimer = ShareThenEquipTimer + elapsed;
	if (ShareThenEquipTimer < 4) then
		return;
	end
	self:Hide();
	ShareThenEquipTimer = 0;
	if (InGroup()) then
		SendChatMessage("equip upgrade", "PARTY");
		DisplayInfomation("Now let the bots equip what they received.");
	end
end);

local function CommandShareThenEquip(index)
	if (not SendToGroup("share gear")) then
		return;
	end
	ShareThenEquipTimer = 0;
	ShareThenEquipFrame:Show();
	DisplayInfomation("Gear shared. Equipping follows in a moment.");
end

local function Add(index, group, cmdType, command, icon, title, help, realize)
	UnBotCommandToGroups[index] = group;
	UnBotCommandType[index] = cmdType;
	UnBotExecuteCommand[index] = command;
	UnBotIconFiles[index] = icon;
	UnBotTooltipTitle[index] = title;
	UnBotTooltipHelp[index] = help;
	if (realize ~= nil) then
		UnBotCommandRealize[index] = realize;
	end
end

-- Group 11: Dungeon
Add(90, 11, 4, "co -wait for attack,+avoid aoe", 435, "Prepare for the dungeon",
	"Every group bot switches to dungeon mode: they follow you without wandering off to hunt, engage with you and step out of ground effects. In a known dungeon its boss strategy is enabled as well.",
	CommandDungeonProfile);
Add(91, 11, 4, "co +<donjon>", 3649, "Current dungeon strategy",
	"Detects the dungeon or raid you are in and enables the matching boss strategy for the group (BC and WotLK dungeons and raids, including Molten Core, Onyxia, Blackwing Lair, AQ20).",
	CommandInstanceStrategy);
Add(92, 11, 4, "co +avoid aoe", 5509, "Avoid ground effects",
	"Group bots step out of dangerous ground effects.");
Add(93, 11, 4, "co +mark rti", 3730, "Mark the targets",
	"Group bots put raid icons on the targets to attack.");
Add(94, 11, 4, "co +wait for attack", 5567, "Wait before engaging",
	"The bots wait 3 seconds before attacking, so the tank can take aggro. |cffbb0000Not recommended with CoA bots: they may stop engaging altogether (use Leave dungeon mode to cancel it).|r",
	CommandWaitForAttack);
Add(95, 11, 4, "max dps", 5948, "All out",
	"Group bots use their full damage potential, cooldowns included. Ideal on a boss.");
Add(96, 11, 4, "save mana", 327, "Save the resource",
	"Group bots save their mana or class resource.");
Add(97, 11, 4, "revive", 5650, "Resurrect at the spirit healer",
	"Dead group bots get resurrected by the spirit healer.");
Add(98, 11, 4, "co -wait for attack,-avoid aoe,-mark rti", 5809, "Leave dungeon mode",
	"Removes the dungeon settings (waiting, ground effects, marks, current dungeon strategy). Use a profile afterwards (Farm or Strict follow).",
	CommandLeaveDungeonProfile);

-- Group 12: CoA and profiles
Add(99, 12, 4, "co +coa,+dps assist", 81, "Group CoA rotation",
	"Re-enables the CoA ability rotation and group assist for every bot, should a setting have removed it.");
Add(100, 12, 4, "nc +grind,+loot,+food", 134, "Farm profile",
	"Group bots hunt the monsters around, pick up the loot, eat after the fight and save their resource.",
	CommandFarmProfile);
Add(101, 12, 4, "nc +follow,-grind,-rpg,-move random", 5660, "Strict follow profile",
	"Group bots follow you and no longer wander off to hunt or live their own life. They still fight whatever attacks you.",
	CommandStrictFollowProfile);
Add(102, 12, 4, "autogear", 1665, "Gear up",
	"Group bots receive the best gear for their level (rare quality at most, a server setting). The gear is created, not bought.");
Add(103, 12, 4, "repair", 5989, "Repair",
	"Group bots repair their gear.");
Add(104, 12, 4, "maintenance", 2051, "Full maintenance",
	"Group bots run their maintenance: repairs, spells, consumables.");
Add(111, 7, 4, "s gray", 3056, "Sell grey items",
	"Every group bot sells its poor quality items. The bots sell to a vendor they can reach, so stand at one: out of a vendor's range nothing is sold and the bots stay silent.");
Add(112, 7, 4, "s vendor", 3057, "Sell everything unused",
	"Every group bot sells what it has no use for, not only the grey items: anything with a sell price that its own item rating does not want, white and green gear included as long as it is not soulbound. |cffbb0000Wider than Sell grey items: check what the bots are carrying first.|r A vendor has to be in reach, as above.");
Add(113, 7, 2, "s gray", 3058, "Target bot sells grey items",
	"The same as Sell grey items, for the targeted bot alone. Target a bot of your group and stand at a vendor.");
Add(114, 7, 4, "share gear", 1665, "Share gear in the group",
	"Every group bot offers the weapons and armour in its bags to whoever they are worth more to, you included, and hands them over on the spot. Soulbound items stay where they are, even inside their trade window. Everybody has to be within 30 yards.");
Add(115, 7, 4, "equip upgrade", 2178, "Equip upgrades",
	"Every group bot puts on what is better than the piece it wears, out of its own bags. Your own character is not touched: equip what you were handed yourself.");
Add(116, 7, 4, "share gear", 1665, "Share gear, then equip it",
	"Share gear in the group, and four seconds later Equip upgrades. The pause is needed because a bot receives its gear on one of the next server ticks, not at once.",
	CommandShareThenEquip);
Add(105, 12, 2, "co ?", 3006, "Target bot strategies",
	"The target bot whispers you its active strategies, in combat then out of combat.",
	CommandShowTargetStrategies);
Add(106, 12, 2, "spells", 5635, "Target bot spells",
	"The target bot whispers you the list of its spells, CoA abilities included.");
-- Replaces the original button 3 (init=auto), which the server refuses on random bots.
Add(3, UnBotCommandToGroups[3], 1, ".character level", UnBotIconFiles[3], "Raise group bots to your level",
	"Raises every group bot below you to your level (GM command). A CoA bot crossing level 10 picks its specialization and role (tank, healer or DPS).",
	CommandRaiseGroupToMyLevel);

Add(108, 12, 1, ".playerbots coa tank", 448, "Add a tank bot",
	"Brings the nearest free bot able to tank into your group: set to your level, with a tank specialization, teleported next to you (GM command).");
Add(109, 12, 1, ".playerbots coa heal", 5635, "Add a healer bot",
	"Brings the nearest free bot able to heal into your group: set to your level, with a healing specialization, teleported next to you (GM command).");
Add(110, 12, 1, ".playerbots coa dps", 443, "Add a DPS bot",
	"Brings the nearest free bot into your group, with a damage specialization, set to your level and teleported next to you (GM command).");

Add(107, 12, 1, "composition", 442, "Group composition",
	"Shows the group members in the chat: level, class and state.",
	CommandGroupComposition);
