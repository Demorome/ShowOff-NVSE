#include "LambdaVarContext.h"

// Taken from JohnnyGuitar, made by Kormakur.

LambdaVarContext::LambdaVarContext(Script* scriptLambda) : scriptLambda(scriptLambda)
{
	if (scriptLambda)
		CaptureLambdaVars(scriptLambda);
}

LambdaVarContext::LambdaVarContext(LambdaVarContext&& other) noexcept : scriptLambda(other.scriptLambda)
{
	other.scriptLambda = nullptr;
}

LambdaVarContext& LambdaVarContext::operator=(LambdaVarContext&& other) noexcept
{
	if (this == &other)
		return *this;
	if (this->scriptLambda)
		UncaptureLambdaVars(this->scriptLambda);
	scriptLambda = other.scriptLambda;
	other.scriptLambda = nullptr;
	return *this;
}

LambdaVarContext::~LambdaVarContext()
{
	if (this->scriptLambda)
		UncaptureLambdaVars(this->scriptLambda);
}

