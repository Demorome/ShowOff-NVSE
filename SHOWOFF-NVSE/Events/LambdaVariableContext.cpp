#include "LambdaVariableContext.h"

// Taken from JohnnyGuitar, made by Kormakur.

LambdaVariableContext::LambdaVariableContext(Script* scriptLambda) : scriptLambda(scriptLambda)
{
	if (scriptLambda)
		CaptureLambdaVars(scriptLambda);
}

LambdaVariableContext::LambdaVariableContext(LambdaVariableContext&& other) noexcept : scriptLambda(other.scriptLambda)
{
	other.scriptLambda = nullptr;
}

LambdaVariableContext& LambdaVariableContext::operator=(LambdaVariableContext&& other) noexcept
{
	if (this == &other)
		return *this;
	if (this->scriptLambda)
		UncaptureLambdaVars(this->scriptLambda);
	scriptLambda = other.scriptLambda;
	other.scriptLambda = nullptr;
	return *this;
}

LambdaVariableContext::~LambdaVariableContext()
{
	if (this->scriptLambda)
		UncaptureLambdaVars(this->scriptLambda);
}

