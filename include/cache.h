#pragma once

#include "version.h"
#include "externals.h"

namespace Cache {

	constexpr std::uint32_t Header = 0x42534848; // = HHSB
	constexpr std::uint32_t Version = 1;

	constexpr char* CacheFilename = "Data\\F4SE\\Plugins\\HHS.bin";

	constexpr std::uint16_t MaxFilename = 512;

	enum class Error {

		ReadWrite = -3,
		Filename,
		NotOpen,
		Success
	};

	struct InfoCache {

		float height{ ZeroValue };
		std::string path;
		bool isText{};
		bool isCached{};
	};

	class Map {

	public:

		[[nodiscard]] static Map& GetSingleton() noexcept { 
			static Map instance;
			return instance; 
		}

		void Insert(const std::string& filename, float value, bool isText = false, bool forceInsert = false) noexcept;
		void Erase(const std::string& filename) noexcept;

		[[nodiscard]] float Find(const std::string& filename) noexcept;

		[[nodiscard]] bool IsText(const std::string& filename) noexcept;

		[[nodiscard]] bool Load() noexcept;
		[[nodiscard]] bool Save() noexcept;

	private:

		Map() noexcept {}
		~Map() noexcept {}

		Map(const Map&) = delete;
		Map(Map&&) = delete;

		Map& operator=(const Map&) = delete;
		Map& operator=(Map&&) = delete;

		[[nodiscard]] bool ReadHeader(std::ifstream& ifs) noexcept;
		[[nodiscard]] bool WriteHeader(std::ofstream& ofs) noexcept;

		std::unordered_map<std::string, InfoCache> cacheMap;

		bool saved{};
	};
}
