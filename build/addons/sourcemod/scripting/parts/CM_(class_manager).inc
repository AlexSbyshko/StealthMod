#define MAX_CLASSES 32
#define CLASS_NAME_LENGTH 32

static int ClassesCount
static char Class_Name[MAX_CLASSES][CLASS_NAME_LENGTH]
static char Class_ConfigPath[MAX_CLASSES][PLATFORM_MAX_PATH]
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

	public void GetConfigPath(char[] buffer, int maxLenght)
	{
		strcopy(buffer, CLASS_NAME_LENGTH, Class_ConfigPath[this.Id])
	}

	public void SetConfigPath(const char[] value)
	{
		strcopy(Class_ConfigPath[this.Id], CLASS_NAME_LENGTH, value)
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
	DefaultClass.SetConfigPath("cfg/sourcemod/stealthmod")
	DefaultClass.SetStartMoney(100)
}

public CM_OnMapStart()
{
	ReadClassConfig("player")
}

void ReadClassConfig(const char[] fileName, const char[] baseClassName = "")
{
	PrintToChatAll("read class from %s", fileName)
	STMClass baseClass = DefaultClass
	if (!StrEqual(baseClassName, ""))
	{
	}

	char configPath[PLATFORM_MAX_PATH]
	baseClass.GetConfigPath(configPath, PLATFORM_MAX_PATH)
	Format(configPath, PLATFORM_MAX_PATH, "%s/%s", configPath, fileName)

	char filePath[PLATFORM_MAX_PATH]
	Format(filePath, PLATFORM_MAX_PATH, "%s.cfg", configPath)

	new Handle:kv = CreateKeyValues("player")
	FileToKeyValues(kv, filePath)

	char name[CLASS_NAME_LENGTH]
	baseClass.GetName(name, CLASS_NAME_LENGTH)
	PrintToChatAll("base class name: %s", name)
	KvGetString(kv, "name", name, CLASS_NAME_LENGTH, name)
	PrintToChatAll("new class name: %s", name)

	STMClass newSTMClass = STMClasses.Create(name)

	newSTMClass.SetConfigPath(configPath)

	int startMoney = KvGetNum(kv, "start_money", baseClass.GetStartMoney())
	newSTMClass.SetStartMoney(startMoney)	
	PrintToChatAll("money: %i", startMoney)

	CloseHandle(kv)
	

	PrintToChatAll("check for dir %s", configPath)
	if (DirExists(configPath))
	{
		PrintToChatAll("dir exists! Load files!")
		new Handle:dir = OpenDirectory(configPath)
		char buffer[256];

		FileType fileType
		while (ReadDirEntry(dir, buffer, 256, fileType))
		{
			ReadDirEntry(dir, buffer, 256, fileType)
			//if (fileType == FileType_File)
			{
				PrintToChatAll("file: %s", buffer);
			}
		}
		CloseHandle(dir)
    }
	else
	{
		PrintToChatAll("not exists...")
	}
	PrintToChatAll("end load class: %s", name)
}