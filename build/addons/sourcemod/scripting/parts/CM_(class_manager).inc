#define MAX_CLASSES 32
#define CLASS_NAME_LENGTH 32

static int ClassesCount
static char Class_Name[MAX_CLASSES][CLASS_NAME_LENGTH]
static char Class_ConfigName[MAX_CLASSES][CLASS_NAME_LENGTH]
static int Class_StartMoney[MAX_CLASSES]

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
	new Handle:kv = CreateKeyValues("players")
	FileToKeyValues(kv, "cfg/sourcemod/stealthmod/players.cfg")
	ReadClassConfig(kv, "baseplayer", DefaultClass)
	CloseHandle(kv)
}

void ReadClassConfig(Handle:kv, const char[] className, STMClass baseClass)
{
	KvRewind(kv)
	if (!KvJumpToKey(kv, className))
	{
		PrintToChatAll("class %s not found!", className)
		return
	}

	PrintToChatAll("read class %s", className)

	char name[CLASS_NAME_LENGTH]
	KvGetString(kv, "name", name, CLASS_NAME_LENGTH, className)
	PrintToChatAll("new class name: %s", name)

	STMClass newSTMClass = STMClasses.Create(name)
	newSTMClass.SetConfigName(className)

	int startMoney = KvGetNum(kv, "start_money", baseClass.GetStartMoney())
	newSTMClass.SetStartMoney(startMoney)	
	PrintToChatAll("money: %i", startMoney)

	char buffer[256]

	KvGetString(kv, "inherited", buffer, sizeof(buffer), "")
	if (!StrEqual(buffer, ""))
	{
		char inheritedClassNames[MAX_CLASSES][CLASS_NAME_LENGTH]
		int classCount = ExplodeString(buffer, ",", inheritedClassNames, MAX_CLASSES, CLASS_NAME_LENGTH)
		PrintToChatAll("inherited classes count: %i", classCount)
		for (int i = 0; i < classCount; i++)
		{
			ReadClassConfig(kv, inheritedClassNames[i], newSTMClass)
		}
	}	
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