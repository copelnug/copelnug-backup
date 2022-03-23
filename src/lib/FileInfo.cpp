#include "FileInfo.hpp"
#include <filesystem>

CopelnugBackup::FileInfo CopelnugBackup::FileInfo::from(const std::filesystem::path& path, const std::filesystem::path& base)
{
	auto target = canonical(path);

	FileInfo info;
	info.relative = std::filesystem::relative(target, canonical(base));
	info.file_size = std::filesystem::file_size(target);
	info.last_mod = std::filesystem::last_write_time(target);
	info.hash = MD5::hash(target);
	return info;
}