#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/container/allocator.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

#include "FileHash.hpp"
#include "State.hpp"

const std::string OPERATION("backup");


std::string to_hex(__uint128_t val)
{
	uint64_t low = (uint64_t)val;
	uint64_t high = (val >> 64);

	std::ostringstream o;
	o << std::hex << high;
	o << std::hex << low;
	return o.str();
}

int main(int argc, char* argv[])
{
	std::map<std::string, std::function<int()>> operations;

	operations.emplace("hash", [&]() -> int {
		if(argc < 3)
		{
			std::cerr << "Need path argument" << std::endl;
			return 1;
		}

		std::cout << "Listing" << std::endl;
		auto list = CopelnugBackup::State::recursive_list(argv[2]);
		for(auto& file : list)
		{
			std::cout << to_hex(file.hash) << ' ' << file.relative << '\n';
		}
		return 0;
	});
	operations.emplace("dedup", [&]() -> int {
		if(argc < 3)
		{
			std::cerr << "Need path argument" << std::endl;
			return 1;
		}

		// De-dup

		std::cout << "Listing" << std::endl;
		auto list = CopelnugBackup::State::recursive_list(argv[2]);
		std::cout << "Building duplicates" << std::endl;
		std::map<CopelnugBackup::MD5::Hash, std::vector<std::filesystem::path>> entries;
		for(auto& file : list)
		{
			entries[file.hash].emplace_back(file.relative);
		}
		std::cout << "Listing duplicates" << std::endl;
		for(auto& entry : entries)
		{
			if(entry.second.size() <= 1)
				continue; // We don't care.

			std::cout << "Duplicates on " << to_hex(entry.first) << ": ";
			for(auto& p : entry.second)
				std::cout << p << ", ";
			std::cout << "\n";
		}
		return 0;
	});

	operations.emplace("backup", [&]() -> int {
		if(argc < 4)
			return 1;

		std::string source{argv[2]};
		std::string dest{argv[3]};

		std::cout << "# List source" << std::endl;
		auto list_source = CopelnugBackup::State::recursive_list(source);
		std::cout << "# List dest" << std::endl;
		auto list_dest = CopelnugBackup::State::recursive_list(dest);

		std::cout << "# Create dest dirs" << std::endl;
		std::set<std::filesystem::path> dirs;
		for(auto& file : list_source)
		{
			auto dir = file.relative.parent_path();
			if(!dirs.insert(dir).second) // Already done
				continue;

			auto fullpath = dest/dir;
			if(!std::filesystem::exists(fullpath))
			{
				std::cout << "mkdir -p " << fullpath << '\n';
			}
		}

		std::cout << "# Build maps" << std::endl;
		// Need to specify allocator because of bug in boost
		using bimap = boost::bimap<std::filesystem::path, boost::bimaps::multiset_of<CopelnugBackup::MD5::Hash>, boost::container::allocator<int>>;
		using entry = bimap::value_type;
		bimap map_source;
		bimap map_dest;
		for(auto& file : list_source)
			map_source.insert(entry(file.relative, file.hash));
		for(auto& file : list_dest)
			map_dest.insert(entry(file.relative, file.hash));

		std::cout << "# Compare new files" << std::endl;
		for(auto& f : list_source)
		{
			auto lit = map_dest.left.find(f.relative);
			if(lit != map_dest.left.end())
			{
				// If file changed
				// TODO Compare other attributes (time/size?)
				if(lit->second != f.hash)
					std::cout  << "cp " << source/f.relative << ' ' << dest/f.relative << " # Overwrite" << '\n';

				continue;
			}

			auto rit = map_dest.right.equal_range(f.hash);
			if(rit.first != rit.second)
			{
				std::vector<std::filesystem::path> targets;
				for(; rit.first != rit.second; ++rit.first)
				{
					auto p = rit.first->second;
					if(map_source.left.count(p) > 0)
						continue; // Also exist in the source. Well be handled later
					
					targets.emplace_back(p);
				}

				if(targets.size() > 0)
				{
					// File moved
					std::cout << "mv " << dest/targets[0] << ' ' << dest/f.relative << '\n';
					for(unsigned int i = 1; i < targets.size(); ++i)
					{
						std::cout << "rm " << dest/targets[i] << " # Duplicate found with a move" << '\n';
					}
				}
				else
				{
					std::cout  << "cp " << source/f.relative << ' ' << dest/f.relative << " # Create new copy" << '\n';
				}
				continue;
			}

			// New file
			std::cout  << "cp " << source/f.relative << ' ' << dest/f.relative << '\n';
		}

		std::cout << "# Cleanup old files" << std::endl;
		for(auto& f : list_dest)
		{
			auto lit = map_source.left.find(f.relative);
			if(lit != map_dest.left.end())
				continue; // Already exist

			auto rit = map_source.right.find(f.hash);
			if(rit != map_dest.right.end())
				continue; // Already exist under another name
			
			std::cout << "rm " << dest/f.relative << '\n';
		}

		// TODO Remove empty dirs at dest
		std::cout << "# Remove empty dirs" << std::endl;
		std::cout << "find " << dest << " -type d -empty -delete" << '\n';

		return 0;
	});

	if(argc <= 1 || !operations.contains(argv[1]))
	{
		std::cerr << "Missing arguments for operation: " << std::endl;
		for(auto& [key, value] : operations)
			std::cerr << "\t" << key << std::endl;
		return 1;
	}
	return operations[argv[1]]();
}