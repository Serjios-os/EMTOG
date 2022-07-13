#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <configuration.h>
#include <datacalculator.h>



class DataWrapper
{
public:
    DataWrapper();
    float wrappe(const QString, QString);
    float default_value(const std::string);
    float default_value(const std::string, const double);
private:
    std::map<std::string, float> _defult_values;
    DataCalculator _data_caculator;
};

#endif // DATAWRAPPER_H
