#ifndef FINANCIAL_REPORT_HPP
#define FINANCIAL_REPORT_HPP

#include <string>

class FinancialReport {
private:
    std::string period;
    double totalRevenue;
    double totalRefunds;
    double netRevenue;

public:
    void displayReport();
};

#endif // FINANCIAL_REPORT_HPP