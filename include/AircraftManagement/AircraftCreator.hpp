#ifndef AIRCRAFTCREATOR_HPP
#define AIRCRAFTCREATOR_HPP

#include <string>
#include <memory>
#include "Aircraft.hpp"
#include "UserInterface.hpp"

using std::string;
using std::unique_ptr;
using std::shared_ptr;

// ==================== Aircraft Validator ====================

class AircraftValidator
{
public:
	// Validation methods
	static bool isValidAircraftType(const string& aircraftType);
	static bool isValidManufacturer(const string& manufacturer);
	static bool isValidModel(const string& model);
	static bool isValidSeatLayout(const string& seatLayout);
	static bool isValidSeatCount(int totalSeats);
	static bool isValidRows(int rows);
	static bool isValidFleetCount(int fleetCount);
};

// ==================== Aircraft Validation Exception ====================

enum class AircraftValidationErrorCode
{
	INVALID_AIRCRAFT_TYPE,
	INVALID_MANUFACTURER,
	INVALID_MODEL,
	INVALID_SEAT_LAYOUT,
	INVALID_SEAT_COUNT,
	INVALID_ROWS,
	INVALID_FLEET_COUNT
};

class AircraftValidationException : public std::exception
{
private:
	AircraftValidationErrorCode errorCode;
	string getErrorMessage() const noexcept;

public:
	AircraftValidationException(AircraftValidationErrorCode code);
	const char* what() const noexcept override;
	virtual ~AircraftValidationException() noexcept = default;
	AircraftValidationErrorCode getErrorCode() const noexcept;
};

// ==================== AircraftCreator ====================

class AircraftCreator
{
private:
	UserInterface* ui;
	
	// Prompts user for each field with validation
	string getValidAircraftType();
	string getValidManufacturer();
	string getValidModel();
	string getValidSeatLayout();
	int getValidSeatCount();
	int getValidRows();
	int getValidFleetCount();
	string getValidStatus();
	
public:
	AircraftCreator();
	
	// Create new aircraft by prompting user for all details
	shared_ptr<Aircraft> createNewAircraft();
	
	// Create aircraft from JSON data (deserialization)
	shared_ptr<Aircraft> createFromJson(const json& data);
	
	// Serialize aircraft to JSON
	json toJson(const shared_ptr<Aircraft>& aircraft);
	
	// Destructor
	~AircraftCreator() noexcept = default;
	
	friend class AircraftManager; // For AircraftManager to use getValid methods
};

#endif // AIRCRAFTCREATOR_HPP