#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include <string>
#include <vector>
#include <exception>
#include <memory>
#include "json.hpp"

using nlohmann::json;
using std::string;
using std::vector;

// ==================== Aircraft Class (Pure Data) ====================

class Aircraft
{
private:
	string aircraftType;
	string manufacturer;
	string model;
	int totalSeats;
	string seatLayout;
	int rows;
	int fleetCount;
	string status;
	
	// Private constructors - only AircraftManager and AircraftCreator can create
	Aircraft(const string& aircraftType, const string& manufacturer, const string& model,
	         int totalSeats, const string& seatLayout, int rows, int fleetCount,
	         const string& status);
	
public:
	// Getters
	string getAircraftType() const noexcept;
	string getManufacturer() const noexcept;
	string getModel() const noexcept;
	int getTotalSeats() const noexcept;
	string getSeatLayout() const noexcept;
	int getRows() const noexcept;
	int getFleetCount() const noexcept;
	string getStatus() const noexcept;
	
	// Setters
	void setManufacturer(const string& manufacturer) noexcept;
	void setModel(const string& model) noexcept;
	void setTotalSeats(int totalSeats) noexcept;
	void setSeatLayout(const string& seatLayout) noexcept;
	void setRows(int rows) noexcept;
	void setFleetCount(int fleetCount) noexcept;
	void setStatus(const string& status) noexcept;
	
	// Destructor
	virtual ~Aircraft() noexcept = default;

	friend class AircraftManager;
	friend class AircraftCreator;
};

// ==================== Aircraft Exception Class ====================

enum class AircraftErrorCode
{
	AIRCRAFT_NOT_FOUND,
	AIRCRAFT_EXISTS,
	INVALID_AIRCRAFT_TYPE,
	INVALID_SEAT_LAYOUT,
	INVALID_SEAT_COUNT,
	INVALID_MANUFACTURER,
	INVALID_MODEL,
	INVALID_FLEET_COUNT,
	DATABASE_ERROR
};

class AircraftException : public std::exception
{
private:
	AircraftErrorCode errorCode;
	string message;
	string getErrorMessage() const noexcept;

public:
	AircraftException(AircraftErrorCode code);
	AircraftException(AircraftErrorCode code, const string& customMessage);
	const char* what() const noexcept override;
	virtual ~AircraftException() noexcept = default;
	AircraftErrorCode getErrorCode() const noexcept;
};

#endif // AIRCRAFT_HPP