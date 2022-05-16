#pragma once

DEFINE_COMMAND_PLUGIN(GetPosArray, "Returns an array of the 3 axis positions of a reference", 
	true, nullptr);
bool Cmd_GetPosArray_Execute(COMMAND_ARGS)
{
	ArrayElementL const posElems[] =
		{ thisObj->posX, thisObj->posY, thisObj->posZ };
	auto const arr = g_arrInterface->CreateArray(posElems, 3, scriptObj);
	g_arrInterface->AssignCommandResult(arr, result);
	return true;
}