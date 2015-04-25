#define MAX_CLASSES 32
#define CLASS_NAME_LENGTH 64

static int ClassesCount

static char Class_Name[MAX_CLASSES][CLASS_NAME_LENGTH]
static char Class_ConfigName[MAX_CLASSES][CLASS_NAME_LENGTH]
static int Class_StartMoney[MAX_CLASSES]
static int Class_MaxHealth[MAX_CLASSES]
static int Class_HealthRegenAmount[MAX_CLASSES]
static float Class_HealthRegenPeriod[MAX_CLASSES]
static float Class_SpeedMultiplier[MAX_CLASSES]
static float Class_GravityMultiplier[MAX_CLASSES]
static bool Class_IsCanWallHang[MAX_CLASSES]
static bool Class_IsChoosable[MAX_CLASSES]
static bool Class_IsProducesBreathSound[MAX_CLASSES]
static bool Class_IsFootStepsEnabled[MAX_CLASSES]
static bool Class_IsInvisNotMoving[MAX_CLASSES]
static bool Class_IsInvisInAir[MAX_CLASSES]
static bool Class_IsInvisWalking[MAX_CLASSES]


methodmap Class
{
    public Class(int id)
    { 
    	return view_as<Class>(id)
    }

	property int Id
	{
		public get() { return view_as<int>(this); }
	}

	public bool IsCanWallHang()
	{
		return Class_IsCanWallHang[this.Id]
	}

	public void SetCanWallHang(bool value)
	{
		Class_IsCanWallHang[this.Id] = value
	}


	public bool IsChoosable()
	{
		return Class_IsChoosable[this.Id]
	}

	public void SetChoosable(bool value)
	{
		Class_IsChoosable[this.Id] = value
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


	public float GetSpeedMultiplier()
	{
		return Class_SpeedMultiplier[this.Id]
	}

	public void SetSpeedMultiplier(float value)
	{
		Class_SpeedMultiplier[this.Id] = value
	}


	public float GetGravityMultiplier()
	{
		return Class_GravityMultiplier[this.Id]
	}

	public void SetGravityMultiplier(float value)
	{
		Class_GravityMultiplier[this.Id] = value
	}


	public bool IsProducesBreathSound()
	{
		return Class_IsProducesBreathSound[this.Id]
	}

	public void SetProducesBreathSound(bool value)
	{
		Class_IsProducesBreathSound[this.Id] = value
	}


	public void SetFootstepsEnabled(bool value)
	{
		Class_IsFootStepsEnabled[this.Id] = value
	}

	public bool IsFootStepsEnabled()
	{
		return Class_IsFootStepsEnabled[this.Id]
	}


	public void SetInvisNotMoving(bool value)
	{
		Class_IsInvisNotMoving[this.Id] = value
	}

	public bool IsInvisNotMoving()
	{
		return Class_IsInvisNotMoving[this.Id]
	}


	public void SetInvisInAir(bool value)
	{
		Class_IsInvisInAir[this.Id] = value
	}

	public bool IsInvisInAir()
	{
		return Class_IsInvisInAir[this.Id]
	}


	public void SetInvisWalking(bool value)
	{
		Class_IsInvisWalking[this.Id] = value
	}

	public bool IsInvisWalking()
	{
		return Class_IsInvisWalking[this.Id]
	}
}

methodmap Classes
{
	public static Class Create(const char[] name)
	{
		Class newClass = Class(ClassesCount++)
		newClass.SetName(name)	
		return newClass
	}

	public static Class Get(int id)
	{
		return Class(id)
	}

	public static int FindByConfigName(const char[] name)
	{
		for (int i = 0; i < ClassesCount; i++)
		{
			if (StrEqual(name, Class_ConfigName[i], false))
			{
				return Classes.Get(i).Id
			}
		}
		return -1
	}
}

static Class Player_Class[MAXPLAYERS+1]

methodmap Player
{
	public Player(int id)
	{
		return view_as<Player>(id)
	}

	property int Id
	{
		public get() { return view_as<int>(this); }
	}

	public Class GetClass()
	{
		return Player_Class[this.Id]
	}

	public void SetClass(Class stmClass)
	{
		Player_Class[this.Id] = stmClass
	}

	public void SetClassByName(const char[] name)
	{
		int stmClassId = Classes.FindByConfigName(name)
		if (stmClassId != -1)
		{
			Class stmClass = Classes.Get(stmClassId)
			this.SetClass(stmClass)
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

	public float GetSpeedMultiplier()
	{
		return this.GetClass().GetSpeedMultiplier()
	}

	public float GetGravityMultiplier()
	{
		return this.GetClass().GetGravityMultiplier()
	}

	public bool IsProducesBreathSound()
	{
		return this.GetClass().IsProducesBreathSound()
	}

	public bool IsFootStepsEnabled()
	{
		return this.GetClass().IsFootStepsEnabled()
	}

	public bool IsInvisNotMoving()
	{
		return this.GetClass().IsInvisNotMoving()
	}

	public bool IsInvisInAir()
	{
		return this.GetClass().IsInvisInAir()
	}

	public bool IsInvisWalking()
	{
		return this.GetClass().IsInvisWalking()
	}

	public bool IsCanWallHang()
	{
		return this.GetClass().IsCanWallHang()
	}
}