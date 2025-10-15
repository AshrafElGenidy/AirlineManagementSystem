#include <fstream>
#include <filesystem>
#include "Database.hpp"

namespace fs = std::filesystem;

// ==================== Database Class ====================

Database::Database(const string& entityName) 
{
	filePath = "Databases/" + entityName + ".json";
	initializeIfNotExists();
	loadFromFile();
}

// ==================== Private Helper Methods ====================

void Database::loadFromFile()
{
	std::ifstream file(filePath);
	
	if (!file.is_open())
	{
		cachedData = std::make_unique<json>(json::object());
		return;
	}
	
	try
	{
		json data;
		file >> data;
		cachedData = std::make_unique<json>(data);
	}
	catch (const json::exception& e)
	{
		file.close();
		throw DatabaseException("Invalid JSON format in database file.");
	}
	
	file.close();
}

void Database::writeToFile() const
{
	// Ensure directory exists
	try
	{
		fs::path filePath_obj(filePath);
		fs::create_directories(filePath_obj.parent_path());
	}
	catch (const std::exception& e)
	{
		throw DatabaseException("Error writing to database file.");
	}
	
	std::ofstream file(filePath);
	
	if (!file.is_open())
	{
		throw DatabaseException("Error writing to database file.");
	}
	
	try
	{
		if (cachedData)
		{
			file << cachedData->dump(4);
		}
		else
		{
			file << json::object().dump(4);
		}
	}
	catch (const json::exception& e)
	{
		throw DatabaseException("Error writing to database file.");
	}
	
	file.close();
}

// ==================== File Operations ====================

json Database::loadAll() const
{
	if (cachedData)
	{
		return *cachedData;
	}
	return json::object();
}

void Database::saveAll(const json& data)
{
	cachedData = std::make_unique<json>(data);
	writeToFile();
}

// ==================== Entry Operations ====================

json Database::getEntry(const string& entryKey) const
{
	if (!cachedData)
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException("Entry does not exist in database.");
	}
	
	return (*cachedData)[entryKey];
}

void Database::addEntry(const string& entryKey, const json& entryData)
{
	if (!cachedData)
	{
		cachedData = std::make_unique<json>(json::object());
	}
	
	if (cachedData->contains(entryKey))
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	(*cachedData)[entryKey] = entryData;
	writeToFile();
}

void Database::deleteEntry(const string& entryKey)
{
	if (!cachedData)
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException("Entry does not exist in database.");
	}
	
	cachedData->erase(entryKey);
	writeToFile();
}

bool Database::entryExists(const string& entryKey) const
{
	if (!cachedData)
	{
		return false;
	}
	
	return cachedData->contains(entryKey);
}

void Database::updateEntry(const string& entryKey, const json& updates)
{
	if (!cachedData)
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException("Entry does not exist in database.");
	}
	
	for (const auto& [key, value] : updates.items())
	{
		(*cachedData)[entryKey][key] = value;
	}
	
	writeToFile();
}

// ==================== Attribute Operations ====================

auto Database::getAttribute(const string& entryKey, const string& attributeKey) const
{
	if (!cachedData)
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException("Entry does not exist in database.");
	}
	
	json entry = (*cachedData)[entryKey];
	
	if (!entry.contains(attributeKey))
	{
		throw DatabaseException("Attribute does not exist in entry.");
	}
	
	return entry[attributeKey];
}

void Database::setAttribute(const string& entryKey, const string& attributeKey, const json& value)
{
	if (!cachedData)
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException("Entry does not exist in database.");
	}
	
	(*cachedData)[entryKey][attributeKey] = value;
	writeToFile();
}

void Database::deleteAttribute(const string& entryKey, const string& attributeKey)
{
	if (!cachedData)
	{
		throw DatabaseException("An error occurred while accessing the database.");
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException("Entry does not exist in database.");
	}
	
	if (!(*cachedData)[entryKey].contains(attributeKey))
	{
		throw DatabaseException("Attribute does not exist in entry.");
	}
	
	(*cachedData)[entryKey].erase(attributeKey);
	writeToFile();
}

bool Database::attributeExists(const string& entryKey, const string& attributeKey) const
{
	if (!cachedData)
	{
		return false;
	}
	
	if (!cachedData->contains(entryKey))
	{
		return false;
	}
	
	return (*cachedData)[entryKey].contains(attributeKey);
}

// ==================== Utility Operations ====================

int Database::getEntryCount() const
{
	if (cachedData)
	{
		return cachedData->size();
	}
	return 0;
}

bool Database::isEmpty() const
{
	return getEntryCount() == 0;
}

void Database::clear()
{
	cachedData = std::make_unique<json>(json::object());
	writeToFile();
}

void Database::initializeIfNotExists()
{
	std::ifstream testFile(filePath);
	if (!testFile.is_open())
	{
		// File doesn't exist, create directory and empty file
		try
		{
			fs::path filePath_obj(filePath);
			fs::create_directories(filePath_obj.parent_path());
		}
		catch (const std::exception& e)
		{
			// Directory creation failed, but we'll try to create the file anyway
		}
		
		std::ofstream createFile(filePath);
		if (createFile.is_open())
		{
			createFile << json::object().dump(4);
			createFile.close();
		}
	}
	else
	{
		testFile.close();
	}
}

// ==================== DatabaseException Class ====================

DatabaseException::DatabaseException(const string& message) : message(message) {}

const char* DatabaseException::what() const noexcept
{
	return message.c_str();
}