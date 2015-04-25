public DA_Init()
{
}

public DA_OnClientPutInServer(client)
{
	SDKHook(client, SDKHook_OnTakeDamage, DA_OnPlayerTakeDamage); 
}

public Action:DA_OnPlayerTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype) 
{
	if ((attacker > 0) && (attacker < MaxClients) && IsClientInGame(attacker))
	{
		PrintToChatAll("attacker: %i, damage: %f", attacker, damage);
		damage *= Player(attacker).GetDamageAmplifier()
		return Plugin_Changed
	}
	return Plugin_Continue
}