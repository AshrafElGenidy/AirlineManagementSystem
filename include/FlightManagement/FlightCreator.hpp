#ifndef FLIGHTCREATOR_HPP
#define FLIGHTCREATOR_HPP

#include <string>
#include <memory>
#include "Flight.hpp"
#include "UserInterface.hpp"

using std::string;
using std::unique_ptr;
using std::shared_ptr;

// ==================== Flight Validator ====================

class FlightValidator
{
public:
	// Validation methods
	static bool isValidFlightNumber(const string& flightNumber);
	static bool isValidOrigin(const string& origin);
	static bool isValidDestination(const string& destination);
	static bool isValidDateTime(const string& dateTime);
	static bool isValidPrice(double price);
};

// ==================== Flight Validation Exception ====================

enum class FlightValidationErrorCode
{
	INVALID_FLIGHT_NUMBER,
	INVALID_ORIGIN,
	INVALID_DESTINATION,
	INVALID_DATE_TIME,
	INVALID_PRICE,
	NO_AIRCRAFT_TYPES_AVAILABLE
};

class FlightValidationException : public std::exception
{
private:
	FlightValidationErrorCode errorCode;
	string getErrorMessage() const noexcept;

public:
	FlightValidationException(FlightValidationErrorCode code);
	const char* what() const noexcept override;
	virtual ~FlightValidationException() noexcept = default;
	FlightValidationErrorCode getErrorCode() const noexcept;
};

// ==================== FlightCreator ====================

class FlightCreator
{
private:
	UserInterface* ui;
	
	// Prompts user for each field with validation
	string getValidFlightNumber();
	string getValidOrigin();
	string getValidDestination();
	string getValidDepartureDateTime();
	string getValidArrivalDateTime();
	string getValidAircraftType();
	string getValidStatus();
	double getValidPrice();
	
	// Helper to validate date/time format: YYYY-MM-DD HH:MM
	static bool validateDateTimeFormat(const string& dateTime);
	
public:
	FlightCreator();
	
	// Create new flight by prompting user for all details
	shared_ptr<Flight> createNewFlight();
	
	// Create flight from JSON data (deserialization)
	shared_ptr<Flight> createFromJson(const json& data);
	
	// Serialize flight to JSON
	json toJson(const shared_ptr<Flight>& flight);
	
	// Destructor
	~FlightCreator() noexcept = default;
	
	friend class FlightManager;	 // For FlightManager to use getValid methods
};

#endif // FLIGHTCREATOR_HPP