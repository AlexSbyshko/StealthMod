#define MAX_CLASSES 32
#define CLASS_NAME_LENGTH 32

static int ClassesCount
static char Class_Name[MAX_CLASSES][CLASS_NAME_LENGTH]
static char Class_ConfigName[MAX_CLASSES][CLASS_NAME_LENGTH]
static int Class_StartMoney[MAX_CLASSES]
static int Class_MaxHealth[MAX_CLASSES]
static int Class_HealthRegenAmount[MAX_CLASSES]
static float Class_HealthRegenPeriod[MAX_CLASSES]

methodmap STMClass
{
    public STMClass(int id)
    { 
    	return view_as<STMClass>(id)
    }

	property int Id
	{
		public get() { return view_as<int>(this); }
	}

	public void GetConfigName(char[] buffer, int maxLenght)
	{
		strcopy(buffer, CLASS_NAME_LENGTH, Class_ConfigName[this.Id])
	}

	public void SetConfigName(const char[] name)
	{
		strcopy(Class_ConfigName[this.Id], CLASS_NAME_LENGTH, name)
	}


	public void GetName(char[] buffer, int maxLenght)
	{
		strcopy(buffer, CLASS_NAME_LENGTH, Class_Name[this.Id])
	}

	public void SetName(const char[] name)
	{
		strcopy(Class_Name[this.Id], CLASS_NAME_LENGTH, name)
	}


	public int GetStartMoney()
	{
		return Class_StartMoney[this.Id]
	}

	public void SetStartMoney(int value)
	{
		Class_StartMoney[this.Id] = value
	}


	public int GetMaxHealth()
	{
		return Class_MaxHealth[this.Id]
	}

	public void SetMaxHealth(int value)
	{
		Class_MaxHealth[this.Id] = value
	}


	public int GetHealthRegenAmount()
	{
		return Class_HealthRegenAmount[this.Id]
	}

	public void SetHealthRegenAmount(int value)
	{
		Class_HealthRegenAmount[this.Id] = value
	}


	public float GetHealthRegenPeriod()
	{
		return Class_HealthRegenPeriod[this.Id]
	}

	public void SetHealthRegenPeriod(float value)
	{
		Class_HealthRegenPeriod[this.Id] = value
	}
}


static STMClass Classes[MAX_CLASSES]
static STMClass DefaultClass

methodmap STMClasses
{
	public static STMClass Create(const char[] name)
	{
		STMClass newClass = STMClass(ClassesCount)
		newClass.SetName(name)
		Classes[ClassesCount++] = newClass		
		return newClass
	}

	public static STMClass Get(int id)
	{
		return Classes[id]
	}

	public static int FindByConfigName(const char[] name)
	{
		for (int i = 0; i < ClassesCount; i++)
		{
			if (StrEqual(name, Class_ConfigName[i], false))
			{
				return STMClasses.Get(i).Id
			}
		}
		return -1
	}
}

static STMClass Player_Class[MAXPLAYERS+1]

methodmap STMPlayer
{
	public STMPlayer(int id)
	{
		return view_as<STMPlayer>(id)
	}

	property int Id
	{
		public get() { return view_as<int>(this); }
	}

	public STMClass GetClass()
	{
		return Player_Class[this.Id]
	}

	public void SetClass(STMClass stmClass)
	{
		Player_Class[this.Id] = stmClass
	}

	public void SetClassByName(const char[] name)
	{
		int stmClassId = STMClasses.FindByConfigName(name)
		if (stmClassId != -1)
		{
			STMClass stmClass = STMClasses.Get(stmClassId)
			this.SetClass(stmClass)
		}
		else
		{
			this.SetClass(DefaultClass)
		}
	}

	public int GetStartMoney()
	{
		return this.GetClass().GetStartMoney()
	}

	public int GetMaxHealth()
	{
		return this.GetClass().GetMaxHealth()
	}

	public int GetHealthRegenAmount()
	{
		return this.GetClass().GetHealthRegenAmount()
	}

	public float GetHealthRegenPeriod()
	{
		return this.GetClass().GetHealthRegenPeriod()
	}
}

static STMPlayer Players[MAXPLAYERS+1]


methodmap STMPlayers
{
	public static STMPlayer Get(int id)
	{
		return Players[id]
	}
}

public CM_Init()
{
	InitDefaultClass()
}

void InitDefaultClass()
{
	DefaultClass = STMClasses.Create("DefaultClass")
	DefaultClass.SetStartMoney(100)
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

			PrintToChatAll("from cache: config %s with base %s", configName, baseConfigName)

			if (!StrEqual(baseConfigName, "") && FindStringInArray(remainingConfigsArray, baseConfigName) != -1)
			{
				PrintToChatAll("skip config %s with base %s", configName, baseConfigName)
				continue
			}
			PrintToChatAll("read config %s with base %s", configName, baseConfigName)

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
		PrintToChatAll("class %s not found!", classConfigName)
		return
	}
	STMClass baseClass
	int baseClassId = STMClasses.FindByConfigName(baseClassConfigName)
	if (baseClassId != -1)
	{
		baseClass = STMClasses.Get(baseClassId)
	}
	else
	{
		baseClass = DefaultClass
	}

	PrintToChatAll("read class %s", classConfigName)

	char name[CLASS_NAME_LENGTH]
	baseClass.GetConfigName(name, CLASS_NAME_LENGTH)
	PrintToChatAll("base class name: %s", name)

	KvGetString(kv, "name", name, CLASS_NAME_LENGTH, classConfigName)
	PrintToChatAll("new class name: %s", name)

	STMClass newSTMClass = STMClasses.Create(name)
	newSTMClass.SetConfigName(classConfigName)

	int startMoney = KvGetNum(kv, "start_money", baseClass.GetStartMoney())
	newSTMClass.SetStartMoney(startMoney)


	int maxHealth = KvGetNum(kv, "max_health", baseClass.GetMaxHealth())
	newSTMClass.SetMaxHealth(maxHealth)

	int healthRegenAmount = KvGetNum(kv, "hp_regen_amount", baseClass.GetHealthRegenAmount())
	newSTMClass.SetHealthRegenAmount(healthRegenAmount)

	float healthRegenPeriod = KvGetFloat(kv, "hp_regen_period", baseClass.GetHealthRegenPeriod())
	newSTMClass.SetHealthRegenPeriod(healthRegenPeriod)

	PrintToChatAll("end load class: %s", name)
}

public CM_Event_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client = GetClientOfUserId(GetEventInt(event, "userid"))
	new team = GetEventInt(event, "team");
	STMPlayer player = STMPlayers.Get(client)
	switch (team)
	{
		case CS_TEAM_T:
			player.SetClassByName("terrorist")
		case CS_TEAM_CT:
			player.SetClassByName("counter-terrorist")
		default:
			player.SetClassByName("baseplayer")
	}
}