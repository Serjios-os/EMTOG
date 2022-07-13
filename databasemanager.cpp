#include <databasemanager.h>


QSqlDatabase DatabaseManager::m_database;

DatabaseManager::DatabaseManager(QString name, QList <model_elm_t> model_initializer)
    : m_table_name(name)
{
    if (DatabaseManager::init_db()) {
        qDebug() << "[DB-MGR] Database Initialization failed !";
    }
    if (this->init_model(model_initializer) != model_initializer.size()) {
        qDebug() << "[DB-MGR] Model Initialization failed !";
    }
    if (this->init_table(m_table_name)) {
        qDebug() << "[DB-MGR] Table Initialization failed !";
    }
}

DatabaseManager::~DatabaseManager() {

}

int DatabaseManager::init_db() {
    Configuration configuration = Configuration();
    QString db_file = configuration.dir_data().filePath(QString(DB_NAME) + ".db");
    if (! DatabaseManager::m_database.isOpen()) {
        DatabaseManager::m_database = QSqlDatabase::addDatabase("QSQLITE");
        DatabaseManager::m_database.setDatabaseName(db_file);
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int DatabaseManager::open_db() {
    if (DatabaseManager::m_database.isOpen() == false) {
        if (DatabaseManager::m_database.open() != 0) {
            qDebug() << "[DB-MGR] Failed to open database";
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int DatabaseManager::reload() {
    if (DatabaseManager::m_database.isOpen()) {
        DatabaseManager::m_database.close();
    }
    DatabaseManager::m_database.open();
    return EXIT_SUCCESS;
}

qsizetype DatabaseManager::init_model(QList <model_elm_t> model_initializer) {
    QList <model_elm_t *> l_model = this->get_model();
    for(uint it = 0; it < model_initializer.size(); it++) {
        model_elm_t * l_model_elm = new model_elm_t(model_initializer.at(it));
        l_model.append(l_model_elm);
    }
    this->set_model(l_model);
    return l_model.size();
}

int DatabaseManager::init_table(QString table) {
    QList <model_elm_t *> l_model = this->get_model();
    QString request = "create table";
    request += " " + table + " (";
    for(uint it = 0; it < l_model.size(); it++) {
        model_elm_t * l_model_elm = l_model.at(it);
        if (it) {
            request += ",";
        }
        request += " " + l_model_elm->parameter + " ";
        request += " " + l_model_elm->type + " ";
    }
    request += ")";
    if (this->open_db() == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    QSqlQuery query;
    query.exec(request);
    if(query.lastError().nativeErrorCode().toInt() == query.lastError().NoError) {
        this->m_new_table = true;
    } else if (query.lastError().databaseText().contains("table " + table + " already exists")) {
        qDebug() << "[DB-MGR] table accounts already exists !";
        this->m_new_table = false;
    } else {
        qDebug() << "[DB-MGR] Query error: " << query.lastError();
        qDebug() << "[DB-MGR] Table creation failed.";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

bool DatabaseManager::is_new_table() {
    return this->m_new_table;
}

int DatabaseManager::set_value(model_elm_t elem) {
    return EXIT_SUCCESS;
}

model_elm_t DatabaseManager::get_value(model_elm_t elem) {
    return elem;
}

bool DatabaseManager::check_entry(QList <model_elm_t *> model) {
    QString request = "select * from";
    request += " " + m_table_name + " where ";
    for(uint it = 0; it < model.size(); it++) {
        model_elm_t * l_model_elm = model.at(it);
        if (it) {
            request += " and ";
        }
        request += l_model_elm->parameter + "=='" + l_model_elm->value + "'";
    }
    if (this->open_db() != EXIT_SUCCESS){
        return false;
    }
    QSqlQuery query;
    query.exec(request);
    if (query.next()) {
        return true;
    }
    return false;
}

QStringList DatabaseManager::get_all_entries(QString parameter) {
    QStringList list;
    QString request = "select * from";
    request += " " + m_table_name ;
    if (this->open_db() != EXIT_SUCCESS){
        return list;
    }
    QSqlQuery query;
    query.exec(request);
    QSqlRecord rec = query.record();
    int index = rec.indexOf(parameter); // index of the field "name"
    while (query.next()) {
        QString elem = query.value(index).toString();
        list.append(elem);
    }
    return list;
}

int DatabaseManager::get_all_entries(QSqlQueryModel * model, QString parameter) {
    QString request = "select * from ";
    request += m_table_name + " ORDER BY id DESC";
    if (this->open_db() != EXIT_SUCCESS){
        return -1 ;
    }
    QSqlQuery query;
    query.prepare(request);
    query.exec();
    QSqlRecord rec = query.record();
    int index;
    if ((index = rec.indexOf(parameter)) < 0) {
        index = rec.indexOf("name");
    }
    model->setQuery(request, DatabaseManager::m_database);
    return index;
}

int DatabaseManager::get_all_entries(QSqlQueryModel * model, QString pattern, QString parameter) {
    QString request = "select * from";
    request += " " + m_table_name + " WHERE " + parameter + " like '%" + pattern + "%' ORDER BY id DESC";
    if (this->open_db() != EXIT_SUCCESS){
        return -1 ;
    }
    QSqlQuery query;
    query.prepare(request);
    query.exec();
    QSqlRecord rec = query.record();
    int index;
    if ((index = rec.indexOf(parameter)) < 0) {
        index = rec.indexOf("name");
    }
    model->setQuery(request, DatabaseManager::m_database);
    return index;
}

int DatabaseManager::get_entries(QSqlQueryModel * model, QString parameter, QString value) {
    QString request = "select * from " + m_table_name + " WHERE " + parameter + " = '" + value + "' ORDER BY id DESC";
    if (this->open_db() != EXIT_SUCCESS){
        return -1 ;
    }
    QSqlQuery query;
    query.prepare(request);
    query.exec();
    QSqlRecord rec = query.record();
    int index;
    if ((index = rec.indexOf(parameter)) < 0) {
        index = rec.indexOf("name");
    }
    model->setQuery(request, DatabaseManager::m_database);
    return index;
}

int DatabaseManager::get_entries(QSqlQuery * query, QString parameter, QString value) {
    QString request = "select * from " + m_table_name + " WHERE " + parameter + " = '" + value + "' ORDER BY id DESC";
    if (this->open_db() != EXIT_SUCCESS){
        return -1 ;
    }
    query->prepare(request);
    query->exec();
    return 0;
}

QSqlRecord DatabaseManager::add_entry(QList <model_elm_t *> model) {
    QSqlRecord record = QSqlRecord();
    QString request = "insert into";
    QString values = "values (";
    request += " " + m_table_name + " (";
    bool first = true;
    for(uint it = 1; it < model.size(); it++) {
        model_elm_t * l_model_elm = model.at(it);
        if (first) {
            first =false;
        } else {
            request += ",";
            values += ",";
        }
        request += l_model_elm->parameter;
        values += "'" + l_model_elm->value + "'";
    }
    request += ") " + values + ")";
    if (this->open_db() != EXIT_SUCCESS){
        return record;
    }
    QSqlQuery query;
    query.exec(request);
    if(query.lastError().nativeErrorCode().toInt() == query.lastError().NoError) {
        query.clear();
        query.exec("select * from " + m_table_name + " order by id desc");
        query.first();
        record = query.record();
    } else if (query.lastError().databaseText().contains("UNIQUE constraint failed")) {
        qDebug() << "Unique constraint failed !";
    } else {
        qDebug() << "Query error: " << query.lastError();
        qDebug() << "add entry failed.";
    }
    return record;
}

QSqlRecord DatabaseManager::add_entry(QSqlRecord record) {
    QString request = "insert into " + m_table_name + " (";
    QString values = "values (";
    bool first = true;
    for(int it = 1; it < record.count(); it++) {
        if (first) {
            first =false;
        } else {
            request += ",";
            values += ",";
        }
        request += record.fieldName(it);
        values += "'" + record.value(it).toString() + "'";
    }
    request += ") " + values + ")";
    if (this->open_db() == EXIT_SUCCESS){
        QSqlQuery query;
        query.exec(request);
        if(query.lastError().nativeErrorCode().toInt() == query.lastError().NoError) {
            query.clear();
            query.exec("select * from " + m_table_name + " order by id desc");
            query.first();
            record = query.record();
        } else if (query.lastError().databaseText().contains("UNIQUE constraint failed")) {
            qDebug() << "Unique constraint failed !";
        } else {
            qDebug() << "Query error: " << query.lastError();
            qDebug() << "add entry failed.";
        }
    } else {
        qDebug() << "Open database failed !";
    }
    return record;
}

QSqlRecord DatabaseManager::edit_entry(QSqlRecord record) {
    QString request = "update " + m_table_name + " set ";
    bool first = true;
    for(int it = 1; it < record.count(); it++) {
        if (first) {
            first =false;
        } else {
            request += ",";
        }
        request += " " + record.fieldName(it) + " = '" + record.value(it).toString() + "' ";
    }
    request += " where " + record.fieldName(0) + " = " + record.value(0).toString();
    if (this->open_db() == EXIT_SUCCESS){
        QSqlQuery query;
        query.exec(request);
        if(query.lastError().nativeErrorCode().toInt() == query.lastError().NoError) {
        } else if (query.lastError().databaseText().contains("UNIQUE constraint failed")) {
            qDebug() << "Unique constraint failed !";
        } else {
            qDebug() << "Query error: " << query.lastError();
            qDebug() << "add entry failed.";
        }
    } else {
        qDebug() << "Open database failed !";
    }
    return record;
}

bool DatabaseManager::delete_entry(QSqlRecord record) {
    QSqlQuery query;
    QString request = "delete from "+ m_table_name + " WHERE id = '" + record.value("id").toString() + "'";
    if (this->open_db() != EXIT_SUCCESS){
        return false;
    }
    query.exec(request);
    if(query.lastError().nativeErrorCode().toInt() == query.lastError().NoError) {
        return true;
    } else if (query.lastError().databaseText().contains("UNIQUE constraint failed")) {
        qDebug() << "Unique constraint failed !";
        return false;
    } else {
        qDebug() << "Query error: " << query.lastError();
        qDebug() << "add entry failed.";
        return false;
    }
    return false;
}

bool DatabaseManager::delete_entries(QString parameter, QString value) {
    QSqlQuery query;
    QString request = "delete from "+ m_table_name + " WHERE " + parameter + " = '" + value + "'";
    qDebug() << "Request : " << request;
    if (this->open_db() != EXIT_SUCCESS){
        return false;
    }
    query.exec(request);
    if(query.lastError().nativeErrorCode().toInt() == query.lastError().NoError) {
        return true;
    } else {
        qDebug() << "Query error: " << query.lastError();
        qDebug() << "delete entries failed.";
        return false;
    }
    return false;
}
