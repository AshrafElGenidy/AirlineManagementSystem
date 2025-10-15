#ifndef MAINTENANCE_HPP
#define MAINTENANCE_HPP

#include <string>
#include <vector>
#include <memory>
#include "Database.hpp"
#include "UserInterface.hpp"
#include "json.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using nlohmann::json;

// Result struct to communicate back to AircraftManager
struct MaintenanceResult
{
	bool success;
	string aircraftType;
	string newAircraftStatus;  // "Maintenance", "Available", or ""
	string message;
};

class Maintenance
{
private:
	static Maintenance* instance;
	
	unique_ptr<Database> db;
	UserInterface* ui;
	
	Maintenance();
	
	void displayMaintenanceTable(const vector<json>& records, const string& title = "");
	string getCurrentDate() const;
	
public:
	static Maintenance* getInstance();
	
	// UI menu methods - handle both UI and database persistence
	// Return MaintenanceResult to tell AircraftManager what status to set
	MaintenanceResult scheduleMaintenance(const string& aircraftType);
	MaintenanceResult completeMaintenance(const string& aircraftType);
	MaintenanceResult cancelMaintenance(const string& aircraftType);
	
	// Display methods - read-only
	void displayAllMaintenance(const string& aircraftType);
	void displayUpcomingMaintenance(const string& aircraftType);
	
	// Query methods for reports
	vector<json> getAllMaintenanceForAircraft(const string& aircraftType);
	vector<json> getUpcomingMaintenanceForAircraft(const string& aircraftType);
	vector<json> getOverdueMaintenanceForAircraft(const string& aircraftType);
	vector<json> getAllMaintenance();
	
	~Maintenance() noexcept;
	
	Maintenance(const Maintenance&) = delete;
	Maintenance(Maintenance&&) = delete;
	Maintenance& operator=(const Maintenance&) = delete;
	Maintenance& operator=(Maintenance&&) = delete;
};

#endif // MAINTENANCE_HPP