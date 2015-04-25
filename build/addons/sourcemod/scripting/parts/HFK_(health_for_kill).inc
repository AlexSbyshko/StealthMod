public HFK_Init()
{
}

public HFK_Event_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
	new attacker = GetClientOfUserId(GetEventInt(event, "attacker"))
	if (attacker)
	{
		int health = GetEntProp(attacker, Prop_Data, "m_iHealth")
		int maxHealth = Player(attacker).GetMaxHealth()
		int healthForKill = Player(attacker).GetHealthForKill()
		int newHealth = health + healthForKill
		if (newHealth > maxHealth)
		{
			newHealth = maxHealth
		}
		SetEntProp(attacker, Prop_Data, "m_iHealth", newHealth)
	}
}