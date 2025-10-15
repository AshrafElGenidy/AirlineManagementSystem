#include <ctime>
#include <sstream>
#include <iomanip>
#include "Maintenance.hpp"

Maintenance* Maintenance::instance = nullptr;

Maintenance::Maintenance()
{
	db = std::make_unique<Database>("Maintenance");
	ui = UserInterface::getInstance();
}

Maintenance* Maintenance::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Maintenance();
	}
	return instance;
}

Maintenance::~Maintenance() noexcept {}

MaintenanceResult Maintenance::scheduleMaintenance(const string& aircraftType)
{
	MaintenanceResult result = {false, aircraftType, "", ""};
	
	ui->clearScreen();
	ui->printHeader("Schedule Maintenance for " + aircraftType);
	
	try
	{
		string scheduledDate = ui->getDate("Enter Scheduled Date: ", "YYYY-MM-DD");
		string description = ui->getString("Enter Description: ");
		
		// Generate maintenance ID
		json allData = db->loadAll();
		int count = allData.size() + 1;
		std::ostringstream oss;
		oss << "MNT" << std::setfill('0') << std::setw(3) << count;
		string maintenanceId = oss.str();
		
		// Create maintenance record
		json maintenanceRecord = {
			{"maintenanceId", maintenanceId},
			{"aircraftType", aircraftType},
			{"status", "SCHEDULED"},
			{"scheduledDate", scheduledDate},
			{"completedDate", ""},
			{"description", description}
		};
		
		// Save to maintenance database
		db->addEntry(maintenanceId, maintenanceRecord);
		
		result.success = true;
		result.newAircraftStatus = "Maintenance";
		result.message = "Maintenance '" + maintenanceId + "' scheduled successfully.";
	}
	catch (const std::exception& e)
	{
		result.success = false;
		result.message = string(e.what());
	}
	
	return result;
}

MaintenanceResult Maintenance::completeMaintenance(const string& aircraftType)
{
	MaintenanceResult result = {false, aircraftType, "", ""};
	
	ui->clearScreen();
	ui->printHeader("Complete Maintenance for " + aircraftType);
	
	try
	{
		// Get all maintenance records for this aircraft
		vector<json> records = getAllMaintenanceForAircraft(aircraftType);
		vector<json> pending;
		
		for (const auto& record : records)
		{
			if (record["status"] != "COMPLETED" && record["status"] != "CANCELED")
			{
				pending.push_back(record);
			}
		}
		
		if (pending.empty())
		{
			ui->printWarning("No pending maintenance for " + aircraftType);
			ui->pauseScreen();
			result.message = "No pending maintenance found.";
			return result;
		}
		
		displayMaintenanceTable(pending, "Pending Maintenance");
		
		string maintenanceId = ui->getString("Enter Maintenance ID to complete: ");
		
		if (!db->entryExists(maintenanceId))
		{
			ui->printError("Maintenance record not found.");
			ui->pauseScreen();
			result.message = "Maintenance record not found.";
			return result;
		}
		
		json record = db->getEntry(maintenanceId);
		
		// Verify it belongs to this aircraft type
		if (record["aircraftType"] != aircraftType)
		{
			ui->printError("This maintenance record does not belong to " + aircraftType);
			ui->pauseScreen();
			result.message = "Maintenance record does not belong to this aircraft.";
			return result;
		}
		
		// Check if already completed
		if (record["status"] == "COMPLETED")
		{
			ui->printError("This maintenance has already been completed.");
			ui->pauseScreen();
			result.message = "Maintenance already completed.";
			return result;
		}
		
		// Display current record
		ui->println("\nMaintenance Record:");
		ui->println("ID: " + record["maintenanceId"].get<string>());
		ui->println("Aircraft Type: " + record["aircraftType"].get<string>());
		ui->println("Status: " + record["status"].get<string>());
		ui->println("Scheduled Date: " + record["scheduledDate"].get<string>());
		ui->println("Description: " + record["description"].get<string>());
		
		bool confirm = ui->getYesNo("\nMark this maintenance as completed?");
		if (!confirm)
		{
			ui->printWarning("Operation canceled.");
			ui->pauseScreen();
			result.message = "Operation canceled by user.";
			return result;
		}
		
		// Update record in maintenance database
		record["status"] = "COMPLETED";
		record["completedDate"] = getCurrentDate();
		db->updateEntry(maintenanceId, record);
		
		result.success = true;
		result.newAircraftStatus = "Available";
		result.message = "Maintenance '" + maintenanceId + "' marked as completed.";
	}
	catch (const std::exception& e)
	{
		result.success = false;
		result.message = string(e.what());
	}
	
	return result;
}

MaintenanceResult Maintenance::cancelMaintenance(const string& aircraftType)
{
	MaintenanceResult result = {false, aircraftType, "", ""};
	
	ui->clearScreen();
	ui->printHeader("Cancel Maintenance for " + aircraftType);
	
	try
	{
		// Get all maintenance records for this aircraft
		vector<json> records = getAllMaintenanceForAircraft(aircraftType);
		vector<json> pending;
		
		for (const auto& record : records)
		{
			if (record["status"] != "COMPLETED" && record["status"] != "CANCELED")
			{
				pending.push_back(record);
			}
		}
		
		if (pending.empty())
		{
			ui->printWarning("No pending maintenance for " + aircraftType);
			ui->pauseScreen();
			result.message = "No pending maintenance found.";
			return result;
		}
		
		displayMaintenanceTable(pending, "Pending Maintenance");
		
		string maintenanceId = ui->getString("Enter Maintenance ID to cancel: ");
		
		if (!db->entryExists(maintenanceId))
		{
			ui->printError("Maintenance record not found.");
			ui->pauseScreen();
			result.message = "Maintenance record not found.";
			return result;
		}
		
		json record = db->getEntry(maintenanceId);
		
		// Verify it belongs to this aircraft type
		if (record["aircraftType"] != aircraftType)
		{
			ui->printError("This maintenance record does not belong to " + aircraftType);
			ui->pauseScreen();
			result.message = "Maintenance record does not belong to this aircraft.";
			return result;
		}
		
		string currentStatus = record["status"].get<string>();
		if (currentStatus == "COMPLETED")
		{
			ui->printError("Cannot cancel completed maintenance.");
			ui->pauseScreen();
			result.message = "Cannot cancel completed maintenance.";
			return result;
		}
		
		if (currentStatus == "CANCELED")
		{
			ui->printWarning("This maintenance has already been canceled.");
			ui->pauseScreen();
			result.message = "Maintenance already canceled.";
			return result;
		}
		
		// Display current record
		ui->println("\nMaintenance Record:");
		ui->println("ID: " + record["maintenanceId"].get<string>());
		ui->println("Aircraft Type: " + record["aircraftType"].get<string>());
		ui->println("Status: " + currentStatus);
		ui->println("Scheduled Date: " + record["scheduledDate"].get<string>());
		ui->println("Description: " + record["description"].get<string>());
		
		bool confirm = ui->getYesNo("\nCancel this maintenance?");
		if (!confirm)
		{
			ui->printWarning("Operation canceled.");
			ui->pauseScreen();
			result.message = "Operation canceled by user.";
			return result;
		}
		
		// Update record in maintenance database
		record["status"] = "CANCELED";
		db->updateEntry(maintenanceId, record);
		
		result.success = true;
		result.newAircraftStatus = "Available";
		result.message = "Maintenance '" + maintenanceId + "' canceled.";
	}
	catch (const std::exception& e)
	{
		result.success = false;
		result.message = string(e.what());
	}
	
	return result;
}

void Maintenance::displayAllMaintenance(const string& aircraftType)
{
	ui->clearScreen();
	ui->printHeader("View All Maintenance for " + aircraftType);
	
	try
	{
		vector<json> records = getAllMaintenanceForAircraft(aircraftType);
		
		if (records.empty())
		{
			ui->printWarning("No maintenance records found for " + aircraftType);
		}
		else
		{
			displayMaintenanceTable(records, "Maintenance Records for " + aircraftType);
			ui->println("\nTotal Records: " + std::to_string(records.size()));
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void Maintenance::displayUpcomingMaintenance(const string& aircraftType)
{
	ui->clearScreen();
	ui->printHeader("View Upcoming Maintenance for " + aircraftType);
	
	try
	{
		vector<json> upcoming = getUpcomingMaintenanceForAircraft(aircraftType);
		
		if (upcoming.empty())
		{
			ui->printWarning("No upcoming maintenance scheduled.");
		}
		else
		{
			displayMaintenanceTable(upcoming, "Upcoming Maintenance (Next 30 Days)");
			ui->println("\nTotal Upcoming: " + std::to_string(upcoming.size()));
		}
		
		// Also show overdue maintenance
		vector<json> overdue = getOverdueMaintenanceForAircraft(aircraftType);
		if (!overdue.empty())
		{
			ui->printWarning("\n*** OVERDUE MAINTENANCE DETECTED ***");
			displayMaintenanceTable(overdue, "Overdue Maintenance");
			ui->println("\nTotal Overdue: " + std::to_string(overdue.size()));
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void Maintenance::displayMaintenanceTable(const vector<json>& records, const string& title)
{
	if (records.empty())
	{
		ui->printWarning("No maintenance records to display.");
		return;
	}
	
	vector<string> headers = {
		"Maintenance ID", "Status", "Scheduled Date", "Completed Date", "Description"
	};
	
	vector<vector<string>> rows;
	
	for (const auto& record : records)
	{
		try
		{
			string completedDate = record["completedDate"].get<string>();
			if (completedDate.empty())
			{
				completedDate = "N/A";
			}
			
			rows.push_back({
				record["maintenanceId"].get<string>(),
				record["status"].get<string>(),
				record["scheduledDate"].get<string>(),
				completedDate,
				record["description"].get<string>()
			});
		}
		catch (const std::exception& e)
		{
			continue;
		}
	}
	
	if (rows.empty())
	{
		ui->printWarning("No valid maintenance records to display.");
		return;
	}
	
	if (!title.empty())
	{
		ui->println("\n" + title);
		ui->printSeparator();
	}
	
	ui->displayTable(headers, rows);
}

vector<json> Maintenance::getAllMaintenanceForAircraft(const string& aircraftType)
{
	vector<json> filtered;
	
	try
	{
		json allData = db->loadAll();
		
		for (const auto& [key, value] : allData.items())
		{
			if (value["aircraftType"].get<string>() == aircraftType)
			{
				filtered.push_back(value);
			}
		}
	}
	catch (const std::exception& e)
	{
		// Silent fail
	}
	
	return filtered;
}

vector<json> Maintenance::getUpcomingMaintenanceForAircraft(const string& aircraftType)
{
	vector<json> upcoming;
	vector<json> records = getAllMaintenanceForAircraft(aircraftType);
	string today = getCurrentDate();
	
	for (const auto& record : records)
	{
		try
		{
			string status = record["status"].get<string>();
			string scheduledDate = record["scheduledDate"].get<string>();
			
			if (status == "SCHEDULED" && scheduledDate >= today)
			{
				upcoming.push_back(record);
			}
		}
		catch (const std::exception& e)
		{
			continue;
		}
	}
	
	return upcoming;
}

vector<json> Maintenance::getOverdueMaintenanceForAircraft(const string& aircraftType)
{
	vector<json> overdue;
	vector<json> records = getAllMaintenanceForAircraft(aircraftType);
	string today = getCurrentDate();
	
	for (const auto& record : records)
	{
		try
		{
			string status = record["status"].get<string>();
			string scheduledDate = record["scheduledDate"].get<string>();
			
			if (status != "COMPLETED" && status != "CANCELED" && scheduledDate < today)
			{
				overdue.push_back(record);
			}
		}
		catch (const std::exception& e)
		{
			continue;
		}
	}
	
	return overdue;
}

vector<json> Maintenance::getAllMaintenance()
{
	vector<json> records;
	
	try
	{
		json allData = db->loadAll();
		
		for (const auto& [key, value] : allData.items())
		{
			records.push_back(value);
		}
	}
	catch (const std::exception& e)
	{
		// Silent fail
	}
	
	return records;
}

string Maintenance::getCurrentDate() const
{
	time_t now = time(nullptr);
	tm* ltm = localtime(&now);
	
	std::ostringstream oss;
	oss << (1900 + ltm->tm_year) << "-"
	    << std::setfill('0') << std::setw(2) << (1 + ltm->tm_mon) << "-"
	    << std::setfill('0') << std::setw(2) << ltm->tm_mday;
	
	return oss.str();
}