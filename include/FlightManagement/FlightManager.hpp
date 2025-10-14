#ifndef FLIGHTMANAGER_HPP
#define FLIGHTMANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Flight.hpp"
#include "FlightCreator.hpp"
#include "Database.hpp"
#include "UserInterface.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

// ==================== FlightManager ====================

class FlightManager
{
private:
	static FlightManager* instance;
	
	unique_ptr<Database> db;
	UserInterface* ui;
	unique_ptr<FlightCreator> creator;
	
	// Private constructor for singleton
	FlightManager();
	
	// Private menu methods
	void addFlight();
	void viewAllFlights();
	void updateFlight();
	void removeFlight();
	void assignCrewToFlight(const shared_ptr<Flight>& flight);
	
	// Helper methods
	shared_ptr<Flight> loadFlightFromDatabase(const string& flightNumber);
	void saveFlightToDatabase(const shared_ptr<Flight>& flight);
	void deleteFlightFromDatabase(const string& flightNumber);
	void updateCrewFlightHours(const shared_ptr<Flight>& flight);
	
	// Helper for updating flight details
	void updateFlightDetails(const shared_ptr<Flight>& flight);
	
	// Helper for selecting flight status
	string selectFlightStatus();
	
	// Helper to display flights in a table format
	void displayFlightsTable(const vector<shared_ptr<Flight>>& flights, const string& title = "");
	
public:
	// Singleton accessor
	static FlightManager* getInstance();
	
	// Main menu
	void manageFlights();
	void searchFlights();
	
	// Query methods (for other classes to use)
	shared_ptr<Flight> getFlight(const string& flightNumber);
	vector<shared_ptr<Flight>> searchFlightsByRoute(const string& origin, const string& destination, const string& departureDate);
	vector<string> getAllFlightNumbers();
	bool flightExists(const string& flightNumber);
	
	// Deletion check (for cascade operations)
	static bool hasActiveReservations(const string& flightNumber);
	
	// Destructor
	~FlightManager() noexcept;
	
	// Delete copy and move constructors/assignments
	FlightManager(const FlightManager&) = delete;
	FlightManager(FlightManager&&) = delete;
	FlightManager& operator=(const FlightManager&) = delete;
	FlightManager& operator=(FlightManager&&) = delete;
};

#endif // FLIGHTMANAGER_HPP