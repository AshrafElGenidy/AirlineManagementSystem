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

class DatabaseException : public std::exception
{
private:
	string message;
public:
	DatabaseException(const string& message);
	const char* what() const noexcept override;
	virtual ~DatabaseException() noexcept = default;
};

#endif // DATABASE_HPP