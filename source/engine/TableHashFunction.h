#pragma once

#include "Type.h"
#include "../engine/String.h"

uint tableHashFunc_Uint(const uint& key, uint tableSlots);
uint tableHashFunc_HashString(const HashString& key, uint tableSlots);

