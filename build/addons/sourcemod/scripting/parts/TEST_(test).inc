public TEST_Init()
{
	RegAdminCmd("sm_t", TEST_Cmd_Test, ADMFLAG_ROOT)
	RegAdminCmd("sm_t2", TEST_Cmd_Test2, ADMFLAG_ROOT)
}

public Action:TEST_Cmd_Test(client, args)
{
	PrintToChatAll("TEST1")
	//ReadClassConfigs()
	SwapTeams()
	return Plugin_Handled
}

public Action:TEST_Cmd_Test2(client, args)
{
	PrintToChatAll("TEST2")
	STMClass myclass = Player(client).GetClass()
	char className[32]
	myclass.GetName(className, 32)
	PrintToChatAll("my class: %s (client %i) breath: %i", className, client, Player(client).IsProducesBreathSound())
	return Plugin_Handled
}