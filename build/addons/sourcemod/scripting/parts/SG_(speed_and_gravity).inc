public SG_Init()
{
}

public SG_Event_PlayerActivate(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	SDKHook(client, SDKHook_PreThink, SG_OnClientThink)
}

public SG_OnClientThink(client)
{
	Client_FixSpeed(client)
}

Client_FixSpeed(client)
{
	if (IsClientInGame(client))
	{
		new Float:speedMultiplier = Player(client).GetSpeedMultiplier()
		new Float:gravityMultiplier = (1.0 / speedMultiplier) * Player(client).GetGravityMultiplier()
		SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", speedMultiplier)
		SetEntPropFloat(client, Prop_Data, "m_flGravity", gravityMultiplier)		
	}
}