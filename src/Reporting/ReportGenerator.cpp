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

// ==================== Main Menu ====================

void ReportGenerator::generateReports()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Flight Performance Report",
			"Revenue Report",
			"Occupancy Report",
			"Reservation Statistics Report",
			"Crew Summary Report",
			"Maintenance Summary Report",
			"User Activity Report",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Generate Reports", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 8);
			
			switch (choice)
			{
				case 1:
					generateFlightPerformanceReport();
					break;
				case 2:
					generateRevenueReport();
					break;
				case 3:
					generateOccupancyReport();
					break;
				case 4:
					generateReservationStatisticsReport();
					break;
				case 5:
					generateCrewSummaryReport();
					break;
				case 6:
					generateMaintenanceSummaryReport();
					break;
				case 7:
					generateUsersReport();
					break;
				case 8:
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
		std::map<string, int> routeCounts;
		double totalOccupancy = 0.0;
		int validOccupancyCount = 0;
		
		for (const auto& flightNumber : flightNumbers)
		{
			shared_ptr<Flight> flight = FlightManager::getInstance()->getFlight(flightNumber);

			statusCounts[flight->getStatus()]++;
			string route = flight->getOrigin() + " -> " + flight->getDestination();
			routeCounts[route]++;
			
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
		reportContent << "Flight Performance Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		ui->println("Total Flights: " + std::to_string(totalFlights));
		
		std::ostringstream avgOccStream;
		avgOccStream << std::fixed << std::setprecision(2) << averageOccupancy << "%";
		ui->println("Average Occupancy Rate: " + avgOccStream.str());
		
		reportContent << "Total Flights: " << totalFlights << "\n";
		reportContent << "Average Occupancy Rate: " << avgOccStream.str() << "\n\n";
		
		ui->println("\nFlights by Status:");
		ui->printSeparator();
		
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		reportContent << "Flights by Status:\n";
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
			reportContent << status << ": " << count << "\n";
		}
		
		ui->displayTable(statusHeaders, statusRows);
		
		ui->println("\nTop 10 Routes:");
		ui->printSeparator();
		
		vector<std::pair<string, int>> sortedRoutes(routeCounts.begin(), routeCounts.end());
		std::sort(sortedRoutes.begin(), sortedRoutes.end(),
		          [](const auto& a, const auto& b) { return a.second > b.second; });
		
		vector<string> routeHeaders = {"Route", "Flight Count"};
		vector<vector<string>> routeRows;
		
		reportContent << "\nTop 10 Routes:\n";
		int routeCount = 0;
		for (const auto& [route, count] : sortedRoutes)
		{
			if (routeCount++ >= 10) break;
			routeRows.push_back({route, std::to_string(count)});
			reportContent << route << ": " << count << "\n";
		}
		
		ui->displayTable(routeHeaders, routeRows);
		
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
		std::map<string, double> revenueByFlight;
		
		for (const auto& reservation : reservations)
		{
			string status = reservation->getStringStatus();
			if (status == "CONFIRMED" || status == "COMPLETED")
			{
				double cost = reservation->getTotalCost();
				totalRevenue += cost;
				confirmedCount++;
				
				string flightNumber = reservation->getFlightNumber();
				revenueByFlight[flightNumber] += cost;
			}
		}
		
		double averageTicket = confirmedCount > 0 ? (totalRevenue / confirmedCount) : 0.0;
		
		vector<std::pair<string, double>> sortedFlights(revenueByFlight.begin(), revenueByFlight.end());
		std::sort(sortedFlights.begin(), sortedFlights.end(),
		          [](const auto& a, const auto& b) { return a.second > b.second; });
		
		std::ostringstream reportContent;
		reportContent << "Revenue Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		ui->println("Total Revenue: " + ui->formatCurrency(totalRevenue));
		ui->println("Number of Confirmed Reservations: " + std::to_string(confirmedCount));
		ui->println("Average Ticket Price: " + ui->formatCurrency(averageTicket));
		
		reportContent << "Total Revenue: " << ui->formatCurrency(totalRevenue) << "\n";
		reportContent << "Number of Confirmed Reservations: " << confirmedCount << "\n";
		reportContent << "Average Ticket Price: " << ui->formatCurrency(averageTicket) << "\n\n";
		
		ui->println("\nTop 10 Revenue Flights:");
		ui->printSeparator();
		
		vector<string> headers = {"Flight Number", "Revenue"};
		vector<vector<string>> rows;
		
		reportContent << "Top 10 Revenue Flights:\n";
		int flightCount = 0;
		for (const auto& [flightNumber, revenue] : sortedFlights)
		{
			if (flightCount++ >= 10) break;
			rows.push_back({flightNumber, ui->formatCurrency(revenue)});
			reportContent << flightNumber << ": " << ui->formatCurrency(revenue) << "\n";
		}
		
		ui->displayTable(headers, rows);
		
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

// ==================== Report 3: Occupancy Report ====================

void ReportGenerator::generateOccupancyReport()
{
	ui->clearScreen();
	ui->printHeader("Occupancy Report");
	
	try
	{
		auto flights = FlightManager::getInstance()->getAllFlights();
		
		if (flights.empty())
		{
			ui->printWarning("No flights found in the system.");
			ui->pauseScreen();
			return;
		}
		
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
		
		std::ostringstream reportContent;
		reportContent << "Occupancy Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		
		std::ostringstream avgStream;
		avgStream << std::fixed << std::setprecision(2) << averageOccupancy << "%";
		ui->println("Average Occupancy: " + avgStream.str());
		
		reportContent << "Average Occupancy: " << avgStream.str() << "\n\n";
		
		ui->println("\nAll Flights Occupancy:");
		ui->printSeparator();
		
		vector<string> headers = {"Flight", "Route", "Reserved", "Total", "Occupancy"};
		vector<vector<string>> rows;
		
		reportContent << "All Flights Occupancy:\n";
		for (const auto& [flightNum, route, occupancy, reserved, total] : flightOccupancy)
		{
			std::ostringstream occStream;
			occStream << std::fixed << std::setprecision(2) << occupancy << "%";
			
			rows.push_back({
				flightNum,
				route,
				std::to_string(reserved),
				std::to_string(total),
				occStream.str()
			});
			
			reportContent << flightNum << " (" << route << "): "
			             << reserved << "/" << total << " = " << occStream.str() << "\n";
		}
		
		ui->displayTable(headers, rows);
		
		ui->println("\nHigh Occupancy Flights (>80%):");
		ui->printSeparator();
		
		vector<vector<string>> highRows;
		reportContent << "\nHigh Occupancy Flights (>80%):\n";
		
		for (const auto& [flightNum, route, occupancy, reserved, total] : flightOccupancy)
		{
			if (occupancy > 80.0)
			{
				std::ostringstream occStream;
				occStream << std::fixed << std::setprecision(2) << occupancy << "%";
				
				highRows.push_back({
					flightNum,
					route,
					std::to_string(reserved),
					std::to_string(total),
					occStream.str()
				});
				
				reportContent << flightNum << " (" << route << "): " << occStream.str() << "\n";
			}
		}
		
		if (highRows.empty())
		{
			ui->println("No flights with occupancy >80%");
			reportContent << "No flights with occupancy >80%\n";
		}
		else
		{
			ui->displayTable(headers, highRows);
		}
		
		ui->println("\nLow Occupancy Flights (<50%):");
		ui->printSeparator();
		
		vector<vector<string>> lowRows;
		reportContent << "\nLow Occupancy Flights (<50%):\n";
		
		for (const auto& [flightNum, route, occupancy, reserved, total] : flightOccupancy)
		{
			if (occupancy < 50.0)
			{
				std::ostringstream occStream;
				occStream << std::fixed << std::setprecision(2) << occupancy << "%";
				
				lowRows.push_back({
					flightNum,
					route,
					std::to_string(reserved),
					std::to_string(total),
					occStream.str()
				});
				
				reportContent << flightNum << " (" << route << "): " << occStream.str() << "\n";
			}
		}
		
		if (lowRows.empty())
		{
			ui->println("No flights with occupancy <50%");
			reportContent << "No flights with occupancy <50%\n";
		}
		else
		{
			ui->displayTable(headers, lowRows);
		}
		
		bool exportReport = ui->getYesNo("\nExport report to file?");
		if (exportReport)
		{
			exportToFile("Occupancy", reportContent.str());
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error generating report: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Report 4: Reservation Statistics Report ====================

void ReportGenerator::generateReservationStatisticsReport()
{
	ui->clearScreen();
	ui->printHeader("Reservation Statistics Report");
	
	try
	{
		vector<shared_ptr<Reservation>> reservations = ReservationManager::getInstance()->getAllReservations();
		
		if (reservations.empty())
		{
			ui->printWarning("No reservations found in the system.");
			ui->pauseScreen();
			return;
		}
		
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
		
		vector<std::pair<string, int>> sortedPassengers(passengerBookingCounts.begin(), 
		                                                passengerBookingCounts.end());
		std::sort(sortedPassengers.begin(), sortedPassengers.end(),
		          [](const auto& a, const auto& b) { return a.second > b.second; });
		
		vector<std::pair<string, int>> sortedAgents(agentBookingCounts.begin(), 
		                                            agentBookingCounts.end());
		std::sort(sortedAgents.begin(), sortedAgents.end(),
		          [](const auto& a, const auto& b) { return a.second > b.second; });
		
		std::ostringstream reportContent;
		reportContent << "Reservation Statistics Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		ui->println("Total Reservations: " + std::to_string(totalReservations));
		
		std::ostringstream cancelStream;
		cancelStream << std::fixed << std::setprecision(2) << cancelRate << "%";
		ui->println("Cancellation Rate: " + cancelStream.str());
		
		reportContent << "Total Reservations: " << totalReservations << "\n";
		reportContent << "Cancellation Rate: " << cancelStream.str() << "\n\n";
		
		ui->println("\nReservations by Status:");
		ui->printSeparator();
		
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		reportContent << "Reservations by Status:\n";
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
			reportContent << status << ": " << count << "\n";
		}
		
		ui->displayTable(statusHeaders, statusRows);
		
		ui->println("\nTop 10 Passengers by Booking Count:");
		ui->printSeparator();
		
		vector<string> passengerHeaders = {"Passenger Username", "Booking Count"};
		vector<vector<string>> passengerRows;
		
		reportContent << "\nTop 10 Passengers by Booking Count:\n";
		int passengerCount = 0;
		for (const auto& [passenger, count] : sortedPassengers)
		{
			if (passengerCount++ >= 10) break;
			passengerRows.push_back({passenger, std::to_string(count)});
			reportContent << passenger << ": " << count << "\n";
		}
		
		ui->displayTable(passengerHeaders, passengerRows);
		
		ui->println("\nTop 10 Booking Agents by Booking Count:");
		ui->printSeparator();
		
		vector<string> agentHeaders = {"Booking Agent", "Booking Count"};
		vector<vector<string>> agentRows;
		
		reportContent << "\nTop 10 Booking Agents by Booking Count:\n";
		int agentCount = 0;
		for (const auto& [agent, count] : sortedAgents)
		{
			if (agentCount++ >= 10) break;
			agentRows.push_back({agent, std::to_string(count)});
			reportContent << agent << ": " << count << "\n";
		}
		
		ui->displayTable(agentHeaders, agentRows);
		
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

// ==================== Report 5: Crew Summary Report ====================

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
		reportContent << "Crew Summary Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		ui->println("Total Crew Members: " + std::to_string(totalCrew));
		
		reportContent << "Total Crew Members: " << totalCrew << "\n\n";
		
		ui->println("\nCrew by Role:");
		ui->printSeparator();
		
		vector<string> roleHeaders = {"Role", "Count"};
		vector<vector<string>> roleRows;
		
		reportContent << "Crew by Role:\n";
		for (const auto& [role, count] : roleCounts)
		{
			roleRows.push_back({role, std::to_string(count)});
			reportContent << role << ": " << count << "\n";
		}
		
		ui->displayTable(roleHeaders, roleRows);
		
		ui->println("\nCrew by Status:");
		ui->printSeparator();
		
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		reportContent << "\nCrew by Status:\n";
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
			reportContent << status << ": " << count << "\n";
		}
		
		ui->displayTable(statusHeaders, statusRows);
		
		ui->println("\nTop 10 Crew by Flight Hours:");
		ui->printSeparator();
		
		vector<string> crewHeaders = {"Crew ID", "Name", "Role", "Flight Hours"};
		vector<vector<string>> crewRows;
		
		reportContent << "\nTop 10 Crew by Flight Hours:\n";
		int crewCount = 0;
		for (const auto& member : sortedCrew)
		{
			if (crewCount++ >= 10) break;
			
			std::ostringstream hoursStream;
			hoursStream << std::fixed << std::setprecision(2) << member->getTotalFlightHours();
			
			crewRows.push_back({
				member->getCrewId(),
				member->getName(),
				member->getRoleString(),
				hoursStream.str()
			});
			
			reportContent << member->getCrewId() << " - " << member->getName() 
			             << " (" << member->getRoleString() << "): " 
			             << hoursStream.str() << " hours\n";
		}
		
		ui->displayTable(crewHeaders, crewRows);
		
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

// ==================== Report 6: Maintenance Summary Report ====================

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
		std::map<string, int> typeCounts;
		std::map<string, int> statusCounts;
		std::map<string, std::pair<int, double>> byAircraft;
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
				string type = record["type"].get<string>();
				string status = record["status"].get<string>();
				string aircraftType = record["aircraftType"].get<string>();
				string scheduledDate = record["scheduledDate"].get<string>();
				
				typeCounts[type]++;
				statusCounts[status]++;
				
				byAircraft[aircraftType].first++;
				if (status == "COMPLETED" && record.contains("cost"))
				{
					double cost = record["cost"].get<double>();
					byAircraft[aircraftType].second += cost;
					totalCost += cost;
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
		reportContent << "Maintenance Summary Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		ui->println("Total Maintenance Records: " + std::to_string(totalRecords));
		ui->println("Total Cost (Completed): " + ui->formatCurrency(totalCost));
		ui->println("Upcoming Maintenance (Next 30 Days): " + std::to_string(upcomingMaintenance.size()));
		ui->println("Overdue Maintenance: " + std::to_string(overdueMaintenance.size()));
		
		reportContent << "Total Maintenance Records: " << totalRecords << "\n";
		reportContent << "Total Cost (Completed): " << ui->formatCurrency(totalCost) << "\n";
		reportContent << "Upcoming Maintenance (Next 30 Days): " << upcomingMaintenance.size() << "\n";
		reportContent << "Overdue Maintenance: " << overdueMaintenance.size() << "\n\n";
		
		ui->println("\nMaintenance by Type:");
		ui->printSeparator();
		
		vector<string> typeHeaders = {"Type", "Count"};
		vector<vector<string>> typeRows;
		
		reportContent << "Maintenance by Type:\n";
		for (const auto& [type, count] : typeCounts)
		{
			typeRows.push_back({type, std::to_string(count)});
			reportContent << type << ": " << count << "\n";
		}
		
		ui->displayTable(typeHeaders, typeRows);
		
		ui->println("\nMaintenance by Status:");
		ui->printSeparator();
		
		vector<string> statusHeaders = {"Status", "Count"};
		vector<vector<string>> statusRows;
		
		reportContent << "\nMaintenance by Status:\n";
		for (const auto& [status, count] : statusCounts)
		{
			statusRows.push_back({status, std::to_string(count)});
			reportContent << status << ": " << count << "\n";
		}
		
		ui->displayTable(statusHeaders, statusRows);
		
		ui->println("\nMaintenance by Aircraft:");
		ui->printSeparator();
		
		vector<string> aircraftHeaders = {"Aircraft Type", "Record Count", "Total Cost"};
		vector<vector<string>> aircraftRows;
		
		reportContent << "\nMaintenance by Aircraft:\n";
		for (const auto& [aircraft, data] : byAircraft)
		{
			aircraftRows.push_back({
				aircraft,
				std::to_string(data.first),
				ui->formatCurrency(data.second)
			});
			reportContent << aircraft << ": " << data.first << " records, " 
			             << ui->formatCurrency(data.second) << "\n";
		}
		
		ui->displayTable(aircraftHeaders, aircraftRows);
		
		ui->println("\nUpcoming Maintenance (Next 30 Days):");
		ui->printSeparator();
		
		if (upcomingMaintenance.empty())
		{
			ui->println("No upcoming maintenance scheduled.");
			reportContent << "\nNo upcoming maintenance scheduled.\n";
		}
		else
		{
			vector<string> upcomingHeaders = {"ID", "Aircraft", "Type", "Scheduled Date"};
			vector<vector<string>> upcomingRows;
			
			reportContent << "\nUpcoming Maintenance (Next 30 Days):\n";
			for (const auto& record : upcomingMaintenance)
			{
				upcomingRows.push_back({
					record["maintenanceId"].get<string>(),
					record["aircraftType"].get<string>(),
					record["type"].get<string>(),
					record["scheduledDate"].get<string>()
				});
				
				reportContent << record["maintenanceId"].get<string>() << " - "
				             << record["aircraftType"].get<string>() << " ("
				             << record["type"].get<string>() << "): "
				             << record["scheduledDate"].get<string>() << "\n";
			}
			
			ui->displayTable(upcomingHeaders, upcomingRows);
		}
		
		if (!overdueMaintenance.empty())
		{
			ui->printWarning("\nOVERDUE MAINTENANCE DETECTED!");
			ui->printSeparator();
			
			vector<string> overdueHeaders = {"ID", "Aircraft", "Type", "Scheduled Date", "Status"};
			vector<vector<string>> overdueRows;
			
			reportContent << "\n!!! OVERDUE MAINTENANCE !!!\n";
			for (const auto& record : overdueMaintenance)
			{
				overdueRows.push_back({
					record["maintenanceId"].get<string>(),
					record["aircraftType"].get<string>(),
					record["type"].get<string>(),
					record["scheduledDate"].get<string>(),
					record["status"].get<string>()
				});
				
				reportContent << record["maintenanceId"].get<string>() << " - "
				             << record["aircraftType"].get<string>() << " ("
				             << record["type"].get<string>() << "): "
				             << record["scheduledDate"].get<string>() 
				             << " [" << record["status"].get<string>() << "]\n";
			}
			
			ui->displayTable(overdueHeaders, overdueRows);
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

// ==================== Report 7: Users Report ====================

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
				string roleStr = user->getRoleString();
				roleCounts[roleStr]++;
			}
			catch (const std::exception& e)
			{
				continue;
			}
		}
		
		std::ostringstream reportContent;
		reportContent << "User Activity Report\n";
		reportContent << "Generated: " << getCurrentDateTime() << "\n";
		reportContent << string(50, '=') << "\n\n";
		
		ui->println("\nSummary Statistics:");
		ui->printSeparator();
		ui->println("Total Users: " + std::to_string(totalUsers));

		reportContent << "Total Users: " << totalUsers << "\n";
		
		ui->println("\nUsers by Role:");
		ui->printSeparator();
		vector<string> roleHeaders = {"Role", "Count"};
		vector<vector<string>> roleRows;
		
		reportContent << "Users by Role:\n";
		for (const auto& [role, count] : roleCounts)
		{
			roleRows.push_back({role, std::to_string(count)});
			reportContent << role << ": " << count << "\n";
		}
		
		ui->displayTable(roleHeaders, roleRows);
		
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

// ==================== Export Functionality ====================

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

string ReportGenerator::getCurrentDateTime() const
{
	std::time_t now = std::time(nullptr);
	std::tm* now_tm = std::localtime(&now);
	
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);
	
	return string(buffer);
}