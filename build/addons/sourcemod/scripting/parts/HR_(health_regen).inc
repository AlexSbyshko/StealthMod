Handle HealthRegenTimers[MAXPLAYERS+1]

public HR_Init()
{
	for (int i = 1; i <= MaxClients; i++)
	{
		HealthRegenTimers[i] = INVALID_HANDLE
	}
}

public HR_Event_MatchEndRestart(Handle:event, const String:name[], bool:dontBroadcast)
{	
	for (int i = 1; i <= MaxClients; i++)
	{
		StopClientHPRegen(i)
	}
}

public HR_Event_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{	
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	new team = GetClientTeam(client)
	if (team != CS_TEAM_NONE && IsPlayerAlive(client))
	{
		StopClientHPRegen(client)
		float healthRegenPeriod = STMPlayers.Get(client).GetHealthRegenPeriod()
		if (healthRegenPeriod > 0)
		{
			HealthRegenTimers[client] = CreateTimer(healthRegenPeriod, Timer_RegenHealth, client, TIMER_REPEAT)
		}	
	}
}

public HR_Event_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	StopClientHPRegen(client)
}

public Action:Timer_RegenHealth(Handle:timer, any:client)
{	
	if (IsClientInGame(client) && IsPlayerAlive(client))
	{
		int health = GetEntProp(client, Prop_Data, "m_iHealth")
		int maxHealth = STMPlayers.Get(client).GetMaxHealth()
		int newHealth = health + STMPlayers.Get(client).GetHealthRegenAmount()
		if (newHealth > maxHealth)
		{
			newHealth = maxHealth
		}
		SetEntProp(client, Prop_Data, "m_iHealth", newHealth)
	}
	return Plugin_Continue
}

StopClientHPRegen(client)
{
	if (HealthRegenTimers[client] != INVALID_HANDLE)
	{
		KillTimer(HealthRegenTimers[client])
		HealthRegenTimers[client] = INVALID_HANDLE
	}
}