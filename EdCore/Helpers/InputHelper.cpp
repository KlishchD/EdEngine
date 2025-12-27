#include "EdCore.h"
#include "InputHelper.h"

InputEventHandle InputHelper::GenerateHandle()
{
    static InputEventHandle handle = 0;
    return handle++;
}