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

string Reservation::getStringStatus() const noexcept
{
	if (status == ReservationStatus::CONFIRMED) return "CONFIRMED";
	if (status == ReservationStatus::COMPLETED) return "COMPLETED";
	if (status == ReservationStatus::CANCELED)  return "CANCELED";
	return "CONFIRMED";
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

ReservationException::ReservationException(const string& message) : message(message) {}

const char* ReservationException::what() const noexcept
{
	return message.c_str();
}