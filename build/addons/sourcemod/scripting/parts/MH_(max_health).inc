public MH_Init()
{
}

public MH_Event_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{	
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	if (client)
	{
		int maxHealth = Player(client).GetMaxHealth()
		SetEntProp(client, Prop_Data, "m_iHealth", maxHealth)
	}	
}