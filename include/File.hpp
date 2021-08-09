#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class File
{
public:
	static std::string Read(const char* filePath)
	{
		std::string content;
		std::ifstream file;
		// ensures ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			file.open(filePath);
			std::stringstream stream;
			// Read file's buffer contents into streams
			stream << file.rdbuf();
			// close file handlers
			file.close();
			// Convert stream into string
			content = stream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		return content;
	}
};