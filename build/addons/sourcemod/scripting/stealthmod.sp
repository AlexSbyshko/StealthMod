#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <sdkhooks>


#define PLUGIN_VERSION "1.2.3"
#define PLUGIN_URL "http://steamcommunity.com/groups/stealthmod"

#define STM_CONFIG_DIRECTORY "cfg/sourcemod/stealthmod"
#define DAMAGE_FILTER_NAME "filter_no_weapons_damage"

new bool:IsPlayerVisible[MAXPLAYERS + 1];
new bool:IsPlayerSpawned[MAXPLAYERS + 1];
new RoundCounter;
new PlayerSpottedOffset
new PlayerManagerEntity
new BombSpottedOffset

#include "parts"
#include "parts/CM_(class_manager)"
#include "parts/HR_(health_regen)"
#include "parts/MH_(max_health)"
#include "parts/PV_(player_visibility)"
#include "parts/RFS_(remove_footsteps)"
#include "parts/ST_(swap_teams)"
#include "parts/TEST_(test)"


public Plugin:myinfo = 
{
	name = "StealthMod",
	author = "Alex Sbyshko",
	description = "CTs vs invisible terrorists",
	version = PLUGIN_VERSION,
	url = PLUGIN_URL
};

public OnPluginStart()
{
	HookEvent("player_spawn", Event_PlayerSpawn)
	HookEvent("player_death", Event_PlayerDeath)
	HookEvent("item_pickup", Event_ItemPickup)
	HookEvent("round_start", Event_RoundStart)
	HookEvent("round_end", Event_RoundEnd)
	BombSpottedOffset = FindSendPropOffs("CCSPlayerResource", "m_bBombSpotted")
	PlayerSpottedOffset = FindSendPropOffs("CCSPlayerResource", "m_bPlayerSpotted")
	
	CreateConVar("stealthmod_version", PLUGIN_VERSION, "StealthMod Version", FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY)

	InitPartSystem()

	RegisterPart("CM") // Remove Footsteps
	RegisterPart("HR") // Health Regen
	RegisterPart("MH") // Max Health
	RegisterPart("RFS") // Remove Footsteps
	RegisterPart("PV") // Player Visibility
	RegisterPart("ST") // Swap Teams
	RegisterPart("TEST") // Test

	InitParts()
}

public OnMapStart()
{
	FireOnMapStart()
	RoundCounter = 0;
	AutoExecConfig(true, "stealthmod");
}

public OnClientPutInServer(client) 
{
	FireOnClientPutInServer(client)
	
	SDKHook(client, SDKHook_OnTakeDamage, Hook_OnTakeDamage)
	if (BombSpottedOffset != -1 && PlayerSpottedOffset != -1)
	{
		SDKHook(client, SDKHook_PreThink, Hook_PreThink)
	}	
	CreateTimer(5.0, Timer_PrintModInfo, GetClientUserId(client))
	IsPlayerSpawned[client] = false

	STMPlayer player = STMPlayers.Get(client)
	player.SetClassByName("baseplayer")
}

public Hook_PreThink(client)
{
	for (new target = 1; target <= MAXPLAYERS; target++)
	{
		SetEntData(PlayerManagerEntity, PlayerSpottedOffset + target, 0, 4, true);
	}
	SetEntData(PlayerManagerEntity, BombSpottedOffset, 0, 4, true);
}

public Action:Timer_PrintModInfo(Handle:timer, any:userId) 
{
	new client = GetClientOfUserId(userId);
	if (client)
	{
		PrintStmMessage(client, "StealthMod v%s", PLUGIN_VERSION);
		PrintStmMessage(client, PLUGIN_URL);
	}
	return Plugin_Continue;
}

public Action:Hook_OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
	if (!attacker || attacker == victim || IsClientCanAttack(attacker))
	{
		return Plugin_Continue;
	}
	return Plugin_Handled
}

public Action:Event_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
	PlayerManagerEntity = FindEntityByClassname(0, "cs_player_manager");
	RoundCounter++;
	SpawnDamageFilter()
	AcceptDamageFilterToHostages()
}

public Action:Event_RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
	return Plugin_Continue
}

public Action:Event_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
	new userId = GetEventInt(event, "userid")
	CreateTimer(0.1, Timer_PostPlayerSpawn, userId)
	return Plugin_Continue
}

public Action:Timer_PostPlayerSpawn(Handle:timer, any:userId) 
{
	new client = GetClientOfUserId(userId);
	if (client)
	{
		if (IsPlayerAlive(client))
		{			
			new team = GetClientTeam(client);
			if (team == CS_TEAM_T)
			{
				StripWeapons(client)
				GivePlayerItem(client, "item_assaultsuit")
				SetClientInvisible(client)
			}
			else
			{
				SetClientVisible(client)
				SetEntProp(client, Prop_Send, "m_ArmorValue", 0, 1)
			}
			SetClientMoney(client, STMPlayers.Get(client).GetStartMoney())
			IsPlayerSpawned[client] = true
		}
	}
	return Plugin_Continue;
}

public Action:CS_OnBuyCommand(client, const String:weapon[])
{
	if (GetClientTeam(client) == CS_TEAM_T)
	{
		return Plugin_Handled
	}
	if (StrEqual(weapon, "vesthelm") || StrEqual(weapon, "vest"))
	{
		return Plugin_Handled
	}
	return Plugin_Continue
}

public Action:Event_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"));
	IsPlayerSpawned[client] = false;
}

public Action:Event_ItemPickup(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"));
	if (GetClientTeam(client) == CS_TEAM_T
		&& IsPlayerSpawned[client])
	{
		DropWeapons(client);
	}
}


////////////////////////////////
// TOOLS ///////////////////////
////////////////////////////////

#define MAX_CHAT_MESSAGE_LENGTH 250
#define FFADE_STAYOUT 0x0008
#define	FFADE_PURGE 0x0010  

SetClientMoney(client, value)
{
	new accountOffset = FindSendPropOffs("CCSPlayer", "m_iAccount");
	if (accountOffset >= 0)
	{
		SetEntData(client, accountOffset, value);
	}
}

FadeClient(client, r, g, b, a) 
{
	new Handle:hFadeClient = StartMessageOne("Fade",client)
	if (GetUserMessageType() == UM_Protobuf)
	{
		new color[4]
		color[0] = r
		color[1] = g
		color[2] = b
		color[3] = a
		PbSetInt(hFadeClient, "duration", 0)
		PbSetInt(hFadeClient, "hold_time", 0)
		PbSetInt(hFadeClient, "flags", (FFADE_PURGE|FFADE_STAYOUT))
		PbSetColor(hFadeClient, "clr", color)
	}
	else
	{		
		BfWriteShort(hFadeClient, 0)
		BfWriteShort(hFadeClient, 0)
		BfWriteShort(hFadeClient, (FFADE_PURGE|FFADE_STAYOUT))
		BfWriteByte(hFadeClient, r)
		BfWriteByte(hFadeClient, g)
		BfWriteByte(hFadeClient, b)
		BfWriteByte(hFadeClient, a)		
	}
	EndMessage()
}



StripWeapons(client)
{
	new weapon;
	for (new i = 0; i < CS_SLOT_C4; i++)
	{
		if ((weapon = GetPlayerWeaponSlot(client, i)) != -1)
		{  
			RemovePlayerItem(client, weapon);
			if (IsValidEdict(weapon))
			{
				RemoveEdict(weapon);
			}
		}
	}
	GivePlayerItem(client, "weapon_knife");
}

DropWeapons(client)
{
	new weapon;
	for (new i = 0; i < CS_SLOT_C4 && i != 2; i++)
	{
		if ((weapon = GetPlayerWeaponSlot(client, i)) != -1)
		{  
			if (IsValidEdict(weapon))
			{
				CS_DropWeapon(client, weapon, true);
			}
		}
	}
}

bool:IsAliveT(client)
{
	return IsPlayerAlive(client) 
		&& GetClientTeam(client) == CS_TEAM_T;
}

bool:IsAliveCT(client)
{
	return IsPlayerAlive(client) 
		&& GetClientTeam(client) == CS_TEAM_CT;
}

PrintStmMessage(client, String:text[], any:...)
{
	decl String:message[MAX_CHAT_MESSAGE_LENGTH];
	VFormat(message, sizeof(message), text, 3);
	PrintToChat(client, "\x04[\x03StM\x04] \x03%s", message);
}

bool:IsClientCanAttack(client)
{
	if ((client > 0 && client <= MaxClients)
		&& IsClientInGame(client)
		&& (IsPlayerVisible[client]))
	{
		return true;
	}
	return false;
}

SpawnDamageFilter()
{
	new damageFilterEntity = CreateEntityByName("filter_damage_type")
	DispatchKeyValue(damageFilterEntity, "targetname", DAMAGE_FILTER_NAME)
	DispatchKeyValue(damageFilterEntity, "negated", "1")
	DispatchKeyValue(damageFilterEntity, "damagetype", "4098")
	DispatchSpawn(damageFilterEntity)
	ActivateEntity(damageFilterEntity)
}

AcceptDamageFilterToHostages()
{
	new hostageEntity = -1;
	while ((hostageEntity = FindEntityByClassname(hostageEntity, "hostage_entity")) != INVALID_ENT_REFERENCE) 
	{
	    SetVariantString(DAMAGE_FILTER_NAME);
	    AcceptEntityInput(hostageEntity, "SetDamageFilter");
	}
}