#include <iostream>
#include <algorithm>
#include <iomanip>
#include "AircraftManager.hpp"
#include "FlightManager.hpp"
#include "SeatMap.hpp"
#include "Maintenance.hpp"

// Static member initialization
AircraftManager* AircraftManager::instance = nullptr;

// ==================== Constructor & Singleton ====================

AircraftManager::AircraftManager()
{
	db = std::make_unique<Database>("Aircrafts");
	ui = UserInterface::getInstance();
	creator = std::make_unique<AircraftCreator>();
}

AircraftManager* AircraftManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new AircraftManager();
	}
	return instance;
}

AircraftManager::~AircraftManager() noexcept {}

// ==================== Menu Methods ====================

void AircraftManager::manageAircraft()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Add New Aircraft Type",
			"View All Aircraft Types",
			"Update Aircraft Type",
			"Remove Aircraft Type",
			"Manage Maintenance",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Manage Aircraft", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 6);
			
			switch (choice)
			{
				case 1:
					addAircraft();
					break;
				case 2:
					viewAllAircraft();
					break;
				case 3:
					updateAircraft();
					break;
				case 4:
					removeAircraft();
					break;
				case 5:
					manageMaintenance();
					break;
				case 6:
					return;
				default:
					ui->printError("Invalid choice.");
					ui->pauseScreen();
					break;
			}
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
			ui->pauseScreen();
		}
	}
}

void AircraftManager::addAircraft()
{
	ui->clearScreen();
	ui->printHeader("Add New Aircraft Type");
	
	try
	{
		shared_ptr<Aircraft> newAircraft = creator->createNewAircraft();
		
		if (!newAircraft)
		{
			return;
		}
		
		// Check if aircraft already exists
		if (db->entryExists(newAircraft->getAircraftType()))
		{
			ui->printError("Aircraft type '" + newAircraft->getAircraftType() + "' already exists.");
			ui->pauseScreen();
			return;
		}
		
		// Save to database
		saveAircraftToDatabase(newAircraft);
		ui->printSuccess("Aircraft type '" + newAircraft->getAircraftType() + "' has been successfully added.");
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void AircraftManager::viewAllAircraft()
{
	ui->clearScreen();
	ui->printHeader("View All Aircraft Types");
	
	try
	{
		json allAircraftData = db->loadAll();
		
		if (allAircraftData.empty())
		{
			ui->printWarning("No aircraft types found in the system.");
			ui->pauseScreen();
			return;
		}
		
		vector<shared_ptr<Aircraft>> aircraft;
		
		for (const auto& [aircraftType, aircraftData] : allAircraftData.items())
		{
			try
			{
				aircraft.push_back(creator->createFromJson(aircraftType, aircraftData));
			}
			catch (const std::exception& e)
			{
				// Skip aircraft with errors
				continue;
			}
		}
		
		displayAircraftTable(aircraft, "All Aircraft Types");
		ui->println("\nTotal Aircraft Types: " + std::to_string(aircraft.size()));
	}
	catch (const std::exception& e)
	{
		ui->printError("Error retrieving aircraft: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void AircraftManager::updateAircraft()
{
	ui->clearScreen();
	ui->printHeader("Update Aircraft Type");
	
	try
	{
		string aircraftType = ui->getString("Enter Aircraft Type to Update: ");
		
		shared_ptr<Aircraft> aircraft = loadAircraftFromDatabase(aircraftType);
		if (!aircraft)
		{
			ui->printError("Aircraft type not found.");
			ui->pauseScreen();
			return;
		}
		
		vector<string> options = {
			"Aircraft Details",
			"Fleet Count",
			"Status",
			"Back to Manage Aircraft"
		};
		
		ui->displayMenu("Update Aircraft Type", options);
		
		int choice = ui->getChoice("Enter choice: ", 1, 4);
		
		switch (choice)
		{
			case 1:
				updateAircraftDetails(aircraft);
				break;
			case 2:
			{
				int newFleetCount = ui->getInt("Enter new Fleet Count: ");
				if (newFleetCount > 0)
				{
					aircraft->setFleetCount(newFleetCount);
					ui->printSuccess("Fleet count updated successfully.");
					saveAircraftToDatabase(aircraft);
				}
				else
				{
					ui->printError("Fleet count must be positive.");
				}
				break;
			}
			case 3:
			{
				string newStatus = selectAircraftStatus();
				aircraft->setStatus(newStatus);
				ui->printSuccess("Aircraft status updated successfully.");
				saveAircraftToDatabase(aircraft);
				break;
			}
			case 4:
				ui->println("Returning to Manage Aircraft menu.");
				ui->pauseScreen();
				return;
			default:
				ui->printError("Invalid choice.");
				break;
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void AircraftManager::removeAircraft()
{
	ui->clearScreen();
	ui->printHeader("Remove Aircraft Type");
	
	try
	{
		string aircraftType = ui->getString("Enter Aircraft Type to Remove: ");
		
		if (!db->entryExists(aircraftType))
		{
			ui->printError("Aircraft type not found.");
			ui->pauseScreen();
			return;
		}
		
		// Check if aircraft is used by any flights
		if (isAircraftUsedByFlights(aircraftType))
		{
			ui->printError("Cannot delete aircraft type in use by active flights.");
			ui->pauseScreen();
			return;
		}
		
		bool confirm = ui->getYesNo("Are you sure you want to remove aircraft type '" + aircraftType + "'?");
		if (confirm)
		{
			deleteAircraftFromDatabase(aircraftType);
			ui->printSuccess("Aircraft type '" + aircraftType + "' has been removed successfully.");
		}
		else
		{
			ui->printWarning("Aircraft removal canceled.");
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void AircraftManager::manageMaintenance()
{
	ui->clearScreen();
	ui->printHeader("Manage Maintenance");

	Maintenance* maintenance = Maintenance::getInstance();	
	vector<string> aircraftTypes = getAllAircraftTypes();
	
	if (aircraftTypes.empty())
	{
		ui->printError("No aircraft types available.");
		ui->pauseScreen();
		return;
	}
	
	// Display aircraft types for selection
	ui->println("\nAvailable Aircraft Types:");
	for (size_t i = 0; i < aircraftTypes.size(); ++i)
	{
		ui->println(std::to_string(i + 1) + ". " + aircraftTypes[i]);
	}
	ui->println(std::to_string(aircraftTypes.size() + 1) + ". Back to Manage Aircraft Menu\n");
	
	try
	{
		int choice = ui->getChoice("Select Aircraft Type: ", 1, static_cast<int>(aircraftTypes.size()) + 1);
		string selectedAircraftType = aircraftTypes[choice - 1];

		ui->clearScreen();
		
		vector<string> options = {
			"Schedule Maintenance",
			"View All Maintenance",
			"View Upcoming Maintenance",
			"Complete Maintenance",
			"Cancel Maintenance",
			"Back to Aircraft Selection"
		};
		
		ui->displayMenu("Maintenance for " + selectedAircraftType, options);
		
		choice = ui->getChoice("Enter choice: ", 1, 6);
		
		switch (choice)
		{
			case 1:
			{
				MaintenanceResult result = maintenance->scheduleMaintenance(selectedAircraftType);
				
				if (result.success && !result.newAircraftStatus.empty())
				{
					auto aircraft = getAircraft(selectedAircraftType);
					if (aircraft)
					{
						aircraft->setStatus(result.newAircraftStatus);
						saveAircraftToDatabase(aircraft);
					}
					ui->printSuccess(result.message);
				}
				else if (!result.success)
				{
					ui->printError(result.message);
				}
				
				ui->pauseScreen();
				break;
			}
			case 2:
			{
				maintenance->displayAllMaintenance(selectedAircraftType);
				break;
			}
			case 3:
			{
				maintenance->displayUpcomingMaintenance(selectedAircraftType);
				break;
			}
			case 4:
			{
				MaintenanceResult result = maintenance->completeMaintenance(selectedAircraftType);
				
				if (result.success && !result.newAircraftStatus.empty())
				{
					auto aircraft = getAircraft(selectedAircraftType);
					if (aircraft)
					{
						aircraft->setStatus(result.newAircraftStatus);
						saveAircraftToDatabase(aircraft);
					}
					ui->printSuccess(result.message);
				}
				else if (!result.success)
				{
					ui->printError(result.message);
				}
				
				ui->pauseScreen();
				break;
			}
			case 5:
			{
				MaintenanceResult result = maintenance->cancelMaintenance(selectedAircraftType);
				
				if (result.success && !result.newAircraftStatus.empty())
				{
					auto aircraft = getAircraft(selectedAircraftType);
					if (aircraft)
					{
						aircraft->setStatus(result.newAircraftStatus);
						saveAircraftToDatabase(aircraft);
					}
					ui->printSuccess(result.message);
				}
				else if (!result.success)
				{
					ui->printError(result.message);
				}
				
				ui->pauseScreen();
				break;
			}
			case 6:
				return;
			default:
				ui->printError("Invalid choice.");
				ui->pauseScreen();
				break;
		}
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
		ui->pauseScreen();
	}
}

// ==================== Helper Methods ====================

shared_ptr<Aircraft> AircraftManager::loadAircraftFromDatabase(const string& aircraftType)
{
	if (!db->entryExists(aircraftType))
	{
		return nullptr;
	}
	
	try
	{
		json aircraftData = db->getEntry(aircraftType);
		return creator->createFromJson(aircraftType, aircraftData);
	}
	catch (const std::exception& e)
	{
		ui->printError("Error loading aircraft: " + string(e.what()));
		return nullptr;
	}
}

void AircraftManager::saveAircraftToDatabase(const shared_ptr<Aircraft>& aircraft)
{
	if (!aircraft)
	{
		throw AircraftException("An error occurred while accessing the database.");
	}
	
	try
	{
		json aircraftData = creator->toJson(aircraft);
		if (db->entryExists(aircraft->getAircraftType()))
		{
			db->updateEntry(aircraft->getAircraftType(), aircraftData);
		}
		else
		{
			db->addEntry(aircraft->getAircraftType(), aircraftData);
		}
	}
	catch (const DatabaseException& e)
	{
		throw AircraftException("An error occurred while accessing the database." + string(e.what()));
	}
}

void AircraftManager::deleteAircraftFromDatabase(const string& aircraftType)
{
	try
	{
		db->deleteEntry(aircraftType);
	}
	catch (const DatabaseException& e)
	{
		throw AircraftException("An error occurred while accessing the database." + string(e.what()));
	}
}

void AircraftManager::updateAircraftDetails(const shared_ptr<Aircraft>& aircraft)
{
	ui->clearScreen();
	ui->printHeader("Update Aircraft Details");
	
	ui->println("Current Aircraft Information:");
	ui->println("1. Manufacturer: " + aircraft->getManufacturer());
	ui->println("2. Model: " + aircraft->getModel());
	ui->println("3. Total Seats: " + std::to_string(aircraft->getTotalSeats()));
	ui->println("4. Seat Layout: " + aircraft->getSeatLayout());
	ui->println("5. Rows: " + std::to_string(aircraft->getRows()));
	ui->println("6. Back to Previous Menu\n");
	
	try
	{
		int choice = ui->getChoice("Select field to update (1-6): ", 1, 6);
		
		switch (choice)
		{
			case 1:  // Update Manufacturer
			{
				string newManufacturer = creator->getValidManufacturer();
				aircraft->setManufacturer(newManufacturer);
				saveAircraftToDatabase(aircraft);
				ui->printSuccess("Manufacturer updated successfully.");
				break;
			}
			case 2:  // Update Model
			{
				string newModel = creator->getValidModel();
				aircraft->setModel(newModel);
				saveAircraftToDatabase(aircraft);
				ui->printSuccess("Model updated successfully.");
				break;
			}
			case 3:  // Update Total Seats
			{
				int newTotalSeats = creator->getValidSeatCount();
				
				// Verify that the new seat count matches the layout and rows
				int calculatedSeats = SeatMap::calculateSeatCount(aircraft->getSeatLayout(), aircraft->getRows());
				if (newTotalSeats != calculatedSeats)
				{
					ui->printWarning("Total seats (" + std::to_string(newTotalSeats) + 
					                ") doesn't match calculated seats (" + std::to_string(calculatedSeats) + 
					                ") based on current layout and rows.");
					ui->println("Please update seat layout and rows first if needed.");
					ui->pauseScreen();
					break;
				}
				
				aircraft->setTotalSeats(newTotalSeats);
				saveAircraftToDatabase(aircraft);
				ui->printSuccess("Total seats updated successfully.");
				break;
			}
			case 4:  // Update Seat Layout
			{
				ui->println("\nExamples: 3-3 (single aisle), 2-4-2 (twin aisle), 3-4-3 (wide body)");
				string newSeatLayout = creator->getValidSeatLayout();
				
				// Show preview of new seat map
				ui->println("\nPreview of new seat layout:");
				vector<string> previewLabels;
				vector<vector<string>> previewData;
				SeatMap::getSampleSeatMapDisplayData(newSeatLayout, aircraft->getRows(), 
				                                     previewLabels, previewData, 5);
				
				GridDisplayConfig previewConfig;
				previewConfig.title = "Sample Seat Map Preview";
				previewConfig.legend = SeatMap::getSampleSeatMapFooter(newSeatLayout, aircraft->getRows());
				previewConfig.showSeparator = true;
				
				ui->displayGrid(previewLabels, previewData, previewConfig);
				
				bool confirm = ui->getYesNo("\nApply this seat layout?");
				if (confirm)
				{
					// Update seat layout and recalculate total seats
					int newTotalSeats = SeatMap::calculateSeatCount(newSeatLayout, aircraft->getRows());
					aircraft->setSeatLayout(newSeatLayout);
					aircraft->setTotalSeats(newTotalSeats);
					saveAircraftToDatabase(aircraft);
					ui->printSuccess("Seat layout updated successfully. Total seats: " + std::to_string(newTotalSeats));
				}
				else
				{
					ui->printWarning("Seat layout update canceled.");
				}
				break;
			}
			case 5:  // Update Rows
			{
				int newRows = creator->getValidRows();
				
				// Show preview with new row count
				ui->println("\nPreview with " + std::to_string(newRows) + " rows:");
				vector<string> previewLabels;
				vector<vector<string>> previewData;
				SeatMap::getSampleSeatMapDisplayData(aircraft->getSeatLayout(), newRows, 
				                                     previewLabels, previewData, 5);
				
				GridDisplayConfig previewConfig;
				previewConfig.title = "Sample Seat Map Preview";
				previewConfig.legend = SeatMap::getSampleSeatMapFooter(aircraft->getSeatLayout(), newRows);
				previewConfig.showSeparator = true;
				
				ui->displayGrid(previewLabels, previewData, previewConfig);
				
				bool confirm = ui->getYesNo("\nApply this row count?");
				if (confirm)
				{
					// Update rows and recalculate total seats
					int newTotalSeats = SeatMap::calculateSeatCount(aircraft->getSeatLayout(), newRows);
					aircraft->setRows(newRows);
					aircraft->setTotalSeats(newTotalSeats);
					saveAircraftToDatabase(aircraft);
					ui->printSuccess("Rows updated successfully. Total seats: " + std::to_string(newTotalSeats));
				}
				else
				{
					ui->printWarning("Rows update canceled.");
				}
				break;
			}
			case 6:  // Back
				ui->printWarning("Returning to previous menu.");
				return;
			default:
				ui->printError("Invalid choice.");
				break;
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

string AircraftManager::selectAircraftStatus()
{
	vector<string> statusOptions = {
		"Available",
		"In Flight",
		"Maintenance",
		"Out of Service"
	};
	
	ui->displayMenu("Select Aircraft Status", statusOptions);
	
	try
	{
		int choice = ui->getChoice("Enter status: ", 1, static_cast<int>(statusOptions.size()));
		return statusOptions[choice - 1];
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
		return "Available";
	}
}

void AircraftManager::displayAircraftTable(const vector<shared_ptr<Aircraft>>& aircraft, const string& title)
{
	if (aircraft.empty())
	{
		ui->printWarning("No aircraft to display.");
		return;
	}
	
	vector<string> headers = {
		"Aircraft Type", "Manufacturer", "Model", "Total Seats", "Layout", "Rows", "Fleet Count", "Status"
	};
	
	vector<vector<string>> rows;
	
	for (const auto& acft : aircraft)
	{
		if (acft)
		{
			try
			{
				rows.push_back({
					acft->getAircraftType(),
					acft->getManufacturer(),
					acft->getModel(),
					std::to_string(acft->getTotalSeats()),
					acft->getSeatLayout(),
					std::to_string(acft->getRows()),
					std::to_string(acft->getFleetCount()),
					acft->getStatus()
				});
			}
			catch (const std::exception& e)
			{
				// Skip aircraft with display errors
				continue;
			}
		}
	}
	
	if (rows.empty())
	{
		ui->printWarning("No valid aircraft to display.");
		return;
	}
	
	if (!title.empty())
	{
		ui->println("\n" + title);
		ui->printSeparator();
	}
	
	ui->displayTable(headers, rows);
}

// ==================== Query Methods ====================

shared_ptr<Aircraft> AircraftManager::getAircraft(const string& aircraftType)
{
	return loadAircraftFromDatabase(aircraftType);
}

vector<string> AircraftManager::getAllAircraftTypes()
{
	vector<string> aircraftTypes;
	
	try
	{
		json allAircraftData = db->loadAll();
		for (const auto& [aircraftType, aircraftData] : allAircraftData.items())
		{
			aircraftTypes.push_back(aircraftType);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error retrieving aircraft types: " + string(e.what()));
	}
	
	return aircraftTypes;
}

bool AircraftManager::aircraftTypeExists(const string& aircraftType)
{
	try
	{
		return db->entryExists(aircraftType);
	}
	catch (const std::exception& e)
	{
		return false;
	}
}

bool AircraftManager::isAircraftUsedByFlights(const string& aircraftType)
{
	// Query FlightManager to check if any flights use this aircraft type
	try
	{
		vector<string> flightNumbers = FlightManager::getInstance()->getAllFlightNumbers();
		
		for (const auto& flightNumber : flightNumbers)
		{
			shared_ptr<Flight> flight = FlightManager::getInstance()->getFlight(flightNumber);
			if (flight && flight->getAircraftType() == aircraftType)
			{
				return true;  // Aircraft is used by at least one flight
			}
		}
	}
	catch (const std::exception& e)
	{
		// If there's an error, assume it's in use to be safe
		return true;
	}
	
	return false;  // Aircraft is not used by any flights
}