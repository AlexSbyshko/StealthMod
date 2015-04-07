#define MAX_CLASSES 32
#define CLASS_NAME_LENGTH 32

static int ClassesCount
static char Class_Name[MAX_CLASSES][CLASS_NAME_LENGTH]
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
}

static STMPlayer Players[MAXPLAYERS+1]


methodmap STMPlayers
{
	public STMPlayer Get(int id)
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
	DefaultClass = STMClasses.Create("defaultClass")
	DefaultClass.SetStartMoney(100)
}

public CM_OnMapStart()
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
		return;
	}

	PrintToChatAll("read class %s", className)

	char name[CLASS_NAME_LENGTH]
	KvGetString(kv, "name", name, CLASS_NAME_LENGTH, className)
	PrintToChatAll("new class name: %s", name)

	STMClass newSTMClass = STMClasses.Create(name)

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