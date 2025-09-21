#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

namespace zaphod
{
template<typename Flag>
class Flags
{
	static_assert(std::is_enum_v<Flag>);
	static_assert(std::is_same_v<std::underlying_type_t<Flag>, uint32_t>);
	static_assert(static_cast<std::underlying_type_t<Flag>>(Flag::EMPTY) == 0, "The enum's 'EMPTY' value must be 0.");

  public:
	Flags(): flags(0) {}
	Flags(uint32_t flags): flags(flags) {}
	Flags(std::vector<Flag> flags) : flags(0)
	{
		for (const auto& flag : flags)
			this->flags |= (1 << static_cast<uint32_t>(flag));
	}

	void updateFlags(const uint32_t flags) { this->flags |= flags; }
	void operator|=(const uint32_t flags) { this->flags |= flags; }
	void updateFlags(const Flags& other) { flags |= other.flags; }
	void operator|=(const Flags& other) { flags |= other.flags; }

	void setFlags(const uint32_t flags) { this->flags = flags; }
	void operator=(const uint32_t flags) { this->flags = flags; }
	void setFlags(const Flags& other) { flags = other.flags; }
	//void operator=(const Flags& other) { flags = other.flags; }
	void setFlag(const Flag flag, const bool enable = true)
	{
		if (enable)
			flags |= (1 << static_cast<uint32_t>(flag));
		else
			flags &= ~(1 << static_cast<uint32_t>(flag));
	}
	void operator|=(const Flag flag) { flags |= (1 << static_cast<uint32_t>(flag)); }
	void operator=(const Flag flag) { flags = (1 << static_cast<uint32_t>(flag)); }

	void operator&=(const uint32_t flag) { flags &= flag; }
	void operator&=(const Flags& other) { flags &= other.flags; }
	void operator&=(const Flag flag) { flags &= (1 << static_cast<uint32_t>(flag)); }

	void unsetFlags(const uint32_t flag) { flags &= ~flag; }
	void unsetFlags(const Flags& other) { flags &= ~other.flags; }
	void unsetFlag(const Flag flag) { flags &= ~(1 << static_cast<uint32_t>(flag)); }

	uint32_t operator&(const uint32_t flag) const { return flags & flag; }
	Flags	 operator&(const Flags& other) const { return Flags(flags & other.flags); }
	Flags	 operator&(const Flag flag) const { return Flags(flags & (1 << static_cast<uint32_t>(flag))); }

	bool checkFlags(const uint32_t flag) const { return (flags & flag) != 0; }
	bool checkFlags(const Flags& other) const { return (flags & other.flags) != 0; }
	bool checkFlag(const Flag flag) const { return (flags & (1 << static_cast<uint32_t>(flag))) != 0; }

	void reset() { flags = 0; }

  private:
	uint32_t flags = 0;
};
}	 // namespace zaphod
