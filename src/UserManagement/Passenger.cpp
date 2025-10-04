#include "Passenger.hpp"

// Constructor
Passenger::Passenger(const std::string& username, const std::string& password, const std::string& name)
    : User(username, password, UserRole::PASSENGER)
{
}

// UI
void Passenger::displayMenu()
{
}

void Passenger::handleMenuChoice(int choice)
{
}

// Passenger Operations
void Passenger::searchFlightsMenu()
{
}

Reservation* Passenger::bookFlightFromSearch(const std::string& flightNumber)
{
}

void Passenger::viewMyReservationsMenu()
{
}

void Passenger::checkInMenu()
{
}

// Loyalty Program
void Passenger::earnLoyaltyPoints(int points)
{
}

double Passenger::redeemLoyaltyPoints(int points)
{
}