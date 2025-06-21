#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------
bool FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename)
{

	FILE* file = new FILE();

	if(fopen_s(&file, filename.c_str(), "rb") != 0)
	{
		ERROR_RECOVERABLE(Stringf("Could not open file %s", filename.c_str()));
		return false;
	}

	if(fseek(file, 0, SEEK_END) != 0)
	{
		ERROR_RECOVERABLE(Stringf("SEEK TO THE END FAILED.\nFilename: %s", filename.c_str()));
		return false;
	}

	int fileSize = ftell(file);

	outBuffer.resize(fileSize);

	fseek(file, 0, SEEK_SET);

	if(fread(outBuffer.data(), sizeof(uint8_t), fileSize, file) != static_cast<unsigned int>(fileSize))
	{
		ERROR_RECOVERABLE(Stringf("Could not read file:.\nFilename: %s", filename.c_str()));
		return false;
	}

	fclose(file);

	return true;

}


//------------------------------------------------------------------------------------------------------------------
bool FileReadToString(std::string& outBuffer, const std::string& filename)
{
	
	std::vector<uint8_t> vecBuffer;

	FileReadToBuffer(vecBuffer, filename);

	vecBuffer.push_back('\0');

	outBuffer = std::string(vecBuffer.begin(), vecBuffer.end());

	return true;

}
