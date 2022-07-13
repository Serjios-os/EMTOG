#include "datawrapper.h"

DataWrapper::DataWrapper() {
    this->_defult_values = {
        {"facteur_extension", DEFAULT_VALUE_FACTEUR_EXTENTION},
        {"facteur_puissance", DEFAULT_VALUE_FACTEUR_PUISSANCE_0},
        {"rendement", DEFAULT_VALUE_RENDEMENT_0},
        {"facteur_utilisation", DEFAULT_VALUE_FACTEUR_UTILISATION},
        {"facteur_simultaneite", DEFAULT_VALUE_FACTEUR_SIMULTANEITE},
    };
}

float DataWrapper::wrappe(const QString parameter, QString value) {
    QMap<QString, QMap<QString,int>> unit = QMap<QString, QMap<QString,int>>({{"puissance",{{"",1},{"w",1},{"W",1},{"k",1000},{"K",1000},{"M",1000000}}},
                                                  {"facteur_conversion",{{"0-0",0},{"1-127",1},{"1-230",2},{"3-230",3},{"3-400",4}}}});
    QMap<QString, QString> patterns ({{"puissance","(\\d+)(?:\\s*)(k|K|M|w|W)"},{"facteur_conversion","(\\d+)-(\\d+)(?:\\s*)"}});
    QRegularExpressionMatch expression_match (QRegularExpression(patterns.value(parameter)).match(value));
    if (parameter == "puissance") {
        if (expression_match.hasMatch()) {
            return (unit.value(parameter).value(expression_match.capturedTexts().value(2)) * expression_match.capturedTexts().value(1).toFloat());
        } else {
            return value.toFloat();
        }
    } else if (parameter == "facteur_conversion") {
        QMap<int, float> wrapper = QMap<int, float>({{0,0},{1,8},{2,4.35},{3,2.5},{4,1.44}});
        return (wrapper.value(value.toInt()));
    }
    return -1 ;
}

float DataWrapper::default_value(const std::string parameter) {
    return this->_defult_values.find(parameter)->second;
    return -1 ;
}

float DataWrapper::default_value(const std::string parameter, const double value) {
    if (value < 1000) {
        if (parameter == "facteur_puissance")
            return  DEFAULT_VALUE_FACTEUR_PUISSANCE_0;
        else if (parameter == "rendement")
            return DEFAULT_VALUE_RENDEMENT_0;
        else if (parameter == "puissance")
            return DEFAULT_VALUE_RENDEMENT_0;
    } else if (value < 4000) {
        if (parameter == "facteur_puissance")
            return  DEFAULT_VALUE_FACTEUR_PUISSANCE_1000;
        else if (parameter == "rendement")
            return DEFAULT_VALUE_RENDEMENT_1000;
        else if (parameter == "puissance")
            return DEFAULT_VALUE_RENDEMENT_1000;
    } else if (value < 50000) {
        if (parameter == "facteur_puissance")
            return  DEFAULT_VALUE_FACTEUR_PUISSANCE_4000;
        else if (parameter == "rendement")
            return DEFAULT_VALUE_RENDEMENT_4000;
        else if (parameter == "puissance")
            return DEFAULT_VALUE_RENDEMENT_4000;
    } else {
        if (parameter == "facteur_puissance")
            return  DEFAULT_VALUE_FACTEUR_PUISSANCE_50000;
        else if (parameter == "rendement")
            return DEFAULT_VALUE_RENDEMENT_50000;
        else if (parameter == "puissance")
            return DEFAULT_VALUE_RENDEMENT_50000;
    }
    return -1 ;
}
