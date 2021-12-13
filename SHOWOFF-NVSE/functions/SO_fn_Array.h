#pragma once


//ripped code from FOSE's ListAddForm
DEFINE_COMMAND_ALT_PLUGIN_EXP(ListAddArray, AddArrayToFormList, "", false, kNVSEParams_OneForm_OneArray_OneOptionalIndex);
bool Cmd_ListAddArray_Execute(COMMAND_ARGS)
{
	*result = 1;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto const pListForm = DYNAMIC_CAST(eval.GetNthArg(0)->GetTESForm(), TESForm, BGSListForm);
		NVSEArrayVar* inArr = eval.GetNthArg(1)->GetArrayVar();
		SInt32 index = eListEnd;
		if (eval.NumArgs() == 3)
			index = eval.GetNthArg(2)->GetInt();
		if (!pListForm || !inArr) return true;
		
		UInt32 size = g_arrInterface->GetArraySize(inArr);
		auto elements = std::make_unique<NVSEArrayElement[]>(size);
		g_arrInterface->GetElements(inArr, elements.get(), nullptr);

		auto const Add_Array_Element_To_FormList = [&](int elemIndex)
		{
			if (elements[elemIndex].Form() == nullptr) return true;  //acts as a continue.
			UInt32 const addedAtIndex = pListForm->AddAt(elements[elemIndex].Form(), index);
			if (addedAtIndex == eListInvalid)
			{
				*result = 0;
				return false;
			}
			return true;
		};

		if (index == eListEnd)
		{
			for (int i = 0; i < size; i++) {
				if (!Add_Array_Element_To_FormList(i)) break;
			}
		}
		else
		{
			for (int i = size - 1; i >= 0; i--) {
				if (!Add_Array_Element_To_FormList(i)) break;
			}
		}
	}
	return true;
}













#ifdef _DEBUG




DEFINE_COMMAND_PLUGIN_EXP(ar_Test, "debug array func", false, kNVSEParams_OneArray /*Params_OneArray*/);

bool Cmd_ar_Test_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto const arrToken = eval.GetNthArg(0);
		auto const arr = arrToken->GetArrayVar();
		auto const size = g_arrInterface->GetArraySize(arr);
		Console_Print("ar_Test >> size of array: %u", size);
	}
	return true;
}


DEFINE_COMMAND_PLUGIN(Ar_GetInvalidRefs, "", false, kParams_OneArrayID);  //failed experiment
bool Cmd_Ar_GetInvalidRefs_Execute(COMMAND_ARGS)
{
	//Code stolen off TommInfinite's Ar_HasInvalidRefs.
	*result = 0;
	UInt32 arrID;
	int iMDArrayNumber;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID, &iMDArrayNumber)) return true;

	NVSEArrayVar* inArr = g_arrInterface->LookupArrayByID(arrID);
	if (!inArr) return true;
	NVSEArrayElement ElementArray;
	NVSEArrayElement* elements;

	UInt32 size;
	size = g_arrInterface->GetArraySize(inArr);  //Multidimensional arrays are not accounted for.

	//****Will this work with arrays of different types?

	//Console_Print("Array size is  is >>> %d", size);

	if (size < 1) return true;
	
	NVSEArrayVar* InvalidRefArr = g_arrInterface->CreateArray(NULL, 0, scriptObj);
	
	elements = new NVSEArrayElement[size];
	g_arrInterface->GetElements(inArr, elements, NULL);

	for (int i = 0; i < size; i++)
	{
		//**I messed around and added definitions for ArrayElementR and more from JIP. Could cause issues.
		//Check out JIP's GetArrayValue.
		ArrayElementR resElement;

		if (GetElement((NVSEArrayVar*)arrID, ArrayElementL(i), resElement))  //key is given an int; is this compatible with map arrays? Probably redundant anyways.
		{
			if (elements[i].GetType() == 2)  //if ref-type
			{
				Console_Print("helppppp");
				TESForm* CheckRef = elements[i].Form();
				if (!CheckRef) //form is invalid
				{
					if (resElement.form)
						Console_Print("Maybe?");
					//g_arrInterface->AppendElement(InvalidRefArr, ArrayElementL(resElement.raw));
					Console_Print("plzzzzz");
				}
			}
		}

	}
	delete[] elements;

	g_arrInterface->AssignCommandResult(InvalidRefArr, result);
	return true;
}

#endif