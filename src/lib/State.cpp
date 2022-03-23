#include "State.hpp"
#include "FileInfo.hpp"

CopelnugBackup::State CopelnugBackup::State::recursive_list(const std::filesystem::path& path)
{
	State result;
	for(auto& p: std::filesystem::recursive_directory_iterator(path))
	{
		if(p.is_regular_file())
		{
			result.content_.push_back(FileInfo::from(p.path(), path));
		}
	}
	return result;
}
CopelnugBackup::State::iterator CopelnugBackup::State::begin()
{
	return content_.begin();
}
CopelnugBackup::State::const_iterator CopelnugBackup::State::begin() const
{
	return content_.begin();
}
CopelnugBackup::State::iterator CopelnugBackup::State::end()
{
	return content_.end();
}
CopelnugBackup::State::const_iterator CopelnugBackup::State::end() const
{
	return content_.end();
}
bool CopelnugBackup::State::empty() const
{
	return content_.empty();
}