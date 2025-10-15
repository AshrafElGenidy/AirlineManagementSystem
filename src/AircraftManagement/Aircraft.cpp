#include "Aircraft.hpp"

// ==================== Aircraft Class ====================

Aircraft::Aircraft(const string& aircraftType, const string& manufacturer, const string& model,
                   int totalSeats, const string& seatLayout, int rows, int fleetCount,
                   const string& status)
	: aircraftType(aircraftType), manufacturer(manufacturer), model(model),
	  totalSeats(totalSeats), seatLayout(seatLayout), rows(rows),
	  fleetCount(fleetCount), status(status)
{}

// ==================== Getters ====================

string Aircraft::getAircraftType() const noexcept
{
	return aircraftType;
}

string Aircraft::getManufacturer() const noexcept
{
	return manufacturer;
}

string Aircraft::getModel() const noexcept
{
	return model;
}

int Aircraft::getTotalSeats() const noexcept
{
	return totalSeats;
}

string Aircraft::getSeatLayout() const noexcept
{
	return seatLayout;
}

int Aircraft::getRows() const noexcept
{
	return rows;
}

int Aircraft::getFleetCount() const noexcept
{
	return fleetCount;
}

string Aircraft::getStatus() const noexcept
{
	return status;
}

// ==================== Setters ====================

void Aircraft::setManufacturer(const string& manufacturer) noexcept
{
	this->manufacturer = manufacturer;
}

void Aircraft::setModel(const string& model) noexcept
{
	this->model = model;
}

void Aircraft::setTotalSeats(int totalSeats) noexcept
{
	this->totalSeats = totalSeats;
}

void Aircraft::setSeatLayout(const string& seatLayout) noexcept
{
	this->seatLayout = seatLayout;
}

void Aircraft::setRows(int rows) noexcept
{
	this->rows = rows;
}

void Aircraft::setFleetCount(int fleetCount) noexcept
{
	this->fleetCount = fleetCount;
}

void Aircraft::setStatus(const string& status) noexcept
{
	this->status = status;
}

// ==================== AircraftException Class ====================

AircraftException::AircraftException(AircraftErrorCode code) : errorCode(code), message(getErrorMessage()) {}

AircraftException::AircraftException(AircraftErrorCode code, const string& customMessage) : errorCode(code), message(customMessage) {}

const char* AircraftException::what() const noexcept
{
	return message.c_str();
}

AircraftErrorCode AircraftException::getErrorCode() const noexcept
{
	return errorCode;
}

string AircraftException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case AircraftErrorCode::AIRCRAFT_NOT_FOUND:
			return "Aircraft type does not exist.";
		case AircraftErrorCode::AIRCRAFT_EXISTS:
			return "Aircraft type already exists in the system.";
		case AircraftErrorCode::INVALID_AIRCRAFT_TYPE:
			return "Invalid aircraft type. Must be 2-30 characters, alphanumeric only.";
		case AircraftErrorCode::INVALID_SEAT_LAYOUT:
			return "Invalid seat layout. Expected format: N-N or N-N-N (e.g., 3-3 or 2-4-2).";
		case AircraftErrorCode::INVALID_SEAT_COUNT:
			return "Invalid seat count. Must be between 50 and 500.";
		case AircraftErrorCode::INVALID_MANUFACTURER:
			return "Invalid manufacturer. Must not be empty and less than 50 characters.";
		case AircraftErrorCode::INVALID_MODEL:
			return "Invalid model. Must not be empty and less than 50 characters.";
		case AircraftErrorCode::INVALID_FLEET_COUNT:
			return "Invalid fleet count. Must be positive.";
		case AircraftErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the database.";
		default:
			return "An unknown aircraft error occurred.";
	}
}