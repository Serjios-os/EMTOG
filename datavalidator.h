
#ifndef DATAVALIDATOR_H
#define DATAVALIDATOR_H

#include <configuration.h>


class DataValidator
{
public:
    DataValidator();
    QValidator * validator(validator_category_t, QObject *);
    QString validate(const QString, QString &);
    QString couplage(QString, const bool = false);
private:
    std::map<validator_category_t, std::string> _validator_regexp_patterns;
    std::map<validator_category_t, QValidator *> _validators;
};

#endif // DATAVALIDATOR_H
