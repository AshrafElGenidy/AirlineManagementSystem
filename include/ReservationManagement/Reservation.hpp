#ifndef RESERVATION_HPP
#define RESERVATION_HPP

#include <string>
#include <exception>
#include <memory>
#include "json.hpp"

using nlohmann::json;
using std::string;

// ==================== Reservation Status Enum ====================

enum class ReservationStatus
{
	CONFIRMED,
	COMPLETED,
	CANCELED
};

// ==================== Reservation Class (Pure Data) ====================

class Reservation
{
private:
	string reservationId;
	string passengerUsername;
	string flightNumber;
	string seatNumber;
	ReservationStatus status;
	string bookingDate;
	double totalCost;
	string bookedByAgent;
	bool checkedIn;
	string checkInDate;
	
	// Private constructor - only ReservationManager can create
	Reservation(const string& reservationId, const string& passengerUsername,
	            const string& flightNumber, const string& seatNumber,
	            ReservationStatus status, const string& bookingDate,
	            double totalCost, const string& bookedByAgent,
	            bool checkedIn, const string& checkInDate);
	
	friend class ReservationManager;
	
public:
	// Getters
	string getReservationId() const noexcept;
	string getPassengerUsername() const noexcept;
	string getFlightNumber() const noexcept;
	string getSeatNumber() const noexcept;
	ReservationStatus getStatus() const noexcept;
	string getStringStatus() const noexcept;
	string getBookingDate() const noexcept;
	double getTotalCost() const noexcept;
	string getBookedByAgent() const noexcept;
	bool isCheckedIn() const noexcept;
	string getCheckInDate() const noexcept;
	
	// Setters (minimal)
	void setStatus(ReservationStatus status) noexcept;
	void setSeatNumber(const string& seatNumber) noexcept;
	void setCheckedIn(const string& checkInDate) noexcept;
	
	// Destructor
	virtual ~Reservation() noexcept = default;
};

// ==================== Reservation Exception Class ====================

class ReservationException : public std::exception
{
private:
	string message;
public:
	ReservationException(const string& message);
	const char* what() const noexcept override;
	virtual ~ReservationException() noexcept = default;
};

#endif // RESERVATION_HPP