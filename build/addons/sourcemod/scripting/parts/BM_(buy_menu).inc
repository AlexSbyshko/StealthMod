public BM_Init()
{

}

void ShowBuyMenu(int client)
{
	Handle menu = CreateMenu(Handler_BuyMenu)
	SetMenuTitle(menu, "Buy item")

	WeaponSet itemSet = Player(client).GetWeaponSet()
	
	for (int i = 0; i < ItemTypes.Count(); i++)
	{
		ItemType itemType = ItemType(i)
		if (itemType.GetSetId() == itemSet.Id)
		{
			char itemTypeName[ITEMTYPE_NAME_LENGTH]
			itemType.GetName(itemTypeName, sizeof(itemTypeName))
			AddMenuItem(menu, itemTypeName, itemTypeName)
		}
	}
	
	SetMenuExitButton(menu, true)
	DisplayMenu(menu, client, 20)
}

public Handler_BuyMenu(Handle:menu, MenuAction:action, client, choice) 
{
	if (action == MenuAction_End)
	{
		CloseHandle(menu)
	}
	else if (action == MenuAction_Select)
	{
		decl String:info[ITEMTYPE_NAME_LENGTH]
		GetMenuItem(menu, choice, info, sizeof(info))
		ShowBuyMenuByType(client, info)
	}
}

void ShowBuyMenuByType(int client, const char[] typeName)
{
	Handle menu = CreateMenu(Handler_BuyMenuByType)
	SetMenuTitle(menu, typeName)

	WeaponSet itemSet = Player(client).GetWeaponSet()
	int typeId = ItemTypes.FindByNameAndSetId(typeName, itemSet.Id)
	
	for (int i = 0; i < itemSet.WeaponsCount; i++)
	{
		Item item = itemSet.GetWeapon(i)
		if (item.IsForPurchase() && item.GetTypeId() == typeId)
		{
			char itemClassName[ITEM_NAME_LENGTH]
			item.GetClassName(itemClassName, sizeof(itemClassName))
			char itemName[ITEM_NAME_LENGTH]
			item.GetName(itemName, sizeof(itemName))
			char teamIdString[8]
			IntToString(item.Id, teamIdString, sizeof(teamIdString));
			AddMenuItem(menu, teamIdString, itemName)
		}
	}
	
	SetMenuExitButton(menu, true)
	DisplayMenu(menu, client, 20)
}

public Handler_BuyMenuByType(Handle:menu, MenuAction:action, client, choice) 
{
	if (action == MenuAction_End)
	{
		CloseHandle(menu)
	}
	else if (action == MenuAction_Select)
	{
		decl String:info[MAX_NAME_LENGTH]
		GetMenuItem(menu, choice, info, sizeof(info))
		int itemId = StringToInt(info)
		GivePlayerItemById(client, itemId)
	}
}

void GivePlayerItemById(int client, int itemId)
{
	char itemClassName[ITEM_NAME_LENGTH]
	Item item = Item(itemId)
	item.GetClassName(itemClassName, sizeof(itemClassName))
	/*int weapon = */
	GivePlayerItem(client, itemClassName)
	
	// I still can not get it to work for CS: GO
	/*if (weapon == -1)
	{
		return
	}
	int ammo = item.GetAmmo()
	if (ammo >= 0)
	{
		int maxClip = GetEntProp(weapon, Prop_Data, "m_iClip1")
		if (maxClip > ammo)
		{
			SetWeaponClip(weapon, ammo)
			SetWeaponAmmo(client, weapon, 0)
		}
		else
		{		
			SetWeaponAmmo(client, weapon, ammo - maxClip)
		}
	}*/
}

stock void SetWeaponClip(int weapon, int value)
{
	SetEntProp(weapon, Prop_Data, "m_iClip1", value)
}

stock void SetWeaponAmmo(int client, int weapon, int value)
{
	PrintToChatAll("set ammo %i", value)
	int ammoTypeOffset = FindSendPropOffs("CBaseCombatWeapon", "m_iPrimaryAmmoType") // FindDataMapOffs(client, "m_iAmmo")
	int ammoType = GetEntData(weapon, ammoTypeOffset);
	//int offset += GetEntProp(weapon, Prop_Data, "m_iPrimaryAmmoType") * 4	
	int ammoOffset = FindSendPropOffs("CCSPlayer", "m_iAmmo");
	SetEntData(client, ammoOffset + (ammoType << 2), value, 4, true)
}