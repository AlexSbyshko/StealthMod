#define MAX_ITEMSETS 32
#define MAX_ITEMS_IN_SET 64
#define MAX_ITEMS MAX_ITEMSETS * MAX_ITEMS_IN_SET
#define ITEM_NAME_LENGTH 32
#define ITEMSET_NAME_LENGTH 32
#define MAX_ITEMTYPES_IN_SET 32
#define MAX_ITEMTYPES MAX_ITEMSETS * MAX_ITEMTYPES_IN_SET
#define ITEMTYPE_NAME_LENGTH 32

static char Item_ClassName[MAX_ITEMS][ITEM_NAME_LENGTH]
static char Item_Name[MAX_ITEMS][ITEM_NAME_LENGTH]
static int Item_Ammo[MAX_ITEMS]
static bool Item_IsForPurchase[MAX_ITEMS]
static bool Item_IsDefault[MAX_ITEMS]
static int Item_TypeId[MAX_ITEMS]

methodmap Item
{
	public Item(int id)
    { 
    	return view_as<Item>(id)
    }

	property int Id
	{
		public get() { return view_as<int>(this); }
	}

	public void GetClassName(char[] buffer, int maxLenght)
	{
		strcopy(buffer, ITEM_NAME_LENGTH, Item_ClassName[this.Id])
	}

	public void SetClassName(const char[] value)
	{
		strcopy(Item_ClassName[this.Id], ITEM_NAME_LENGTH, value)
	}

	public void GetName(char[] buffer, int maxLenght)
	{
		strcopy(buffer, ITEM_NAME_LENGTH, Item_Name[this.Id])
	}

	public void SetName(const char[] value)
	{
		strcopy(Item_Name[this.Id], ITEM_NAME_LENGTH, value)
	}

	public int GetAmmo()
	{
		return Item_Ammo[this.Id]
	}

	public void SetAmmo(int value)
	{
		Item_Ammo[this.Id] = value
	}

	public bool IsForPurchase()
	{
		return Item_IsForPurchase[this.Id]
	}

	public void SetForPurchase(bool value)
	{
		Item_IsForPurchase[this.Id] = value
	}

	public bool IsDefault()
	{
		return Item_IsDefault[this.Id]
	}

	public void SetDefault(bool value)
	{
		Item_IsDefault[this.Id] = value
	}

	public int GetTypeId()
	{
		return Item_TypeId[this.Id]
	}

	public void SetTypeId(int value)
	{
		Item_TypeId[this.Id] = value
	}
}


static char ItemType_Name[MAX_ITEMTYPES][ITEMTYPE_NAME_LENGTH]
static int ItemType_SetId[MAX_ITEMTYPES]

methodmap ItemType
{
	public ItemType(int id)
	{ 
		PrintToChatAll("init itemType %i", id)
		return view_as<ItemType>(id)
	}

	property int Id
	{
		public get() 
		{ 
			int id = view_as<int>(this)
			PrintToChatAll("if of this: %i", id)
			return view_as<int>(this); 
		}
	}

	public void GetName(char[] buffer, int maxLenght)
	{
		strcopy(buffer, ITEMTYPE_NAME_LENGTH, ItemType_Name[this.Id])
	}

	public void SetName(const char[] value)
	{
		strcopy(ItemType_Name[this.Id], ITEMTYPE_NAME_LENGTH, value)
	}

	public int GetSetId()
	{
		return ItemType_SetId[this.Id]
	}

	public void SetSetId(int value)
	{
		ItemType_SetId[this.Id] = value
	}
}

static int ItemTypesCount

methodmap ItemTypes
{
	public static ItemType Create(const char[] name)
	{
		ItemType itemType = ItemType(ItemTypesCount++)
		itemType.SetName(name)
		return itemType
	}

	public static void Clear()
	{
		ItemTypesCount = 0
	}

	public static int Count()
	{
		return ItemTypesCount
	}

	public static int FindByNameAndSetId(const char[] name, int setId)
	{
		for (int i = 0; i < ItemTypesCount; i++)
		{
			//PrintToChatAll("check t: %s id: %i setid: %i", name, i, setId)
			if (StrEqual(name, ItemType_Name[i], false)
				&& ItemType_SetId[i] == setId)
			{
				//PrintToChatAll("yes!")
				return i
			}
		}
		return -1
	}
}

static int WeaponsCount

methodmap Weapons
{
	public static Item Create(const char[] className)
	{
		Item weapon = Item(WeaponsCount++)
		weapon.SetClassName(className)
		return weapon
	}

	public static void Clear()
	{
		WeaponsCount = 0
	}

	property int Count
	{
		public get() { return WeaponsCount; }
	}
}

static int WeaponSetsCount
static char WeaponSet_Name[MAX_ITEMSETS][ITEMSET_NAME_LENGTH]
static int WeaponSet_WeaponsCount[MAX_ITEMSETS]
static Item WeaponSet_Weapons[MAX_ITEMSETS][MAX_ITEMS_IN_SET]

methodmap WeaponSet
{
	public WeaponSet(int id)
    { 
    	return view_as<WeaponSet>(id)
    }

	property int Id
	{
		public get() { return view_as<int>(this); }
	}


	public void GetName(char[] buffer, int maxLenght)
	{
		strcopy(buffer, ITEM_NAME_LENGTH, WeaponSet_Name[this.Id])
	}

	public void SetName(const char[] value)
	{
		strcopy(WeaponSet_Name[this.Id], ITEM_NAME_LENGTH, value)
	}

	public void AddWeapon(Item weapon)
	{
		int weaponIndex = WeaponSet_WeaponsCount[this.Id]++
		WeaponSet_Weapons[this.Id][weaponIndex] = weapon
	}

	property int WeaponsCount
	{
		public get() 
		{ 
			return WeaponSet_WeaponsCount[this.Id]; 
		}
	}

	public Item GetWeapon(int weaponId)
	{
		return WeaponSet_Weapons[this.Id][weaponId]
	}
}


methodmap WeaponSets
{
	public static WeaponSet Create(const char[] name)
	{
		WeaponSet weaponSet = WeaponSet(WeaponSetsCount++)
		weaponSet.SetName(name)	
		return weaponSet
	}

	public static void Clear()
	{
		for (int i = 0 ; i < WeaponSetsCount; i++)
		{
			WeaponSet_WeaponsCount[i] = 0
		}
		WeaponSetsCount = 0
	}

	public static int FindByName(const char[] name)
	{
		for (int i = 0; i < WeaponSetsCount; i++)
		{
			if (StrEqual(name, WeaponSet_Name[i], false))
			{
				return WeaponSet(i).Id
			}
		}
		return -1
	}
	
	public static int Count()
	{
		return WeaponSetsCount
	}
}