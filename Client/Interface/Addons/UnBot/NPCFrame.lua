
function NPCCommand_OnEnter(self,tipType,title,text,command)
	GameTooltip:SetOwner(self, "ANCHOR_TOPRIGHT");
	GameTooltip:AddLine(title,0,0.7,0.7,1);
	if (tipType == 1) then
		GameTooltip:AddLine("Creates a bot of class "..text,0,1,0,1);
		-- GameTooltip:AddLine("Out of combat, right click the bot to open its menu (role and gear).",0,1,0,1);
		-- GameTooltip:AddLine("Target yourself before using the command.",1,0,0,1);
	elseif (tipType == 2) then
		GameTooltip:AddLine(text,0,1,0,1);
		GameTooltip:AddLine("Target yourself, or an NPC bot, before using the command.",1,0,0,1);
	end
	-- GameTooltip:AddLine("Commande indisponible en combat.",1,1,1,1);
	-- GameTooltip:AddLine(" ",1,1,1,1);
	if (command ~= nil) then
		GameTooltip:AddDoubleLine("Command: ",command,0,0.85,0.85,0,0.85,0.85);
	end
	GameTooltip:Show();
end
