#ifndef OPERATIONAL_REPORT_HPP
#define OPERATIONAL_REPORT_HPP

#include <string>
#include <map>

class FlightStats {
public:
    std::string flightNumber;
    std::string status;
    int totalBookings;
    double revenue;
};

class OperationalReport {
private:
    std::string reportId;
    std::string month;
    std::string year;
    int totalFlightsScheduled;
    int flightsCompleted;
    int flightsDelayed;
    int flightsCanceled;
    int totalReservations;
    double totalRevenue;
    std::map<std::string, FlightStats> flightPerformance;

public:
    OperationalReport(const std::string& month, const std::string& year);
    
    void displayReportSummary();
    void displayDetailedPerformance();
};

#endif // OPERATIONAL_REPORT_HPP