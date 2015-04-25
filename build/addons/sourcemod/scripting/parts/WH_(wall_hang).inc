static bool Player_IsHanged[MAXPLAYERS+1]

public WH_Init()
{

}

public WH_OnClientPutInServer(client)
{
	SDKHook(client, SDKHook_Touch, WH_OnPlayerTouch)
	SDKHook(client, SDKHook_PreThink, WH_OnPlayerPreThink)
}

public WH_OnPlayerTouch(client, entity)
{
	if (Player(client).IsCanWallHang())
	{
		int flags = GetEntityFlags(client)
		if (!(flags & FL_ONGROUND) && IsEntitySolid(entity))
		{
			int buttons = GetClientButtons(client)
			if (!Player_IsHanged[client] && (buttons & IN_JUMP))
			{		
				SetEntityMoveType(client, MOVETYPE_NONE)
				Player_IsHanged[client] = true
			}
		}
	}	
}

public WH_OnPlayerPreThink(client)
{
	if (Player_IsHanged[client] )
	{
		int buttons = GetClientButtons(client)
		if (!(buttons & IN_JUMP))
		{
			SetEntityMoveType(client, MOVETYPE_WALK)
			Player_IsHanged[client] = false
		}
	}
}

bool IsEntitySolid(entity)
{
	return (GetEntProp(entity, Prop_Send, "m_nSolidType") != 0) 
		&& ((GetEntProp(entity, Prop_Send, "m_usSolidFlags") & 4) == 0)
}