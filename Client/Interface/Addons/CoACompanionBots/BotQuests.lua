-- CoA Companion: the quests near you that only some of your bots can take.
-- The server (mod-playerbots, PlayerbotsNearbyQuests.cpp) answers "CoAQ\tSCAN\t<0|1>" with BEGIN, ZONE, Q and END
-- lines; mode 0 is class quests, mode 1 adds race and profession quests. The list shows in a window, on the world
-- map and on the minimap. "ACCEPT\t<bot>\t<quest>" lets that bot take a listed quest; the server answers TAKEN.

local PREFIX = "CoAQ"
local REFRESH_SECONDS = 20
local SCAN_TIMEOUT_SECONDS = 10
local PIN_UPDATE_SECONDS = 0.2
local ROW_HEIGHT = 16
local VISIBLE_ROWS = 18
local MINIMAP_DIAMETER = {
    outdoor = { 466 + 2 / 3, 400, 333 + 1 / 3, 266 + 2 / 3, 200, 133 + 1 / 3 },
    indoor = { 300, 240, 180, 120, 80, 50 },
}
local KIND_COLOR = { c = "|cff69ccf0", r = "|cffffd100", s = "|cff9df27a" }

local TEXTS = {
    en = {
        TITLE = "Bot quests nearby",
        ALL = "All (also race and profession)",
        MAP = "Show on the map",
        REFRESH = "Refresh",
        WAITING = "Asking the server...",
        NONE = "No quests near you that only one of your bots can take.",
        NOBOTS = "No bot of yours in the group.",
        KIND_c = "class quest",
        KIND_r = "race quest",
        KIND_s = "profession quest",
        FOR = "for %s",
        GIVER = "at %s",
        DISTANCE = "%d yd",
        ACCEPT = "Accept",
        TAKEN = "%s accepted %s.",
        NOT_TAKEN = "%s could not accept %s.",
    },
    fr = {
        TITLE = "Quêtes des bots à proximité",
        ALL = "Toutes (aussi race et métier)",
        MAP = "Afficher sur la carte",
        REFRESH = "Actualiser",
        WAITING = "Demande au serveur...",
        NONE = "Aucune quête proche que seul un de vos bots peut prendre.",
        NOBOTS = "Aucun de vos bots dans le groupe.",
        KIND_c = "quête de classe",
        KIND_r = "quête de race",
        KIND_s = "quête de métier",
        FOR = "pour %s",
        GIVER = "auprès de %s",
        DISTANCE = "%d m",
        ACCEPT = "Accepter",
        TAKEN = "%s a accepté %s.",
        NOT_TAKEN = "%s n'a pas pu accepter %s.",
    },
}
local DIRECTIONS = {
    en = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" },
    fr = { "N", "NE", "E", "SE", "S", "SO", "O", "NO" },
}

local function Language()
    if CoACompanionBots_Language then return CoACompanionBots_Language() end
    return (GetLocale() == "frFR") and "fr" or "en"
end

local function T(key)
    local texts = TEXTS[Language()] or TEXTS.en
    return texts[key] or TEXTS.en[key] or key
end

local offers, pending, zones = {}, nil, {}
local scanning, hasBots, scanStarted = false, false, 0

local function Settings()
    SquidBotsLiteDB = SquidBotsLiteDB or {}
    if SquidBotsLiteDB.botQuestsMap == nil then SquidBotsLiteDB.botQuestsMap = true end
    return SquidBotsLiteDB
end

local function Split(text)
    local parts = {}
    for part in string.gmatch(text .. "\t", "([^\t]*)\t") do table.insert(parts, part) end
    return parts
end

local function BotsInGroup()
    local count = GetNumPartyMembers() + GetNumRaidMembers()
    return count > 0
end

local function RequestScan()
    if not BotsInGroup() then
        offers, hasBots = {}, false
        return false
    end
    scanning, scanStarted = true, GetTime()
    SendAddonMessage(PREFIX, "SCAN\t" .. (Settings().botQuestsAll and "1" or "0"), "WHISPER", UnitName("player"))
    return true
end

-- The player's own position in the zone's percent, read without moving an open world map.
local function PlayerZonePosition()
    if WorldMapFrame and WorldMapFrame:IsShown() then
        local x, y = GetPlayerMapPosition("player")
        local continent, zone = GetCurrentMapContinent(), GetCurrentMapZone()
        if zone and zone > 0 and x and x > 0 and select(zone, GetMapZones(continent)) == GetRealZoneText() then
            return x * 100, y * 100
        end
        return nil
    end
    SetMapToCurrentZone()
    local x, y = GetPlayerMapPosition("player")
    if not x or (x == 0 and y == 0) then return nil end
    return x * 100, y * 100
end

local function DirectionTo(offer)
    if offer.zone ~= GetRealZoneText() or offer.x < 0 then return nil end
    local size = zones[offer.zone]
    local px, py = PlayerZonePosition()
    if not size or not px then return nil end
    local dx = (offer.x - px) / 100 * size.width
    local dy = (offer.y - py) / 100 * size.height
    if dx * dx + dy * dy < 4 then return nil end
    local angle = math.deg(math.atan2(dx, -dy)) % 360
    return DIRECTIONS[Language()][math.floor((angle + 22.5) / 45) % 8 + 1]
end

-- Window -------------------------------------------------------------------------------------------------------------

local frame = CreateFrame("Frame", "CoACompanionBotQuestsFrame", UIParent)
frame:SetWidth(460)
frame:SetHeight(110 + VISIBLE_ROWS * ROW_HEIGHT)
frame:SetPoint("CENTER")
frame:SetFrameStrata("MEDIUM")
frame:SetBackdrop({
    bgFile = "Interface\\DialogFrame\\UI-DialogBox-Background",
    edgeFile = "Interface\\DialogFrame\\UI-DialogBox-Border",
    tile = true, tileSize = 32, edgeSize = 24,
    insets = { left = 6, right = 6, top = 6, bottom = 6 },
})
frame:SetMovable(true)
frame:EnableMouse(true)
frame:RegisterForDrag("LeftButton")
frame:SetScript("OnDragStart", frame.StartMoving)
frame:SetScript("OnDragStop", frame.StopMovingOrSizing)
frame:SetClampedToScreen(true)
frame:Hide()
table.insert(UISpecialFrames, frame:GetName())

local title = frame:CreateFontString(nil, "ARTWORK", "GameFontNormalLarge")
title:SetPoint("TOP", 0, -14)

local close = CreateFrame("Button", nil, frame, "UIPanelCloseButton")
close:SetPoint("TOPRIGHT", -4, -4)

local allBox = CreateFrame("CheckButton", "CoACompanionBotQuestsAll", frame, "UICheckButtonTemplate")
allBox:SetPoint("TOPLEFT", 14, -36)
allBox:SetScript("OnClick", function(self)
    Settings().botQuestsAll = self:GetChecked() and true or false
    RequestScan()
end)

local mapBox = CreateFrame("CheckButton", "CoACompanionBotQuestsMap", frame, "UICheckButtonTemplate")
mapBox:SetPoint("TOPLEFT", 14, -60)
mapBox:SetScript("OnClick", function(self)
    Settings().botQuestsMap = self:GetChecked() and true or false
end)

local refresh = CreateFrame("Button", nil, frame, "UIPanelButtonTemplate")
refresh:SetWidth(110)
refresh:SetHeight(22)
refresh:SetPoint("TOPRIGHT", -16, -40)
refresh:SetScript("OnClick", function() RequestScan() end)

local status = frame:CreateFontString(nil, "ARTWORK", "GameFontHighlightSmall")
status:SetPoint("BOTTOMLEFT", 16, 14)
status:SetPoint("BOTTOMRIGHT", -16, 14)
status:SetJustifyH("LEFT")

local scroll = CreateFrame("ScrollFrame", "CoACompanionBotQuestsScroll", frame, "FauxScrollFrameTemplate")
scroll:SetPoint("TOPLEFT", 14, -88)
scroll:SetPoint("BOTTOMRIGHT", -34, 32)

local rows = {}
for i = 1, VISIBLE_ROWS do
    local row = CreateFrame("Button", nil, frame)
    row:SetHeight(ROW_HEIGHT)
    row:SetPoint("TOPLEFT", scroll, "TOPLEFT", 0, -(i - 1) * ROW_HEIGHT)
    row:SetPoint("RIGHT", scroll, "RIGHT", 0, 0)
    row.accept = CreateFrame("Button", nil, row, "UIPanelButtonTemplate")
    row.accept:SetWidth(70)
    row.accept:SetHeight(ROW_HEIGHT)
    row.accept:SetPoint("RIGHT", row, "RIGHT", 0, 0)
    row.accept:SetScript("OnClick", function(self)
        local offer = self:GetParent().offer
        if not offer then return end
        self:Disable()
        SendAddonMessage(PREFIX, "ACCEPT\t" .. offer.bot .. "\t" .. offer.quest, "WHISPER", UnitName("player"))
    end)
    row.text = row:CreateFontString(nil, "ARTWORK", "GameFontHighlightSmall")
    row.text:SetPoint("TOPLEFT")
    row.text:SetPoint("BOTTOMRIGHT", row.accept, "BOTTOMLEFT", -4, 0)
    row.text:SetJustifyH("LEFT")
    row:SetHighlightTexture("Interface\\QuestFrame\\UI-QuestTitleHighlight", "ADD")
    row:SetScript("OnEnter", function(self)
        local offer = self.offer
        if not offer then return end
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        GameTooltip:AddLine(offer.title, 1, 0.82, 0)
        GameTooltip:AddLine(string.format(T("FOR"), offer.bot) .. " - " .. T("KIND_" .. offer.kind), 1, 1, 1)
        GameTooltip:AddLine(string.format(T("GIVER"), offer.giver) .. ", " .. offer.zone, 0.8, 0.8, 0.8)
        GameTooltip:Show()
    end)
    row:SetScript("OnLeave", function() GameTooltip:Hide() end)
    rows[i] = row
end

-- One line per bot heading, then its quests.
local function Lines()
    local lines, lastBot = {}, nil
    for _, offer in ipairs(offers) do
        if offer.bot ~= lastBot then
            table.insert(lines, { heading = offer.bot })
            lastBot = offer.bot
        end
        table.insert(lines, { offer = offer })
    end
    return lines
end

local function RenderList()
    if not frame:IsShown() then return end
    local lines = Lines()
    FauxScrollFrame_Update(scroll, #lines, VISIBLE_ROWS, ROW_HEIGHT)
    local offset = FauxScrollFrame_GetOffset(scroll)
    for i = 1, VISIBLE_ROWS do
        local row, line = rows[i], lines[offset + i]
        row.offer = line and line.offer or nil
        if not line then
            row:Hide()
        elseif line.heading then
            row.accept:Hide()
            row.text:SetText("|cffffd100" .. line.heading .. "|r")
            row:Show()
        else
            local offer = line.offer
            local where = string.format(T("DISTANCE"), offer.distance)
            local direction = DirectionTo(offer)
            if direction then where = where .. " " .. direction end
            row.text:SetText(string.format("   %s[%d]|r %s  |cffaaaaaa- %s, %s|r",
                KIND_COLOR[offer.kind] or "", offer.level, offer.title, offer.giver, where))
            row.accept:SetText(T("ACCEPT"))
            row.accept:Enable()
            row.accept:Show()
            row:Show()
        end
    end
    if scanning then
        status:SetText(T("WAITING"))
    elseif not hasBots then
        status:SetText(T("NOBOTS"))
    elseif #offers == 0 then
        status:SetText(T("NONE"))
    else
        status:SetText("")
    end
end

scroll:SetScript("OnVerticalScroll", function(self, offset)
    FauxScrollFrame_OnVerticalScroll(self, offset, ROW_HEIGHT, RenderList)
end)

local function ApplyTexts()
    title:SetText(T("TITLE"))
    _G[allBox:GetName() .. "Text"]:SetText(T("ALL"))
    _G[mapBox:GetName() .. "Text"]:SetText(T("MAP"))
    refresh:SetText(T("REFRESH"))
end

frame:SetScript("OnShow", function()
    ApplyTexts()
    allBox:SetChecked(Settings().botQuestsAll)
    mapBox:SetChecked(Settings().botQuestsMap)
    RequestScan()
    RenderList()
end)

-- Pins ---------------------------------------------------------------------------------------------------------------

local worldPins, minimapPins = {}, {}

local function PinTooltip(self)
    local offer = self.offer
    GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
    GameTooltip:AddLine(offer.title, 1, 0.82, 0)
    GameTooltip:AddLine(string.format(T("FOR"), offer.bot) .. " - " .. T("KIND_" .. offer.kind), 1, 1, 1)
    GameTooltip:AddLine(string.format(T("GIVER"), offer.giver), 0.8, 0.8, 0.8)
    GameTooltip:Show()
end

local function Pin(pool, index, parent, size)
    local pin = pool[index]
    if not pin then
        pin = CreateFrame("Frame", nil, parent)
        pin:SetWidth(size)
        pin:SetHeight(size)
        pin.icon = pin:CreateTexture(nil, "OVERLAY")
        pin.icon:SetAllPoints()
        pin.icon:SetTexture("Interface\\GossipFrame\\AvailableQuestIcon")
        pin:EnableMouse(true)
        pin:SetScript("OnEnter", PinTooltip)
        pin:SetScript("OnLeave", function() GameTooltip:Hide() end)
        pool[index] = pin
    end
    pin:SetFrameLevel(parent:GetFrameLevel() + 5)
    return pin
end

local function HidePins(pool, from)
    for i = from, #pool do pool[i]:Hide() end
end

local function ShownMapZone()
    local continent, zone = GetCurrentMapContinent(), GetCurrentMapZone()
    if not zone or zone < 1 then return nil end
    return select(zone, GetMapZones(continent))
end

local function UpdateWorldPins()
    local used = 0
    if Settings().botQuestsMap and WorldMapFrame:IsShown() then
        local zone = ShownMapZone()
        local width, height = WorldMapButton:GetWidth(), WorldMapButton:GetHeight()
        for _, offer in ipairs(offers) do
            if offer.x >= 0 and offer.zone == zone then
                used = used + 1
                local pin = Pin(worldPins, used, WorldMapButton, 16)
                pin.offer = offer
                pin:ClearAllPoints()
                pin:SetPoint("CENTER", WorldMapButton, "TOPLEFT", offer.x / 100 * width, -offer.y / 100 * height)
                pin:Show()
            end
        end
    end
    HidePins(worldPins, used + 1)
end

local function UpdateMinimapPins()
    local used = 0
    if Settings().botQuestsMap and #offers > 0 and not (WorldMapFrame and WorldMapFrame:IsShown()) then
        local zone = GetRealZoneText()
        local size = zones[zone]
        local px, py = PlayerZonePosition()
        if size and px then
            local sizes = IsIndoors() and MINIMAP_DIAMETER.indoor or MINIMAP_DIAMETER.outdoor
            local radius = (sizes[Minimap:GetZoom() + 1] or sizes[1]) / 2
            local half = Minimap:GetWidth() / 2
            local facing = GetCVar("rotateMinimap") == "1" and GetPlayerFacing() or 0
            local cosine, sine = math.cos(facing), math.sin(facing)
            for _, offer in ipairs(offers) do
                if offer.x >= 0 and offer.zone == zone then
                    local dx = (offer.x - px) / 100 * size.width
                    local dy = (offer.y - py) / 100 * size.height
                    local rx, ry = dx * cosine - dy * sine, dx * sine + dy * cosine
                    if rx * rx + ry * ry <= radius * radius then
                        used = used + 1
                        local pin = Pin(minimapPins, used, Minimap, 12)
                        pin.offer = offer
                        pin:ClearAllPoints()
                        pin:SetPoint("CENTER", Minimap, "CENTER", rx / radius * half, -ry / radius * half)
                        pin:Show()
                    end
                end
            end
        end
    end
    HidePins(minimapPins, used + 1)
end

-- Events -------------------------------------------------------------------------------------------------------------

local function OnAddonMessage(message)
    local parts = Split(message)
    local kind = parts[1]
    if kind == "BEGIN" then
        pending, zones = {}, {}
    elseif kind == "ZONE" and pending then
        zones[parts[2]] = { width = tonumber(parts[3]) or 0, height = tonumber(parts[4]) or 0 }
    elseif kind == "Q" and pending then
        table.insert(pending, {
            bot = parts[2], quest = tonumber(parts[3]), level = tonumber(parts[4]) or 0, kind = parts[5],
            distance = tonumber(parts[6]) or 0, zone = parts[7], x = tonumber(parts[8]) or -1,
            y = tonumber(parts[9]) or -1, giver = parts[10], title = parts[11],
        })
    elseif kind == "TAKEN" then
        local quest = tonumber(parts[3])
        local name = tostring(quest)
        for _, offer in ipairs(offers) do
            if offer.quest == quest then
                name = offer.title
                break
            end
        end
        local text = parts[4] == "1" and T("TAKEN") or T("NOT_TAKEN")
        DEFAULT_CHAT_FRAME:AddMessage("|cff69ccf0CoA Companion:|r " .. string.format(text, parts[2], name))
        RequestScan()
        RenderList()
    elseif kind == "END" then
        offers, pending, scanning, hasBots = pending or {}, nil, false, true
        RenderList()
        UpdateWorldPins()
    end
end

local events = CreateFrame("Frame")
events:RegisterEvent("CHAT_MSG_ADDON")
events:RegisterEvent("WORLD_MAP_UPDATE")
events:RegisterEvent("ZONE_CHANGED_NEW_AREA")
events:RegisterEvent("PARTY_MEMBERS_CHANGED")
events:SetScript("OnEvent", function(_, event, prefix, message)
    if event == "CHAT_MSG_ADDON" then
        if prefix == PREFIX and message and string.sub(message, 1, 5) ~= "SCAN\t"
            and string.sub(message, 1, 7) ~= "ACCEPT\t" then
            OnAddonMessage(message)
        end
    elseif event == "WORLD_MAP_UPDATE" then
        UpdateWorldPins()
    elseif Settings().botQuestsMap or frame:IsShown() then
        RequestScan()
        RenderList()
    end
end)

local sinceScan, sincePins = 0, 0
events:SetScript("OnUpdate", function(_, elapsed)
    sincePins = sincePins + elapsed
    if sincePins >= PIN_UPDATE_SECONDS then
        sincePins = 0
        UpdateMinimapPins()
    end
    if scanning and GetTime() - scanStarted > SCAN_TIMEOUT_SECONDS then
        scanning, pending = false, nil
    end
    sinceScan = sinceScan + elapsed
    if sinceScan >= REFRESH_SECONDS then
        sinceScan = 0
        if (Settings().botQuestsMap or frame:IsShown()) and not scanning then RequestScan() end
        RenderList()
    end
end)

function CoACompanionBots_ToggleBotQuests()
    if frame:IsShown() then frame:Hide() else frame:Show() end
end
