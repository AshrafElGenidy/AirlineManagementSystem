#include "Reservation.hpp"

// ==================== Constructor ====================

Reservation::Reservation(const string& reservationId, const string& passengerUsername,
                         const string& flightNumber, const string& seatNumber,
                         ReservationStatus status, const string& bookingDate,
                         double totalCost, const string& bookedByAgent,
                         bool checkedIn, const string& checkInDate)
	: reservationId(reservationId), passengerUsername(passengerUsername),
	  flightNumber(flightNumber), seatNumber(seatNumber), status(status),
	  bookingDate(bookingDate), totalCost(totalCost), bookedByAgent(bookedByAgent),
	  checkedIn(checkedIn), checkInDate(checkInDate)
{}

// ==================== Getters ====================

string Reservation::getReservationId() const noexcept
{
	return reservationId;
}

string Reservation::getPassengerUsername() const noexcept
{
	return passengerUsername;
}

string Reservation::getFlightNumber() const noexcept
{
	return flightNumber;
}

string Reservation::getSeatNumber() const noexcept
{
	return seatNumber;
}

ReservationStatus Reservation::getStatus() const noexcept
{
	return status;
}

string Reservation::getBookingDate() const noexcept
{
	return bookingDate;
}

double Reservation::getTotalCost() const noexcept
{
	return totalCost;
}

string Reservation::getBookedByAgent() const noexcept
{
	return bookedByAgent;
}

bool Reservation::isCheckedIn() const noexcept
{
	return checkedIn;
}

string Reservation::getCheckInDate() const noexcept
{
	return checkInDate;
}

// ==================== Setters ====================

void Reservation::setStatus(ReservationStatus status) noexcept
{
	this->status = status;
}

void Reservation::setSeatNumber(const string& seatNumber) noexcept
{
	this->seatNumber = seatNumber;
}

void Reservation::setCheckedIn(const string& checkInDate) noexcept
{
	this->checkedIn = !checkInDate.empty();
	this->checkInDate = checkInDate;
}

// ==================== ReservationException Class ====================

ReservationException::ReservationException(ReservationErrorCode code) : errorCode(code) {}

const char* ReservationException::what() const noexcept
{
	static string message;
	message = getErrorMessage();
	return message.c_str();
}

ReservationErrorCode ReservationException::getErrorCode() const noexcept
{
	return errorCode;
}

string ReservationException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case ReservationErrorCode::PASSENGER_NOT_FOUND:
			return "Passenger not found in system.";
		case ReservationErrorCode::FLIGHT_NOT_FOUND:
			return "Flight not found.";
		case ReservationErrorCode::RESERVATION_NOT_FOUND:
			return "Reservation not found.";
		case ReservationErrorCode::INVALID_STATUS_TRANSITION:
			return "Invalid status transition for this reservation.";
		case ReservationErrorCode::UNAUTHORIZED_ACCESS:
			return "You do not have access to this reservation.";
		case ReservationErrorCode::SEAT_OPERATION_FAILED:
			return "Failed to reserve or release seat. Please try again.";
		case ReservationErrorCode::DATABASE_ERROR:
			return "Database error occurred while processing reservation.";
		default:
			return "An unknown reservation error occurred.";
	}
}