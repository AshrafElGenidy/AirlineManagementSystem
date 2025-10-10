#ifndef REPORT_GENERATOR_HPP
#define REPORT_GENERATOR_HPP

#include <string>
#include "OperationalReport.hpp"
#include "MaintenanceReport.hpp"
#include "FinancialReport.hpp"

class AirlineManagementSystem;

enum class ReportType {
	OPERATIONAL,
	MAINTENANCE,
	FINANCIAL
};

class ReportGenerator {
private:
    AirlineManagementSystem* airlineManagementSystem;

public:
    ReportGenerator(AirlineManagementSystem* controller);
    
    void generateOperationalReport(const std::string& month, const std::string& year);
    void generateMaintenanceReport();
    void generateUserActivityReport(const std::string& userId);
    void generateFinancialReport(const std::string& period);
};

#endif // REPORT_GENERATOR_HPP