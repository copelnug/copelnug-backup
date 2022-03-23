#ifndef COPELNUG_BACKUP_LIB_FILE_HASH_HPP_INCLUDED
#define COPELNUG_BACKUP_LIB_FILE_HASH_HPP_INCLUDED
#include <filesystem>

namespace CopelnugBackup
{
	namespace MD5
	{
		using Hash = __uint128_t; // TODO Find something better? Default value?

		Hash hash(const std::filesystem::path& file);
	}
}

#endif