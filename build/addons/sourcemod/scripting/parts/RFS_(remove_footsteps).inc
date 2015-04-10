static Handle:TFootstepsEnabledConVar

public RFS_Init()
{
	TFootstepsEnabledConVar = CreateConVar("stm_t_footsteps_enabled", "0", "Enable/Disable Terrorists footsteps", 0, true, 0.0, true, 1.0)
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
			&& GetClientTeam(entity) == CS_TEAM_T 
			&& !GetConVarBool(TFootstepsEnabledConVar))
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