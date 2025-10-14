#include "ReservationManager.hpp"
#include "FlightManager.hpp"
#include "AircraftManager.hpp"
#include "UsersManager.hpp"
#include "SeatMap.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

ReservationManager* ReservationManager::instance = nullptr;

ReservationManager::ReservationManager()
{
	db = std::make_unique<Database>("Reservations"); 
	ui = UserInterface::getInstance();
}

ReservationManager* ReservationManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new ReservationManager();
	}
	return instance;
}

ReservationManager::~ReservationManager() noexcept {}

// ==================== Booking Operations ====================

void ReservationManager::createReservation(const string& agentUsername)
{
	ui->clearScreen();
	ui->printHeader("Create Reservation");
	
	try
	{
		string passenger = ui->getString("Passenger username: ");
		if (!UsersManager::getInstance()->userExists(passenger))
			throw ReservationException(ReservationErrorCode::PASSENGER_NOT_FOUND);
		
		string flight = ui->getString("Flight number: ");
		if (!FlightManager::getInstance()->flightExists(flight))
			throw ReservationException(ReservationErrorCode::FLIGHT_NOT_FOUND);
		
		auto flightObj = FlightManager::getInstance()->getFlight(flight);
		
		string seat = "";
		while (true)
		{
			if (ui->getYesNo("View seat map?"))
			{
				auto aircraft = AircraftManager::getInstance()->getAircraft(flightObj->getAircraftType());
				SeatMap seatMap(aircraft->getSeatLayout(), aircraft->getRows(), flightObj->getReservedSeats());
				
				vector<string> rowLabels;
				vector<vector<string>> gridData;
				seatMap.getSeatMapDisplayData(rowLabels, gridData);
				
				GridDisplayConfig config;
				config.title = "Seat Map";
				config.headerLines = seatMap.getSeatMapHeader(flight, flightObj->getOrigin(),
					flightObj->getDestination(), flightObj->getAircraftType());
				config.legend = seatMap.getSeatMapLegend();
				
				ui->displayGrid(rowLabels, gridData, config);
			}
			
			seat = ui->getString("Seat number: ");
			if (flightObj->isSeatAvailable(seat)) break;
			ui->printError("Seat not available.");
		}
		
		double cost = flightObj->getPrice();
		
		ui->println("\n=== Summary ===");
		ui->println("Passenger: " + passenger);
		ui->println("Flight: " + flight);
		ui->println("Seat: " + seat);
		ui->println("Cost: " + ui->formatCurrency(cost));
		
		if (!ui->getYesNo("\nConfirm?"))
		{
			ui->printWarning("Canceled.");
			ui->pauseScreen();
			return;
		}
		
		ui->println("\nProcessing payment...");
		ui->println("Payment successful!");
		
		if (!flightObj->reserveSeat(seat))
			throw ReservationException(ReservationErrorCode::SEAT_OPERATION_FAILED);
		
		string resId = "RES_" + std::to_string(db->getEntryCount() + 1);
		auto res = shared_ptr<Reservation>(new Reservation(
			resId, passenger, flight, seat, ReservationStatus::CONFIRMED,
			getCurrentDateTime(), cost, agentUsername, false, ""));
		
		saveReservationToDatabase(res);
		
		ui->printSuccess("Reservation created!");
		ui->println("ID: " + resId);
	}
	catch (const std::exception& e) { ui->printError(string(e.what())); }
	
	ui->pauseScreen();
}

void ReservationManager::modifyReservation(const string& agentUsername)
{
	ui->clearScreen();
	ui->printHeader("Modify Reservation");
	updateOrCancel(true);
}

void ReservationManager::cancelReservation(const string& agentUsername)
{
	ui->clearScreen();
	ui->printHeader("Cancel Reservation");
	updateOrCancel(false);
}

void ReservationManager::updateOrCancel(bool isModify)
{
	try
	{
		auto res = loadReservationFromDatabase(ui->getString("Reservation ID: "));
		if (!res) throw ReservationException(ReservationErrorCode::RESERVATION_NOT_FOUND);
		if (res->getStatus() != ReservationStatus::CONFIRMED)
			throw ReservationException(ReservationErrorCode::INVALID_STATUS_TRANSITION);
		
		if (isModify)
		{
			ui->println("\nCurrent Seat: " + res->getSeatNumber());
			
			auto flight = FlightManager::getInstance()->getFlight(res->getFlightNumber());
			string newSeat = "";
			while (true)
			{
				newSeat = ui->getString("New seat number: ");
				if (flight->isSeatAvailable(newSeat)) break;
				ui->printError("Seat not available.");
			}
			
			if (newSeat == res->getSeatNumber())
			{
				ui->printWarning("Same seat selected.");
				ui->pauseScreen();
				return;
			}
			
			if (!ui->getYesNo("Change to " + newSeat + "?"))
			{
				ui->printWarning("Canceled.");
				ui->pauseScreen();
				return;
			}
			
			flight->releaseSeat(res->getSeatNumber());
			if (!flight->reserveSeat(newSeat))
			{
				flight->reserveSeat(res->getSeatNumber());
				throw ReservationException(ReservationErrorCode::SEAT_OPERATION_FAILED);
			}
			
			res->setSeatNumber(newSeat);
			if (res->isCheckedIn()) res->setCheckedIn("");
			saveReservationToDatabase(res);
			ui->printSuccess("Reservation updated!");
		}
		else
		{
			if (!ui->getYesNo("Cancel reservation?"))
			{
				ui->printWarning("Canceled.");
				ui->pauseScreen();
				return;
			}
			
			auto flight = FlightManager::getInstance()->getFlight(res->getFlightNumber());
			if (flight) flight->releaseSeat(res->getSeatNumber());
			
			res->setStatus(ReservationStatus::CANCELED);
			saveReservationToDatabase(res);
			ui->printSuccess("Reservation canceled!");
		}
	}
	catch (const std::exception& e) { ui->printError(string(e.what())); }
	
	ui->pauseScreen();
}

// ==================== Check-In Operations ====================

void ReservationManager::checkIn(const string& passengerUsername)
{
	ui->clearScreen();
	ui->printHeader("Check In");
	
	try
	{
		auto res = loadReservationFromDatabase(ui->getString("Reservation ID: "));
		if (!res) throw ReservationException(ReservationErrorCode::RESERVATION_NOT_FOUND);
		if (res->getPassengerUsername() != passengerUsername)
			throw ReservationException(ReservationErrorCode::UNAUTHORIZED_ACCESS);
		if (res->getStatus() != ReservationStatus::CONFIRMED)
			throw ReservationException(ReservationErrorCode::INVALID_STATUS_TRANSITION);
		
		if (!res->isCheckedIn())
		{
			res->setCheckedIn(getCurrentDateTime());
			saveReservationToDatabase(res);
		}
		
		ui->printSuccess("Checked in!");
		displayBoardingPass(res);
	}
	catch (const std::exception& e) { ui->printError(string(e.what())); }
	
	ui->pauseScreen();
}

void ReservationManager::viewBoardingPass(const string& passengerUsername)
{
	ui->clearScreen();
	ui->printHeader("View Boarding Pass");
	
	try
	{
		auto res = loadReservationFromDatabase(ui->getString("Reservation ID: "));
		if (!res) throw ReservationException(ReservationErrorCode::RESERVATION_NOT_FOUND);
		if (res->getPassengerUsername() != passengerUsername)
			throw ReservationException(ReservationErrorCode::UNAUTHORIZED_ACCESS);
		if (!res->isCheckedIn())
			throw ReservationException(ReservationErrorCode::INVALID_STATUS_TRANSITION);
		
		displayBoardingPass(res);
	}
	catch (const std::exception& e) { ui->printError(string(e.what())); }
	
	ui->pauseScreen();
}

// ==================== Query Operations ====================

void ReservationManager::viewReservations(const string& username, UserRole role)
{
	ui->clearScreen();
	ui->printHeader("View Reservations");
	
	try
	{
		vector<shared_ptr<Reservation>> reservations;
		
		if (role == UserRole::PASSENGER)
		{
			reservations = getReservationsByPassenger(username);
		}
		else if (role == UserRole::BOOKING_AGENT)
		{
			vector<string> filters = {"All", "By Passenger", "By Flight", "My Bookings", "Back"};
			ui->displayMenu("Filter", filters);
			
			switch (ui->getChoice("Choice: ", 1, 5))
			{
				case 1:
					reservations = loadAllReservations();
					break;
				case 2:
					reservations = getReservationsByPassenger(ui->getString("Passenger: "));
					break;
				case 3:
					reservations = getReservationsByFlight(ui->getString("Flight: "));
					break;
				case 4:
				{
					reservations = loadAllReservations();
					auto it = std::remove_if(reservations.begin(), reservations.end(),
						[&username](const shared_ptr<Reservation>& r) { return r->getBookedByAgent() != username; });
					reservations.erase(it, reservations.end());
					break;
				}
				case 5:
					return;
			}
		}
		
		displayReservationsTable(reservations);
	}
	catch (const std::exception& e) { ui->printError(string(e.what())); }
	
	ui->pauseScreen();
}

vector<shared_ptr<Reservation>> ReservationManager::getReservationsByPassenger(const string& passengerUsername)
{
	vector<shared_ptr<Reservation>> results;
	try
	{
		json allData = db->loadAll();
		for (const auto& [id, data] : allData.items())
		{
			try
			{
				auto res = reservationFromJson(data);
				if (res->getPassengerUsername() == passengerUsername)
					results.push_back(res);
			}
			catch (const std::exception&) {}
		}
	}
	catch (const std::exception& e) { ui->printError("Error loading reservations: " + string(e.what())); }
	return results;
}

vector<shared_ptr<Reservation>> ReservationManager::getReservationsByFlight(const string& flightNumber)
{
	vector<shared_ptr<Reservation>> results;
	try
	{
		json allData = db->loadAll();
		for (const auto& [id, data] : allData.items())
		{
			try
			{
				auto res = reservationFromJson(data);
				if (res->getFlightNumber() == flightNumber)
					results.push_back(res);
			}
			catch (const std::exception&) {}
		}
	}
	catch (const std::exception& e) { ui->printError("Error loading reservations: " + string(e.what())); }
	return results;
}

vector<shared_ptr<Reservation>> ReservationManager::getAllReservations()
{
	return loadAllReservations();
}

vector<shared_ptr<Reservation>> ReservationManager::loadAllReservations()
{
	vector<shared_ptr<Reservation>> results;
	try
	{
		json allData = db->loadAll();
		for (const auto& [id, data] : allData.items())
		{
			try
			{
				results.push_back(reservationFromJson(data));
			}
			catch (const std::exception&) {}
		}
	}
	catch (const std::exception& e) { ui->printError("Error loading reservations: " + string(e.what())); }
	return results;
}

bool ReservationManager::hasActiveReservations(const string& flightNumber)
{
	try
	{
		json allData = ReservationManager::getInstance()->db->loadAll();
		for (const auto& [id, data] : allData.items())
		{
			try
			{
				auto res = ReservationManager::getInstance()->reservationFromJson(data);
				if (res->getFlightNumber() == flightNumber && res->getStatus() == ReservationStatus::CONFIRMED)
					return true;
			}
			catch (const std::exception&) {}
		}
	}
	catch (const std::exception&) { return true; }
	return false;
}

// ==================== Database Operations ====================

shared_ptr<Reservation> ReservationManager::loadReservationFromDatabase(const string& reservationId)
{
	if (!db->entryExists(reservationId)) return nullptr;
	try
	{
		return reservationFromJson(db->getEntry(reservationId));
	}
	catch (const std::exception&)
	{
		throw ReservationException(ReservationErrorCode::DATABASE_ERROR);
	}
}

void ReservationManager::saveReservationToDatabase(const shared_ptr<Reservation>& reservation)
{
	if (!reservation) throw ReservationException(ReservationErrorCode::DATABASE_ERROR);
	try
	{
		json data = reservationToJson(reservation);
		if (db->entryExists(reservation->getReservationId()))
			db->updateEntry(reservation->getReservationId(), data);
		else
			db->addEntry(reservation->getReservationId(), data);
	}
	catch (const DatabaseException&)
	{
		throw ReservationException(ReservationErrorCode::DATABASE_ERROR);
	}
}

// ==================== JSON Serialization ====================

json ReservationManager::reservationToJson(const shared_ptr<Reservation>& reservation)
{
	json data;
	data["reservationId"] = reservation->getReservationId();
	data["passengerUsername"] = reservation->getPassengerUsername();
	data["flightNumber"] = reservation->getFlightNumber();
	data["seatNumber"] = reservation->getSeatNumber();
	data["status"] = static_cast<int>(reservation->getStatus());
	data["bookingDate"] = reservation->getBookingDate();
	data["totalCost"] = reservation->getTotalCost();
	data["bookedByAgent"] = reservation->getBookedByAgent();
	data["isCheckedIn"] = reservation->isCheckedIn();
	data["checkInDate"] = reservation->getCheckInDate();
	return data;
}

shared_ptr<Reservation> ReservationManager::reservationFromJson(const json& data)
{
	return shared_ptr<Reservation>(new Reservation(
		data["reservationId"], data["passengerUsername"], data["flightNumber"],
		data["seatNumber"], static_cast<ReservationStatus>(data["status"].get<int>()),
		data["bookingDate"], data["totalCost"], data["bookedByAgent"],
		data["isCheckedIn"], data["checkInDate"]));
}

// ==================== Display Helpers ====================

void ReservationManager::displayReservationsTable(const vector<shared_ptr<Reservation>>& reservations)
{
	if (reservations.empty())
	{
		ui->printWarning("No reservations found.");
		return;
	}
	
	vector<string> headers = {"ID", "Passenger", "Flight", "Seat", "Status", "Cost", "Booked By", "Checked In"};
	vector<vector<string>> rows;
	
	for (const auto& res : reservations)
	{
		string status = (res->getStatus() == ReservationStatus::CONFIRMED ? "CONFIRMED" :
		                res->getStatus() == ReservationStatus::COMPLETED ? "COMPLETED" : "CANCELED");
		
		rows.push_back({
			res->getReservationId(),
			res->getPassengerUsername(),
			res->getFlightNumber(),
			res->getSeatNumber(),
			status,
			ui->formatCurrency(res->getTotalCost()),
			res->getBookedByAgent(),
			res->isCheckedIn() ? "Yes" : "No"
		});
	}
	
	ui->displayTable(headers, rows);
	ui->println("\nTotal: " + std::to_string(reservations.size()) + " reservation(s)");
}

void ReservationManager::displayBoardingPass(const shared_ptr<Reservation>& res)
{
	string passengerName;
	try
	{
		passengerName = UsersManager::getInstance()->getUser(res->getPassengerUsername())->getName();
	}
	catch (const std::exception&) { passengerName = res->getPassengerUsername(); }
	
	auto flight = FlightManager::getInstance()->getFlight(res->getFlightNumber());
	
	ui->println("\n" + string(50, '='));
	ui->println("            BOARDING PASS");
	ui->println(string(50, '='));
	ui->println("");
	ui->println("  Passenger: " + passengerName);
	ui->println("  Flight: " + res->getFlightNumber());
	ui->println("  Seat: " + res->getSeatNumber());
	ui->println("  Gate: " + flight->getGate());
	ui->println("  Boarding Time: " + flight->getBoardingTime());
	ui->println("  Check-In Date: " + res->getCheckInDate());
	ui->println("");
	ui->println(string(50, '='));
	ui->println("  Please arrive at gate 30 minutes before boarding");
	ui->println(string(50, '='));
}

string ReservationManager::getCurrentDateTime() const
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
	return ss.str();
}