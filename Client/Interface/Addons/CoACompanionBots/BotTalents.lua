-- CoA Companion: the CoA talent window for an alt bot.
-- The trees are the client's own (Ascension_CoATalents: TalentTreeBase with the CoA node templates); only their
-- data comes from the bot instead of the player's Character Advancement. The server (mod-playerbots,
-- PlayerbotsBotTalents.cpp) answers "CoAT" addon whispers: GET, LEARN, SPEC and RESET, each with the bot's state
-- (BEGIN, PTS, E lines, END). Ranks clicked here stay pending until Save sends them; right-click takes a pending
-- rank back, Reset takes back every talent the bot bought. CoA Talent Path marks its leveling path here too.

local PREFIX = "CoAT"
local TALENT_ADDON = "Ascension_CoATalents"
local MAX_LEARN_MESSAGE = 220

local TEXTS = {
    en = {
        TITLE = "%s - Talents (level %d)",
        LOADING = "Asking %s for its talents...",
        NO_ALT = "%s is not one of your CoA alts in this group, or is on another map.",
        NO_UI = "The CoA talent window could not be loaded.",
        NO_SPEC = "No specialization yet: choose one with the button on the left.",
        CHOOSE_SPEC = "Specialization",
        SAVE = "Save (%d)",
        UNDO = "Undo",
        RESET = "Reset talents",
        RESET_ASK = "Take back every talent %s bought? Its points can then be spent again.",
        SPEC_ASK = "Switch %s to %s?",
        CLASS_POINTS = "%d class points left",
        SPEC_POINTS = "%d specialization points left",
        LEARN = "Click to learn",
        UNLEARN = "Right-click to take back",
        LEVEL = "Requires level %d",
        GATED = "Spend more points in this tree first",
        NOT_OFFERED = "Not available for this bot",
    },
    fr = {
        TITLE = "%s - Talents (niveau %d)",
        LOADING = "Demande des talents à %s...",
        NO_ALT = "%s n'est pas un de vos alts CoA dans ce groupe, ou se trouve sur une autre carte.",
        NO_UI = "La fenêtre de talents CoA n'a pas pu être chargée.",
        NO_SPEC = "Pas encore de spécialisation : choisissez-en une avec le bouton à gauche.",
        CHOOSE_SPEC = "Spécialisation",
        SAVE = "Sauvegarder (%d)",
        UNDO = "Annuler",
        RESET = "Réinitialiser",
        RESET_ASK = "Reprendre tous les talents achetés par %s ? Ses points pourront être dépensés à nouveau.",
        SPEC_ASK = "Passer %s en %s ?",
        CLASS_POINTS = "%d points de classe restants",
        SPEC_POINTS = "%d points de spécialisation restants",
        LEARN = "Clic pour apprendre",
        UNLEARN = "Clic droit pour reprendre",
        LEVEL = "Niveau %d requis",
        GATED = "Dépensez d'abord plus de points dans cet arbre",
        NOT_OFFERED = "Indisponible pour ce bot",
    },
}

local function T(key)
    local language = CoACompanionBots_Language and CoACompanionBots_Language()
        or ((GetLocale() == "frFR") and "fr" or "en")
    return (TEXTS[language] or TEXTS.en)[key] or TEXTS.en[key] or key
end

local frame, treeView, refreshPath
local botName, state, incoming = nil, nil, nil
local pending = {}
local groupMembers = {}

local function Split(text, separator)
    local parts = {}
    for part in string.gmatch(text .. separator, "([^" .. separator .. "]*)" .. separator) do table.insert(parts, part) end
    return parts
end

local function Request(command, argument)
    if not botName then return end
    SendAddonMessage(PREFIX, command .. "\t" .. botName .. (argument and ("\t" .. argument) or ""), "WHISPER",
        UnitName("player"))
end

local function BotUnit(name)
    if UnitInRaid("player") then
        for i = 1, GetNumRaidMembers() do
            if UnitName("raid" .. i) == name then return "raid" .. i end
        end
    end
    for i = 1, GetNumPartyMembers() do
        if UnitName("party" .. i) == name then return "party" .. i end
    end
    return nil
end

-- Bot data -----------------------------------------------------------------------------------------------------------

local function Known(entryID)
    return state and state.entries[entryID]
end

local function Rank(entryID)
    local known = Known(entryID)
    return (known and known.rank or 0) + (pending[entryID] or 0)
end

local function MaxRank(entry)
    local known = Known(entry.ID)
    if known then return known.max end
    return type(entry.Spells) == "table" and #entry.Spells or 1
end

local function PendingCost(tab)
    local cost = 0
    for entryID, count in pairs(pending) do
        local known = Known(entryID)
        if known and known.tab == tab then cost = cost + known.cost * count end
    end
    return cost
end

local function PendingCount()
    local count = 0
    for _, ranks in pairs(pending) do count = count + ranks end
    return count
end

local function Invested(tab)
    if not state then return 0 end
    local spent = tab == "c" and state.classSpent or state.specSpent
    return spent + PendingCost(tab)
end

local function Remaining(tab)
    if not state then return 0 end
    local budget = tab == "c" and state.classBudget or state.specBudget
    return budget - Invested(tab)
end

local function GateRequirement(entry, tab)
    if tab == "c" then return entry.RequiredTabAEInvestment or 0 end
    return entry.RequiredTabTEInvestment or 0
end

local function MeetsGate(entry)
    local known = Known(entry.ID)
    if not known then return true end
    return GateRequirement(entry, known.tab) <= Invested(known.tab)
end

local function GroupRivals(entry)
    local rivals = {}
    if entry.Group and entry.Group ~= 0 then
        for _, entryID in ipairs(groupMembers[entry.Group] or {}) do
            if entryID ~= entry.ID then table.insert(rivals, entryID) end
        end
    end
    return rivals
end

-- Why the next rank cannot be taken, or nil: the server's rules (RefusalFor) plus the tree's gates and choices.
local function AddProblem(entry)
    local known = Known(entry.ID)
    if not known or known.cost == 0 then return T("NOT_OFFERED") end
    if Rank(entry.ID) >= known.max then return "max" end
    if known.level > state.level then return string.format(T("LEVEL"), known.level) end
    for _, required in ipairs(entry.RequiredIDs or {}) do
        if Rank(required) == 0 then return "requires" end
    end
    if Remaining(known.tab) < known.cost then return "points" end
    if not MeetsGate(entry) then return T("GATED") end
    for _, rival in ipairs(GroupRivals(entry)) do
        local held = Known(rival)
        if held and held.rank > 0 then return "choice" end
    end
    return nil
end

local function CanAdd(entry)
    return state ~= nil and AddProblem(entry) == nil
end

local Changed

local function Prune()
    local again = true
    while again do
        again = false
        for entryID in pairs(pending) do
            local entry = C_CharacterAdvancement.GetEntryByInternalID(entryID)
            for _, required in ipairs(entry and entry.RequiredIDs or {}) do
                if Rank(required) == 0 then
                    pending[entryID] = nil
                    again = true
                    break
                end
            end
        end
    end
end

local function Add(entry)
    if not CanAdd(entry) then return false end
    for _, rival in ipairs(GroupRivals(entry)) do pending[rival] = nil end
    pending[entry.ID] = (pending[entry.ID] or 0) + 1
    Changed()
    return true
end

local function Remove(entry)
    if not pending[entry.ID] then return false end
    pending[entry.ID] = pending[entry.ID] > 1 and pending[entry.ID] - 1 or nil
    Prune()
    Changed()
    return true
end

-- Tree nodes ---------------------------------------------------------------------------------------------------------

local NodeOverrides = {}

function NodeOverrides:UpdateDisplay()
    if not self.entry then return end
    self.canLearn = CanAdd(self.entry)
    self.rank, self.maxRank = Rank(self.entry.ID), MaxRank(self.entry)
    self.isGated = not MeetsGate(self.entry)
    self.spellID = type(self.entry.Spells) == "table" and self.entry.Spells[math.max(self.rank, 1)] or self.entry.Spells
    self:UpdateIcon()
    self:UpdateRank()
    self.visualState = self:CalculateVisualState()
    self:UpdateConnectionVisual(self.visualState)
    self:ApplyVisualState(self.visualState)
    if GameTooltip:IsOwned(self) then self:OnEnter() end
end

function NodeOverrides:CanConnect()
    return self.entry ~= nil and Rank(self.entry.ID) > 0
end

function NodeOverrides:UpdateSearchMarker()
    self.Icon.LocationIcon:Hide()
end

function NodeOverrides:OnClick(button)
    CloseDropDownMenus()
    if not self.entry then return end
    if IsModifiedClick("CHATLINK") and self.spellID then
        local link = GetSpellLink(self.spellID)
        if link and ChatEdit_InsertLink(link) then return end
    end
    if button == "RightButton" then
        if Remove(self.entry) and self.deselectSound then PlaySound(self.deselectSound) end
    elseif Add(self.entry) and self.selectSound then
        PlaySound(self.selectSound)
    end
end

function NodeOverrides:OnEnter()
    local spellID = self:GetTooltipSpellID()
    if not spellID then return end
    GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
    GameTooltip:SetSpellByID(spellID)
    GameTooltip:AddLine(TOOLTIP_TALENT_RANK:format(self.rank or 0, self.maxRank or 1), HIGHLIGHT_FONT_COLOR:GetRGB())
    GameTooltip_AddSpacer(GameTooltip)
    local problem = AddProblem(self.entry)
    if not problem then
        GameTooltip:AddLine(T("LEARN"), GREEN_FONT_COLOR:GetRGB())
    elseif problem ~= "max" and problem ~= "requires" and problem ~= "points" and problem ~= "choice" then
        local r, g, b = RED_FONT_COLOR:GetRGB()
        GameTooltip:AddLine(problem, r, g, b, true)
    end
    if pending[self.entry.ID] then GameTooltip:AddLine(T("UNLEARN"), DISABLED_FONT_COLOR:GetRGB()) end
    GameTooltip:Show()
end

function NodeOverrides:OnDragStart()
end

local ChoiceOverrides = {}

function ChoiceOverrides:GetSelectedNodeInfo()
    for _, node in ipairs(self.nodes) do
        if Rank(node.entry.ID) > 0 then return node end
    end
end

function ChoiceOverrides:UpdateDisplay()
    local selected = self:GetSelectedNodeInfo()
    for _, subNode in ipairs(self.nodes) do subNode:UpdateDisplay() end
    self.entry = selected and selected.entry
    if self.entry then
        NodeOverrides.UpdateDisplay(self)
        return
    end
    self.canLearn = false
    for _, subNode in ipairs(self.nodes) do
        if CanAdd(subNode.entry) then self.canLearn = true break end
    end
    self.rank, self.maxRank = 0, 1
    self:UpdateRank()
    self:UpdateIcon()
    self.visualState = self:CalculateVisualState()
    self:UpdateConnectionVisual(self.visualState)
    self:ApplyVisualState(self.visualState)
end

function ChoiceOverrides:CanConnect()
    for _, node in ipairs(self.nodes) do
        if Rank(node.entry.ID) > 0 then return true end
    end
    return false
end

ChoiceOverrides.UpdateSearchMarker = NodeOverrides.UpdateSearchMarker

local function Apply(node, overrides, scripts)
    for key, value in pairs(overrides) do node[key] = value end
    for _, script in ipairs(scripts) do node:SetScript(script, overrides[script]) end
end

local function ResetNode(pool, node)
    if node.Reset then node:Reset() end
    FramePool_HideAndClearAnchors(pool, node)
end

local function CreateNode(tree, entry)
    local pool = tree.nodePool:GetOrCreatePool("Button", tree, tree.getNodeTemplate(entry), ResetNode)
    local node = pool:Acquire()
    Apply(node, NodeOverrides, { "OnClick", "OnEnter", "OnDragStart" })
    if node:GetParent() ~= tree then node:SetParent(tree) end
    node:SetSize(tree.buttonWidth, tree.buttonHeight)
    node:SetEntry(entry)
    node:UpdateDisplay()
    node:Show()
    tree.nodeCache[entry.ID] = node
    if entry.Group and entry.Group ~= 0 then
        groupMembers[entry.Group] = groupMembers[entry.Group] or {}
        table.insert(groupMembers[entry.Group], entry.ID)
    end
    return node
end

local function GetOrCreateChoiceNode(tree, entry)
    local choiceNode = tree.groupCache[entry.Group]
    if choiceNode then return choiceNode end
    local pool = tree.nodePool:GetOrCreatePool("Button", tree, tree.getChoiceNodeTemplate(entry), ResetNode)
    choiceNode = pool:Acquire()
    Apply(choiceNode, ChoiceOverrides, {})
    choiceNode.OnClick = NodeOverrides.OnClick
    choiceNode:SetScript("OnClick", NodeOverrides.OnClick)
    choiceNode:SetScript("OnDragStart", NodeOverrides.OnDragStart)
    choiceNode:SetSize(tree.buttonWidth, tree.buttonHeight)
    choiceNode:Show()
    tree.groupCache[entry.Group] = choiceNode
    return choiceNode
end

local function CreateTree(tab, anchors)
    local tree = CreateFrame("Frame", nil, treeView, "TalentTreeBase")
    for _, anchor in ipairs(anchors) do tree:SetPoint(unpack(anchor)) end
    tree.buttonWidth, tree.buttonHeight, tree.buttonSpacingX, tree.buttonSpacingY = 30, 30, 14, 14
    tree.getNodeTemplate = CoACharacterAdvancementUtil.GetDisplayTemplateForEntry
    tree.getChoiceNodeTemplate = CoACharacterAdvancementUtil.GetDisplayTemplateForEntryChoice
    tree.getConnectionTemplate = CoACharacterAdvancementUtil.GetDisplayTemplateForConnection
    tree.useGates, tree.onlyOneGate = true, true
    tree.getGateTemplate = CoACharacterAdvancementUtil.GetDisplayTemplateForGate
    tree.getGateAttachmentPoint = CoACharacterAdvancementUtil.GetGateLeftAttachmentPoint
    tree.getEntryGateRequirement = tab == "c" and CoACharacterAdvancementUtil.GetEntryAEGateRequirement
        or CoACharacterAdvancementUtil.GetEntryTEGateRequirement
    tree.gateCurrencyCount = function() return Invested(tab) end
    tree.CreateNode = CreateNode
    tree.GetOrCreateChoiceNode = GetOrCreateChoiceNode
    tree.ResetTree = function() end

    tree.Label = tree:CreateFontString(nil, "OVERLAY", "GameFontHighlightHuge2")
    tree.Label:SetPoint("BOTTOM", tree, "TOP", 0, 70)
    tree.Points = tree:CreateFontString(nil, "OVERLAY", "GameFontHighlightHuge")
    tree.Points:SetPoint("TOP", tree.Label, "BOTTOM", 0, -2)
    return tree
end

-- Window -------------------------------------------------------------------------------------------------------------

local function UpdateBar()
    if not frame then return end
    local bar = treeView.BottomBar
    local count = PendingCount()
    bar.save:SetText(string.format(T("SAVE"), count))
    bar.save:SetEnabled(count > 0)
    bar.undo:SetEnabled(count > 0)
    bar.reset:SetEnabled(state ~= nil and count == 0)
    if state then
        local class, spec = Remaining("c"), Remaining("s")
        treeView.ClassTree.Points:SetText(string.format(T("CLASS_POINTS"), class))
        treeView.SpecTree.Points:SetText(string.format(T("SPEC_POINTS"), spec))
        treeView.ClassTree.Points:SetTextColor((class > 0 and GREEN_FONT_COLOR or DISABLED_FONT_COLOR):GetRGB())
        treeView.SpecTree.Points:SetTextColor((spec > 0 and GREEN_FONT_COLOR or DISABLED_FONT_COLOR):GetRGB())
    end
end

Changed = function()
    if not frame then return end
    treeView.ClassTree:MarkDirty(TalentTreeBaseMixin.DirtyReason.Nodes)
    if treeView.SpecTree:IsShown() then treeView.SpecTree:MarkDirty(TalentTreeBaseMixin.DirtyReason.Nodes) end
    UpdateBar()
    if refreshPath then refreshPath() end
end

local function SpecInfo()
    if not state or not state.specID or state.specID == 0 then return nil end
    return C_ClassInfo.GetSpecInfoByID(state.specID)
end

local function ApplyState()
    local unit = BotUnit(botName)
    local _, classToken = UnitClass(unit or "player")
    state.classToken = classToken
    frame.title:SetText(string.format(T("TITLE"), botName, state.level))

    local classDBC = CharacterAdvancementUtil.GetClassDBCByFile(classToken)
    if treeView.classDBC ~= classDBC then
        wipe(groupMembers)
        treeView.classDBC = classDBC
        treeView.ClassTree:SetClassTab(classDBC, "Class")
        treeView.ClassTree.Label:SetText((LOCALIZED_CLASS_NAMES_MALE[classToken] or classToken or ""):upper())
        treeView.specKey = nil
    end

    local specInfo = SpecInfo()
    local specKey = specInfo and (classDBC .. ":" .. specInfo.Spec) or nil
    if specInfo then
        if treeView.specKey ~= specKey then
            treeView.specKey = specKey
            treeView.SpecTree:SetClassTab(classDBC, CharacterAdvancementUtil.GetSpecDBCByFile(specInfo.Spec))
            treeView.SpecTree.Label:SetText(specInfo.Name:upper())
            local art = CharacterAdvancementUtil.GetBackgroundAtlas(specInfo.Class, specInfo.Spec)
            if art then treeView.Background:SetAtlas(art) end
        end
        treeView.SpecTree:Show()
        frame.status:SetText("")
        UIDropDownMenu_SetText(treeView.BottomBar.spec, specInfo.Name)
    else
        treeView.specKey = nil
        treeView.SpecTree:Hide()
        frame.status:SetText(T("NO_SPEC"))
        UIDropDownMenu_SetText(treeView.BottomBar.spec, T("CHOOSE_SPEC"))
    end
    Changed()
end

local function InitializeSpecMenu(_, level)
    if not state or not state.classToken then return end
    local ok, specs = pcall(C_ClassInfo.GetAllSpecs, state.classToken)
    if not ok or type(specs) ~= "table" then return end
    for _, spec in ipairs(specs) do
        local infoOk, specInfo = pcall(C_ClassInfo.GetSpecInfo, state.classToken, spec)
        if infoOk and type(specInfo) == "table" and specInfo.ID then
            local info = UIDropDownMenu_CreateInfo()
            info.text = specInfo.Name
            info.checked = specInfo.ID == state.specID
            info.func = function()
                if specInfo.ID == state.specID then return end
                StaticPopupDialogs.COACOMPANION_BOT_SPEC.text = string.format(T("SPEC_ASK"), botName, specInfo.Name)
                StaticPopup_Show("COACOMPANION_BOT_SPEC", nil, nil, specInfo.ID)
            end
            UIDropDownMenu_AddButton(info, level)
        end
    end
end

local function Save()
    local list = {}
    for entryID, count in pairs(pending) do
        local known = Known(entryID)
        for _ = 1, count do table.insert(list, { id = entryID, level = known and known.level or 0 }) end
    end
    table.sort(list, function(a, b)
        if a.level ~= b.level then return a.level < b.level end
        return a.id < b.id
    end)
    local chunk = ""
    for _, item in ipairs(list) do
        local text = tostring(item.id)
        if #chunk + #text + 1 > MAX_LEARN_MESSAGE then
            Request("LEARN", chunk)
            chunk = ""
        end
        chunk = chunk == "" and text or (chunk .. "," .. text)
    end
    if chunk ~= "" then Request("LEARN", chunk) end
    wipe(pending)
    UpdateBar()
end

local function BarButton(bar, width, text, onClick)
    local button = CreateFrame("Button", nil, bar, "UIPanelButtonTemplate")
    button:SetWidth(width)
    button:SetHeight(24)
    button:SetText(text)
    button:SetScript("OnClick", onClick)
    return button
end

local function CreateWindow()
    frame = CreateFrame("Frame", "CoACompanionBotTalentFrame", UIParent)
    frame:SetWidth(1294)
    frame:SetHeight(666)
    frame:SetPoint("CENTER")
    frame:SetFrameStrata("DIALOG")
    frame:SetToplevel(true)
    frame:SetBackdrop({
        bgFile = "Interface\\DialogFrame\\UI-DialogBox-Background",
        edgeFile = "Interface\\DialogFrame\\UI-DialogBox-Border",
        tile = true, tileSize = 32, edgeSize = 24,
        insets = { left = 6, right = 6, top = 6, bottom = 6 },
    })
    frame:EnableMouse(true)
    frame:SetMovable(true)
    frame:RegisterForDrag("LeftButton")
    frame:SetScript("OnDragStart", frame.StartMoving)
    frame:SetScript("OnDragStop", frame.StopMovingOrSizing)
    frame:SetClampedToScreen(true)
    frame:Hide()
    table.insert(UISpecialFrames, frame:GetName())

    frame.title = frame:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
    frame.title:SetPoint("TOP", 0, -12)
    local close = CreateFrame("Button", nil, frame, "UIPanelCloseButton")
    close:SetPoint("TOPRIGHT", -4, -4)

    treeView = CreateFrame("Frame", nil, frame)
    treeView:SetPoint("TOPLEFT", 8, -32)
    treeView:SetPoint("BOTTOMRIGHT", -8, 8)
    treeView.Background = treeView:CreateTexture(nil, "BACKGROUND")
    treeView.Background:SetAllPoints()

    treeView.ClassTree = CreateTree("c", { { "TOPLEFT", treeView, "TOPLEFT", 128, -110 },
        { "BOTTOMRIGHT", treeView, "BOTTOM", -140, 40 } })
    treeView.SpecTree = CreateTree("s", { { "TOPRIGHT", treeView, "TOPRIGHT", -128, -110 },
        { "BOTTOMLEFT", treeView, "BOTTOM", 140, 40 } })

    local bar = CreateFrame("Frame", nil, treeView)
    bar:SetHeight(66)
    bar:SetPoint("BOTTOMLEFT")
    bar:SetPoint("BOTTOMRIGHT")
    treeView.BottomBar = bar

    bar.spec = CreateFrame("Frame", "CoACompanionBotTalentSpec", bar, "UIDropDownMenuTemplate")
    bar.spec:SetPoint("LEFT", bar, "LEFT", 0, 4)
    UIDropDownMenu_SetWidth(bar.spec, 180)
    UIDropDownMenu_Initialize(bar.spec, InitializeSpecMenu)

    bar.save = BarButton(bar, 140, string.format(T("SAVE"), 0), Save)
    bar.save:SetPoint("RIGHT", bar, "RIGHT", -16, 4)
    bar.undo = BarButton(bar, 90, T("UNDO"), function()
        wipe(pending)
        Changed()
    end)
    bar.undo:SetPoint("RIGHT", bar.save, "LEFT", -6, 0)
    bar.reset = BarButton(bar, 120, T("RESET"), function()
        StaticPopupDialogs.COACOMPANION_BOT_RESET.text = string.format(T("RESET_ASK"), botName or "?")
        StaticPopup_Show("COACOMPANION_BOT_RESET")
    end)
    bar.reset:SetPoint("RIGHT", bar.undo, "LEFT", -6, 0)

    frame.status = bar:CreateFontString(nil, "OVERLAY", "GameFontNormal")
    frame.status:SetPoint("BOTTOM", treeView, "BOTTOM", 0, 92)

    frame:SetScript("OnHide", function()
        wipe(pending)
        CloseDropDownMenus()
    end)

    if CoATalentPath_RegisterView then
        refreshPath = CoATalentPath_RegisterView({
            name = "bot",
            treeView = function() return treeView end,
            rank = Rank,
            level = function() return state and state.level or 1 end,
            specInfo = SpecInfo,
            controlsBar = bar,
            controlsPoint = { "LEFT", bar.spec, "RIGHT", 10, 12 },
        })
    end
end

StaticPopupDialogs.COACOMPANION_BOT_RESET = {
    text = "", button1 = YES, button2 = NO, timeout = 0, whileDead = true, hideOnEscape = true,
    OnAccept = function() Request("RESET") end,
}

StaticPopupDialogs.COACOMPANION_BOT_SPEC = {
    text = "", button1 = YES, button2 = NO, timeout = 0, whileDead = true, hideOnEscape = true,
    OnAccept = function(_, specID)
        wipe(pending)
        Request("SPEC", tostring(specID))
    end,
}

function CoACompanionBots_OpenBotTalents(name)
    if not name then return end
    if not IsAddOnLoaded(TALENT_ADDON) then LoadAddOn(TALENT_ADDON) end
    if not (TalentTreeBaseMixin and CoACharacterAdvancementUtil and CharacterAdvancementUtil) then
        DEFAULT_CHAT_FRAME:AddMessage("|cff69ccf0CoA Companion:|r " .. T("NO_UI"))
        return
    end
    if not frame then CreateWindow() end
    if botName ~= name then
        botName, state = name, nil
        wipe(pending)
        treeView.classDBC, treeView.specKey = nil, nil
        frame.title:SetText(name)
    end
    frame.status:SetText(string.format(T("LOADING"), name))
    frame:Show()
    Request("GET")
end

-- Server answers -----------------------------------------------------------------------------------------------------

local function OnMessage(message)
    local parts = Split(message, "\t")
    local kind, name = parts[1], parts[2]
    if not frame or name ~= botName then return end
    if kind == "BEGIN" then
        incoming = { level = tonumber(parts[3]) or 1, specID = tonumber(parts[4]) or 0, entries = {},
            classSpent = 0, classBudget = 0, specSpent = 0, specBudget = 0 }
    elseif kind == "PTS" and incoming then
        incoming.classSpent, incoming.classBudget = tonumber(parts[3]) or 0, tonumber(parts[4]) or 0
        incoming.specSpent, incoming.specBudget = tonumber(parts[5]) or 0, tonumber(parts[6]) or 0
    elseif kind == "E" and incoming then
        for item in string.gmatch(parts[3] or "", "[^,]+") do
            local entry, rank, max, cost, tab, level = string.match(item, "^(%d+):(%d+):(%d+):(%d+):(%a):(%d+)$")
            if entry then
                incoming.entries[tonumber(entry)] = { rank = tonumber(rank), max = tonumber(max), cost = tonumber(cost),
                    tab = tab, level = tonumber(level) }
            end
        end
    elseif kind == "END" and incoming then
        state, incoming = incoming, nil
        if frame:IsShown() then ApplyState() end
    elseif kind == "MSG" then
        DEFAULT_CHAT_FRAME:AddMessage("|cff69ccf0CoA Companion:|r " .. name .. ": " .. (parts[3] or ""))
    elseif kind == "NONE" then
        frame.status:SetText(string.format(T("NO_ALT"), name))
    end
end

local events = CreateFrame("Frame")
events:RegisterEvent("CHAT_MSG_ADDON")
events:SetScript("OnEvent", function(_, _, prefix, message, _, sender)
    if prefix ~= PREFIX or not message or sender ~= UnitName("player") then return end
    local kind = string.match(message, "^(%u+)\t")
    if kind == "GET" or kind == "LEARN" or kind == "SPEC" or kind == "RESET" then return end
    OnMessage(message)
end)

SLASH_COACOMPANIONBOTTALENTS1 = "/bottalents"
SlashCmdList.COACOMPANIONBOTTALENTS = function(message)
    local name = string.match(message or "", "^%s*(%S+)")
    if not name and UnitExists("target") and UnitIsPlayer("target") then name = UnitName("target") end
    CoACompanionBots_OpenBotTalents(name)
end
