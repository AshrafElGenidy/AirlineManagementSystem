#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <filesystem>
#include "ReportGenerator.hpp"
#include "UsersManager.hpp"
#include "FlightManager.hpp"
#include "ReservationManager.hpp"
#include "CrewManager.hpp"
#include "AircraftManager.hpp"
#include "Maintenance.hpp"

namespace fs = std::filesystem;

// ==================== Constructor ====================

ReportGenerator::ReportGenerator()
{
	ui = UserInterface::getInstance();
}

// ==================== Helper Methods ====================

void ReportGenerator::addReportHeader(std::ostringstream& oss, const string& title) const
{
	oss << title << "\n";
	oss << "Generated: " << getCurrentDateTime() << "\n";
	oss << string(50, '=') << "\n\n";
}

void ReportGenerator::displayAndLogSummary(const vector<std::pair<string, string>>& stats, 
                                           std::ostringstream& reportContent)
{
	ui->println("\nSummary Statistics:");
	ui->printSeparator();
	
	for (const auto& [label, value] : stats)
	{
		ui->println(label + ": " + value);
		reportContent << label << ": " << value << "\n";
	}
	reportContent << "\n";
}

void ReportGenerator::displayAndLogSection(const string& title, const vector<string>& headers, 
                                           const vector<vector<string>>& rows, 
                                           std::ostringstream& reportContent)
{
	ui->println("\n" + title + ":");
	ui->printSeparator();
	
	reportContent << title << ":\n";
	
	if (rows.empty())
	{
		ui->println("No data available.");
		reportContent << "No data available.\n";
	}
	else
	{
		ui->displayTable(headers, rows);
		
		for (const auto& row : rows)
		{
			for (size_t i = 0; i < row.size(); ++i)
			{
				reportContent << row[i];
				if (i < row.size() - 1) reportContent << " | ";
			}
			reportContent << "\n";
		}
	}
}

string ReportGenerator::getCurrentDateTime() const
{
	std::time_t now = std::time(nullptr);
	std::tm* now_tm = std::localtime(&now);
	
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);
	
	return string(buffer);
}

void ReportGenerator::exportToFile(const string& reportName, const string& content)
{
	try
	{
		fs::create_directories("Reports");
		
		string filename = "Reports/" + reportName + "_" + getCurrentDateTime() + ".txt";
		
		std::replace(filename.begin(), filename.end(), ' ', '_');
		std::replace(filename.begin(), filename.end(), ':', '-');
		
		std::ofstream outFile(filename);
		
		if (!outFile.is_open())
		{
			ui->printError("Failed to create report file.");
			return;
		}
		
		outFile << content;
		outFile.close();
		
		ui->printSuccess("Report exported successfully to: " + filename);
	}
	catch (const std::exception& e)
	{
		ui->printError("Error exporting report: " + string(e.what()));
	}
}

// ==================== Main Menu ====================

void ReportGenerator::generateReports()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Flight Performance Report",
			"Revenue Report",
			"Reservation Statistics Report",
			"Crew Summary Report",
			"Maintenance Summary Report",
			"User Activity Report",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Generate Reports", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 7);
			
			switch (choice)
			{
				case 1:
					generateFlightPerformanceReport();
					break;
				case 2:
					generateRevenueReport();
					break;
				case 3:
					generateReservationStatisticsReport();
					break;
				case 4:
					generateCrewSummaryReport();
					break;
				case 5:
					generateMaintenanceSummaryReport();
					break;
				case 6:
					generateUsersReport();
					break;
				case 7:
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
}

// ==================== Report 1: Flight Performance Report ====================

void ReportGenerator::generateFlightPerformanceReport()
{
	ui->clearScreen();
	ui->printHeader("Flight Performance Report");
	
	try
	{
		vector<string> flightNumbers = FlightManager::getInstance()->getAllFlightNumbers();
		
		if (flightNumbers.empty())
		{
			ui->printWarning("No flights found in the system.");
			ui->pauseScreen();
			return;
		}
		
		int totalFlights = flightNumbers.size();
		std::map<string, int> statusCounts;
		double totalOccupancy = 0.0;
		int validOccupancyCount = 0;
		
		for (const auto& flightNumber : flightNumbers)
		{
			shared_ptr<Flight> flight = FlightManager::getInstance()->getFlight(flightNumber);
			statusCounts[flight->getStatus()]++;
			
			try
			{
				int totalSeats = flight->getTotalSeats();
				int reservedSeats = flight->getReservedSeats().size();
				if (totalSeats > 0)
				{
					double occupancy = (static_cast<double>(reservedSeats) / totalSeats) * 100.0;
					totalOccupancy += occupancy;
					validOccupancyCount++;
				}
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
		
		double averageOccupancy = validOccupancyCount > 0 ? (totalOccupancy / validOccupancyCount) : 0.0;
		
		std::ostringstream reportContent;
		addReportHeader(reportContent, "Flight Performance Report");
		
		std::ostringstream avgOccStream;
		avgOccStream << std::fixed << std::setprecision(2) << averageOccupancy << "%";
		
		vector<std::pair<string, string>> summaryStats = {
			{"Total Flights", std::to_string(totalFlights)},
			{"Average Occupancy Rate", avgOccStream.str()}
		};
		
		displayAndLogSummary(summaryStats, reportContent);
		
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
		}
		
		displayAndLogSection("Flights by Status", statusHeaders, statusRows, reportContent);
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("FlightPerformance", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error generating report: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Report 2: Revenue Report ====================

void ReportGenerator::generateRevenueReport()
{
	ui->clearScreen();
	ui->printHeader("Revenue Report");
	
	try
	{
		vector<shared_ptr<Reservation>> reservations = ReservationManager::getInstance()->getAllReservations();
		
		if (reservations.empty())
		{
			ui->printWarning("No reservations found in the system.");
			ui->pauseScreen();
			return;
		}
		
		double totalRevenue = 0.0;
		int confirmedCount = 0;
		
		for (const auto& reservation : reservations)
		{
			string status = reservation->getStringStatus();
			if (status == "CONFIRMED" || status == "COMPLETED")
			{
				totalRevenue += reservation->getTotalCost();
				confirmedCount++;
			}
		}
		
		double averageTicket = confirmedCount > 0 ? (totalRevenue / confirmedCount) : 0.0;
		
		std::ostringstream reportContent;
		addReportHeader(reportContent, "Revenue Report");
		
		vector<std::pair<string, string>> summaryStats = {
			{"Total Revenue", ui->formatCurrency(totalRevenue)},
			{"Number of Confirmed Reservations", std::to_string(confirmedCount)},
			{"Average Ticket Price", ui->formatCurrency(averageTicket)}
		};
		
		displayAndLogSummary(summaryStats, reportContent);
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("Revenue", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error generating report: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Report 3: Reservation Statistics Report ====================

void ReportGenerator::generateReservationStatisticsReport()
{
	ui->clearScreen();
	ui->printHeader("Reservation Statistics Report");
	
	try
	{
		auto reservations = ReservationManager::getInstance()->getAllReservations();
		auto flights = FlightManager::getInstance()->getAllFlights();
		
		if (reservations.empty())
		{
			ui->printWarning("No reservations found in the system.");
			ui->pauseScreen();
			return;
		}
		
		// Reservation statistics
		int totalReservations = reservations.size();
		std::map<string, int> statusCounts;
		std::map<string, int> passengerBookingCounts;
		std::map<string, int> agentBookingCounts;
		int canceledCount = 0;
		
		for (const auto& reservation : reservations)
		{
			string status = reservation->getStringStatus();
			statusCounts[status]++;
			
			if (status == "CANCELED")
			{
				canceledCount++;
			}
			
			passengerBookingCounts[reservation->getPassengerUsername()]++;
			agentBookingCounts[reservation->getBookedByAgent()]++;
		}
		
		double cancelRate = totalReservations > 0 ? 
		                   (static_cast<double>(canceledCount) / totalReservations * 100.0) : 0.0;
		
		// Occupancy statistics
		vector<std::tuple<string, string, double, int, int>> flightOccupancy;
		double totalOccupancy = 0.0;
		int validCount = 0;
		
		for (const auto& flight : flights)
		{
			try
			{
				int totalSeats = flight->getTotalSeats();
				int reservedSeats = flight->getReservedSeats().size();
				
				if (totalSeats > 0)
				{
					double occupancy = (static_cast<double>(reservedSeats) / totalSeats) * 100.0;
					flightOccupancy.push_back({
						flight->getFlightNumber(),
						flight->getOrigin() + " -> " + flight->getDestination(),
						occupancy,
						reservedSeats,
						totalSeats
					});
					
					totalOccupancy += occupancy;
					validCount++;
				}
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
		
		double averageOccupancy = validCount > 0 ? (totalOccupancy / validCount) : 0.0;
		
		std::sort(flightOccupancy.begin(), flightOccupancy.end(),
		          [](const auto& a, const auto& b) { return std::get<2>(a) > std::get<2>(b); });
		
		vector<std::pair<string, int>> sortedPassengers(passengerBookingCounts.begin(), 
		                                                passengerBookingCounts.end());
		std::sort(sortedPassengers.begin(), sortedPassengers.end(),
		          [](const auto& a, const auto& b) { return a.second > b.second; });
		
		vector<std::pair<string, int>> sortedAgents(agentBookingCounts.begin(), 
		                                            agentBookingCounts.end());
		std::sort(sortedAgents.begin(), sortedAgents.end(),
		          [](const auto& a, const auto& b) { return a.second > b.second; });
		
		std::ostringstream reportContent;
		addReportHeader(reportContent, "Reservation Statistics Report");
		
		// Summary
		std::ostringstream cancelStream, avgOccStream;
		cancelStream << std::fixed << std::setprecision(2) << cancelRate << "%";
		avgOccStream << std::fixed << std::setprecision(2) << averageOccupancy << "%";
		
		vector<std::pair<string, string>> summaryStats = {
			{"Total Reservations", std::to_string(totalReservations)},
			{"Cancellation Rate", cancelStream.str()},
			{"Average Occupancy", avgOccStream.str()}
		};
		
		displayAndLogSummary(summaryStats, reportContent);
		
		// Reservations by Status
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
		}
		
		displayAndLogSection("Reservations by Status", statusHeaders, statusRows, reportContent);
		
		// All Flights Occupancy
		vector<string> occupancyHeaders = {"Flight", "Route", "Reserved", "Total", "Occupancy"};
		vector<vector<string>> occupancyRows;
		
		for (const auto& [flightNum, route, occupancy, reserved, total] : flightOccupancy)
		{
			std::ostringstream occStream;
			occStream << std::fixed << std::setprecision(2) << occupancy << "%";
			
			occupancyRows.push_back({
				flightNum,
				route,
				std::to_string(reserved),
				std::to_string(total),
				occStream.str()
			});
		}
		
		displayAndLogSection("Flight Occupancy", occupancyHeaders, occupancyRows, reportContent);
		
		// Top 10 Passengers
		vector<string> passengerHeaders = {"Passenger Username", "Booking Count"};
		vector<vector<string>> passengerRows;
		
		int passengerCount = 0;
		for (const auto& [passenger, count] : sortedPassengers)
		{
			if (passengerCount++ >= 10) break;
			passengerRows.push_back({passenger, std::to_string(count)});
		}
		
		displayAndLogSection("Top 10 Passengers by Booking Count", passengerHeaders, passengerRows, reportContent);
		
		// Top 10 Booking Agents
		vector<string> agentHeaders = {"Booking Agent", "Booking Count"};
		vector<vector<string>> agentRows;
		
		int agentCount = 0;
		for (const auto& [agent, count] : sortedAgents)
		{
			if (agentCount++ >= 10) break;
			agentRows.push_back({agent, std::to_string(count)});
		}
		
		displayAndLogSection("Top 10 Booking Agents by Booking Count", agentHeaders, agentRows, reportContent);
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("ReservationStatistics", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error generating report: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Report 4: Crew Summary Report ====================

void ReportGenerator::generateCrewSummaryReport()
{
	ui->clearScreen();
	ui->printHeader("Crew Summary Report");
	
	try
	{
		auto crew = CrewManager::getInstance()->getAllCrew();
		
		if (crew.empty())
		{
			ui->printWarning("No crew members found in the system.");
			ui->pauseScreen();
			return;
		}
		
		int totalCrew = crew.size();
		std::map<string, int> roleCounts;
		std::map<string, int> statusCounts;
		
		for (const auto& member : crew)
		{
			roleCounts[member->getRoleString()]++;
			statusCounts[member->getStatusString()]++;
		}
		
		vector<shared_ptr<Crew>> sortedCrew = crew;
		std::sort(sortedCrew.begin(), sortedCrew.end(),
		          [](const auto& a, const auto& b) { 
		              return a->getTotalFlightHours() > b->getTotalFlightHours(); 
		          });
		
		std::ostringstream reportContent;
		addReportHeader(reportContent, "Crew Summary Report");
		
		vector<std::pair<string, string>> summaryStats = {
			{"Total Crew Members", std::to_string(totalCrew)}
		};
		
		displayAndLogSummary(summaryStats, reportContent);
		
		// Crew by Role
		vector<string> roleHeaders = {"Role", "Count"};
		vector<vector<string>> roleRows;
		
		for (const auto& [role, count] : roleCounts)
		{
			roleRows.push_back({role, std::to_string(count)});
		}
		
		displayAndLogSection("Crew by Role", roleHeaders, roleRows, reportContent);
		
		// Crew by Status
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
		}
		
		displayAndLogSection("Crew by Status", statusHeaders, statusRows, reportContent);
		
		// All Crew with Flight Hours
		vector<string> crewHeaders = {"Crew ID", "Name", "Role", "Flight Hours"};
		vector<vector<string>> crewRows;
		
		for (const auto& member : sortedCrew)
		{
			std::ostringstream hoursStream;
			hoursStream << std::fixed << std::setprecision(2) << member->getTotalFlightHours();
			
			crewRows.push_back({
				member->getCrewId(),
				member->getName(),
				member->getRoleString(),
				hoursStream.str()
			});
		}
		
		displayAndLogSection("All Crew Members by Flight Hours", crewHeaders, crewRows, reportContent);
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("CrewSummary", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Report 5: Maintenance Summary Report ====================

void ReportGenerator::generateMaintenanceSummaryReport()
{
	ui->clearScreen();
	ui->printHeader("Maintenance Summary Report");
	
	try
	{
		auto maintenanceRecords = Maintenance::getInstance()->getAllMaintenance();
		
		if (maintenanceRecords.empty())
		{
			ui->printWarning("No maintenance records found in the system.");
			ui->pauseScreen();
			return;
		}
		
		int totalRecords = maintenanceRecords.size();
		std::map<string, int> statusCounts;
		double totalCost = 0.0;
		vector<json> upcomingMaintenance;
		vector<json> overdueMaintenance;
		
		std::time_t now = std::time(nullptr);
		std::tm* now_tm = std::localtime(&now);
		char currentDateStr[11];
		std::strftime(currentDateStr, sizeof(currentDateStr), "%Y-%m-%d", now_tm);
		string currentDate = currentDateStr;
		
		std::tm future_tm = *now_tm;
		future_tm.tm_mday += 30;
		std::mktime(&future_tm);
		char futureDateStr[11];
		std::strftime(futureDateStr, sizeof(futureDateStr), "%Y-%m-%d", &future_tm);
		string futureDate = futureDateStr;
		
		for (const auto& record : maintenanceRecords)
		{
			try
			{
				string status = record["status"].get<string>();
				string scheduledDate = record["scheduledDate"].get<string>();
				
				statusCounts[status]++;
				
				if (status == "COMPLETED" && record.contains("cost"))
				{
					totalCost += record["cost"].get<double>();
				}
				
				if (status == "SCHEDULED" && scheduledDate >= currentDate && scheduledDate <= futureDate)
				{
					upcomingMaintenance.push_back(record);
				}
				
				if (status != "COMPLETED" && status != "CANCELED" && scheduledDate < currentDate)
				{
					overdueMaintenance.push_back(record);
				}
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
		
		std::ostringstream reportContent;
		addReportHeader(reportContent, "Maintenance Summary Report");
		
		vector<std::pair<string, string>> summaryStats = {
			{"Total Maintenance Records", std::to_string(totalRecords)},
			{"Total Cost (Completed)", ui->formatCurrency(totalCost)},
			{"Upcoming Maintenance (Next 30 Days)", std::to_string(upcomingMaintenance.size())},
			{"Overdue Maintenance", std::to_string(overdueMaintenance.size())}
		};
		
		displayAndLogSummary(summaryStats, reportContent);
		
		// Maintenance by Status
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
		}
		
		displayAndLogSection("Maintenance by Status", statusHeaders, statusRows, reportContent);
		
		// Upcoming Maintenance
		vector<string> upcomingHeaders = {"ID", "Aircraft", "Type", "Scheduled Date"};
		vector<vector<string>> upcomingRows;
		
		for (const auto& record : upcomingMaintenance)
		{
			upcomingRows.push_back({
				record["maintenanceId"].get<string>(),
				record["aircraftType"].get<string>(),
				record["type"].get<string>(),
				record["scheduledDate"].get<string>()
			});
		}
		
		displayAndLogSection("Upcoming Maintenance (Next 30 Days)", upcomingHeaders, upcomingRows, reportContent);
		
		// Overdue Maintenance
		if (!overdueMaintenance.empty())
		{
			ui->printWarning("\nOVERDUE MAINTENANCE DETECTED!");
			ui->printSeparator();
			
			reportContent << "\n!!! OVERDUE MAINTENANCE !!!\n";
			
			vector<string> overdueHeaders = {"ID", "Aircraft", "Type", "Scheduled Date", "Status"};
			vector<vector<string>> overdueRows;
			
			for (const auto& record : overdueMaintenance)
			{
				overdueRows.push_back({
					record["maintenanceId"].get<string>(),
					record["aircraftType"].get<string>(),
					record["type"].get<string>(),
					record["scheduledDate"].get<string>(),
					record["status"].get<string>()
				});
			}
			
			ui->displayTable(overdueHeaders, overdueRows);
			
			for (const auto& row : overdueRows)
			{
				for (size_t i = 0; i < row.size(); ++i)
				{
					reportContent << row[i];
					if (i < row.size() - 1) reportContent << " | ";
				}
				reportContent << "\n";
			}
		}
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("MaintenanceSummary", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error generating report: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Report 6: Users Report ====================

void ReportGenerator::generateUsersReport()
{
	ui->clearScreen();
	ui->printHeader("User Activity Report");
	
	try
	{
		auto allUsers = UsersManager::getInstance()->getAllUsers();
		
		int totalUsers = allUsers.size();
		std::map<string, int> roleCounts;
		
		for (const auto& user : allUsers)
		{
			try
			{
				roleCounts[user->getRoleString()]++;
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
		
		std::ostringstream reportContent;
		addReportHeader(reportContent, "User Activity Report");
		
		vector<std::pair<string, string>> summaryStats = {
			{"Total Users", std::to_string(totalUsers)}
		};
		
		displayAndLogSummary(summaryStats, reportContent);
		
		vector<string> roleHeaders = {"Role", "Count"};
		vector<vector<string>> roleRows;
		
		for (const auto& [role, count] : roleCounts)
		{
			roleRows.push_back({role, std::to_string(count)});
		}
		
		displayAndLogSection("Users by Role", roleHeaders, roleRows, reportContent);
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("UserActivity", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error generating report: " + string(e.what()));
	}
	
	ui->pauseScreen();
}