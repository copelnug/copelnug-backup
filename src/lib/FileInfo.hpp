#ifndef COPELNUG_BACKUP_LIB_FILE_INFO_HPP_INCLUDED
#define COPELNUG_BACKUP_LIB_FILE_INFO_HPP_INCLUDED
#include <filesystem>
#include "FileHash.hpp"

namespace CopelnugBackup
{
	struct FileInfo
	{
		std::filesystem::path relative;
		std::uintmax_t file_size = 0;
		std::filesystem::file_time_type last_mod;
		MD5::Hash hash;

		static FileInfo from(const std::filesystem::path& path, const std::filesystem::path& base);

	};
}

#endif