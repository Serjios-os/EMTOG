#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <signal.h>
#include <QtSql>
#include <QtDebug>
#include <QDebug>
#include <QString>
#include <QList>
#include <QStandardPaths>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QFileDialog>
#include <QInputDialog>
#include <QWidget>
#include <QListWidgetItem>
#include <QRegularExpressionValidator>
#include <QAbstractButton>
#include <QRegularExpressionValidator>
#include <QStandardItemModel>
#include <QValidator>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QMessageBox>


#define DB_NAME                             ("local")
#define APP_ICON                            ("icon.png")
#define FILE_OUTPUT_NAME                    ("electric_mtu_generated")
#define FILE_NORM                           ("NFC-15-105.pdf")
#define SIZE_FRAME_CHARGE_DATA              (310)
#define VALIDATOR_REGEXP_STR_PUISSANCE      ("(\\d+).?(\\d*)(?:\\s*)")
#define VALIDATOR_REGEXP_STR_METRIC_LENGTH  ("(\\d+).?(\\d*)(?:\\s*)(m|cm|km)")
#define VALIDATOR_REGEXP_STR_METRIC         ("(\\d+).?(\\d*)(?:\\s*)")

#define DEFAULT_VALUE_FACTEUR_EXTENTION                 (1.2)
#define DEFAULT_VALUE_FACTEUR_PUISSANCE_0               (0.5)
#define DEFAULT_VALUE_FACTEUR_PUISSANCE_1000            (0.7)
#define DEFAULT_VALUE_FACTEUR_PUISSANCE_4000            (0.8)
#define DEFAULT_VALUE_FACTEUR_PUISSANCE_50000           (0.9)
#define DEFAULT_VALUE_RENDEMENT_0                       (0.5)
#define DEFAULT_VALUE_RENDEMENT_1000                    (0.7)
#define DEFAULT_VALUE_RENDEMENT_4000                    (0.8)
#define DEFAULT_VALUE_RENDEMENT_50000                   (0.9)
#define DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_0     (4)
#define DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_1000  (2)
#define DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_4000  (1.5)
#define DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_50000 (1.2)
#define DEFAULT_VALUE_FACTEUR_UTILISATION               (0.75)
#define DEFAULT_VALUE_FACTEUR_SIMULTANEITE              (1)

typedef enum validation_category_e {
    VALIDATOR_UNIVERSAL_INTEGER,
    VALIDATOR_UNIVERSAL_FLOAT,
    VALIDATOR_REGEXP_PUISSANCE,
    VALIDATOR_REGEXP_METRIC_LENGTH,
    VALIDATOR_REGEXP_METRIC,
} validator_category_t;


class Configuration
{
public:
    Configuration() {

    };
    ~Configuration() {

    };
    QDir dir_configuration () {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        qDebug() << "$ siraj > standard path : " << path;
        QDir dir(path);
        if (QDir(path).exists() == false) {
            qDebug() << "$ siraj > mkdir path : " << QDir().mkdir(path);
        }
        return dir;
    }
    QDir dir_data () {
        QDir dir = this->dir_configuration();
        if (dir.exists("data") == false) {
            qDebug() << "$ siraj > mkdir data : " << dir.mkdir("data");
        }
        dir = QDir(dir.filePath("data"));
        return dir;
    }
    QDir dir_temp () {
        QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        qDebug() << "$ siraj > dir_temp : " << path;
        QDir dir(path);
        if (QDir(path).exists() == false) {
            qDebug() << "$ siraj > mkdir path : " << QDir().mkdir(path);
        }
        return dir;
    }
private:
    static Configuration * _instance;
    QDir _dir_configuration;
    QDir _dir_data;
};

#endif // CONFIGURATION_H
