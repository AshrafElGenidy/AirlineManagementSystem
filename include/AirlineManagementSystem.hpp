#ifndef AIRLINE_MANAGEMENT_SYSTEM_HPP
#define AIRLINE_MANAGEMENT_SYSTEM_HPP

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "User.hpp"
#include "Reservation.hpp"
#include "ReportGenerator.hpp"
#include "CheckInSystem.hpp"
#include "Aircraft.hpp"
#include "CrewMember.hpp"
#include "Pilot.hpp"
#include "FlightAttendant.hpp"
#include "Flight.hpp"

class AirlineManagementSystem : public std::enable_shared_from_this<AirlineManagementSystem>
{
private:
	static std::shared_ptr<AirlineManagementSystem> instance;
	std::map<std::string, Flight*> flights;
	std::map<std::string, Aircraft*> aircraft;
	std::map<std::string, Reservation*> reservations;
	std::map<std::string, CrewMember*> crewMembers;
	std::vector<Pilot*> pilots;
	std::vector<FlightAttendant*> flightAttendants;
	User* currentUser;
	ReportGenerator* reportGenerator;
	CheckInSystem* checkInSystem;
	
	AirlineManagementSystem();

public:
	static std::shared_ptr<AirlineManagementSystem> getInstance();
	
	void initialize();
	void start();
	void displayWelcome();
	int selectRole();
	User* login(UserRole role);
	void logout();
	
	// Flight Management - delegates menu operations to Flight/FlightManager
	void manageFlights();
	bool addFlight(Flight* flight);
	bool removeFlight(const std::string& flightNumber);
	bool updateFlight(const std::string& flightNumber);
	Flight* getFlight(const std::string& flightNumber);
	std::vector<Flight*> getAllFlights();
	std::vector<Flight*> searchFlights(const SearchCriteria& criteria);
	
	// Aircraft Management - delegates menu operations to Aircraft/AircraftManager
	void manageAircraft();
	bool addAircraft(Aircraft* aircraft);
	Aircraft* getAircraft(const std::string& aircraftId);
	std::vector<Aircraft*> getAllAircraft();
	
	// Crew Management
	bool addCrewMember(CrewMember* crew);
	std::vector<Pilot*> getPilots();
	std::vector<FlightAttendant*> getFlightAttendants();
	bool assignCrewToFlight(const std::string& flightNumber, const std::string& pilotId, 
	                        const std::string& attendantId);
	
	// Reservation Management
	bool createReservation(Reservation* reservation);
	Reservation* getReservation(const std::string& reservationId);
	bool cancelReservation(const std::string& reservationId);
	std::vector<Reservation*> getPassengerReservations(const std::string& passengerId);
	std::string generateReservationId();
	bool isSeatAvailable(const std::string& flightNumber, const std::string& seatNumber);
	
	// Report Generation - delegates to ReportGenerator
	void generateReports();
	
	void shutdown();
};

#endif // AIRLINE_MANAGEMENT_SYSTEM_HPP