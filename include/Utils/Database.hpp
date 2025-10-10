#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <exception>
#include "json.hpp"

using nlohmann::json;
using std::string;

// ==================== Database Class ====================

class Database
{
private:
	string filePath;
	
	// Helpers
	json loadFile() const;
	void saveFile(const json& data) const;

public:
	// Constructor
	explicit Database(const string& filePath);
	
	// File operations
	json loadAll() const;
	void saveAll(const json& data) const;
	
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
	
	virtual ~Database() noexcept = default;
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