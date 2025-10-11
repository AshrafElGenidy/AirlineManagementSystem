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
		throw DatabaseException(DatabaseErrorCode::INVALID_JSON);
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
		throw DatabaseException(DatabaseErrorCode::FILE_WRITE_ERROR);
	}
	
	std::ofstream file(filePath);
	
	if (!file.is_open())
	{
		throw DatabaseException(DatabaseErrorCode::FILE_WRITE_ERROR);
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
		throw DatabaseException(DatabaseErrorCode::FILE_WRITE_ERROR);
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
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
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
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	(*cachedData)[entryKey] = entryData;
	writeToFile();
}

void Database::deleteEntry(const string& entryKey)
{
	if (!cachedData)
	{
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
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
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
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
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	json entry = (*cachedData)[entryKey];
	
	if (!entry.contains(attributeKey))
	{
		throw DatabaseException(DatabaseErrorCode::ATTRIBUTE_NOT_FOUND);
	}
	
	return entry[attributeKey];
}

void Database::setAttribute(const string& entryKey, const string& attributeKey, const json& value)
{
	if (!cachedData)
	{
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	(*cachedData)[entryKey][attributeKey] = value;
	writeToFile();
}

void Database::deleteAttribute(const string& entryKey, const string& attributeKey)
{
	if (!cachedData)
	{
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	if (!cachedData->contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	if (!(*cachedData)[entryKey].contains(attributeKey))
	{
		throw DatabaseException(DatabaseErrorCode::ATTRIBUTE_NOT_FOUND);
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

DatabaseException::DatabaseException(DatabaseErrorCode code) : errorCode(code) {}

const char* DatabaseException::what() const noexcept
{
	static string message;
	message = getErrorMessage();
	return message.c_str();
}

DatabaseErrorCode DatabaseException::getErrorCode() const noexcept
{
	return errorCode;
}

string DatabaseException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case DatabaseErrorCode::FILE_NOT_FOUND:
			return "Database file not found.";
		case DatabaseErrorCode::FILE_READ_ERROR:
			return "Error reading from database file.";
		case DatabaseErrorCode::FILE_WRITE_ERROR:
			return "Error writing to database file.";
		case DatabaseErrorCode::INVALID_JSON:
			return "Invalid JSON format in database file.";
		case DatabaseErrorCode::ENTRY_NOT_FOUND:
			return "Entry does not exist in database.";
		case DatabaseErrorCode::ATTRIBUTE_NOT_FOUND:
			return "Attribute does not exist in entry.";
		case DatabaseErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the database.";
		default:
			return "An unknown database error occurred.";
	}
}