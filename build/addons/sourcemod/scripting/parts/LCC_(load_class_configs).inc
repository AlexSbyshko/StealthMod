static Class DefaultClass

public CM_Init()
{
	InitDefaultClass()
}

void InitDefaultClass()
{
	DefaultClass = STMClasses.Create("DefaultClass")
	DefaultClass.SetStartMoney(100)
	DefaultClass.SetProducesBreathSound(false)
	DefaultClass.SetFootstepsEnabled(true)
	DefaultClass.SetInvisNotMoving(false)
	DefaultClass.SetInvisInAir(false)
	DefaultClass.SetInvisWalking(false)
}

public CM_OnMapStart()
{
	ReadClassConfigs()
}


void ReadClassConfigs()
{
	Handle kv = CreateKeyValues("players")
	FileToKeyValues(kv, "cfg/sourcemod/stealthmod/players.cfg")

	new Handle:configsArray = CreateArray(CLASS_NAME_LENGTH)
	new Handle:baseConfigByConfig = CreateTrie()

	if (KvGotoFirstSubKey(kv))
	{	
		char configName[CLASS_NAME_LENGTH]
		char baseConfigName[CLASS_NAME_LENGTH]
		do
		{
			KvGetSectionName(kv, configName, CLASS_NAME_LENGTH)
			KvGetString(kv, "base", baseConfigName, CLASS_NAME_LENGTH, "")
			
			PushArrayString(configsArray, configName)
			SetTrieString(baseConfigByConfig, configName, baseConfigName)
		}
		while (KvGotoNextKey(kv))
	}

	new Handle:remainingConfigsArray = CloneArray(configsArray)

	while (GetArraySize(remainingConfigsArray) > 0)
	{
		int currentCount = GetArraySize(remainingConfigsArray)
		for (int i = currentCount - 1; i >= 0; i --)
		{
			char configName[CLASS_NAME_LENGTH]
			GetArrayString(remainingConfigsArray, i, configName, CLASS_NAME_LENGTH)	

			char baseConfigName[CLASS_NAME_LENGTH] = ""
			GetTrieString(baseConfigByConfig, configName, baseConfigName, CLASS_NAME_LENGTH)

			if (!StrEqual(baseConfigName, "") && FindStringInArray(remainingConfigsArray, baseConfigName) != -1)
			{
				continue
			}

			ReadClassConfig(kv, configName, baseConfigName)
			RemoveFromArray(remainingConfigsArray, i)
		}
	}

	CloseHandle(baseConfigByConfig)
	CloseHandle(remainingConfigsArray)
	CloseHandle(configsArray)
	CloseHandle(kv)
}

void ReadClassConfig(Handle kv, const char[] classConfigName, const char[] baseClassConfigName)
{
	KvRewind(kv)
	if (!KvJumpToKey(kv, classConfigName))
	{
		return
	}
	Class baseClass
	int baseClassId = STMClasses.FindByConfigName(baseClassConfigName)
	if (baseClassId != -1)
	{
		baseClass = STMClasses.Get(baseClassId)
	}
	else
	{
		baseClass = DefaultClass
	}

	char name[CLASS_NAME_LENGTH]
	baseClass.GetConfigName(name, CLASS_NAME_LENGTH)

	KvGetString(kv, "name", name, CLASS_NAME_LENGTH, classConfigName)

	Class newSTMClass = STMClasses.Create(name)
	newSTMClass.SetConfigName(classConfigName)

	int startMoney = KvGetNum(kv, "start_money", baseClass.GetStartMoney())
	newSTMClass.SetStartMoney(startMoney)

	int maxHealth = KvGetNum(kv, "max_health", baseClass.GetMaxHealth())
	newSTMClass.SetMaxHealth(maxHealth)

	int healthRegenAmount = KvGetNum(kv, "hp_regen_amount", baseClass.GetHealthRegenAmount())
	newSTMClass.SetHealthRegenAmount(healthRegenAmount)

	float healthRegenPeriod = KvGetFloat(kv, "hp_regen_period", baseClass.GetHealthRegenPeriod())
	newSTMClass.SetHealthRegenPeriod(healthRegenPeriod)

	float gravityMultiplier = KvGetFloat(kv, "gravity_multiplier", baseClass.GetGravityMultiplier())
	newSTMClass.SetGravityMultiplier(gravityMultiplier)

	float speedMultiplier = KvGetFloat(kv, "speed_multiplier", baseClass.GetSpeedMultiplier())
	newSTMClass.SetSpeedMultiplier(speedMultiplier)

	bool isProducesBrathSound = KvGetNum(kv, "produces_breath_sound", baseClass.IsProducesBreathSound()) > 0
	newSTMClass.SetProducesBreathSound(isProducesBrathSound)

	bool isFootStepsEnabled = KvGetNum(kv, "footsteps_enabled", baseClass.IsFootStepsEnabled()) > 0
	newSTMClass.SetFootstepsEnabled(isFootStepsEnabled)

	bool isInvisNotMoving = KvGetNum(kv, "is_invisible_when_not_moving", baseClass.IsInvisNotMoving()) > 0
	newSTMClass.SetInvisNotMoving(isInvisNotMoving)

	bool isInvisInAir = KvGetNum(kv, "is_invisible_in_the_air", baseClass.IsInvisInAir()) > 0
	newSTMClass.SetInvisInAir(isInvisInAir)

	bool isInvisWalking = KvGetNum(kv, "is_invisible_when_walking", baseClass.IsInvisWalking()) > 0
	newSTMClass.SetInvisWalking(isInvisWalking)
}

public CM_Event_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	new team = GetEventInt(event, "team");
	SetPlayerClassByTeam(client, team)
}

SetPlayerClassByTeam(int client, int team)
{
	Player player = Player(client)
	
	switch (team)
	{
		case CS_TEAM_T:
		{
			player.SetClassByName("terrorist")			
		}
		case CS_TEAM_CT:
		{
			player.SetClassByName("counter-terrorist")
		}
		default:
		{
			player.SetClassByName("baseplayer")
		}
	}
}