static Handle:SwapTeamsConditionConVar


const SWAP_CONDITION_ROUND_END	= 0
const SWAP_CONDITION_CT_WIN 	= 1
const SWAP_CONDITION_T_WIN 		= 2

public ST_Init()
{
	SwapTeamsConditionConVar = CreateConVar("stm_swap_teams_condition", "0", "Swap teams condition:\n0 - End of any round\n1 - CT win\n2 - T win", 0, true, 0.0, true, 2.0)
}

public Action:ST_Event_RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
	new reason = GetEventInt(event, "reason")
	if (reason != _:CSRoundEnd_GameStart)
	{		
		new condition = GetConVarInt(SwapTeamsConditionConVar)
		new winnerTeam = GetEventInt(event, "winner")
		
		switch (condition)
		{
		
		case SWAP_CONDITION_ROUND_END:

			SwapTeams()

		case SWAP_CONDITION_CT_WIN:

			if (winnerTeam == CS_TEAM_CT)
			{
				SwapTeams()
			}

		case SWAP_CONDITION_T_WIN:

			if (winnerTeam == CS_TEAM_T)
			{
				SwapTeams()
			}
		}
	}
	return Plugin_Continue
}


SwapTeams()
{
	HookEvent("player_team", Event_PlayerTeam, EventHookMode_Pre)

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			new team = GetClientTeam(i)
			if (team == CS_TEAM_CT)
			{
				CS_SwitchTeam(i, CS_TEAM_T)
			}
			else if (team == CS_TEAM_T)
			{
				CS_SwitchTeam(i, CS_TEAM_CT)
			}
		}
	}
	new tScore = GetTeamScore(CS_TEAM_T);
	new ctScore = GetTeamScore(CS_TEAM_CT);
	CS_SetTeamScore(CS_TEAM_T, ctScore)
	SetTeamScore(CS_TEAM_T, ctScore)
	CS_SetTeamScore(CS_TEAM_CT, tScore)
	SetTeamScore(CS_TEAM_CT, tScore)

	UnhookEvent("player_team", Event_PlayerTeam, EventHookMode_Pre)
}

public Action:Event_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
	return Plugin_Handled
}