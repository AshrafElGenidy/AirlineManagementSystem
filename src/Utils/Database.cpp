#include <fstream>
#include "Database.hpp"

// ==================== Database Class ====================

Database::Database(const string& filePath) : filePath(filePath)
{
	initializeIfNotExists();
}

// ==================== File Operations ====================

json Database::loadFile() const
{
	std::ifstream file(filePath);
	
	if (!file.is_open())
	{
		return json::object();
	}
	
	json data;
	try
	{
		file >> data;
	}
	catch (const json::exception& e)
	{
		throw DatabaseException(DatabaseErrorCode::INVALID_JSON);
	}
	
	file.close();
	return data;
}

void Database::saveFile(const json& data) const
{
	std::ofstream file(filePath);
	
	if (!file.is_open())
	{
		throw DatabaseException(DatabaseErrorCode::FILE_WRITE_ERROR);
	}
	
	try
	{
		file << data.dump(4);
	}
	catch (const json::exception& e)
	{
		throw DatabaseException(DatabaseErrorCode::FILE_WRITE_ERROR);
	}
	
	file.close();
}

json Database::loadAll() const
{
	return loadFile();
}

void Database::saveAll(const json& data) const
{
	saveFile(data);
}

// ==================== Entry Operations ====================

json Database::getEntry(const string& entryKey) const
{
	json data = loadFile();
	
	if (!data.contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	return data[entryKey];
}

void Database::addEntry(const string& entryKey, const json& entryData)
{
	json data = loadFile();
	
	if (data.contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::DATABASE_ERROR);
	}
	
	data[entryKey] = entryData;
	saveFile(data);
}

void Database::deleteEntry(const string& entryKey)
{
	json data = loadFile();
	
	if (!data.contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	data.erase(entryKey);
	saveFile(data);
}

bool Database::entryExists(const string& entryKey) const
{
	json data = loadFile();
	return data.contains(entryKey);
}

void Database::updateEntry(const string& entryKey, const json& updates)
{
	json data = loadFile();
	
	if (!data.contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	for (const auto& [key, value] : updates.items())
	{
		data[entryKey][key] = value;
	}
	
	saveFile(data);
}

// ==================== Attribute Operations ====================

auto Database::getAttribute(const string& entryKey, const string& attributeKey) const
{
	json entry = getEntry(entryKey);
	
	if (!entry.contains(attributeKey))
	{
		throw DatabaseException(DatabaseErrorCode::ATTRIBUTE_NOT_FOUND);
	}
	
	return entry[attributeKey];
}

void Database::setAttribute(const string& entryKey, const string& attributeKey, const json& value)
{
	json data = loadFile();
	
	if (!data.contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	data[entryKey][attributeKey] = value;
	saveFile(data);
}

void Database::deleteAttribute(const string& entryKey, const string& attributeKey)
{
	json data = loadFile();
	
	if (!data.contains(entryKey))
	{
		throw DatabaseException(DatabaseErrorCode::ENTRY_NOT_FOUND);
	}
	
	if (!data[entryKey].contains(attributeKey))
	{
		throw DatabaseException(DatabaseErrorCode::ATTRIBUTE_NOT_FOUND);
	}
	
	data[entryKey].erase(attributeKey);
	saveFile(data);
}

bool Database::attributeExists(const string& entryKey, const string& attributeKey) const
{
	json data = loadFile();
	
	if (!data.contains(entryKey))
	{
		return false;
	}
	
	return data[entryKey].contains(attributeKey);
}

// ==================== Utility Operations ====================

int Database::getEntryCount() const
{
	json data = loadFile();
	return data.size();
}

bool Database::isEmpty() const
{
	return getEntryCount() == 0;
}

void Database::clear()
{
	saveFile(json::object());
}

void Database::initializeIfNotExists()
{
	std::ifstream testFile(filePath);
	if (!testFile.is_open())
	{
		saveFile(json::object());
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
	return getErrorMessage().c_str();
}

DatabaseErrorCode DatabaseException::getErrorCode() const noexcept
{
	return errorCode;
}

string DatabaseException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case DatabaseErrorCode::FILE_NOT_FOUND:			return "Database file not found.";
		case DatabaseErrorCode::FILE_READ_ERROR:		return "Error reading from database file.";
		case DatabaseErrorCode::FILE_WRITE_ERROR:		return "Error writing to database file.";
		case DatabaseErrorCode::INVALID_JSON:			return "Invalid JSON format in database file.";
		case DatabaseErrorCode::ENTRY_NOT_FOUND:		return "Entry does not exist in database.";
		case DatabaseErrorCode::ATTRIBUTE_NOT_FOUND:	return "Attribute does not exist in entry.";
		case DatabaseErrorCode::DATABASE_ERROR:			return "An error occurred while accessing the database.";
		default:										return "An unknown database error occurred.";
	}
}