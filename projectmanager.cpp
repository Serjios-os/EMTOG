#include "projectmanager.h"


ProjectManager * ProjectManager::sp_instance = nullptr;

QList <model_elm_t> project_model_initializer = {
    {"id", "integer primary key autoincrement", ""},
    {"name", "varchar(20) NOT NULL", ""},
    {"nombre_blocks", "varchar(30)", ""},
    {"facteur_extension", "varchar(10)", ""},
    {"facteur_extension_default", "integer", ""},
};

ProjectManager::ProjectManager()
    : mp_model(nullptr)
{
    dbm = new DatabaseManager("projects", project_model_initializer);
    if (this->mp_model == nullptr) {
        this->mp_model = new QSqlQueryModel();
    }
}

ProjectManager::~ProjectManager() {
    delete dbm;
}

ProjectManager * ProjectManager::instance() {
    if (ProjectManager::sp_instance == nullptr) {
        ProjectManager::sp_instance = new ProjectManager();
    }
    return ProjectManager::sp_instance;
}

bool ProjectManager::isFirestUse() {
    return dbm->is_new_table();
}

QStringList ProjectManager::get_projects() {
    QStringList entries = dbm->get_all_entries("name");
    return entries;
}

int ProjectManager::get_projects(QSqlQueryModel * model) {
    QStringList entries = dbm->get_all_entries("name");
    return dbm->get_all_entries(model, "name");
}

bool ProjectManager::create_project(QList <model_elm_t *> model) {
    model = this->dbm->get_model();
    dbm->add_entry(model);
    return true;
}

QSqlRecord ProjectManager::create_project(QString name) {
    QList <model_elm_t *> model;
    model = this->dbm->get_model();
    model.at(1)->value = name;
    return dbm->add_entry(model);
}

QSqlRecord ProjectManager::edit_project(QSqlRecord record) {
    return dbm->edit_entry(record);
}

bool ProjectManager::delete_project(QSqlRecord record) {
    bool is_valid = dbm->delete_entry(record);
    return is_valid;
}

int ProjectManager::search_project(QSqlQueryModel * model, QString pattern) {
    dbm->get_all_entries(model, pattern, "name");
    int index = model->record().indexOf("name");
    return index;
}

QSqlQueryModel * ProjectManager::model(QSqlQueryModel * model) {
    if (model != nullptr) {
        this->mp_model = model;
    }
    return this->mp_model;
}
