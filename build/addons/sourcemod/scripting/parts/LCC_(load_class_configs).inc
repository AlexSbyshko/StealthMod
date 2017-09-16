static WeaponSet EmptyWeaponSet
static Class DefaultClass

public LCC_Init()
{
	EmptyWeaponSet = WeaponSet(-1)
}

public LCC_OnMapStart()
{
	Classes.Clear()	
	InitDefaultClass()	
	ReadWeaponsConfigs()
	ReadClassConfigs()
}

void InitDefaultClass()
{
	DefaultClass = Classes.Create("DefaultClass")
	DefaultClass.SetStartMoney(100)
	DefaultClass.SetProducesBreathSound(false)
	DefaultClass.SetFootstepsEnabled(true)
	DefaultClass.SetInvisNotMoving(false)
	DefaultClass.SetInvisInAir(false)
	DefaultClass.SetInvisWalking(false)
	DefaultClass.SetSpeedMultiplier(1.0)
	DefaultClass.SetGravityMultiplier(1.0)
	DefaultClass.SetChoosable(false)
	DefaultClass.SetCanWallHang(false)
	DefaultClass.SetHealthForKill(0)
	DefaultClass.SetDamageAmplifier(1.0)
}

void InitEmptyItemSet()
{
	EmptyWeaponSet = WeaponSets.Create("empty")
}

void ReadWeaponsConfigs()
{
	ItemTypes.Clear()
	WeaponSets.Clear()
	Weapons.Clear()
	InitEmptyItemSet()

	Handle kv = CreateKeyValues("itemsets")
	FileToKeyValues(kv, "cfg/sourcemod/stealthmod/items.cfg")

	if (KvGotoFirstSubKey(kv))
	{	
		char setName[ITEMSET_NAME_LENGTH]
		do
		{
			KvGetSectionName(kv, setName, ITEMSET_NAME_LENGTH)

			WeaponSet weaponSet = WeaponSets.Create(setName)

			if (KvGotoFirstSubKey(kv))
			{
				char itemClassName[ITEM_NAME_LENGTH]
				char itemName[ITEM_NAME_LENGTH]
				char itemTypeName[ITEMTYPE_NAME_LENGTH]
				do
				{
					KvGetString(kv, "type", itemTypeName, sizeof(itemTypeName))
					ItemType itemType
					int itemTypeId = ItemTypes.FindByNameAndSetId(itemTypeName, weaponSet.Id)
					if (itemTypeId == -1)
					{
						itemType = ItemTypes.Create(itemTypeName)
						itemType.SetSetId(weaponSet.Id)
						itemTypeId = itemType.Id
						PrintToChatAll("new type %s, id: %i", itemTypeName, itemTypeId)
					}
					else
					{
						PrintToChatAll("type %s found, id: %i", itemTypeName, itemTypeId)
					}

					KvGetSectionName(kv, itemClassName, sizeof(itemClassName))
					Item item = Weapons.Create(itemClassName)
					
					KvGetString(kv, "name", itemName, sizeof(itemName))
					item.SetName(itemName)
					
					int ammo = KvGetNum(kv, "ammo", -1)
					item.SetAmmo(ammo)

					bool isForPurchase = KvGetNum(kv, "for_purchase", 0) > 0
					item.SetForPurchase(isForPurchase)

					bool isDefault = KvGetNum(kv, "default", 0) > 0
					item.SetDefault(isDefault)

					PrintToChatAll("Set weapon %s, type: %i", itemName, itemTypeId)
					item.SetTypeId(itemTypeId)

					weaponSet.AddWeapon(item)
				}
				while (KvGotoNextKey(kv))
				KvGoBack(kv)
			}
		}
		while (KvGotoNextKey(kv))
	}

	delete kv
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
	int baseClassId = Classes.FindByConfigName(baseClassConfigName)
	if (baseClassId != -1)
	{
		baseClass = Classes.Get(baseClassId)
	}
	else
	{
		baseClass = DefaultClass
	}

	char name[CLASS_NAME_LENGTH]
	baseClass.GetConfigName(name, CLASS_NAME_LENGTH)

	KvGetString(kv, "name", name, CLASS_NAME_LENGTH, classConfigName)

	Class newClass = Classes.Create(name)
	newClass.SetConfigName(classConfigName)

	int startMoney = KvGetNum(kv, "start_money", baseClass.GetStartMoney())
	newClass.SetStartMoney(startMoney)

	int maxHealth = KvGetNum(kv, "max_health", baseClass.GetMaxHealth())
	newClass.SetMaxHealth(maxHealth)

	int healthRegenAmount = KvGetNum(kv, "hp_regen_amount", baseClass.GetHealthRegenAmount())
	newClass.SetHealthRegenAmount(healthRegenAmount)

	float damageAmplifier = KvGetFloat(kv, "damage_amplifier", baseClass.GetDamageAmplifier())
	newClass.SetDamageAmplifier(damageAmplifier)

	float healthRegenPeriod = KvGetFloat(kv, "hp_regen_period", baseClass.GetHealthRegenPeriod())
	newClass.SetHealthRegenPeriod(healthRegenPeriod)

	float gravityMultiplier = KvGetFloat(kv, "gravity_multiplier", baseClass.GetGravityMultiplier())
	newClass.SetGravityMultiplier(gravityMultiplier)

	float speedMultiplier = KvGetFloat(kv, "speed_multiplier", baseClass.GetSpeedMultiplier())
	newClass.SetSpeedMultiplier(speedMultiplier)

	bool isProducesBrathSound = KvGetNum(kv, "produces_breath_sound", baseClass.IsProducesBreathSound()) > 0
	newClass.SetProducesBreathSound(isProducesBrathSound)

	bool isFootStepsEnabled = KvGetNum(kv, "footsteps_enabled", baseClass.IsFootStepsEnabled()) > 0
	newClass.SetFootstepsEnabled(isFootStepsEnabled)

	bool isInvisNotMoving = KvGetNum(kv, "is_invisible_when_not_moving", baseClass.IsInvisNotMoving()) > 0
	newClass.SetInvisNotMoving(isInvisNotMoving)

	bool isInvisInAir = KvGetNum(kv, "is_invisible_in_the_air", baseClass.IsInvisInAir()) > 0
	newClass.SetInvisInAir(isInvisInAir)

	bool isInvisWalking = KvGetNum(kv, "is_invisible_when_walking", baseClass.IsInvisWalking()) > 0
	newClass.SetInvisWalking(isInvisWalking)

	bool isChoosable = KvGetNum(kv, "is_choosable", baseClass.IsChoosable()) > 0
	newClass.SetChoosable(isChoosable)

	bool isCanWallHang = KvGetNum(kv, "is_can_wallhang", baseClass.IsCanWallHang()) > 0
	newClass.SetCanWallHang(isCanWallHang)

	int healthForKill = KvGetNum(kv, "hp_for_kill", baseClass.GetHealthForKill())
	newClass.SetHealthForKill(healthForKill)

	char weaponSetName[ITEMSET_NAME_LENGTH]
	KvGetString(kv, "itemset", weaponSetName, ITEMSET_NAME_LENGTH, "")
	if (StrEqual(weaponSetName, ""))
	{
		newClass.SetWeaponSet(EmptyWeaponSet)
	}
	else
	{
		int weaponSetId = WeaponSets.FindByName(weaponSetName)
		newClass.SetWeaponSet(WeaponSet(weaponSetId))
	}
}

public LCC_Event_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
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