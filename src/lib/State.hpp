#ifndef COPELNUG_BACKUP_LIB_STATE_HPP_INCLUDED
#define COPELNUG_BACKUP_LIB_STATE_HPP_INCLUDED
#include <filesystem>
#include <optional>
#include <vector>

#include "FileInfo.hpp"

namespace CopelnugBackup
{
	class State
	{
		using Content = std::vector<FileInfo>;
		public:
			using iterator = Content::iterator;
			using const_iterator = Content::const_iterator;

			// Creation
			static State recursive_list(const std::filesystem::path& path);

			// Accessors
			iterator begin();
			const_iterator begin() const;
			iterator end();
			const_iterator end() const;
			bool empty() const;

		private:
			Content content_;
	};
}

#endif