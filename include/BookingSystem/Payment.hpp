#ifndef PAYMENT_HPP
#define PAYMENT_HPP

#include <string>

enum class PaymentMethod {
    CREDIT_CARD,
    CASH,
    PAYPAL
};

enum class TransactionStatus {
    PENDING,
    COMPLETED,
    FAILED,
    REFUNDED
};


class Payment {
private:
    std::string paymentId;
    double amount;
    PaymentMethod paymentMethod;
    std::string paymentDetails;
    TransactionStatus transactionStatus;

public:
    Payment(double amount, PaymentMethod method, const std::string& details);
    
    bool processPayment();
    bool refund();
    double getAmount() const;
    std::string getPaymentMethodString() const;
};

#endif // PAYMENT_HPP