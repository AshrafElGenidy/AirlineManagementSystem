#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <exception>
#include <memory>
#include "json.hpp"

using nlohmann::json;
using std::string;

// ==================== Database Class ====================

class Database
{
private:
	string filePath;
	std::unique_ptr<json> cachedData;  // In-memory cache
	
	// Helper methods
	void loadFromFile();
	void writeToFile() const;

public:
	// Constructor - takes entity name only (e.g., "Flight", "User", "Aircraft")
	// Automatically constructs path: "Databases/{entityName}.json"
	explicit Database(const string& entityName);
	
	// File operations
	json loadAll() const;
	void saveAll(const json& data);
	
	// Entry operations
	json getEntry(const string& entryKey) const;
	void addEntry(const string& entryKey, const json& entryData);
	void deleteEntry(const string& entryKey);
	bool entryExists(const string& entryKey) const;
	void updateEntry(const string& entryKey, const json& updates);	// Bulk Updates
	
	// Attribute operations
	auto getAttribute(const string& entryKey, const string& attributeKey) const;
	void setAttribute(const string& entryKey, const string& attributeKey, const json& value);
	void deleteAttribute(const string& entryKey, const string& attributeKey);
	bool attributeExists(const string& entryKey, const string& attributeKey) const;
	
	// Utility
	int getEntryCount() const;
	bool isEmpty() const;
	void clear();
	void initializeIfNotExists();
	
	// Delete copy and move operations (prevent accidental duplication of cache)
	Database(const Database&) = delete;
	Database(Database&&) = delete;
	Database& operator=(const Database&) = delete;
	Database& operator=(Database&&) = delete;
};

// ==================== Database Exception Class ====================

enum class DatabaseErrorCode
{
	FILE_NOT_FOUND,
	FILE_READ_ERROR,
	FILE_WRITE_ERROR,
	INVALID_JSON,
	ENTRY_NOT_FOUND,
	ATTRIBUTE_NOT_FOUND,
	DATABASE_ERROR
};

class DatabaseException : public std::exception
{
private:
	DatabaseErrorCode errorCode;
	string getErrorMessage() const noexcept;

public:
	DatabaseException(DatabaseErrorCode code);
	const char* what() const noexcept override;
	virtual ~DatabaseException() noexcept = default;
	DatabaseErrorCode getErrorCode() const noexcept;
};

#endif // DATABASE_HPP