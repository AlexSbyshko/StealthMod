
static Handle:CustomForwardsArray
static Handle:CustomForwardNamesArray

static Handle:InitPartForward
static Handle:OnMapStartForward
static Handle:OnEntityCreatedForward
static Handle:OnWeaponDropForward
static Handle:OnTerminateRoundForward
static Handle:OnPlayerRunCmdForward
static Handle:OnClientDisconnectForward

// SJ


#define PART_NAME_LENGTH 16

static Handle:PartsNamesArray = INVALID_HANDLE

InitPartSystem()
{
	PartsNamesArray = CreateArray(PART_NAME_LENGTH)
	CustomForwardsArray = CreateArray()
	CustomForwardNamesArray = CreateArray(MAX_NAME_LENGTH)

	CreateGameForwards()
}

CreateGameForwards()
{
	InitPartForward = CreateForward(ET_Ignore)
	OnMapStartForward = CreateForward(ET_Ignore)
	OnEntityCreatedForward = CreateForward(ET_Ignore, Param_Cell, Param_String)
	OnWeaponDropForward = CreateForward(ET_Event, Param_Cell, Param_Cell)
	OnTerminateRoundForward = CreateForward(ET_Event, Param_FloatByRef, Param_CellByRef)
	OnPlayerRunCmdForward = CreateForward(ET_Event, Param_Cell, Param_CellByRef)
	OnClientDisconnectForward = CreateForward(ET_Ignore, Param_Cell)
}

RegisterCustomForward(Handle:fwd, const String:postfix[])
{
	PushArrayCell(CustomForwardsArray, fwd)
	PushArrayString(CustomForwardNamesArray, postfix)
}

RegisterPart(const String:partName[PART_NAME_LENGTH])
{
	PushArrayString(PartsNamesArray, partName)
	HookPartForward(InitPartForward, partName, "Init")
	HookPartForward(OnMapStartForward, partName, "OnMapStart")
	HookPartForward(OnEntityCreatedForward, partName, "OnEntityCreated")
	HookPartForward(OnWeaponDropForward, partName, "OnWeaponDrop")
	HookPartForward(OnTerminateRoundForward, partName, "OnTerminateRound")
	HookPartForward(OnPlayerRunCmdForward, partName, "OnPlayerRunCmd")
	HookPartForward(OnClientDisconnectForward, partName, "OnClientDisconnect")

	HookPartEvent("cs_match_end_restart", partName, "MatchEndRestart")
	HookPartEvent("cs_win_panel_round", partName, "WinPanelRound")
	HookPartEvent("player_activate", partName, "PlayerActivate")
	HookPartEvent("player_hurt", partName, "PlayerHurt")
	HookPartEvent("player_spawn", partName, "PlayerSpawn")
	HookPartEvent("player_team", partName, "PlayerTeam")
	HookPartEvent("player_death", partName, "PlayerDeath")
	HookPartEvent("player_disconnect", partName, "PlayerDisconnect")
	HookPartEvent("round_end", partName, "RoundEnd")
	HookPartEvent("round_freeze_end", partName, "RoundFreezeEnd")
	HookPartEvent("round_prestart", partName, "RoundPreStart")
	HookPartEvent("round_start", partName, "RoundStart")	
}

InitParts()
{
	Call_StartForward(InitPartForward)
	Call_Finish()

	new partsCount = GetArraySize(PartsNamesArray)
	for (new i = 0; i < partsCount; i++)
	{
		decl String:partName[PART_NAME_LENGTH]
		GetArrayString(PartsNamesArray, i, partName, sizeof(partName))
		HookCustomForwards(partName)
	}
}

HookCustomForwards(const String:partName[PART_NAME_LENGTH])
{
	new forwardsCount = GetArraySize(CustomForwardsArray)
	for (new i = 0; i < forwardsCount; i++)
	{
		new Handle:fwd = GetArrayCell(CustomForwardsArray, i)
		decl String:postfix[MAX_NAME_LENGTH]
		GetArrayString(CustomForwardNamesArray, i, postfix, sizeof(postfix))
		HookPartForward(fwd, partName, postfix)
	}
}

HookPartForward(Handle:fwd, const String:partName[PART_NAME_LENGTH], const String:postfix[])
{
	decl String:partFunctionName[MAX_NAME_LENGTH]
	Format(partFunctionName, MAX_NAME_LENGTH, "%s_%s", partName, postfix);
	new Function:function = GetFunctionByName(INVALID_HANDLE, partFunctionName)
	if (function != INVALID_FUNCTION)
	{
		AddToForward(fwd, INVALID_HANDLE, function);
	}
}

HookPartEvent(const String:hookName[], const String:partName[PART_NAME_LENGTH], const String:postfix[])
{
	decl String:partFunctionName[MAX_NAME_LENGTH]
	Format(partFunctionName, MAX_NAME_LENGTH, "%s_Event_%s", partName, postfix);
	new EventHook:function = GetFunctionByName(INVALID_HANDLE, partFunctionName)
	if (function != INVALID_FUNCTION)
	{
		HookEventEx(hookName, function)
	}

	decl String:partPreFunctionName[MAX_NAME_LENGTH]
	Format(partPreFunctionName, MAX_NAME_LENGTH, "%s_Event_Pre%s", partName, postfix);
	new EventHook:preFunction = GetFunctionByName(INVALID_HANDLE, partPreFunctionName)
	if (preFunction != INVALID_FUNCTION)
	{
		HookEventEx(hookName, preFunction, EventHookMode_Pre)
	}
}

stock FireOnMapStart()
{
	Call_StartForward(OnMapStartForward)
	Call_Finish()
}

stock FireOnEntityCreated(entity, const String:classname[])
{
	Call_StartForward(OnEntityCreatedForward)
	Call_PushCell(entity)
	Call_PushString(classname)
	Call_Finish()
}

stock Action:FireOnWeaponDrop(client, weaponIndex)
{
	Call_StartForward(OnWeaponDropForward)
	Call_PushCell(client)
	Call_PushCell(weaponIndex)
	new Action:result
	Call_Finish(result)
	return result
}

stock Action:FireOnTerminateRound(&Float:delay, &CSRoundEndReason:reason)
{
	Call_StartForward(OnTerminateRoundForward)
	Call_PushFloatRef(delay);
	Call_PushCellRef(reason)
	new Action:result
	Call_Finish(result)
	return result
}

stock Action:FireOnPlayerRunCmd(client, &buttons)
{
	Call_StartForward(OnPlayerRunCmdForward)
	Call_PushCell(client);
	Call_PushCellRef(buttons)
	new Action:result
	Call_Finish(result)
	return result
}

stock FireOnClientDisconnect(client)
{	
	Call_StartForward(OnClientDisconnectForward)
	Call_PushCell(client)
	Call_Finish()
}