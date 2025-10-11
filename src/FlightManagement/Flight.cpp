#include <algorithm>
#include "Flight.hpp"
#include "Aircraft.hpp"
#include "SeatMap.hpp"

// ==================== Flight Constructor ====================

Flight::Flight(const string& flightNumber, const string& origin, const string& destination,
              const string& departureDateTime, const string& arrivalDateTime,
              const string& aircraftType, const string& status, double price,
              const string& gate, const string& boardingTime,
              const vector<string>& reservedSeats)
	: flightNumber(flightNumber), origin(origin), destination(destination),
	  departureDateTime(departureDateTime), arrivalDateTime(arrivalDateTime),
	  aircraftType(aircraftType), status(status), price(price),
	  gate(gate), boardingTime(boardingTime), reservedSeats(reservedSeats)
{
}

// ==================== Getters ====================

string Flight::getFlightNumber() const noexcept
{
	return flightNumber;
}

string Flight::getOrigin() const noexcept
{
	return origin;
}

string Flight::getDestination() const noexcept
{
	return destination;
}

string Flight::getDepartureDateTime() const noexcept
{
	return departureDateTime;
}

string Flight::getArrivalDateTime() const noexcept
{
	return arrivalDateTime;
}

string Flight::getAircraftType() const noexcept
{
	return aircraftType;
}

string Flight::getStatus() const noexcept
{
	return status;
}

double Flight::getPrice() const noexcept
{
	return price;
}

int Flight::getTotalSeats() const
{
	// Query Aircraft for seat count based on aircraft type
	try
	{
		Aircraft aircraft(aircraftType);
		string seatLayout = aircraft.getSeatLayout();
		int rows = aircraft.getRows();
		return SeatMap::calculateSeatCount(seatLayout, rows);
	}
	catch (const AircraftException& e)
	{
		throw FlightException(FlightErrorCode::INVALID_AIRCRAFT_TYPE);
	}
	catch (const SeatMapException& e)
	{
		throw FlightException(FlightErrorCode::DATABASE_ERROR);
	}
}

int Flight::getAvailableSeats() const
{
	int totalSeats = getTotalSeats();
	return totalSeats - static_cast<int>(reservedSeats.size());
}

string Flight::getGate() const noexcept
{
	return gate;
}

string Flight::getBoardingTime() const noexcept
{
	return boardingTime;
}

vector<string> Flight::getReservedSeats() const noexcept
{
	return reservedSeats;
}

// ==================== Setters ====================

void Flight::setStatus(const string& status) noexcept
{
	this->status = status;
}

void Flight::setPrice(double price) noexcept
{
	this->price = price;
}

void Flight::setGate(const string& gate) noexcept
{
	this->gate = gate;
}

void Flight::setBoardingTime(const string& boardingTime) noexcept
{
	this->boardingTime = boardingTime;
}

// ==================== Seat Management ====================

bool Flight::reserveSeat(const string& seatNumber)
{
	// Check if seat already reserved
	if (!isSeatAvailable(seatNumber))
	{
		throw FlightException(FlightErrorCode::SEAT_OPERATION_FAILED, 
							 "Seat " + seatNumber + " is already reserved.");
	}
	
	// Check if seat is valid for this aircraft
	try
	{
		Aircraft aircraft(aircraftType);
		string seatLayout = aircraft.getSeatLayout();
		int rows = aircraft.getRows();
		SeatMap seatMap(seatLayout, rows);
		
		if (!seatMap.isValidSeat(seatNumber))
		{
			throw FlightException(FlightErrorCode::SEAT_OPERATION_FAILED, 
								 "Seat " + seatNumber + " is invalid for this aircraft.");
		}
	}
	catch (const AircraftException& e)
	{
		throw FlightException(FlightErrorCode::INVALID_AIRCRAFT_TYPE);
	}
	catch (const SeatMapException& e)
	{
		throw FlightException(FlightErrorCode::SEAT_OPERATION_FAILED, e.what());
	}
	
	reservedSeats.push_back(seatNumber);
	return true;
}

bool Flight::releaseSeat(const string& seatNumber)
{
	auto it = std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber);
	
	if (it != reservedSeats.end())
	{
		reservedSeats.erase(it);
		return true;
	}
	
	return false;
}

bool Flight::isSeatAvailable(const string& seatNumber) const noexcept
{
	return std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber) == reservedSeats.end();
}

// ==================== FlightException Class ====================

FlightException::FlightException(FlightErrorCode code) 
	: errorCode(code), message(getErrorMessage())
{
}

FlightException::FlightException(FlightErrorCode code, const string& customMessage)
	: errorCode(code), message(customMessage)
{
}

const char* FlightException::what() const noexcept
{
	return message.c_str();
}

FlightErrorCode FlightException::getErrorCode() const noexcept
{
	return errorCode;
}

string FlightException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case FlightErrorCode::FLIGHT_NOT_FOUND:
			return "Flight does not exist.";
		case FlightErrorCode::FLIGHT_EXISTS:
			return "Flight already exists in the system.";
		case FlightErrorCode::INVALID_FLIGHT_NUMBER:
			return "Invalid flight number. Must be 3-10 characters, alphanumeric only.";
		case FlightErrorCode::INVALID_AIRCRAFT_TYPE:
			return "Invalid or missing aircraft type.";
		case FlightErrorCode::INVALID_ORIGIN:
			return "Invalid origin. Must not be empty and less than 50 characters.";
		case FlightErrorCode::INVALID_DESTINATION:
			return "Invalid destination. Must not be empty and less than 50 characters.";
		case FlightErrorCode::INVALID_DATE_FORMAT:
			return "Invalid date/time format. Expected: YYYY-MM-DD HH:MM";
		case FlightErrorCode::SEAT_OPERATION_FAILED:
			return "Seat operation failed.";
		case FlightErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the database.";
		default:
			return "An unknown flight error occurred.";
	}
}