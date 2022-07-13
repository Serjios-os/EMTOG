#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <configuration.h>



typedef struct model_elm_s {
    QString parameter;
    QString type;
    QString value;
} model_elm_t;

class DatabaseManager
{
public:
    DatabaseManager(QString, QList <model_elm_t>);
    ~DatabaseManager();
    QList <model_elm_t *> get_db() { return this->model; };
    int set_db(QList <model_elm_t *> list) { this->model = list; return EXIT_SUCCESS; };
    QList <model_elm_t *> get_model() { return this->model; };
    int set_model(QList <model_elm_t *> list) { this->model = list; return EXIT_SUCCESS; };
    int init_table(QString);
    bool is_new_table();
    bool check_entry(QList <model_elm_t *>);
    QSqlRecord add_entry(QList <model_elm_t *>);
    QSqlRecord add_entry(QSqlRecord);
    QSqlRecord edit_entry(QSqlRecord);
    bool delete_entry(QSqlRecord);
    bool delete_entries(QString, QString);
    int set_value(model_elm_t);
    model_elm_t get_value(model_elm_t);
    QStringList get_all_entries(QString);
    int get_all_entries(QSqlQueryModel *, QString = "*");
    int get_all_entries(QSqlQueryModel *, QString, QString);
    int get_entries(QSqlQueryModel *, QString, QString);
    int get_entries(QSqlQuery *, QString, QString);
    int reload();
    static int init_db();

private:
    static QSqlDatabase m_database;
    QList <model_elm_t *> model;
    QString m_table_name;
    bool m_new_table;
    int open_db();
    qsizetype init_model(QList <model_elm_t>);
};

#endif // DATABASEMANAGER_H
