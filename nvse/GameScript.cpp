#include "GameAPI.h"
#include "GameScript.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "CommandTable.h"
#include "utility.h"

void Script::RefVariable::Resolve(ScriptEventList *eventList)
{
	if (varIdx && eventList)
	{
		ScriptVar *var = eventList->GetVariable(varIdx);
		if (var) form = TESForm::GetFormByNumericID(*(uint32_t*)&var->data);
	}
}

ScriptEventList* Script::CreateEventList(void)
{
	return ThisCall<ScriptEventList*>(0x005ABF60, this);	// 4th sub above Script::Execute (was 1st above in Oblivion) Execute is the second to last call in Run
}

uint32_t ScriptBuffer::GetRefIdx(Script::RefVariable *refVar)
{
	return refVars.GetIndex(refVar);
}

/******************************
 Script
******************************/

class ScriptVarFinder
{
	const char	*m_varName;

public:
	ScriptVarFinder(const char *varName) : m_varName(varName) {}

	bool Accept(VariableInfo *varInfo)
	{
		return StrEqualCI(varInfo->name.m_data, m_varName);
	}
};

VariableInfo *Script::GetVariableByName(const char *varName)
{
	ListNode<VariableInfo> *varIter = varList.Head();
	VariableInfo *varInfo;
	do
	{
		varInfo = varIter->data;
		if (varInfo && StrEqualCI(varName, varInfo->name.m_data))
			return varInfo;
	}
	while ((varIter = varIter->next));
	return NULL;
}

Script::RefVariable	*Script::GetVariable(uint32_t reqIdx)
{
	uint32_t idx = 1;	// yes, really starts at 1
	if (reqIdx)
	{
		ListNode<RefVariable> *varIter = refList.Head();
		do
		{
			if (idx == reqIdx)
				return varIter->data;
			idx++;
		}
		while ((varIter = varIter->next));
	}
	return NULL;
}

VariableInfo *Script::GetVariableInfo(uint32_t idx)
{
	ListNode<VariableInfo> *varIter = varList.Head();
	VariableInfo *varInfo;
	do
	{
		varInfo = varIter->data;
		if (varInfo && (varInfo->idx == idx))
			return varInfo;
	}
	while ((varIter = varIter->next));
	return NULL;
}

uint32_t Script::AddVariable(TESForm *form)
{
	RefVariable	*refVar = (RefVariable*)GameHeapAlloc(sizeof(RefVariable));
	refVar->name.Set("");
	refVar->form = form;
	refVar->varIdx = 0;

	uint32_t resultIdx = refList.Append(refVar) + 1;
	info.numRefs = resultIdx + 1;
	return resultIdx;
}

void Script::CleanupVariables()
{
	refList.RemoveAll();
}

uint32_t Script::RefVarList::GetIndex(RefVariable *refVar)
{
	uint32_t idx = 0;
	ListNode<RefVariable> *varIter = Head();
	do
	{
		idx++;
		if (varIter->data == refVar)
			return idx;
	}
	while ((varIter = varIter->next));
	return 0;
}

/***********************************
 ScriptLineBuffer
***********************************/

//const char	* ScriptLineBuffer::kDelims_Whitespace = " \t\n\r";
//const char  * ScriptLineBuffer::kDelims_WhitespaceAndBrackets = " \t\n\r[]";

bool ScriptLineBuffer::Write(const void *buf, uint32_t bufsize)
{
	if ((dataOffset + bufsize) >= kBufferSize) return false;
	memcpy(dataBuf + dataOffset, buf, bufsize);
	dataOffset += bufsize;
	return true;
}

bool ScriptLineBuffer::Write32(uint32_t buf)
{
	if ((dataOffset + 4) >= kBufferSize) return false;
	*(uint32_t*)(dataBuf + dataOffset) = buf;
	dataOffset += 4;
	return true;
}

bool ScriptLineBuffer::WriteString(const char *buf)
{
	uint32_t len = StrLen(buf);
	if ((dataOffset + 2 + len) >= kBufferSize) return false;
	uint8_t *dataPtr = dataBuf + dataOffset;
	*(uint16_t*)dataPtr = len;
	memcpy(dataPtr + 2, buf, len);
	dataOffset += 2 + len;
	return true;
}

bool ScriptLineBuffer::Write16(uint16_t buf)
{
	if ((dataOffset + 2) >= kBufferSize) return false;
	*(uint16_t*)(dataBuf + dataOffset) = buf;
	dataOffset += 2;
	return true;
}

bool ScriptLineBuffer::WriteByte(uint8_t buf)
{
	if ((dataOffset + 1) >= kBufferSize) return false;
	*(dataBuf + dataOffset) = buf;
	dataOffset++;
	return true;
}

bool ScriptLineBuffer::WriteFloat(double buf)
{
	if ((dataOffset + 8) >= kBufferSize) return false;
	memcpy(dataBuf + dataOffset, &buf, 8);
	dataOffset += 8;
	return true;
}