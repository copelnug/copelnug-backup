#include "FileHash.hpp"

#include <climits>
#include <fstream>
#include <ios>
#include <istream>
#include <openssl/md5.h>

CopelnugBackup::MD5::Hash CopelnugBackup::MD5::hash(const std::filesystem::path& file)
{
	static const size_t BUFFER_SIZE = 1024;

	std::ifstream input{file, std::ios_base::binary | std::ios_base::in};
	if(!input)
		throw std::string{"TODO"};

	MD5_CTX ctx;
	if(MD5_Init(&ctx) != 1)
		throw std::string{"TODO"};

	char buffer[BUFFER_SIZE];
	while(!input.eof())
	{
		input.read(buffer, sizeof(buffer));
		size_t count = input.gcount();
		if(count != 0)
		{
			if(MD5_Update(&ctx, buffer, count) != 1)
				throw std::string{"TODO"};
		}
	}

	unsigned char result[MD5_DIGEST_LENGTH];
	if(MD5_Final(result, &ctx) != 1)
		throw std::string{"TODO"};
	
	static_assert(sizeof(Hash) == MD5_DIGEST_LENGTH, "Invalid hash value");
	
	Hash val = 0;
	for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		val = val << (sizeof(unsigned char)*CHAR_BIT);
		val = val | result[i];
	}
	return val;
}