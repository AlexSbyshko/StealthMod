#define DAMAGE_FILTER_NAME "filter_no_weapons_damage"

public DHD_Init()
{
}

public BS_Event_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
	SpawnDamageFilter()
	AcceptDamageFilterToHostages()
}

SpawnDamageFilter()
{
	new damageFilterEntity = CreateEntityByName("filter_damage_type")
	DispatchKeyValue(damageFilterEntity, "targetname", DAMAGE_FILTER_NAME)
	DispatchKeyValue(damageFilterEntity, "negated", "1")
	DispatchKeyValue(damageFilterEntity, "damagetype", "4098")
	DispatchSpawn(damageFilterEntity)
	ActivateEntity(damageFilterEntity)
}

AcceptDamageFilterToHostages()
{
	new hostageEntity = -1;
	while ((hostageEntity = FindEntityByClassname(hostageEntity, "hostage_entity")) != INVALID_ENT_REFERENCE) 
	{
	    SetVariantString(DAMAGE_FILTER_NAME);
	    AcceptEntityInput(hostageEntity, "SetDamageFilter");
	}
}