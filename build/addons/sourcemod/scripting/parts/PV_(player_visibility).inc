public PV_Init()
{
}

public PV_OnClientPutInServer(client)
{
	SDKHook(client, SDKHook_SetTransmit, Hook_SetTransmit);
}

public Action:Hook_SetTransmit(entity, client) 
{
	if (client == entity
		|| !IsPlayerAlive(client))
	{
		return Plugin_Continue
	}

	if (IsEnemy(entity, client)
		&& !IsPlayerVisible[entity])
	{
		return Plugin_Handled
	}
	return Plugin_Continue
}


public Action:OnPlayerRunCmd(client, &buttons, &impulse, Float:vel[3], Float:angles[3], &weapon)
{
	int flags = GetEntityFlags(client)

	bool isPlayerInvisible =
		( Player(client).IsInvisNotMoving() && IsPlayerNotMoving(buttons) )
		|| ( Player(client).IsInvisWalking() && IsPlayerWalking(buttons, flags) )

	if (isPlayerInvisible && IsPlayerInAir(flags))
	{
		isPlayerInvisible = isPlayerInvisible && Player(client).IsInvisInAir()
	}

	if (isPlayerInvisible)
	{
		SetClientInvisible(client)
	}
	else
	{
		SetClientVisible(client)
	}

	return Plugin_Continue
}

bool IsPlayerNotMoving(int buttons)
{
	return !IsMoveButtonsPressed(buttons)
}

bool IsPlayerWalking(int buttons, int flags)
{
	return IsMoveButtonsPressed(buttons)
		&& (buttons & IN_SPEED || flags & FL_DUCKING)
}

bool IsPlayerInAir(int flags)
{
	return !(flags & FL_ONGROUND)
}

bool IsMoveButtonsPressed(int buttons)
{
	return buttons & IN_FORWARD || buttons & IN_BACK || buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT
}

SetClientVisible(client)
{
	if (!IsPlayerVisible[client])
	{
		SetEntityVisible(client)
		SetClientWeaponsVisibility(client, true)
		IsPlayerVisible[client] = true;	
		FadeClient(client, 255, 255, 255, 0)
	}	 
}

SetClientInvisible(client)
{
	if (IsPlayerVisible[client])
	{
		SetEntityInvisible(client);
		SetClientWeaponsVisibility(client, false);
		IsPlayerVisible[client] = false;
		FadeClient(client, 64, 0, 64, 64) 
	}	
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
		SetEntityRenderColor(entity, 0, 0, 0, 127);
	}
}