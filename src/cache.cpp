
#include "cache.h"

#include "settings.h"
#include "skeleton.h"
#include "text.h"
#include "file.h"

namespace Cache {

	void Map::Insert(const std::string& filename, float value, bool isText, bool forceInsert) noexcept
	{
		if (filename.empty()) {
			return;
		}

		auto& info = cacheMap[File::GetRelativeDir(filename)];

		if (info.isCached && !forceInsert) {
			return;
		}

		if (value != ZeroValue) {

			info.height = value;
			info.isText = isText;

			_DMESSAGE("File : %s Height : %.2f %s", filename.c_str(), value, info.height != ZeroValue ? "added to cache." : "not added to cache!");

			saved = false;
		}

		info.path = filename;
		info.isCached = true;
	}

	void Map::Erase(const std::string& filename) noexcept
	{
		if (filename.empty()) {
			return;
		}

		cacheMap.erase(File::GetRelativeDir(filename));

		saved = false;
	}

	float Map::Find(const std::string& filename) noexcept
	{
		if (filename.empty()) {
			return ZeroValue;
		}

		auto& info = cacheMap[File::GetRelativeDir(filename)];

		if (!info.isCached) {

			if ((info.height = Skeleton::GetHeightFromSkeleton(filename)) == ZeroValue) {

				info.height = Text::GetHeightFromText(filename);
				info.isText = true; // info.height != ZeroValue;
			}

			saved = false;

			info.path = filename;
			info.isCached = true;
		}

		//_DMESSAGE("%s : %s / %s (%f, %s, %i, %i)", __FUNCTION__, File::GetRelativeDir(filename).c_str(), filename.c_str(), info.height, info.path.c_str(), info.isText, info.isCached);
		//_DMESSAGE("%f", info.height);

		return info.height;
	}

	bool Map::IsText(const std::string& filename) noexcept
	{
		if (filename.empty()) {
			return false;
		}

		auto& it = cacheMap.find(File::GetRelativeDir(filename));

		if (it != cacheMap.end()) {
			return it->second.isText;
		}

		return true;
	}
		
	bool Map::ReadHeader(std::ifstream& ifs) noexcept
	{
		std::uint32_t hdr{};
		std::uint32_t version{};

		ifs.read(as_bytes(hdr), sizeof hdr);
		ifs.read(as_bytes(version), sizeof version);

		return (ifs.good() && hdr == Header && version == Version);
	}

	bool Map::Load() noexcept
	{
		if (!Settings::Ini::GetSingleton().Get_bCache()) {
			return false;
		}

		std::ifstream ifs{ CacheFilename, std::ios_base::binary };

		Error err{ Error::Success };

		if (!ifs || !ReadHeader(ifs)) {

			err = Error::NotOpen;
		}

		std::uint32_t found{};

		if (err == Error::Success) {

			std::string filename;
			std::uint16_t lenFilename{};

			while (true) {

				ifs.read(as_bytes(lenFilename), sizeof lenFilename);

				if (lenFilename == 0 || lenFilename >= MaxFilename) {

					err = Error::Filename;

					break;
				} 
				else {

					filename.assign(lenFilename, '\0');

					std::uint32_t errorRead{};

					for (auto& c : filename) {
						
						ifs.read(&c, sizeof c);

						if (c == '\0') {
							++errorRead;
						}
					}

					//_DMESSAGE("%s %i", filename.c_str(), errorRead);

					if (errorRead > 0) {
						filename.clear();
					}

					if (Find(filename) != ZeroValue) {
						++found;
					}
				}

				//_DMESSAGE("%i / %i", ifs.eof(), ifs.fail());
			
				if (ifs.eof()) {

					break;
				}

				if (ifs.fail()) {

					err = Error::ReadWrite;

					break;
				}
			}
		}

		if (err == Error::Success) {

			_DMESSAGE("Cache loaded successfully!");

			saved = true;
		}
		else {

			_DMESSAGE("Cache read with errors! (error code %i)", static_cast<std::int32_t>(err));
		}

		_DMESSAGE("Records found in the cache : %i", cacheMap.size());
		_DMESSAGE("Custom heights found : %i", found);

		return err == Error::Success;
	}

	bool Map::WriteHeader(std::ofstream& ofs) noexcept
	{
		std::uint32_t hdr{ Header }, version{ Version };

		ofs.write(as_bytes(hdr), sizeof hdr);
		ofs.write(as_bytes(version), sizeof version);

		return ofs.good();
	}

	bool Map::Save() noexcept
	{
		if (!Settings::Ini::GetSingleton().Get_bCache() || saved) {
			return false;
		}

		std::ofstream ofs(CacheFilename, std::ios_base::binary);

		Error err{ Error::Success };

		if (!ofs || !WriteHeader(ofs)) {
			err = Error::NotOpen;
		}

		if (err == Error::Success) {

			std::uint16_t lenFilename{};

			for (auto& map : cacheMap) {

				auto& info = map.second;

				lenFilename = info.path.length();

				if (lenFilename == 0 || lenFilename >= MaxFilename) {

					err = Error::Filename;

					break;
				}
				else {

					ofs.write(as_bytes(lenFilename), sizeof lenFilename);
					ofs.write(info.path.c_str(), lenFilename);
				}

				if (ofs.fail()) {

					err = Error::ReadWrite;

					break;
				}
			}
		}

		if (err == Error::Success) {

			_DMESSAGE("Cache saved successfully!");

			saved = true;
		}
		else {

			_DMESSAGE("Cache saved with errors! (error code %i)", static_cast<std::int32_t>(err));
		}

		return err == Error::Success;
	}
}
