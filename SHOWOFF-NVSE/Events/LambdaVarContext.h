#pragma once

// Taken from JohnnyGuitar, made by Kormakur.

class Script;

typedef void (*_CaptureLambdaVars)(Script* scriptLambda);
extern _CaptureLambdaVars CaptureLambdaVars;
typedef void (*_UncaptureLambdaVars)(Script* scriptLambda);
extern _UncaptureLambdaVars UncaptureLambdaVars;

class LambdaVarContext
{
	Script* scriptLambda;
public:
	LambdaVarContext(const LambdaVarContext& other) = delete;
	LambdaVarContext& operator=(const LambdaVarContext& other) = delete;
	explicit LambdaVarContext(Script* scriptLambda);
	LambdaVarContext(LambdaVarContext&& other) noexcept;
	LambdaVarContext& operator=(LambdaVarContext&& other) noexcept;
	~LambdaVarContext();

	[[nodiscard]] Script* Get() const { return scriptLambda; }

	// copied from JIP
	bool operator==(const LambdaVarContext& rhs) const { return scriptLambda == rhs.scriptLambda; }
	bool operator!=(const LambdaVarContext& rhs) const { return scriptLambda != rhs.scriptLambda; }
	bool operator<(const LambdaVarContext& rhs) const { return scriptLambda < rhs.scriptLambda; }
};

typedef LambdaVarContext Function;
