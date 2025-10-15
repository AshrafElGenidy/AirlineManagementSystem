#include <iostream>
#include <algorithm>
#include <iomanip>
#include "CrewManager.hpp"
#include "FlightManager.hpp"
#include "Flight.hpp"

// Static member initialization
CrewManager* CrewManager::instance = nullptr;

// ==================== Constructor & Singleton ====================

CrewManager::CrewManager()
{
	db = std::make_unique<Database>("Crews");
	ui = UserInterface::getInstance();
}

CrewManager* CrewManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new CrewManager();
	}
	return instance;
}

CrewManager::~CrewManager() noexcept {}

// ==================== Menu Methods ====================

void CrewManager::manageCrew()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Add Crew Member",
			"View All Crew",
			"View Crew Details",
			"Update Crew Member",
			"Remove Crew Member",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Manage Crew", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 6);
			
			switch (choice)
			{
				case 1:
					addCrewMember();
					break;
				case 2:
					viewAllCrew();
					break;
				case 3:
					viewCrewDetails();
					break;
				case 4:
					updateCrewMember();
					break;
				case 5:
					removeCrewMember();
					break;
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
			ui->printError(e.what());
			ui->pauseScreen();
		}
	}
}

void CrewManager::addCrewMember()
{
	ui->clearScreen();
	ui->printHeader("Add Crew Member");
	
	try
	{
		string crewId = ui->getString("Enter Crew ID (e.g., CRW001): ");
		
		if (!Crew::isValidCrewId(crewId))
		{
			ui->printError("Invalid crew ID. Must be CRW followed by 3-7 digits.");
			ui->pauseScreen();
			return;
		}
		
		if (db->entryExists(crewId))
		{
			ui->printError("Crew member " + crewId + " already exists.");
			ui->pauseScreen();
			return;
		}
		
		string name = ui->getString("Enter Name: ");
		
		if (!Crew::isValidName(name))
		{
			ui->printError("Invalid name. Must not be empty, less than 50 characters.");
			ui->pauseScreen();
			return;
		}
		
		CrewRole role = getValidRole();
		CrewStatus status = getValidStatus();
		vector<string> certifications = getValidCertifications();
		
		auto newCrew = std::make_shared<Crew>(crewId, name, role, status, 0.0, certifications);
		saveCrewToDatabase(newCrew);
		
		ui->printSuccess("Crew member " + crewId + " has been successfully added.");
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void CrewManager::viewAllCrew()
{
	ui->clearScreen();
	ui->printHeader("View All Crew Members");
	
	try
	{
		json allCrewData = db->loadAll();
		
		if (allCrewData.empty())
		{
			ui->printWarning("No crew members found in the system.");
			ui->pauseScreen();
			return;
		}
		
		vector<shared_ptr<Crew>> crew;
		
		for (const auto& [crewId, crewData] : allCrewData.items())
		{
			try
			{
				string name = crewData.value("name", "");
				string roleStr = crewData.value("role", "Pilot");
				string statusStr = crewData.value("status", "Available");
				double hours = crewData.value("totalFlightHours", 0.0);
				
				vector<string> certifications;
				if (crewData.contains("certifications") && crewData["certifications"].is_array())
				{
					for (const auto& cert : crewData["certifications"])
					{
						certifications.push_back(cert.get<string>());
					}
				}
				
				CrewRole role = Crew::stringToRole(roleStr);
				CrewStatus status = Crew::stringToStatus(statusStr);
				
				crew.push_back(std::make_shared<Crew>(crewId, name, role, status, hours, certifications));
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
		
		displayCrewTable(crew, "All Crew Members");
		ui->println("\nTotal Crew Members: " + std::to_string(crew.size()));
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void CrewManager::viewCrewDetails()
{
	ui->clearScreen();
	ui->printHeader("View Crew Details");
	
	try
	{
		string crewId = ui->getString("Enter Crew ID: ");
		
		shared_ptr<Crew> crew = loadCrewFromDatabase(crewId);
		if (!crew)
		{
			ui->printError("Crew member not found.");
			ui->pauseScreen();
			return;
		}
		
		ui->println("\n--- Crew Member Information ---");
		ui->println("Crew ID: " + crew->getCrewId());
		ui->println("Name: " + crew->getName());
		ui->println("Role: " + Crew::roleToString(crew->getRole()));
		ui->println("Status: " + Crew::statusToString(crew->getStatus()));
		ui->println("Total Flight Hours: " + std::to_string(static_cast<int>(crew->getTotalFlightHours())));
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void CrewManager::updateCrewMember()
{
	ui->clearScreen();
	ui->printHeader("Update Crew Member");
	
	try
	{
		string crewId = ui->getString("Enter Crew ID to Update: ");
		
		shared_ptr<Crew> crew = loadCrewFromDatabase(crewId);
		if (!crew)
		{
			ui->printError("Crew member not found.");
			ui->pauseScreen();
			return;
		}
		
		vector<string> options = {
			"Name",
			"Role",
			"Status",
			"Back to Manage Crew"
		};
		
		ui->displayMenu("Update Crew Member", options);
		
		int choice = ui->getChoice("Enter choice: ", 1, 4);
		
		switch (choice)
		{
			case 1:
			{
				string newName = ui->getString("Enter new Name: ");
				if (Crew::isValidName(newName))
				{
					crew->setName(newName);
					saveCrewToDatabase(crew);
					ui->printSuccess("Crew name updated successfully.");
				}
				else
				{
					ui->printError("Invalid name.");
				}
				break;
			}
			case 2:
			{
				CrewRole newRole = getValidRole();
				crew->setRole(newRole);
				saveCrewToDatabase(crew);
				ui->printSuccess("Crew role updated successfully.");
				break;
			}
			case 3:
			{
				CrewStatus newStatus = getValidStatus();
				crew->setStatus(newStatus);
				saveCrewToDatabase(crew);
				ui->printSuccess("Crew status updated successfully.");
				break;
			}
			case 4:
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

void CrewManager::removeCrewMember()
{
	ui->clearScreen();
	ui->printHeader("Remove Crew Member");
	
	try
	{
		string crewId = ui->getString("Enter Crew ID to Remove: ");
		
		if (!db->entryExists(crewId))
		{
			ui->printError("Crew member not found.");
			ui->pauseScreen();
			return;
		}
		
		if (isCrewAssignedToFlights(crewId))
		{
			ui->printError("Cannot delete crew member assigned to active flights.");
			ui->println("Please complete or reassign the flights before deleting.");
			ui->pauseScreen();
			return;
		}
		
		bool confirm = ui->getYesNo("Are you sure you want to remove crew '" + crewId + "'?");
		if (confirm)
		{
			deleteCrewFromDatabase(crewId);
			ui->printSuccess("Crew member '" + crewId + "' has been removed successfully.");
		}
		else
		{
			ui->printWarning("Crew member removal canceled.");
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Helper Methods ====================

shared_ptr<Crew> CrewManager::loadCrewFromDatabase(const string& crewId)
{
	if (!db->entryExists(crewId))
	{
		return nullptr;
	}
	
	try
	{
		json crewData = db->getEntry(crewId);
		
		string name = crewData.value("name", "");
		string roleStr = crewData.value("role", "Pilot");
		string statusStr = crewData.value("status", "Available");
		double hours = crewData.value("totalFlightHours", 0.0);
		
		vector<string> certifications;
		if (crewData.contains("certifications") && crewData["certifications"].is_array())
		{
			for (const auto& cert : crewData["certifications"])
			{
				certifications.push_back(cert.get<string>());
			}
		}
		
		CrewRole role = Crew::stringToRole(roleStr);
		CrewStatus status = Crew::stringToStatus(statusStr);
		
		return std::make_shared<Crew>(crewId, name, role, status, hours, certifications);
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
		return nullptr;
	}
}

void CrewManager::saveCrewToDatabase(const shared_ptr<Crew>& crew)
{
	if (!crew)
	{
		throw CrewException(CrewErrorCode::DATABASE_ERROR, "Cannot save null crew.");
	}
	
	try
	{
		json crewData;
		crewData["name"] = crew->getName();
		crewData["role"] = Crew::roleToString(crew->getRole());
		crewData["status"] = Crew::statusToString(crew->getStatus());
		crewData["totalFlightHours"] = crew->getTotalFlightHours();
		
		if (db->entryExists(crew->getCrewId()))
		{
			db->updateEntry(crew->getCrewId(), crewData);
		}
		else
		{
			db->addEntry(crew->getCrewId(), crewData);
		}
	}
	catch (const DatabaseException& e)
	{
		throw CrewException(CrewErrorCode::DATABASE_ERROR, e.what());
	}
}

void CrewManager::deleteCrewFromDatabase(const string& crewId)
{
	try
	{
		db->deleteEntry(crewId);
	}
	catch (const DatabaseException& e)
	{
		throw CrewException(CrewErrorCode::DATABASE_ERROR, e.what());
	}
}

void CrewManager::displayCrewTable(const vector<shared_ptr<Crew>>& crew, const string& title)
{
	if (crew.empty())
	{
		ui->printWarning("No crew to display.");
		return;
	}
	
	vector<string> headers = {
		"Crew ID", "Name", "Role", "Status", "Flight Hours"
	};
	
	vector<vector<string>> rows;
	
	for (const auto& member : crew)
	{
		if (member)
		{
			try
			{				
				rows.push_back({
					member->getCrewId(),
					member->getName(),
					Crew::roleToString(member->getRole()),
					Crew::statusToString(member->getStatus()),
					std::to_string(static_cast<int>(member->getTotalFlightHours()))
				});
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
	}
	
	if (rows.empty())
	{
		ui->printWarning("No valid crew to display.");
		return;
	}
	
	if (!title.empty())
	{
		ui->println("\n" + title);
		ui->printSeparator();
	}
	
	ui->displayTable(headers, rows);
}

CrewRole CrewManager::getValidRole()
{
	vector<string> roleOptions = {
		"Pilot",
		"Copilot",
		"Flight Attendant"
	};
	
	ui->displayMenu("Select Crew Role", roleOptions);
	
	try
	{
		int choice = ui->getChoice("Enter role: ", 1, static_cast<int>(roleOptions.size()));
		return Crew::stringToRole(roleOptions[choice - 1]);
	}
	catch (const std::exception& e)
	{
		ui->printError(e.what());
		return CrewRole::PILOT;
	}
}

CrewStatus CrewManager::getValidStatus()
{
	vector<string> statusOptions = {
		"Available",
		"Assigned",
		"On Leave"
	};
	
	ui->displayMenu("Select Crew Status", statusOptions);
	
	try
	{
		int choice = ui->getChoice("Enter status: ", 1, static_cast<int>(statusOptions.size()));
		return Crew::stringToStatus(statusOptions[choice - 1]);
	}
	catch (const std::exception& e)
	{
		ui->printError(e.what());
		return CrewStatus::AVAILABLE;
	}
}

vector<string> CrewManager::getValidCertifications()
{
	vector<string> certifications;
	
	ui->println("\nAdd Aircraft Certifications (enter empty line to finish):");
	
	while (true)
	{
		try
		{
			string input = ui->getString("Enter certification (e.g., Boeing-737): ");
			
			if (input.empty())
				break;
			
			if (std::find(certifications.begin(), certifications.end(), input) == certifications.end())
			{
				certifications.push_back(input);
				ui->printSuccess("Certification '" + input + "' added.");
			}
			else
			{
				ui->printWarning("This certification is already added.");
			}
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
		}
	}
	
	return certifications;
}

// ==================== Query Methods ====================

shared_ptr<Crew> CrewManager::getCrew(const string& crewId)
{
	return loadCrewFromDatabase(crewId);
}

vector<shared_ptr<Crew>> CrewManager::getAvailableCrew(CrewRole role)
{
	vector<shared_ptr<Crew>> available;
	
	try
	{
		json allCrewData = db->loadAll();
		
		for (const auto& [crewId, crewData] : allCrewData.items())
		{
			try
			{
				string name = crewData.value("name", "");
				string roleStr = crewData.value("role", "Pilot");
				string statusStr = crewData.value("status", "Available");
				double hours = crewData.value("totalFlightHours", 0.0);
				
				vector<string> certifications;
				if (crewData.contains("certifications") && crewData["certifications"].is_array())
				{
					for (const auto& cert : crewData["certifications"])
					{
						certifications.push_back(cert.get<string>());
					}
				}
				
				CrewRole crewRole = Crew::stringToRole(roleStr);
				CrewStatus crewStatus = Crew::stringToStatus(statusStr);
				auto crew = std::make_shared<Crew>(crewId, name, crewRole, crewStatus, hours, certifications);
				
				if (crew->getRole() == role &&
					crew->getStatus() == CrewStatus::AVAILABLE)
				{
					available.push_back(crew);
				}
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	return available;
}

vector<string> CrewManager::getAllCrewIds()
{
	vector<string> crewIds;
	
	try
	{
		json allCrewData = db->loadAll();
		for (const auto& [crewId, crewData] : allCrewData.items())
		{
			crewIds.push_back(crewId);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	return crewIds;
}

vector<shared_ptr<Crew>> CrewManager::getAllCrew()
{
	vector<shared_ptr<Crew>> allCrew;
	auto crewIds = getAllCrewIds();
	for (const auto& crew : crewIds)
	{
		allCrew.push_back(getCrew(crew));
	}
	return allCrew;
}

bool CrewManager::crewExists(const string& crewId)
{
	try
	{
		return db->entryExists(crewId);
	}
	catch (const std::exception& e)
	{
		return false;
	}
}

// ==================== Status Update Methods ====================

void CrewManager::markCrewAsAssigned(const string& crewId)
{
	try
	{
		shared_ptr<Crew> crew = loadCrewFromDatabase(crewId);
		if (crew)
		{
			crew->setStatus(CrewStatus::ASSIGNED);
			saveCrewToDatabase(crew);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
}

void CrewManager::markCrewAsAvailable(const string& crewId)
{
	try
	{
		shared_ptr<Crew> crew = loadCrewFromDatabase(crewId);
		if (crew)
		{
			crew->setStatus(CrewStatus::AVAILABLE);
			saveCrewToDatabase(crew);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
}

void CrewManager::addFlightHoursForCrew(const string& crewId, double hours)
{
	try
	{
		shared_ptr<Crew> crew = loadCrewFromDatabase(crewId);
		if (crew)
		{
			crew->addFlightHours(hours);
			saveCrewToDatabase(crew);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
}

// ==================== Cascade Checks ====================

bool CrewManager::isCrewAssignedToFlights(const string& crewId)
{
	try
	{
		vector<string> flightNumbers = FlightManager::getInstance()->getAllFlightNumbers();
		
		for (const string& flightNumber : flightNumbers)
		{
			shared_ptr<Flight> flight = FlightManager::getInstance()->getFlight(flightNumber);
			if (flight && flight->hasCrewMember(crewId) && flight->getStatus() != "Completed")
			{
				return true;
			}
		}
	}
	catch (const std::exception& e)
	{
		return false;
	}
	
	return false;
}