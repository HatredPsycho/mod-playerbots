-- The leveling path of CoA Talent Path on the bot sheet's tree view (TalentTree.lua), drawn as in the player's
-- own talent window: the level under each talent of the path, the next one in a pulsing ring, the path lines in
-- the tooltip and the next pick named in the bottom line. Nothing happens without the CoATalentPath addon.

local TT = SquidBotsLite_TalentTree
if not TT then return end

local NODE, NODE_TIGHT, TIGHT_ROW = 30, 26, 40
local refresh

local function Shown()
	local nodes, index = TT.view and TT.view.nodes or {}, 0
	return function()
		repeat
			index = index + 1
		until not nodes[index] or nodes[index]:IsShown()
		return nodes[index]
	end
end

local tree = {
	EnumerateNodes = function() return Shown() end,
	IsShown = function() return TT.view and TT.view:IsShown() end,
}

local function Rank(entry)
	local node = TT.data and TT.data.nodes[entry]
	return node and node.rank or 0
end

-- The bot's own specialization: the tree it sent ranks for that is not the class tree.
local function SpecInfo()
	local data, layout = TT.data, TT.Layout()
	if not data or not layout then return nil end
	for _, key in ipairs(data.trees) do
		local shown = layout.trees[key]
		if shown and shown.name ~= "Class" and data.sent and data.sent[key] then return { Name = shown.name } end
	end
	return nil
end

local function Level()
	local name = TT.State()
	local info = name and SquidBotsLiteDB and SquidBotsLiteDB.altInfo and SquidBotsLiteDB.altInfo[name]
	if info and info.level then return info.level end
	for i = 1, 4 do
		if UnitName("party" .. i) == name then return UnitLevel("party" .. i) or 0 end
	end
	for i = 1, GetNumRaidMembers() do
		if UnitName("raid" .. i) == name then return UnitLevel("raid" .. i) or 0 end
	end
	return 0
end

-- The level labels sit under the icons: on a tree with close rows the icons give them the room.
local function FitNodes()
	local view, layout = TT.view, TT.Layout()
	local shown = layout and layout.trees[TT.tree]
	if not shown then return end
	local height = view.grid:GetHeight()
	if not height or height <= 0 then height = 366 end
	local marked = not CoATalentPathDB or CoATalentPathDB.enabled ~= false
	local size = (marked and height / shown.h < TIGHT_ROW) and NODE_TIGHT or NODE
	for _, node in ipairs(view.nodes) do
		node:SetWidth(size)
		node:SetHeight(size)
	end
end

hooksecurefunc(TT, "DrawTree", function()
	if not CoATalentPath_RegisterView or not TT.view then return end
	FitNodes()
	if not refresh then
		TT.view.SpecTree = tree
		refresh = CoATalentPath_RegisterView({
			name = "sheet",
			treeView = function() return TT.view end,
			rank = Rank,
			level = Level,
			specInfo = SpecInfo,
			controlsBar = TT.view,
			controlsPoint = { "BOTTOMLEFT", TT.view, "BOTTOMLEFT", 2, -3 },
			compact = true,
			hintRight = 110,
		})
	end
	refresh()
end)
