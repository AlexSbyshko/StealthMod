static Handle BreathTimers[MAXPLAYERS+1]
static Handle BreathSoundConVar
static Handle BreathFrequencyConVar
static Handle BreathVolumeConVar
static Handle BreathDurationConvar

static char BreathSound[PLATFORM_MAX_PATH]
static float BreathFrequency
static float BreathVolume
static float BreathDuration

public BS_Init()
{
	BreathSoundConVar = CreateConVar("stm_breath_sound", "player/breathe1.wav", "Breath sound path")
	BreathFrequencyConVar = CreateConVar("stm_breath_frequency", "10.0", "Breath frequency (seconds)", _, true, 0.0)
	BreathVolumeConVar = CreateConVar("stm_breath_volume", "1.0", "Breath volume (from 0 to 1)", _, true, 0.0, true, 1.0)
	BreathDurationConvar = CreateConVar("stm_breath_duration", "5.0", "Breath duration (in seconds). Must be < stm_breath_frequency", _, true, 0.0)
	for (int i = 1; i <= MaxClients; i++)
	{
		BreathTimers[i] = INVALID_HANDLE
	}
}

public BS_OnMapStart()
{
	GetConVarString(BreathSoundConVar, BreathSound, PLATFORM_MAX_PATH)
	BreathFrequency = GetConVarFloat(BreathFrequencyConVar)
	BreathVolume = GetConVarFloat(BreathVolumeConVar)
	BreathDuration = GetConVarFloat(BreathDurationConvar)

	PrecacheSoundAny(BreathSound)

	decl String:fullSoundPath[PLATFORM_MAX_PATH]
	Format(fullSoundPath, sizeof(fullSoundPath), "sound/%s", BreathSound)
	AddFileToDownloadsTable(fullSoundPath)
}

public BS_Event_MatchEndRestart(Handle:event, const String:name[], bool:dontBroadcast)
{	
	for (int i = 1; i <= MaxClients; i++)
	{
		StopBreathSound(i)
	}
}

public BS_Event_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{	
	int userId = GetEventInt(event, "userid")
	new client = GetClientOfUserId(userId)
	new team = GetClientTeam(client)
	if (team != CS_TEAM_NONE && IsPlayerAlive(client) && Player(client).IsProducesBreathSound())
	{
		StopBreathSound(client)
		if (BreathFrequency > 0)
		{
			BreathTimers[client] = CreateTimer(BreathFrequency, Timer_BreathSound, userId, TIMER_REPEAT)
		}
	}
}

public BS_Event_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	StopBreathSound(client)
}

public Action:Timer_BreathSound(Handle:timer, any:userId)
{	
	new client = GetClientOfUserId(userId)
	if (client && IsPlayerAlive(client) && Player(client).IsProducesBreathSound())
	{
		EmitSoundToAllAny(BreathSound, client, SNDCHAN_BODY, SNDLEVEL_NORMAL, SND_CHANGEVOL, BreathVolume)
		CreateTimer(BreathDuration, Timer_StopSound, GetClientUserId(client))
	}
	return Plugin_Continue
}

public Action:Timer_StopSound(Handle:timer, any:userId)
{
	int client = GetClientOfUserId(userId)
	if (client)
	{
		EmitSoundToAllAny(BreathSound, client, SNDCHAN_BODY, SNDLEVEL_NORMAL, SND_STOPLOOPING)
	}	
}

StopBreathSound(client)
{
	if (BreathTimers[client] != INVALID_HANDLE)
	{
		KillTimer(BreathTimers[client])
		BreathTimers[client] = INVALID_HANDLE
	}
}