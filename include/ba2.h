#pragma once

#include "version.h"

#include "externals.h"

namespace BA2 {

	template<typename T>
	std::ifstream& operator>>(std::ifstream& ifs, T& value) { ifs.read(as_bytes(value), sizeof value); return ifs; }

	enum class Error {

		NotOpened = -5,
		AlreadyOpened,
		Header,
		Entry,
		StringTbl,
		Success
	};

	enum class Type : std::uint32_t {

		GNRL = 0x4c524e47,
		DX10 = 0x30315844,
		XBOX = 0x584f4258
	};

	class Base {

	public:

		Base() noexcept {}
		Base(const std::uint64_t address) noexcept 
			: next_addr{ address } {}

		virtual ~Base() noexcept {}

		[[nodiscard]] operator bool() const noexcept { return isValid; }

		virtual void Read(std::ifstream& ifs, std::uint64_t addr) noexcept {}

		void SetAddr(const std::uint64_t addr) noexcept { next_addr = addr; }
		void SetNextAddr(std::ifstream& ifs) noexcept { isValid = true; next_addr = ifs.tellg(); }
		[[nodiscard]] const std::uint64_t& GetNextAddr() const noexcept { return next_addr; }

	protected:

		std::uint64_t next_addr{};
		bool isValid{};
	};

	class Header : 
		public Base {

		static const std::uint64_t Address_Entry = 24;
		static const std::uint32_t Btdx_Hdr = 0x58445442;
		static const std::uint32_t Version_Hdr = 0x01;

	public:

		[[nodiscard]] const Type& GetType() const noexcept { return type; }
		[[nodiscard]] const std::uint32_t& GetCountFiles() const noexcept { return CountFiles; }
		[[nodiscard]] const std::uint64_t& GetAddrEntry() const noexcept { return Address_Entry; }
		[[nodiscard]] const std::uint64_t& GetAddrStringTable() const noexcept { return AddrStringTable; }

		void Read(std::ifstream& ifs, std::uint64_t addr) noexcept override;

	private:

		std::vector<std::uint32_t> Magic = { 0, 0 };
		Type type{ Type::GNRL };
		std::uint32_t CountFiles{};
		std::uint64_t AddrStringTable{};
	};

	class Entry : 
		public Base {

		static const std::uint32_t Eof = 0xbaadf00d;

	public:

		using Base::Base;

		[[nodiscard]] const std::uint64_t& GetFileAddr() const noexcept { return AddrFile; }
		[[nodiscard]] const std::uint32_t& GetSizeComp() const noexcept { return SizeComp; }
		[[nodiscard]] const std::uint32_t& GetSizeUncomp() const noexcept { return SizeUnComp; }

		void Read(std::ifstream& ifs, std::uint64_t addr) noexcept override;

	private:

		std::uint32_t Unknown0{};	
		std::uint32_t Extension{};
		std::uint32_t Unknown1{};	
		std::uint16_t Unknown2{};	
		std::uint16_t SizeEntry{};	
		std::uint64_t AddrFile{};	
		std::uint32_t SizeComp{};
		std::uint32_t SizeUnComp{};
		std::uint32_t SignEof{};
	};

	class StringTable : 
		public Base {

	public:

		using Base::Base;

		[[nodiscard]] const std::string& GetFilename() const noexcept { return filename; }

		void Read(std::ifstream& ifs, std::uint64_t addr) noexcept override;

	private:

		std::uint16_t len_filename{};
		std::string filename;
	};

	class Reader : 
		public Base {

	public:

		using Entries = std::vector<Entry>;
		using StringsTable = std::vector<StringTable>;

		Reader() {}
		Reader(const std::string& Filename) noexcept { Open(Filename); }

		void Open(const std::string& Filename) noexcept;
		void Close() noexcept { ifs_ba2.close(); }

		[[nodiscard]] Header& GetHeader() noexcept { return hdr; }
		[[nodiscard]] const Error& GetError() const noexcept { return err; }

		[[nodiscard]] Entries& GetEntries() noexcept { return entries; }
		[[nodiscard]] StringsTable& GetStringsTable() noexcept { return strsTbl; }

	private:

		[[nodiscard]] bool GetEntry(Entry& entry) noexcept;
		[[nodiscard]] bool GetStringTable(StringTable& tbl) noexcept;

		Error err{ Error::NotOpened };
		std::ifstream ifs_ba2;
		std::string filename;
		Header hdr;
		Entries entries;
		StringsTable strsTbl;
	};
}
