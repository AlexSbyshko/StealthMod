public TEST_Init()
{
	RegAdminCmd("sm_t", TEST_Cmd_Test, ADMFLAG_ROOT)
	RegAdminCmd("sm_t2", TEST_Cmd_Test2, ADMFLAG_ROOT)
}

public Action:TEST_Cmd_Test(client, args)
{
	PrintToChatAll("TEST1")
	ReadClassConfigs()
	return Plugin_Handled
}

public Action:TEST_Cmd_Test2(client, args)
{
	PrintToChatAll("TEST2")
	STMPlayer player = STMPlayers.Get(client)
	STMClass myclass = player.GetClass()
	char className[32]
	myclass.GetName(className, 32)
	PrintToChatAll("my class: %s (slient %i)", className, client)
	return Plugin_Handled
}