public TEST_Init()
{
	RegAdminCmd("sm_t", TEST_Cmd_Test, ADMFLAG_ROOT)
	RegAdminCmd("sm_t2", TEST_Cmd_Test2, ADMFLAG_ROOT)
}

public Action:TEST_Cmd_Test(client, args)
{
	PrintToChatAll("TEST1")
	ShowBuyMenu(client)
	//ReadWeaponsConfigs()
	//ReadClassConfigs()
	//SwapTeams()
	return Plugin_Handled
}

public Action:TEST_Cmd_Test2(client, args)
{
	ReadWeaponsConfigs()
	return Plugin_Handled
}

public Action:CS_OnCSWeaponDrop(client, weaponIndex)
{
	PrintToChatAll("client %i drop %i", client, weaponIndex)
	return Plugin_Continue
}