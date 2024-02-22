#pragma once
#include <array>
#include <functional>
#include <string>
#include <cmath>
#include <numbers>

constexpr auto PI = std::numbers::pi;

enum EasingMode : UInt32
{
	kMode_EaseIn = 0, kMode_EaseOut, kMode_EaseInOut,
	kModeMax = kMode_EaseInOut, kNumEasingModes
};

template <typename T>	//numeric template
struct EasingFunction_Base
{
protected:
	~EasingFunction_Base() = default;

public:
	//Returns Ease In/Out/InOut functions in an array.
	virtual std::array<std::function<T(T)>, kNumEasingModes> GetModes() = 0;

	virtual T EaseIn(T x) = 0;
	virtual T EaseOut(T x) = 0;
	virtual T EaseInOut(T x) = 0;
};

//Copied easing function code from http://robertpenner.com/easing/

template <typename T>
struct Sine_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return 1 - cos((x * PI) / 2);
	}
	static T EaseOut(T x)
	{
		return sin((x * PI) / 2);
	}
	static T EaseInOut(T x)
	{
		return -(cos(PI * x) - 1) / 2;
	}
};

template <typename T>
struct Cubic_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return x * x * x;
	}
	static T EaseOut(T x)
	{
		return 1 - pow(1 - x, 3);
	}
	static T EaseInOut(T x)
	{
		return x < 0.5 ? (4 * x * x * x) : (1 - pow(-2 * x + 2, 3) / 2);
	}
};

template <typename T>
struct Quint_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return x * x * x * x * x;
	}
	static T EaseOut(T x)
	{
		return 1 - pow(1 - x, 5);
	}
	static T EaseInOut(T x)
	{
		return x < 0.5 ? (16 * x * x * x * x * x) : (1 - pow(-2 * x + 2, 5) / 2);
	}
};

template <typename T>
struct Circ_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		x = pow(x, 2);
		x = std::min<T>(x, 1); // prevent returning -INF by having 1 - 1.blah = negative number in sqrt.
		return 1 - sqrt(1 - x);
	}
	static T EaseOut(T x)
	{
		--x;
		x = pow(x, 2);
		x = std::min<T>(x, 1); // don't want to have negative value in sqrt
		return sqrt(1 - x);
	}
	static T EaseInOut(T x)
	{
		if (x < 0.5)
		{
			if (x < 0.0) // shouldn't ever receive negative values, but w/e
				x = 0.0;
			return (1 - sqrt(1 - pow(2 * x, 2))) / 2;
		}
		// else
		x = -2 * x + 2;
		x = pow(x, 2);
		x = std::min<T>(x, 1); // don't want to have negative value in sqrt
		return (sqrt(1 - x) + 1) / 2;
	}
};

template <typename T>
struct Elastic_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		constexpr T c4 = (2 * PI) / 3;
		return x == 0
			? 0
			: x == 1
			? 1
			: -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
	}
	static T EaseOut(T x)
	{
		constexpr T c4 = (2 * PI) / 3;
		return x == 0
			? 0
			: x == 1
			? 1
			: pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
	}
	static T EaseInOut(T x)
	{
		constexpr T c5 = (2 * PI) / 4.5;
		return x == 0
			? 0
			: x == 1
			? 1
			: x < 0.5
			? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2
			: (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
	}
};

template <typename T>
struct Quad_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return x * x;
	}
	static T EaseOut(T x)
	{
		return 1 - (1 - x) * (1 - x);
	}
	static T EaseInOut(T x)
	{
		return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
	}
};

template <typename T>
struct Quart_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return x * x * x * x;
	}
	static T EaseOut(T x)
	{
		return 1 - pow(1 - x, 4);
	}
	static T EaseInOut(T x)
	{
		return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
	}
};

template <typename T>
struct Expo_Ease : EasingFunction_Base<T>
{
	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return x == 0 ? 0 : pow(2, 10 * x - 10);
	}
	static T EaseOut(T x)
	{
		return x == 1 ? 1 : 1 - pow(2, -10 * x);
	}
	static T EaseInOut(T x)
	{
		return x == 0
			? 0
			: x == 1
			? 1
			: x < 0.5 ? pow(2, 20 * x - 10) / 2
			: (2 - pow(2, -20 * x + 10)) / 2;
	}
};

template <typename T>
struct Back_Ease : EasingFunction_Base<T>
{
	static constexpr T c1 = 1.70158;
	static constexpr T c3 = c1 + 1;
	static constexpr T c2 = c1 * 1.525;

	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return c3 * x * x * x - c1 * x * x;
	}
	static T EaseOut(T x)
	{
		return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
	}
	static T EaseInOut(T x)
	{
		return x < 0.5
			? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
			: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
	}
};

template <typename T>
struct Bounce_Ease : EasingFunction_Base<T>
{
	static constexpr T n1 = 7.5625;
	static constexpr T d1 = 2.75;

	static std::array<std::function<T(T)>, kNumEasingModes> GetModes()
	{
		return { EaseIn, EaseOut, EaseInOut };
	}

	static T EaseIn(T x)
	{
		return 1 - EaseOut(1 - x);
	}
	static T EaseOut(T x)
	{
		if (x < 1 / d1) {
			return n1 * x * x;
		}
		if (x < 2 / d1) {
			return n1 * (x -= 1.5 / d1) * x + 0.75;
		}
		if (x < 2.5 / d1) {
			return n1 * (x -= 2.25 / d1) * x + 0.9375;	//todo: check for UB?
		}
		return n1 * (x -= 2.625 / d1) * x + 0.984375;
	}
	static T EaseInOut(T x)
	{
		return x < 0.5
			? (1 - EaseOut(1 - 2 * x)) / 2
			: (1 + EaseOut(2 * x - 1)) / 2;
	}
};


template <typename T>
std::array<std::function<T(T)>, kNumEasingModes> GetEasingFuncsFromStr(const std::string& str)
{
	if (str.starts_with("sin"))
	{
		return Sine_Ease<T>::GetModes();
	}
	if (str.starts_with("cub"))	//match cube/cubic
	{
		return Cubic_Ease<T>::GetModes();
	}
	if (str.starts_with("quint"))
	{
		return Quint_Ease<T>::GetModes();
	}
	if (str.starts_with("circ"))
	{
		return Circ_Ease<T>::GetModes();
	}
	if (str.starts_with("elastic"))
	{
		return Elastic_Ease<T>::GetModes();
	}
	if (str.starts_with("quad"))
	{
		return Quad_Ease<T>::GetModes();
	}
	if (str.starts_with("quart"))
	{
		return Quart_Ease<T>::GetModes();
	}
	if (str.starts_with("expo"))
	{
		return Expo_Ease<T>::GetModes();
	}
	if (str.starts_with("back"))
	{
		return Back_Ease<T>::GetModes();
	}
	if (str.starts_with("bounce"))
	{
		return Bounce_Ease<T>::GetModes();
	}
	return { nullptr, nullptr, nullptr };
}

enum EasingFuncs : UInt32
{
	Sine = 0,
	Quad,
	Cubic,
	Quart,
	Quint,
	Circ,
	Expo,
	Back,
	Elastic,
	Bounce
};

template <typename T>
std::array<std::function<T(T)>, kNumEasingModes> GetEasingFuncsFromNum(const EasingFuncs num)
{
	switch (num)
	{
	case Sine:
		return Sine_Ease<T>::GetModes();
	case Quad:
		return Quad_Ease<T>::GetModes();
	case Cubic:
		return Cubic_Ease<T>::GetModes();
	case Quart:
		return Quart_Ease<T>::GetModes();
	case Quint:
		return Quint_Ease<T>::GetModes();
	case Circ:
		return Circ_Ease<T>::GetModes();
	case Expo:
		return Expo_Ease<T>::GetModes();
	case Back:
		return Back_Ease<T>::GetModes();
	case Elastic:
		return Elastic_Ease<T>::GetModes();
	case Bounce:
		return Bounce_Ease<T>::GetModes();
	default:
		return { nullptr, nullptr, nullptr };
	}
}