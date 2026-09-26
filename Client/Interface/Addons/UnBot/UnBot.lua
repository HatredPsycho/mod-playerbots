
-- Replaces YssBossLoot:QueryItemInfo: makes an unknown item known to the client,
-- one request every half second so the connection is not dropped.
local UnBotItemQueue = {};
local UnBotItemTimer = 0;
local UnBotItemScanner = CreateFrame("GameTooltip", "UnBotItemScannerTooltip", nil, "GameTooltipTemplate");
local UnBotItemQueueFrame = CreateFrame("Frame");
UnBotItemQueueFrame:SetScript("OnUpdate", function(self, elapsed)
	UnBotItemTimer = UnBotItemTimer + elapsed;
	if (UnBotItemTimer < 0.5 or #UnBotItemQueue == 0) then
		return;
	end
	UnBotItemTimer = 0;
	local itemID = table.remove(UnBotItemQueue, 1);
	if (not GetItemInfo(itemID)) then
		UnBotItemScanner:SetOwner(UIParent, "ANCHOR_NONE");
		UnBotItemScanner:SetHyperlink("item:"..tostring(itemID)..":0:0:0:0:0:0:0");
	end
end);

function UnBotQueryItemInfo(itemID)
	if (itemID == nil or GetItemInfo(itemID)) then
		return;
	end
	for i=1, #UnBotItemQueue do
		if (UnBotItemQueue[i] == itemID) then
			return;
		end
	end
	table.insert(UnBotItemQueue, itemID);
end

function UnBotSplit(str, split_char)
    local sub_str_tab = {};
    while (true) do
        local pos = string.find(str, split_char);
        if (not pos) then
            sub_str_tab[#sub_str_tab + 1] = str;
            break;
        end
        local sub_str = string.sub(str, 1, pos - 1);
        sub_str_tab[#sub_str_tab + 1] = sub_str;
        str = string.sub(str, pos + 1, #str);
    end

    return sub_str_tab;
end

function UnBotCloseAll()
	UnBotHideAllSubFrame();
	UnBotCloseAllBagsFrame();
	UnBotClearAllQRFrame();
	UnBotClearAllStrategyFrame();
	OnlineFrame:Hide();
	NPCFrame:Hide();
	UnBotFrame:Hide();
	DisplayInfomation("Bot bar closed. Type /unbot in the chat to open it again.");
end

local function AddButton(name,fromParent,temp,gi,ci)
	if (fromParent == nil) then
		return nil;
	end
	if (UnBotIconFiles[ci] == nil) then
		return nil;
	end
	local newFrame = CreateFrame("Button",name..tostring(ci),fromParent.lastButton,temp);
	newFrame.groupIndex = gi;
	newFrame.comIndex = ci;
	newFrame.Icon = newFrame:CreateTexture("UnBotSubIcon"..tostring(ci),"BACKGROUND");
	newFrame.Icon:SetTexture(GetIconPathByIndex(UnBotIconFiles[ci]));
	newFrame.Icon:SetAllPoints(newFrame);
	newFrame.Icon:Show();
	newFrame:Show();
	newFrame:SetPushedTexture([[Interface\BUTTONS\UI-Quickslot-Depress]]);
	newFrame:SetHighlightTexture([[Interface\Buttons\UI-Common-MouseHilight]],"ADD");
	fromParent.lastButton = newFrame;
	if(fromParent.nextSubButtonIndex == 1) then
		UpdateGroupButtonActive(gi,true);
		if(UnBotCommandBarConfig[ci] == nil) then
			local groupButton = _G["UnBotCommandButton"..tostring(newFrame.groupIndex)];
			if (groupButton ~= nil) then
				ResetCommandToAction(newFrame,groupButton.comIndex,false);
			else
				ResetCommandToAction(newFrame,ci,false);
			end
		end
	end
	fromParent.nextSubButtonIndex = fromParent.nextSubButtonIndex + 1;
	return newFrame;
end

function InspectFrame_Show(unit)
	if(InspectFrame) then
		DoInspectFrameShow(unit);
	else
		if( IsAddOnLoaded("Blizzard_InspectUI") == nil) then
			local loaded, reason = LoadAddOn("Blizzard_InspectUI");
			if( loaded == nil) then
				DisplayInfomation("Window initialization failed: "..reason);
			else
				DoInspectFrameShow(unit);
			end
		end
	end
end

function DoInspectFrameShow(unit)
	if(InspectFrame) then
		HideUIPanel(InspectFrame);
		if ( CanInspect(unit, true) ) then
			NotifyInspect(unit);
			InspectFrame.unit = unit;
			InspectSwitchTabs(1);
			ShowUIPanel(InspectFrame);
			InspectFrame_UpdateTalentTab();
			DisplayInfomation("InspectFrame_Show "..UnitName(unit));
		end	
	end
end



function InitializeUnBotFrame()
	DisplayInfomation("Initializing the bot controller...");
	if(UnBotFrame.Inited == true) then
		return;
	end
	UnBotFrame.Inited = true;
	
	if (UnBotCommandBarConfig == nil) then
		UnBotCommandBarConfig = {};
	end
	for i=1, UNBOT_GROUP_COUNT do
		UpdateGroupButtonActive(i,false);
	end
	for i=1, #UnBotCommandToGroups do
		if (UnBotExecuteCommand[i] ~= nil and UnBotExecuteCommand[i] ~= "") then
			local toGroup = UnBotCommandToGroups[i];
			local newFrame = AddButton("UnBotSubCommandButton",_G["UnBotSubFrame"..tostring(toGroup)],"UnBotCommandSubButtonTemplate",toGroup,i);
		end
	end

	for i=1, UNBOT_GROUP_COUNT do
		local groupButton = _G["UnBotCommandButton"..tostring(i)];
		ResetCommandToAction(groupButton, UnBotCommandBarConfig[i],false);
	end
	UnBotUpdateHotkeys();

	InitializeStrategy();
	DisplayInfomation("Bot controller ready. Type /unbot to show or hide the bar.");
end

local function GetCommandTypeTextByType(typeIndex)
	if (typeIndex == 4) then
		return "Whole group";
	elseif (typeIndex == 3) then
		return "Hostile target required";
	elseif (typeIndex == 2) then
		return "Friendly target required";
	else
		return "No target required";
	end
end

function CommandButton_OnEnter(self,index,btnType)
	GameTooltip:SetOwner(self, "ANCHOR_TOPRIGHT");
	GameTooltip:AddLine(UnBotTooltipTitle[index],1,0,0,1);
	GameTooltip:AddDoubleLine("Target: ",GetCommandTypeTextByType(UnBotCommandType[index]),0,0,1,1,0,1);
	GameTooltip:AddLine(UnBotTooltipHelp[index],0,1,0,1);
	GameTooltip:AddLine(" ",1,1,1,1);
	if (self.groupIndex > 0) then
		GameTooltip:AddDoubleLine("Command: ",UnBotExecuteCommand[index],0,0.85,0.85,0,0.85,0.85);
	end
	GameTooltip:AddLine("Left click: run the command",0.65,0.55,0,1);
	if (self.groupIndex > 0) then
		if (btnType == 1) then
			GameTooltip:AddLine("Right click: open or close the button group",0.65,0.55,0,1);
		elseif (btnType == 2) then
			GameTooltip:AddLine("Right click: put it on the quick button",0.65,0.55,0,1);
		end
	else
		GameTooltip:AddLine("Right click: close the main bar",0.65,0.55,0,1);
	end
	GameTooltip:AddDoubleLine("Command no.: ",tostring(index),0,0,1,1,0,1);
	GameTooltip:AddTexture(GetIconPathByIndex(UnBotIconFiles[index]));
	GameTooltip:Show();
end

function UnBotHideAllSubFrame()
	for i=0, UNBOT_GROUP_COUNT do
		local frameObject = _G["UnBotSubFrame"..tostring(i)];
		if (frameObject ~= nil) then
			frameObject:Hide();
		end
		local groupButton = _G["UnBotCommandButton"..tostring(i)];
		if (groupButton ~= nil) then
			groupButton.showMenu = false;
		end
	end
end

function UnBotClearAllQRFrame()
	for i=1, #(UnBotFrame.ShowedQRs) do
		local qr = UnBotFrame.ShowedQRs[i];
		qr.showTick = 0;
		qr:Hide();
		qr:SetParent(nil);
	end
	UnBotFrame.ShowedQRs = {};
end

function UnBotClearAllStrategyFrame()
	for i=1, #(UnBotFrame.ShowedStrategy) do
		local ss = UnBotFrame.ShowedStrategy[i];
		ss:Hide();
		ss:SetParent(nil);
	end
	UnBotFrame.ShowedStrategy = {};
end

function UnBotRemoveByQRFrame(targetQR)
	for i=1, #(UnBotFrame.ShowedQRs) do
		if (UnBotFrame.ShowedQRs[i]:GetName() == targetQR:GetName()) then
			table.remove(UnBotFrame.ShowedQRs,i);
			return;
		end
	end
end

function UpdateGroupButtonActive(index,show)
	local groupButton = _G["UnBotCommandButton"..tostring(index)];
	if (groupButton == nil) then
		return;
	end
	if (show == true) then
		groupButton:Show();
	else
		groupButton:Hide();
	end
end

function ResetCommandToAction(self,index,save)
	local groupButton = _G["UnBotCommandButton"..tostring(self.groupIndex)];
	if (groupButton == nil or index == nil) then
		return;
	end
	if (UnBotExecuteCommand[index] == nil or UnBotExecuteCommand[index] == "") then
		return;
	end
	groupButton.Icon:SetTexture(GetIconPathByIndex(UnBotIconFiles[index]));
	groupButton.comIndex = index;
	groupButton.Icon:Show();
	--DisplayInfomation("Command "..tostring(index).." applied to group button "..tostring(self.groupIndex));

	if (save == true) then
		UnBotCommandBarConfig[self.groupIndex] = index;
	end

	local subFrame = _G["UnBotSubFrame"..tostring(self.groupIndex)];
	groupButton.showMenu = false;
	if (subFrame == nil) then
		return;
	end
	subFrame:Hide();
end

function GetIconPathByIndex(index)
	local iconPath = UnBotCommandIconsPath[index];
	return iconPath;
end

function UnBotUpdateHotkeys()
	for i=1, UNBOT_GROUP_COUNT do
		local groupButtonLabel = _G["UnBotCommandButton"..tostring(i).."Label"];
		if (groupButtonLabel ~= nil) then
			local bindingKey = GetBindingKey("CLICK UnBotCommandButton"..tostring(i)..":LeftButton");
			if (bindingKey ~= nil) then
				local keyText = GetBindingText(bindingKey, "KEY_", 1);
				groupButtonLabel:SetText(keyText);
			else
				groupButtonLabel:SetText("");
			end
		end
	end
	local groupFuncLabel = _G["UnBotCommandButtonFuncLabel"];
	if (groupFuncLabel ~= nil) then
		local bindingFuncKey = GetBindingKey("CLICK UnBotCommandButtonFunc:LeftButton");
		if (bindingFuncKey ~= nil) then
			local keyText = GetBindingText(bindingFuncKey, "KEY_", 1);
			groupFuncLabel:SetText(keyText);
		else
			groupFuncLabel:SetText("");
		end
	end
end

function DisplayInfomation(info)
	DEFAULT_CHAT_FRAME:AddMessage("|cff00cccc"..info.."|r");--ChatFrame1
end

function CanAddToUnBotFrame(targetName)
	for i=1, #(UnBotFrame.ShowedBags) do
		if (UnBotFrame.ShowedBags[i]:GetName() == targetName) then
			return false;
		end
	end
	return true;
end

function AddToUnBotFrame(bagsFrame, targetName)
	for i=1, #(UnBotFrame.ShowedBags) do
		if (UnBotFrame.ShowedBags[i]:GetName() == targetName) then
			return false;
		end
	end
	table.insert(UnBotFrame.ShowedBags, bagsFrame);
	return true;
end

function RemoveFromUnBotFrame(bagsFrame)
	for i=1, #(UnBotFrame.ShowedBags) do
		if (UnBotFrame.ShowedBags[i]:GetName() == bagsFrame:GetName()) then
			table.remove(UnBotFrame.ShowedBags,i);
			return;
		end
	end
end

function AddToUnBotStrategyFrame(strategyFrame, targetName)
	for i=1, #(UnBotFrame.ShowedStrategy) do
		if (UnBotFrame.ShowedStrategy[i]:GetName() == targetName) then
			return false;
		end
	end
	table.insert(UnBotFrame.ShowedStrategy, strategyFrame);
	return true;
end

function RemoveFromStrategyFrame(strategyFrame)
	for i=1, #(UnBotFrame.ShowedStrategy) do
		if (UnBotFrame.ShowedStrategy[i]:GetName() == strategyFrame:GetName()) then
			table.remove(UnBotFrame.ShowedStrategy,i);
			return;
		end
	end
end

function UnBotEnableAllFrameFlushButton()
	for i=1, #(UnBotFrame.ShowedBags) do
		local flushButton = _G["BagsFrameFlush"..(UnBotFrame.ShowedBags[i]:GetName())]
		if (flushButton ~= nil) then
			flushButton:Enable();
		end
	end
end
function UnBotDisableAllFrameFlushButton()
	for i=1, #(UnBotFrame.ShowedBags) do
		local flushButton = _G["BagsFrameFlush"..(UnBotFrame.ShowedBags[i]:GetName())]
		if (flushButton ~= nil) then
			flushButton:Disable();
		end
	end
end

function CaseNormalWhisperMsg(name, info)
	local i1, i2 = string.find(info, "Choose reward");
	if (i1 ~= nil and i2 ~= nil) then
		RecvQuestReward(name, info);
	end
end

function SubCommandButton_OnLeftClick(index)
	if (index > 0) then
		if (UnBotExecuteCommand[index] == nil or UnBotExecuteCommand[index] == "") then
			return;
		end
	end
	if (UnBotCommandType[index] == nil) then
		DisplayInfomation("Command not found: no. "..tostring(index)..".");
		return;
	end
	local realize = getglobal("UnBotCommandRealize");
	if (realize ~= nil and realize[index] ~= nil) then
		realize[index](index);
	else
		if (UnBotCommandType[index] == 1) then
			SendChatMessage(UnBotExecuteCommand[index], "SAY");
		elseif (UnBotCommandType[index] == 2) then
			local targetName = UnitName("target");
			local isParty = UnitInParty("target");
			local isRaid = UnitInRaid("target");
			if (targetName == nil or targetName == "") then
				DisplayInfomation("You have no target selected.");
				return;
			end
			if (isParty == nil and isRaid == nil) then
				DisplayInfomation("The target is not in your group.");
				return;
			end
			-- if (not IsRealPartyLeader()) then
			-- 	DisplayInfomation("You are not the group leader.");
			-- else
			SendChatMessage(UnBotExecuteCommand[index], "WHISPER", nil, targetName);
			-- end
		elseif (UnBotCommandType[index] == 3) then
			local targetName = UnitName("target");
			if (targetName == nil or targetName == "") then
				DisplayInfomation("You have no target selected.");
				return;
			end
			-- if (not IsRealPartyLeader()) then
			-- 	DisplayInfomation("You are not the group leader.");
			-- else
			SendChatMessage(UnBotExecuteCommand[index], "PARTY");
			-- end
		elseif (UnBotCommandType[index] == 4) then
			-- if (not IsRealPartyLeader()) then
			-- 	DisplayInfomation("You are not the group leader.");
			-- else
			SendChatMessage(UnBotExecuteCommand[index], "PARTY");
			-- end
		else
			DisplayInfomation("You click button index "..tostring(index)..", execute "..UnBotExecuteCommand[index]);
		end
	end
end

function UnBotFrameUpdate(ubFrame, tick)
	if (ubFrame.DelayInitTime > 0 and tick > ubFrame.DelayInitTime) then
		ubFrame.DelayInitTime = 0;
		UnBotInitInspectFrame();
	end
end
