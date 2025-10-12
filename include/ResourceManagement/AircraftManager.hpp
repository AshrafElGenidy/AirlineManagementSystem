#ifndef AIRCRAFTMANAGER_HPP
#define AIRCRAFTMANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Aircraft.hpp"
#include "AircraftCreator.hpp"
#include "Database.hpp"
#include "UserInterface.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

// ==================== AircraftManager ====================

class AircraftManager
{
private:
	static AircraftManager* instance;
	
	unique_ptr<Database> db;
	UserInterface* ui;
	unique_ptr<AircraftCreator> creator;
	
	// Private constructor for singleton
	AircraftManager();
	
	// Private menu methods
	void addAircraft();
	void viewAllAircraft();
	void updateAircraft();
	void removeAircraft();
	
	// Helper methods
	shared_ptr<Aircraft> loadAircraftFromDatabase(const string& aircraftType);
	void saveAircraftToDatabase(const shared_ptr<Aircraft>& aircraft);
	void deleteAircraftFromDatabase(const string& aircraftType);
	
	// Helper for updating aircraft details
	void updateAircraftDetails(const shared_ptr<Aircraft>& aircraft);
	
	// Helper for selecting aircraft status
	string selectAircraftStatus();
	
	// Helper to display aircraft in a table format
	void displayAircraftTable(const vector<shared_ptr<Aircraft>>& aircraft, const string& title = "");
	
public:
	// Singleton accessor
	static AircraftManager* getInstance();
	
	// Main menu
	void manageAircraft();
	
	// Query methods (for other classes to use)
	shared_ptr<Aircraft> getAircraft(const string& aircraftType);
	vector<string> getAllAircraftTypes();
	bool aircraftTypeExists(const string& aircraftType);
	
	// Deletion check (for cascade operations)
	static bool isAircraftUsedByFlights(const string& aircraftType);
	
	// Destructor
	~AircraftManager() noexcept;
	
	// Delete copy and move constructors/assignments
	AircraftManager(const AircraftManager&) = delete;
	AircraftManager(AircraftManager&&) = delete;
	AircraftManager& operator=(const AircraftManager&) = delete;
	AircraftManager& operator=(AircraftManager&&) = delete;
};

#endif // AIRCRAFTMANAGER_HPP