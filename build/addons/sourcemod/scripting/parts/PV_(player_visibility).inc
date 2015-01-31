static Handle:CrouchWalkInvisEnabledConVar

static bool:IsWalkingInvisEnabled

public PV_Init()
{
	CrouchWalkInvisEnabledConVar = CreateConVar("stm_crouch_walk_invis_enabled", "1", "Enable/Disable invisibility while walking/crouching", 0, true, 0.0, true, 1.0)
}

public PV_OnClientPutInServer(client)
{
	SDKHook(client, SDKHook_SetTransmit, Hook_SetTransmit);
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

public PV_Event_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
	IsWalkingInvisEnabled = GetConVarBool(CrouchWalkInvisEnabledConVar)
}

public Action:OnPlayerRunCmd(client, &buttons, &impulse, Float:vel[3], Float:angles[3], &weapon)
{
	static bool:isClientRunning[MAXPLAYERS+1]
	static flags
	flags = GetEntityFlags(client)
	
	if (!IsWalkingInvisEnabled || !(buttons & IN_SPEED || flags & FL_DUCKING))
	{
		if (buttons & IN_FORWARD || buttons & IN_BACK
			|| buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT)
		{
			if (!isClientRunning[client])
			{
				isClientRunning[client] = true
				OnClientStartRun(client)		
			}
		}
		else 
		{
			if (isClientRunning[client])
			{
				isClientRunning[client] = false
				OnClientEndRun(client)
			}
		}
	}
	else
	{
		if (isClientRunning[client])
		{
			isClientRunning[client] = false
			OnClientEndRun(client)
		}
	}
	return Plugin_Continue
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