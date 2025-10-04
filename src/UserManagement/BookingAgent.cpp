#include "BookingAgent.hpp"

// Constructor
BookingAgent::BookingAgent(const std::string& username, const std::string& password)
    : User(username, password, UserRole::BOOKING_AGENT)
{
}

// UI
void BookingAgent::displayMenu()
{
}

void BookingAgent::handleMenuChoice(int choice)
{
}

// Booking Operations
void BookingAgent::searchFlightsMenu()
{
}

void BookingAgent::bookFlightMenu()
{
}

void BookingAgent::modifyReservationMenu()
{
}

void BookingAgent::cancelReservationMenu()
{
}

bool BookingAgent::processRefund(const std::string& reservationId, double amount)
{
}