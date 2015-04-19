public RFS_Init()
{
	AddNormalSoundHook(RFS_Event_SoundPlayed)
}

public RFS_OnClientPutInServer(client)
{
	if (!IsFakeClient(client))
	{
		SendConVarValue(client, FindConVar("sv_footsteps"), "0");
	}	
}

public Action:RFS_Event_SoundPlayed(clients[64],&numClients,String:sample[PLATFORM_MAX_PATH],&entity,&channel,&Float:volume,&level,&pitch,&flags) 
{
	if (entity 
		&& entity <= MaxClients 
		&& (StrContains(sample, "physics") != -1 || StrContains(sample, "footsteps") != -1))
	{
		if (IsClientInGame(entity) 
			&& !Player(entity).IsFootStepsEnabled())
		{
			return Plugin_Handled
		}
		else
		{
			EmitSoundToAll(sample, entity);
			return Plugin_Handled
		}
	}

	return Plugin_Continue
}