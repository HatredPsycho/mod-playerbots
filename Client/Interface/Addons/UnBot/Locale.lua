
-- Addon language (UnBot CoA). The source strings are English now, so this file only keeps the
-- tables the original French build shipped; they are applied again as a no-op.
-- /unbot lang en is the only meaningful choice: French is no longer present in the sources.
-- Only the display was ever translated: the commands sent to the bots never change.

local TITLE_EN = {
[0] = "Close windows and menus",
[1] = "Initialize all online bots",
[2] = "Initialize target bot",
[3] = "Raise group bots to your level",
[4] = "Update random bots",
[5] = "Reset target bot talents",
[6] = "Target bot: spend points, tree 1",
[7] = "Target bot: spend points, tree 2",
[8] = "Target bot: spend points, tree 3",
[9] = "Reset group bots AI",
[10] = "Reset target bot AI",
[11] = "View bot equipment",
[12] = "Set bot to your level",
[13] = "Summon group bots",
[14] = "Summon target bot",
[15] = "Group bots follow you",
[16] = "Target bot follows you",
[17] = "Learn trainer spells",
[18] = "Dead bots: release spirit",
[19] = "Target bot: release spirit",
[20] = "Group bots quests",
[21] = "Target bot quests",
[22] = "Group bots status",
[23] = "Target bot status",
[24] = "Loot everything",
[25] = "Group bots flee",
[26] = "Tanks flee",
[27] = "DPS flee",
[28] = "Healers flee",
[29] = "Melee flee",
[30] = "Ranged flee",
[31] = "Target bot flees",
[32] = "Bots attack the target",
[33] = "Tanks attack the target",
[34] = "DPS attack the target",
[35] = "Healers attack the target",
[36] = "Melee attack the target",
[37] = "Ranged attack the target",
[38] = "Formation: arrow",
[39] = "Formation: queue",
[40] = "Formation: near",
[41] = "Formation: melee",
[42] = "Formation: line",
[43] = "Formation: circle",
[44] = "Formation: chaos",
[45] = "Formation: shield",
[46] = "Show group formation",
[47] = "Whole group stays",
[48] = "Target bot: automatic targets",
[49] = "Target bot: manual targets",
[50] = "Druid: spell DPS",
[51] = "Druid: bear form",
[52] = "Druid: cat form",
[53] = "Druid: healing",
[54] = "Bot strategy editor",
[55] = "Paladin: Frost resistance",
[56] = "Paladin: Fire resistance",
[57] = "Paladin: Devotion Aura",
[58] = "Paladin: Crusader Aura",
[59] = "Paladin: Blessing of Kings",
[60] = "Paladin: Blessing of Wisdom",
[61] = "Paladin: Blessing of Might",
[62] = "Group AI status",
[63] = "Target bot AI status",
[64] = "View bot bags",
[65] = "Bot destroys an item",
[66] = "Bot equips an item",
[67] = "Bot sells an item",
[68] = "Bot uses an item",
[69] = "Bot casts a spell",
[70] = "Whole group leaves combat",
[71] = "Disband group",
[72] = "Show all icons",
[73] = "Teleport to target player",
[74] = "Log in bots",
[75] = "Target bot out-of-combat strategies",
[76] = "Target bot stays",
[77] = "Create a class bot",
[78] = "Log in all friends",
[79] = "Invite all friends",
[80] = "Ready check",
[81] = "Bots drink",
[82] = "Log out group bots",
[83] = "Log in group bots",
[84] = "Naxxramas",
[85] = "Reset actions",
[86] = "Log out target bot",
[87] = "Log in target bot",
[88] = "Add group as friends",
[89] = "Grind for experience",
[90] = "Prepare the dungeon",
[91] = "Current dungeon strategy",
[92] = "Avoid ground effects",
[93] = "Mark targets",
[94] = "Wait before engaging",
[95] = "Go all out",
[96] = "Save resources",
[97] = "Resurrect at the spirit healer",
[98] = "Leave dungeon mode",
[99] = "Group CoA rotation",
[100] = "Farm profile",
[101] = "Strict follow profile",
[102] = "Best gear",
[103] = "Repair",
[104] = "Full maintenance",
[105] = "Target bot strategies",
[106] = "Target bot spells",
[107] = "Group composition",
[108] = "Add a tank bot",
[109] = "Add a healer bot",
[110] = "Add a DPS bot",
};

local BAGS_NOTE = " (|cffbb0000The window does not follow bag changes: click Refresh. Inactive while the bot eats, drinks or casts.|r)";
local FLEE_NOTE = " leave the fight and come back to you (cancel with Follow, 15 or 16).";

local HELP_EN = {
[0] = "Closes every bot window and open menu. Right click: close the whole bar. Type /unbot (chat or macro) to show or hide it.",
[1] = "Re-initializes every online bot: level (random within the server range), gear and spells. Heavy operation that freezes the server for a moment: do not repeat it often.",
[2] = "Re-initializes the target bot for your level and gear: level, gear, spells.",
[3] = "Raises every group bot below your level to your level (GM command). A CoA bot passing level 10 picks its specialization and role (tank, healer or DPS).",
[4] = "Updates random bots: tops up to the configured count, or replaces some of them.",
[5] = "Resets the target bot's talents (GM account required), then use 6 to 8 to spend points and 9 or 10 (reset botAI). CoA classes: talents are handled by the advancement system, no guaranteed effect.",
[6] = "The target bot spends its remaining talent points in its 1st tree. Then use 9 or 10 (reset botAI).",
[7] = "The target bot spends its remaining talent points in its 2nd tree. Then use 9 or 10 (reset botAI).",
[8] = "The target bot spends its remaining talent points in its 3rd tree. Then use 9 or 10 (reset botAI).",
[9] = "Resets the AI of every group bot. Needed after a talent change so they fight with the new specialization.",
[10] = "Resets the target bot's AI. Needed after a talent change.",
[11] = "Opens the bot's equipment window: left click a slot to change the item, right click to remove it (group bots only).",
[12] = "Sets the target bot to your level and resets spells and gear for that level. For another level, type the command yourself followed by the level.",
[13] = "Teleports every group or raid bot next to you (no effect outside a group).",
[14] = "Teleports the target bot next to you.",
[15] = "Every group bot follows you in the chosen formation and fights in its role. Also cancels a flee.",
[16] = "The target bot follows you. Mostly used to stop it fleeing.",
[17] = "Group bots learn everything they can from the targeted trainer (class or profession). No effect if the target is not a trainer.",
[18] = "Dead group bots release their spirit; summon them afterwards (13 or 14). Dungeons only.",
[19] = "The target bot releases its spirit if dead; summon it afterwards (13 or 14). Dungeons only.",
[20] = "Shows the quests of group bots: completed and in progress.",
[21] = "Shows the target bot's quests: completed and in progress.",
[22] = "Shows group bots' status: experience, free bag slots, money.",
[23] = "Shows the target bot's status: experience, free bag slots, money.",
[24] = "Group bots loot nearby corpses.",
[25] = "Every group bot" .. string.gsub(FLEE_NOTE, "^ leave", " leaves", 1),
[26] = "Group tanks" .. FLEE_NOTE,
[27] = "Group DPS" .. FLEE_NOTE,
[28] = "Group healers" .. FLEE_NOTE,
[29] = "Melee fighters" .. FLEE_NOTE,
[30] = "Ranged fighters" .. FLEE_NOTE,
[31] = "The target bot leaves the fight and comes back to you (cancel with Follow, 15 or 16).",
[32] = "Every group bot attacks the selected target.",
[33] = "Group tanks attack the selected target.",
[34] = "Group DPS attack the selected target.",
[35] = "Group healers attack the selected target.",
[36] = "Melee fighters attack the selected target.",
[37] = "Ranged fighters attack the selected target.",
[38] = "Arrow formation: tanks in front, then melee, then ranged, healers at the back.",
[39] = "Queue formation: bots follow you one behind the other.",
[40] = "Near formation: bots stay packed close to you.",
[41] = "Melee formation: placement suited to close combat.",
[42] = "Line formation: bots stand side by side.",
[43] = "Circle formation: tanks and melee in the middle, ranged and healers around.",
[44] = "Chaos formation: bots spread away from each other.",
[45] = "Shield formation: bots surround you to protect you.",
[46] = "Shows the current formation of group bots.",
[47] = "The whole group stops and stays; bots fight if needed then hold their position.",
[48] = "The target bot picks its own targets in combat (the Attack command is ignored).",
[49] = "You pick the bot's targets yourself, it never switches on its own (DPS and tank strategies have no effect, Attack works).",
[50] = "The target druid fights with spells (moonkin form if available).",
[51] = "The target druid tanks in bear form.",
[52] = "The target druid deals damage in cat form.",
[53] = "The target druid focuses on healing.",
[54] = "Opens the target bot's strategy editor, in and out of combat. CoA class bots: enable CoA rotation in combat.",
[55] = "The target paladin enables Frost Resistance Aura.",
[56] = "The target paladin enables Fire Resistance Aura.",
[57] = "The target paladin enables Devotion Aura.",
[58] = "The target paladin enables Crusader Aura.",
[59] = "The target paladin gives Blessing of Kings to the group.",
[60] = "The target paladin gives Blessing of Wisdom to the group.",
[61] = "The target paladin gives Blessing of Might to the group.",
[62] = "Shows the AI status of every group bot.",
[63] = "Shows the target bot's AI status.",
[64] = "Opens the list of items in the target bot's bags. To destroy, equip, sell or use, see the next commands." .. BAGS_NOTE,
[65] = "Opens the destroy window: left click an item for the bot to destroy it, right click to hide it." .. BAGS_NOTE,
[66] = "Opens the equip window: left click an item for the bot to equip it, right click to hide it." .. BAGS_NOTE,
[67] = "Opens the sell window: left click an item for the bot to sell it to the targeted merchant, right click to hide it." .. BAGS_NOTE,
[68] = "Opens the use window: left click an item for the bot to use it on its target, right click to hide it." .. BAGS_NOTE,
[69] = "Opens the bot's spell list, in and out of combat: left click to have it cast the spell, right click to hide it. Some spells need a suitable target. (|cffbb0000The list does not follow changes: click Refresh. Inactive while the bot eats, drinks or casts.|r)",
[70] = "Makes every player of the group leave combat, you included (GM account required).",
[71] = "Removes group members one by one, so you do not lose the bots by leaving yourself.",
[72] = "Shows every icon of the game.",
[73] = "Teleports you in front of the target character (GM account required).",
[74] = "Opens your friends and guild members list: left click to log in a bot, right click to invite it to the group.",
[75] = "Shows the target bot's out-of-combat strategies.",
[76] = "The target bot stops and stays; it fights if needed then holds its position.",
[77] = "Opens the class bot creation panel. Original classes only: addclass does not support CoA classes.",
[78] = "Logs in every character of your friends list.",
[79] = "Invites every online friend. If the group is full it becomes a raid: run the command again to invite the rest.",
[80] = "Starts a ready check: bots restock food, drink and ammunition if needed.",
[81] = "Every group bot drinks to regain mana.",
[82] = "Logs out every group bot.",
[83] = "Logs in every group bot.",
[84] = "Enables the Naxxramas module for every group bot.",
[85] = "Resets the current action of every group bot.",
[86] = "Logs out the target bot.",
[87] = "Logs in the target bot.",
[88] = "Adds every group bot to your friends list.",
[89] = "Every group bot attacks nearby monsters that give experience.",
[90] = "Every group bot switches to dungeon mode: they follow you without wandering off to grind, engage with you and avoid ground effects. In a known dungeon it also enables its boss strategy.",
[91] = "Detects the dungeon or raid you are in and enables the matching boss strategy for the group (BC/WotLK dungeons and raids, including Molten Core, Onyxia, Blackwing Lair, AQ20).",
[92] = "Group bots step out of dangerous ground areas.",
[93] = "Group bots put raid icons on the targets to attack.",
[94] = "Bots wait 3 seconds before attacking, to let the tank build threat. |cffbb0000Not recommended with CoA bots: they may stop engaging at all (use Leave dungeon mode to cancel).|r",
[95] = "Group bots use their full damage potential, cooldowns included. Best on a boss.",
[96] = "Group bots save their mana or class resource.",
[97] = "Dead group bots get resurrected by the spirit healer.",
[98] = "Removes dungeon settings (waiting, ground effects, marks, current dungeon strategy). Then use a profile (Farm or Strict follow).",
[99] = "Re-enables the CoA ability rotation and group assist for every bot, if a setting removed it.",
[100] = "Group bots hunt nearby monsters, loot, eat after combat and save their resources.",
[101] = "Group bots follow you and stop wandering off to grind or roam. They still fight whatever attacks you.",
[102] = "Group bots receive the best gear for their level (rare quality at most, server setting). Gear is created, not bought.",
[103] = "Group bots repair their gear.",
[104] = "Group bots do their maintenance: repair, spells, consumables.",
[105] = "The target bot whispers its active strategies, in combat then out of combat.",
[106] = "The target bot whispers its spell list, CoA abilities included.",
[107] = "Shows group members in the chat: level, class and status.",
[108] = "Brings the nearest free bot able to tank into your group: set to your level, given a tank specialization, teleported next to you (GM command).",
[109] = "Brings the nearest free bot able to heal into your group: set to your level, given a healing specialization, teleported next to you (GM command).",
[110] = "Brings the nearest free bot into your group with a damage specialization, set to your level and teleported next to you (GM command).",
};

-- {label, description} by index of ClassStrategyCO / ClassStrategyNC (StrategyTips.lua).
local STRATEGY_CO_EN = {
[1] = {"CoA rotation", "Uses Conquest of Azeroth class abilities (classes 12 to 32). Enable it for every CoA class bot."},
[2] = {"Autonomous", "Without a target in combat, the bot looks for one in its line of sight."},
[3] = {"Free attack", "In combat, the bot picks its targets freely."},
[4] = {"Passive attack", "In combat, the bot picks no target: you must designate it."},
[5] = {"Single-target tank", "The bot holds one monster at a time, then moves to the next."},
[6] = {"Multi-target tank", "The bot tries to hold every monster of the fight."},
[7] = {"Tank assist", "The bot attacks the group tank's target."},
[8] = {"Single-target DPS", "The bot hits one monster at a time, then moves to the next."},
[9] = {"AoE DPS", "Against several monsters, the bot uses its area abilities."},
[10] = {"DPS assist", "The bot helps kill the group's monsters."},
[11] = {"Damage over time", "The bot applies its damage over time effects."},
[12] = {"Healing", "The bot heals wounded group members and stops dealing damage."},
[13] = {"Area attacks", "The bot uses its area damage abilities."},
[14] = {"Keep distance", "When its target reaches melee range, the bot backs off."},
[15] = {"Fire", "The mage uses Fire spells."},
[16] = {"Fire AoE", "Against several monsters, the mage uses area Fire spells."},
[17] = {"Frost", "The mage uses Frost spells."},
[18] = {"Frost AoE", "Against several monsters, the mage uses area Frost spells."},
[19] = {"Arcane", "The mage uses Arcane spells."},
[20] = {"Threat control", "The bot holds back damage to avoid pulling aggro."},
[21] = {"Save mana", "The bot saves mana, at the cost of lower damage."},
[22] = {"Shadow", "The warlock or priest uses Shadow spells."},
[23] = {"Shadow AoE", "Against several monsters, area Shadow spells (warlock, priest)."},
[24] = {"Shadow over time", "Applies Shadow damage over time effects (warlock, priest)."},
[25] = {"Melee", "The bot stays in melee range of its target (shaman)."},
[26] = {"Melee AoE", "The bot hits several targets in melee (shaman)."},
[27] = {"Shadow resist aura", "The paladin enables Shadow Resistance Aura."},
[28] = {"Frost resist aura", "The paladin enables Frost Resistance Aura."},
[29] = {"Fire resist aura", "The paladin enables Fire Resistance Aura."},
[30] = {"Devotion Aura", "The paladin enables Devotion Aura."},
[31] = {"Crusader Aura", "The paladin enables Crusader Aura (mount speed)."},
[32] = {"Nature resist", "The hunter enables Aspect of the Wild."},
[33] = {"Health buffs", "The bot gives its health buffs."},
[34] = {"Support buffs", "The bot gives its support buffs."},
[35] = {"Offensive buffs", "The bot gives its offensive buffs."},
[36] = {"Bear form", "The druid fights in bear form."},
[37] = {"Cat form", "The druid fights in cat form."},
[38] = {"Cat AoE", "The druid hits several targets in cat form."},
[39] = {"Ranged spells", "The bot fights with ranged spells (druid, shaman)."},
[40] = {"Area spells", "Against several monsters, ranged area spells (druid, shaman)."},
[41] = {"Spells over time", "Applies ranged damage over time effects (druid, shaman)."},
[42] = {"Totems", "The shaman places totems."},
[43] = {"Loot", "In combat, the bot loots nearby corpses."},
[44] = {"Naxxramas", "Behaviour tuned for Naxxramas (in that dungeon only)."},
};

local STRATEGY_NC_EN = {
[1] = {"Autonomous", "Out of combat, the bot looks for monsters to attack in its line of sight."},
[2] = {"Roam", "Out of combat, the bot wanders around."},
[3] = {"Free attack", "Out of combat, the bot picks its target freely when a fight starts."},
[4] = {"Passive attack", "Out of combat, the bot attacks nothing: you must designate the target."},
[5] = {"Health buffs", "Out of combat, the bot gives its health buffs."},
[6] = {"Support buffs", "Out of combat, the bot gives its support buffs."},
[7] = {"Offensive buffs", "Out of combat, the bot gives its offensive buffs."},
[8] = {"Emotes", "Out of combat, the bot does emotes from time to time."},
[9] = {"Rest", "After combat, the bot eats and drinks if needed."},
[10] = {"Loot", "After combat, the bot loots nearby corpses."},
[11] = {"Stay", "Out of combat, the bot stays put."},
[12] = {"Follow", "Out of combat, the bot follows you."},
[13] = {"Quests", "Out of combat, the bot accepts and turns in quests of the NPC you interact with."},
[14] = {"Naxxramas", "Behaviour tuned for Naxxramas (in that dungeon only)."},
};

-- Message and window fragments (exact text, replaced wherever it appears).
local FRAGMENTS_EN = {
	{"Barre des bots fermée. Tapez /unbot dans le chat pour la rouvrir.", "Bot bar closed. Type /unbot in the chat to open it again."},
	{"Échec d'initialisation de la fenêtre : ", "Window initialization failed: "},
	{"Initialisation du contrôleur de bots...", "Initializing the bot controller..."},
	{"Contrôleur de bots prêt. Tapez /unbot pour afficher ou masquer la barre.", "Bot controller ready. Type /unbot to show or hide the bar."},
	{"Cible hostile requise", "Hostile target required"},
	{"Cible alliée requise", "Friendly target required"},
	{"Aucune cible requise", "No target required"},
	{"Tout le groupe", "Whole group"},
	{"Clic gauche : exécuter la commande", "Left click: run the command"},
	{"Clic droit : ouvrir ou fermer le groupe de boutons", "Right click: open or close the button group"},
	{"Clic droit : placer sur le bouton rapide", "Right click: put it on the quick button"},
	{"Clic droit : fermer la barre principale", "Right click: close the main bar"},
	{"N° de commande : ", "Command no.: "},
	{"Commande introuvable : n° ", "Command not found: no. "},
	{"Commande : ", "Command: "},
	{"Cible : ", "Target: "},
	{"Vous n'avez sélectionné aucune cible.", "You have no target selected."},
	{"La cible n'est pas dans votre groupe.", "The target is not in your group."},
	{"Vous devez cibler un bot joueur.", "You must target a player bot."},
	{"Vous n'êtes pas le chef du groupe.", "You are not the group leader."},
	{"Vous n'avez aucun ami.", "You have no friends in your list."},
	{" Toutes les icônes", " All icons"},
	{" quitte le combat", " leaves combat"},
	{" Voir les objets", " View items"},
	{" Détruire des objets", " Destroy items"},
	{" Équiper des objets", " Equip items"},
	{" Vendre des objets", " Sell items"},
	{" Utiliser des objets", " Use items"},
	{" Lancer des sorts", " Cast spells"},
	{"Vous n'êtes pas en groupe avec des bots.", "You are not in a group with bots."},
	{"Vous n'êtes pas dans un donjon ou un raid.", "You are not in a dungeon or raid."},
	{" : aucune stratégie de boss connue des bots pour ce lieu.", ": bots know no boss strategy for this place."},
	{" : stratégie « ", ": strategy \""},
	{" » activée pour le groupe.", "\" enabled for the group."},
	{"Mode donjon activé, avec la stratégie « ", "Dungeon mode enabled, with strategy \""},
	{"Mode donjon activé.", "Dungeon mode enabled."},
	{"Réglages de donjon retirés.", "Dungeon settings removed."},
	{"Profil Farm : les bots chassent, ramassent et mangent.", "Farm profile: bots hunt, loot and eat."},
	{"Profil Suivi strict : les bots vous suivent sans partir chasser.", "Strict follow profile: bots follow you without wandering off."},
	{"Vous n'êtes pas en groupe.", "You are not in a group."},
	{"Composition du groupe (", "Group composition ("},
	{" membres) :", " members):"},
	{"  niv. ", "  lvl "},
	{"|cffff3333mort|r", "|cffff3333dead|r"},
	{"|cff888888hors ligne|r", "|cff888888offline|r"},
	{" bot(s) montés au niveau ", " bot(s) raised to level "},
	{". Cliquez ensuite sur Réinitialiser l'IA (9) si leur rôle n'a pas changé.", ". Then click Reset AI (9) if their role did not change."},
	{"Coût : ", "Cost: "},
	{" énergie", " energy"},
	{"Quantité : ", "Quantity: "},
	{" m de portée", " yd range"},
	{"Instantané", "Instant"},
	{"Objet encore inconnu du client : interrogation du serveur en cours, cliquez sur Actualiser pour l'afficher.", "Item not yet known to the client: querying the server, click Refresh to show it."},
	{"Objet encore inconnu du client : patientez un instant puis survolez-le à nouveau.", "Item not yet known to the client: wait a moment then hover it again."},
	{"Clic droit : masquer cet objet", "Right click: hide this item"},
	{"Clic droit : masquer ce sort", "Right click: hide this spell"},
	{"Clic droit : masquer cette icône", "Right click: hide this icon"},
	{"ID du sort : ", "Spell ID: "},
	{"Ciblez d'abord un marchand.", "Target a merchant first."},
	{"Actualiser", "Refresh"},
	{"Équiper", "Equip"},
	{"Détruire", "Destroy"},
	{" |cffcccc00quête terminée : choisissez la récompense|r", " |cffcccc00quest complete: choose the reward|r"},
	{"Clic gauche : faire choisir à ", "Left click: have "},
	{" cette récompense", " choose this reward"},
	{"Choisir la récompense de quête", "Choose the quest reward"},
	{"Clic gauche : faire équiper à ", "Left click: have "},
	{" cet objet.", " equip this item."},
	{"Choix de l'équipement", "Equipment choice"},
	{"Module intégré à la fenêtre d'inspection.", "Module built into the inspect window."},
	{"Emplacements : clic gauche pour changer l'objet, clic droit pour le retirer", "Slots: left click to change the item, right click to remove it"},
	{"Hors combat, clic droit sur le bot pour ouvrir son menu (rôle et équipement).", "Out of combat, right click the bot to open its menu (role and gear)."},
	{"Ciblez-vous, ou ciblez un bot PNJ, avant d'utiliser la commande.", "Target yourself, or an NPC bot, before using the command."},
	{"Ciblez-vous avant d'utiliser la commande.", "Target yourself before using the command."},
	{"Crée un bot de classe ", "Creates a bot of class "},
	{"Créer un bot de classe", "Create a class bot"},
	{"Renvoie le bot PNJ ciblé (vous-même : tous les bots PNJ).", "Dismisses the targeted NPC bot (yourself: every NPC bot)."},
	{"Renvoyer les bots PNJ", "Dismiss NPC bots"},
	{"Réinitialise le bot PNJ ciblé (vous-même : tous les bots PNJ).", "Resets the targeted NPC bot (yourself: every NPC bot)."},
	{"Réinitialiser les bots PNJ", "Reset NPC bots"},
	{"Les bots PNJ du groupe restent sur place.", "Group NPC bots stay put."},
	{"Les bots PNJ du groupe vous suivent.", "Group NPC bots follow you."},
	{"Hors combat, ressuscite les bots PNJ morts du groupe.", "Out of combat, resurrects dead group NPC bots."},
	{"Impossible d'ouvrir la fenetre des bots en ligne.", "Could not open the online bots window."},
	{" est déjà dans votre groupe.", " is already in your group."},
	{"Connecter des bots", "Log in bots"},
	{"Clic gauche : connecter le bot. Clic droit : l'inviter dans le groupe.", "Left click: log the bot in. Right click: invite it to the group."},
	{"Certaines stratégies ne valent pas pour toutes les classes : le bot peut les refuser, et certaines s'excluent mutuellement.", "Some strategies do not apply to every class: the bot may refuse them, and some exclude each other."},
	{" Clic : désactiver cette stratégie", " Click: disable this strategy"},
	{" Clic : activer cette stratégie", " Click: enable this strategy"},
	{" : stratégies", ": strategies"},
	{" Activée", " Enabled"},
	{"Hors combat", "Out of combat"},
	{"En combat", "In combat"},
	{"Guilde : ", "Guild: "},
	{"Chevaliers de la mort", "Death Knights"},
	{"chevalier de la mort", "death knight"},
	{"Chevalier de la mort", "Death Knight"},
	{"Démonistes", "Warlocks"},
	{"démoniste", "warlock"},
	{"Démoniste", "Warlock"},
	{"Prêtres", "Priests"},
	{"prêtre", "priest"},
	{"Prêtre", "Priest"},
	{"Guerriers", "Warriors"},
	{"Guerrier", "Warrior"},
	{"guerrier", "warrior"},
	{"Voleurs", "Rogues"},
	{"Voleur", "Rogue"},
	{"voleur", "rogue"},
	{"Druides", "Druids"},
	{"Druide", "Druid"},
	{"druide", "druid"},
	{"Chasseurs", "Hunters"},
	{"Chasseur", "Hunter"},
	{"chasseur", "hunter"},
	{"Chamans", "Shamans"},
	{"Chaman", "Shaman"},
	{"chaman", "shaman"},
	{"Amis", "Friends"},
};

-- Longest first, so a fragment never cuts a sentence that contains it.
table.sort(FRAGMENTS_EN, function(a, b) return string.len(a[1]) > string.len(b[1]); end);

UnBotLanguage = "fr";

local function ReplacePlain(text, from, to)
	local s, e = string.find(text, from, 1, true);
	if (not s) then
		return text;
	end
	local parts, start = {}, 1;
	while (s) do
		table.insert(parts, string.sub(text, start, s - 1));
		table.insert(parts, to);
		start = e + 1;
		s, e = string.find(text, from, start, true);
	end
	table.insert(parts, string.sub(text, start));
	return table.concat(parts);
end

function UnBotTr(text)
	if (UnBotLanguage ~= "en" or type(text) ~= "string" or text == "") then
		return text;
	end
	for i = 1, #FRAGMENTS_EN do
		text = ReplacePlain(text, FRAGMENTS_EN[i][1], FRAGMENTS_EN[i][2]);
	end
	return text;
end

local function TranslateFrame(frame, depth)
	if (not frame or depth > 8) then
		return;
	end
	if (frame.GetText and frame.SetText and frame:GetObjectType() ~= "EditBox") then
		local text = frame:GetText();
		if (text) then
			frame:SetText(UnBotTr(text));
		end
	end
	local regions = { frame:GetRegions() };
	for i = 1, #regions do
		local region = regions[i];
		if (region and region:GetObjectType() == "FontString") then
			local text = region:GetText();
			if (text) then
				region:SetText(UnBotTr(text));
			end
		end
	end
	local children = { frame:GetChildren() };
	for i = 1, #children do
		TranslateFrame(children[i], depth + 1);
	end
end

local function TranslateTopFrames()
	for _, name in ipairs({ "UnBotFrame", "NPCFrame", "OnlineFrame" }) do
		local frame = _G[name];
		if (frame) then
			TranslateFrame(frame, 0);
			if (not frame.UnBotLocaleHooked and frame.HookScript) then
				frame.UnBotLocaleHooked = true;
				frame:HookScript("OnShow", function(self) TranslateFrame(self, 0); end);
			end
		end
	end
end

-- Picks the language and replaces the text tables with English when needed.
local function ApplyLanguage()
	if (UnBotCommandBarConfig == nil) then
		UnBotCommandBarConfig = {};
	end
	local lang = UnBotCommandBarConfig.lang;
	if (lang ~= "fr" and lang ~= "en") then
		lang = (GetLocale() == "frFR") and "fr" or "en";
	end
	UnBotLanguage = lang;
	if (lang ~= "en") then
		return;
	end
	for index, text in pairs(TITLE_EN) do
		if (UnBotTooltipTitle[index] ~= nil) then
			UnBotTooltipTitle[index] = text;
		end
	end
	for index, text in pairs(HELP_EN) do
		if (UnBotTooltipHelp[index] ~= nil) then
			UnBotTooltipHelp[index] = text;
		end
	end
	_G["BINDING_NAME_CLICK UnBotCommandButton11:LeftButton"] = "UnBot: Dungeon group (button 11)";
	_G["BINDING_NAME_CLICK UnBotCommandButton12:LeftButton"] = "UnBot: CoA & profiles group (button 12)";
end

-- Every chat message goes through DisplayInfomation (UnBot.lua).
local originalDisplay = DisplayInfomation;
DisplayInfomation = function(info)
	originalDisplay(UnBotTr(info));
end

-- Tooltips hard-coded in the XML and Lua windows.
for _, method in ipairs({ "AddLine", "SetText" }) do
	local original = GameTooltip[method];
	GameTooltip[method] = function(self, text, ...)
		return original(self, UnBotTr(text), ...);
	end
end
do
	local original = GameTooltip.AddDoubleLine;
	GameTooltip.AddDoubleLine = function(self, left, right, ...)
		return original(self, UnBotTr(left), UnBotTr(right), ...);
	end
end

-- Strategy descriptions, rebuilt on every InitializeStrategy().
local originalInitializeStrategy = InitializeStrategy;
InitializeStrategy = function(...)
	originalInitializeStrategy(...);
	if (UnBotLanguage ~= "en") then
		return;
	end
	for index, texts in pairs(STRATEGY_CO_EN) do
		if (ClassStrategyCO[index]) then
			ClassStrategyCO[index][2], ClassStrategyCO[index][3] = texts[1], texts[2];
		end
	end
	for index, texts in pairs(STRATEGY_NC_EN) do
		if (ClassStrategyNC[index]) then
			ClassStrategyNC[index][2], ClassStrategyNC[index][3] = texts[1], texts[2];
		end
	end
end

-- The language is applied before the bar is initialized (ADDON_LOADED), which loads the saved variables.
local originalInitialize = InitializeUnBotFrame;
InitializeUnBotFrame = function(...)
	ApplyLanguage();
	originalInitialize(...);
	TranslateTopFrames();
end

-- /unbot lang fr|en: changes the language (reloads the interface for the windows).
local originalSlash = SlashCmdList["UNBOT"];
SlashCmdList["UNBOT"] = function(msg)
	local lang = msg and string.match(string.lower(msg), "^%s*lang%s+(%a%a)%s*$");
	if (lang == "fr" or lang == "en") then
		if (UnBotCommandBarConfig == nil) then
			UnBotCommandBarConfig = {};
		end
		UnBotCommandBarConfig.lang = lang;
		ReloadUI();
		return;
	end
	if (msg and string.match(string.lower(msg), "^%s*lang")) then
		DEFAULT_CHAT_FRAME:AddMessage("|cff00ccccUnBot: /unbot lang fr  |  /unbot lang en|r");
		return;
	end
	originalSlash(msg);
end
