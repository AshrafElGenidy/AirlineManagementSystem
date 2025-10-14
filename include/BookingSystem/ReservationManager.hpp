#ifndef RESERVATIONMANAGER_HPP
#define RESERVATIONMANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Reservation.hpp"
#include "User.hpp"
#include "Database.hpp"
#include "UserInterface.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

// ==================== ReservationManager ====================

class ReservationManager
{
private:
	static ReservationManager* instance;
	
	unique_ptr<Database> db;
	UserInterface* ui;
	
	// Private constructor for singleton
	ReservationManager();
	
	// Database operations
	shared_ptr<Reservation> loadReservationFromDatabase(const string& reservationId);
	void saveReservationToDatabase(const shared_ptr<Reservation>& reservation);
	
	// Shared helper for modify and cancel
	void updateOrCancel(bool isModify);
	
	// Query helpers
	vector<shared_ptr<Reservation>> loadAllReservations();
	
	// JSON serialization
	json reservationToJson(const shared_ptr<Reservation>& reservation);
	shared_ptr<Reservation> reservationFromJson(const json& data);
	
	// Display helpers
	void displayReservationsTable(const vector<shared_ptr<Reservation>>& reservations);
	void displayBoardingPass(const shared_ptr<Reservation>& reservation);
	
	// Utility
	string getCurrentDateTime() const;
	
public:
	// Singleton accessor
	static ReservationManager* getInstance();
	
	// Booking operations (BOOKING AGENT)
	void createReservation(const string& agentUsername);
	void modifyReservation(const string& agentUsername);
	void cancelReservation(const string& agentUsername);
	
	// Query operations (role-based)
	void viewReservations(const string& username, UserRole role);
	vector<shared_ptr<Reservation>> getReservationsByPassenger(const string& passengerUsername);
	vector<shared_ptr<Reservation>> getReservationsByFlight(const string& flightNumber);
	vector<shared_ptr<Reservation>> getAllReservations();
	
	// Check-in operations (PASSENGER)
	void checkIn(const string& passengerUsername);
	void viewBoardingPass(const string& passengerUsername);
	
	// Cascade & utility
	static bool hasActiveReservations(const string& flightNumber);
	
	// Destructor
	~ReservationManager() noexcept;
	
	// Delete copy and move constructors/assignments
	ReservationManager(const ReservationManager&) = delete;
	ReservationManager(ReservationManager&&) = delete;
	ReservationManager& operator=(const ReservationManager&) = delete;
	ReservationManager& operator=(ReservationManager&&) = delete;
};

#endif // RESERVATIONMANAGER_HPP