#pragma once

template<typename EnumT>
constexpr bool has_zero_value()
{
	return false;
}

// Recursive case:
template<typename EnumT, EnumT first, EnumT... rest>
constexpr bool has_zero_value()
{
	using UnderlyingType = std::underlying_type_t<EnumT>;
	if (static_cast<UnderlyingType>(first) == 0)
	{
		return true;
	}
	else
	{
		return has_zero_value<EnumT, rest...>();
	}
}