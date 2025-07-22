
#include "ba2.h"

namespace BA2 {

	void Header::Read(std::ifstream& ifs, std::uint64_t addr) noexcept
	{
		ifs.seekg(addr);

		ifs >> Magic[0] >> Magic[1] >> type >> CountFiles >> AddrStringTable;

		if (Magic[0] == Btdx_Hdr) {

			switch (Magic[1]) {

			case Version1:
			case Version7:
			case Version8:

				SetNextAddr(ifs);

				break;

			default:

				_DMESSAGE("Error : BA2 unknown version (%i)!", Magic[1]);

				break;
			}
		}
	}

	void Entry::Read(std::ifstream& ifs, std::uint64_t addr) noexcept
	{
		ifs.seekg(addr);

		ifs >> Unknown0 >> Extension >> Unknown1
			>> Unknown2 >> SizeEntry >> AddrFile
			>> SizeComp >> SizeUnComp >> SignEof;

		if (SignEof == Eof) {
			SetNextAddr(ifs);
		}
	}

	void StringTable::Read(std::ifstream& ifs, std::uint64_t addr) noexcept
	{
		ifs.seekg(addr);

		ifs >> len_filename;

		if (len_filename > 0 && len_filename <= MAX_PATH) {

			filename.assign(len_filename, '\0');

			for (auto& c : filename) {
				ifs.read(&c, sizeof c);
			}

			SetNextAddr(ifs);
		}
	}

	void Reader::Open(const std::string& Filename) noexcept
	{
		if (Filename.empty()) {
			
			err = Error::NotOpened;

			return;
		}

		if (_strcmpi(filename.c_str(), Filename.c_str()) == 0 && ifs_ba2.is_open()) {
	
			err = Error::AlreadyOpened;

			return;
		}

		filename = Filename;
		ifs_ba2.open(Filename, std::ios_base::binary);

		if (!ifs_ba2) {

			err = Error::NotOpened;

			return;
		}

		hdr.Read(ifs_ba2, 0);

		if (!hdr || hdr.GetCountFiles() == 0 || hdr.GetAddrStringTable() == 0) {

			err = Error::Header;

			return;
		}

		Entry entry;
		StringTable str_tbl;

		entries.clear();
		strsTbl.clear();

		entry.SetAddr(hdr.GetAddrEntry());
		str_tbl.SetAddr(hdr.GetAddrStringTable());

		for (std::uint32_t idx{}; idx < hdr.GetCountFiles(); idx++) {

			if (!GetEntry(entry)) {
				return;
			}

			if (!GetStringTable(str_tbl)) {
				return;
			}
		}

		err = Error::Success;

		isValid = true;
	}

	bool Reader::GetEntry(Entry& entry) noexcept
	{
		entry.Read(ifs_ba2, entry.GetNextAddr());

		if (!entry) {
	
			err = Error::Entry;
			
			return false;
		}

		entries.push_back(entry);

		return true;
	}

	bool Reader::GetStringTable(StringTable& tbl) noexcept
	{
		tbl.Read(ifs_ba2, tbl.GetNextAddr());

		if (!tbl) {

			err = Error::StringTbl;

			return false;
		}

		strsTbl.push_back(tbl);

		return true;
	}
}
