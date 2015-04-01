#define MAX_CLASSES 32
#define CLASS_NAME_LENGTH 32

static int ClassesCount
static char Class_Name[MAX_CLASSES][CLASS_NAME_LENGTH]

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

	public void SetName(const char[] name)
	{
		strcopy(Class_Name[this.Id], CLASS_NAME_LENGTH, name)
	}
}


static STMClass Classes[MAX_CLASSES]

methodmap STMClasses
{
	public STMClass Create(const char[] name)
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
}

public CM_OnMapStart()
{
	char filePath[PLATFORM_MAX_PATH]
	GetConfigPath(filePath, PLATFORM_MAX_PATH, "players")

	new Handle:kv = CreateKeyValues("MyFile")
	CloseHandle(kv)
}

void GetConfigPath(char[] buffer, int maxLenght, const char[] configName)
{
	Format(buffer, maxLenght, "cfg/sourcemod/stealthmod/%s", configName)
}