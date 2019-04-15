#include "../engine/TableHashFunction.h"

uint tableHashFunc_Uint(const uint& key, uint tableSlots)
{
	return key % tableSlots;
}

uint tableHashFunc_HashString(const HashString& key, uint tableSlots)
{
	return key.GetHash() % tableSlots;
}