#pragma once



#ifdef _DEBUG

DEFINE_COMMAND_PLUGIN(Ar_GetInvalidRefs, "", 0, 2, kParams_OneArray);  //failed experiment
bool Cmd_Ar_GetInvalidRefs_Execute(COMMAND_ARGS)
{
	//Code stolen off TommInfinite's Ar_HasInvalidRefs.
	*result = 0;
	UInt32 arrID;
	int iMDArrayNumber;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID, &iMDArrayNumber)) return true;

	NVSEArrayVar* inArr = ArrIfc->LookupArrayByID(arrID);
	if (!inArr) return true;
	NVSEArrayElement ElementArray;
	NVSEArrayElement* elements;

	UInt32 size;
	size = ArrIfc->GetArraySize(inArr);  //Multidimensional arrays are not accounted for.

	//****Will this work with arrays of different types?

	//Console_Print("Array size is  is >>> %d", size);

	if (size < 1)
	{
		return true;
	}
	
	NVSEArrayVar* InvalidRefArr = ArrIfc->CreateArray(NULL, 0, scriptObj);
	
	elements = new NVSEArrayElement[size];
	ArrIfc->GetElements(inArr, elements, NULL);

	for (int i = 0; i < size; i++)
	{
		//**I messed around and added definitions for ArrayElementR and more from JIP. Could cause issues.
		//Check out JIP's GetArrayValue.
		ArrayElementR resElement;

		if (GetElement((NVSEArrayVar*)arrID, ArrayElementL(i), resElement))
		{
			if (elements[i].GetType() == 2)  //if ref-type
			{
				Console_Print("helppppp");
				TESForm* CheckRef = elements[i].Form();
				if (!CheckRef) //form is invalid
				{
					if (resElement.form)
						Console_Print("Maybe?");
					//ArrIfc->AppendElement(InvalidRefArr, ArrayElementL(resElement.raw));
					Console_Print("plzzzzz");
				}
			}
		}

	}
	delete[] elements;

	ArrIfc->AssignCommandResult(InvalidRefArr, result);
	return true;
}

#endif