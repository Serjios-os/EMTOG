#include "datavalidator.h"


DataValidator::DataValidator() {
    this->_validators = {
        {VALIDATOR_REGEXP_PUISSANCE     , new QRegularExpressionValidator(QRegularExpression(VALIDATOR_REGEXP_STR_PUISSANCE))},
        {VALIDATOR_REGEXP_METRIC_LENGTH , new QRegularExpressionValidator(QRegularExpression(VALIDATOR_REGEXP_STR_METRIC_LENGTH))},
        {VALIDATOR_REGEXP_METRIC , new QRegularExpressionValidator(QRegularExpression(VALIDATOR_REGEXP_STR_METRIC_LENGTH))},
    };
}

QValidator * DataValidator::validator(validator_category_t category, QObject * parent) {
    this->_validators.find(category)->second->setParent(parent);
    return this->_validators.find(category)->second;
}

#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
QString DataValidator::validate(const QString parameter, QString & value) {
    QMap unit = QMap<QString, QMap<QString,int>>({{"puissance",{{"w",1},{"W",1},{"k",1000},{"K",1000},{"M",1000000}}},
                                                  {"couplage",{{"1-127",1},{"1-230",2},{"3-230",3},{"3-400",4}}}});
    QMap<QString, QString> patterns ({{"puissance","(\\d+)(?:\\s*)(k|K|M|w|W)"},{"couplage","(\\d+)-(\\d+)(?:\\s*)"}});
    QRegularExpressionMatch expression_match (QRegularExpression(patterns.value(parameter)).match(value));
    QRegularExpression _reg_exp(patterns.value(parameter));
    if (parameter == "puissance") {
        if (expression_match.hasMatch()) {
            value = QString(expression_match.captured());
            return value;
        } else {
            value = QString("0");
            return value;
        }
    }
    return QString("") ;
}
#endif

QString DataValidator::couplage(QString couplage_str, const bool inverse) {
    QMap<QString, QString> couplage_map = QMap<QString, QString>({{"0-0","Type de couplage"},{"1-127","Monophase (127 V)"},{"1-230","Monophase (230 V)"},{"3-230","Triphase (230 V)"},{"3-400","Triphase (400 V)"}});
    if (inverse) {
        return couplage_map.key(couplage_str);
    } else {
        return couplage_map.value(couplage_str);
    }
}
