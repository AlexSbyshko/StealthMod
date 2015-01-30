#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <sdkhooks>
#include <smlib>

#include "parts"
#include "parts/ST_(swap_teams)"

#define PLUGIN_VERSION "1.2.2"
#define PLUGIN_URL "http://steamcommunity.com/groups/stealthmod"

#define DAMAGE_FILTER_NAME "filter_no_weapons_damage"

new bool:IsPlayerSpawned[MAXPLAYERS + 1];
new bool:IsPlayerVisible[MAXPLAYERS + 1];
new RoundCounter;
new PlayerSpottedOffset
new PlayerManagerEntity
new BombSpottedOffset



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

	RegisterPart("ST") // Swap Teams

	InitParts()		
}

public OnMapStart()
{
	RoundCounter = 0;
	AutoExecConfig(true, "stealthmod");
}

public OnClientPutInServer(client) 
{
	SDKHook(client, SDKHook_SetTransmit, Hook_SetTransmit);
	SDKHook(client, SDKHook_OnTakeDamage, Hook_OnTakeDamage);
	if (BombSpottedOffset != -1 && PlayerSpottedOffset != -1)
	{
		SDKHook(client, SDKHook_PreThink, Hook_PreThink);
	}	
	CreateTimer(5.0, Timer_PrintModInfo, GetClientUserId(client));
	IsPlayerSpawned[client] = false;
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

public Action:Hook_SetTransmit(entity, client) 
{
	if (client != entity
		&& IsAliveCT(client)
		&& !IsPlayerVisible[entity])
	{
		return Plugin_Handled; 
	}
	return Plugin_Continue; 
}

public Action:Hook_OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
	if (!attacker || attacker == victim || IsClientCanAttack(attacker))
	{
		return Plugin_Continue;
	}
	return Plugin_Handled;
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
				SetClientMoney(client, 0)
			}
			else
			{
				SetClientVisible(client)
				SetEntProp(client, Prop_Send, "m_ArmorValue", 0, 1)
				SetClientMoney(client, 16000)
			}
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

public Action:OnPlayerRunCmd(client, &buttons, &impulse, Float:vel[3], Float:angles[3], &weapon)
{
	static bool:isClientRunning[MAXPLAYERS+1];
	static flags;
	flags = GetEntityFlags(client);
	
	if (!(buttons & IN_SPEED || flags & FL_DUCKING))
	{
		if (buttons & IN_FORWARD || buttons & IN_BACK
			|| buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT)
		{
			if (!isClientRunning[client])
			{
				isClientRunning[client] = true;
				OnClientStartRun(client);			
			}
		}
		else 
		{
			if (isClientRunning[client])
			{
				isClientRunning[client] = false;
				OnClientEndRun(client);
			}
		}
	}
	else
	{
		if (isClientRunning[client])
		{
			isClientRunning[client] = false;
			OnClientEndRun(client);
		}
	}
	return Plugin_Continue;
}

OnClientStartRun(client)
{
	if (IsAliveT(client))
	{
		SetClientVisible(client);
	}
}

OnClientEndRun(client)
{
	if (IsAliveT(client))
	{
		SetClientInvisible(client);
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

SetClientVisible(client)
{
	SetEntityVisible(client);
	SetClientWeaponsVisibility(client, true);
	IsPlayerVisible[client] = true;	
	FadeClient(client, 255, 255, 255, 0) 
}

SetClientInvisible(client)
{
	SetEntityInvisible(client);
	SetClientWeaponsVisibility(client, false);
	IsPlayerVisible[client] = false;
	FadeClient(client, 64, 0, 64, 64) 
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

SetClientWeaponsVisibility(client, bool:visible)
{
	new weapon;
	for (new i = 0; i < CS_SLOT_C4; i++)
	{
		if ((weapon = GetPlayerWeaponSlot(client, i)) != -1)
		{  
			if (IsValidEdict(weapon))
			{
				SetEntityVisibility(weapon, visible);
			}
		}
	}
}

SetEntityVisible(entity)
{
	SetEntityVisibility(entity, true);
}

SetEntityInvisible(entity)
{
	SetEntityVisibility(entity, false);
}

SetEntityVisibility(entity, bool:visible)
{

	SetEntityRenderMode(entity, RENDER_TRANSCOLOR);
	if (visible)
	{		
		SetEntityRenderColor(entity, 255, 255, 255, 255);
	}
	else
	{		
		SetEntityRenderColor(entity, 255, 255, 255, 127);
	}
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