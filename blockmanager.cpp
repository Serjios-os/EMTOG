#include "blockmanager.h"


BlockManager * BlockManager::sp_instance = nullptr;

QList <model_elm_t> block_model_initializer = {
    {"id", "integer primary key autoincrement", ""},
    {"project_id", "varchar(20) NOT NULL", ""},
    {"Type", "varchar(30)", ""},
    {"name", "varchar(30)", ""},
    {"puissance_nominale", "varchar(30)", ""},
    {"rendement", "varchar(5)", ""},
    {"rendement_default", "integer", ""},
    {"couplage", "varchar(10)", ""},
    {"facteur_puissance", "varchar(5)", ""},
    {"facteur_puissance_default", "integer", ""},
    {"facteur_utilisation", "varchar(5)", ""},
    {"facteur_utilisation_default", "integer", ""},
    {"facteur_simultaneite", "varchar(5)", ""},
    {"facteur_simultaneite_default", "integer", ""},
    {"cable_length", "varchar(5)", ""},
    {"courant", "varchar(5)", ""},
    {"mode_pose", "varchar(5)", ""},
    {"cable_meth_ref", "integer", ""},
    {"cable_temperature", "integer", ""},
    {"cable_group_cable", "integer", ""},
    {"cable_resistance_sol", "integer", ""},
    {"cable_group_conduit", "integer", ""},
    {"cable_section", "varchar(5)", ""},
    {"current_admissible", "varchar(5)", ""},
    {"protection", "integer", ""},
    {"current_assigned", "REAL", ""},
    {"current_assigned_default", "integer", ""},
};

BlockManager::BlockManager()
    : mp_model(nullptr)
    , m_projec_id(-1)
{
    dbm = new DatabaseManager("blocks", block_model_initializer);
    if (this->mp_model == nullptr) {
        this->mp_model = new QSqlQueryModel();
    }
}

BlockManager::~BlockManager()
{
    delete dbm;
}

BlockManager * BlockManager::instance() {
    if (BlockManager::sp_instance == nullptr) {
        BlockManager::sp_instance = new BlockManager();
    }
    return BlockManager::sp_instance;
}

int BlockManager::project_id(int id) {
    if (id) {
        this->m_projec_id = id;
    }
    return this->m_projec_id;
}

QStringList BlockManager::get_blocks() {
    QStringList entries = dbm->get_all_entries("name");
    return entries;
}

int BlockManager::get_blocks(QSqlQueryModel * model) {
    QString p_id = QString::number(this->project_id());
    dbm->get_entries(model, "project_id", p_id);
    int index = model->record().indexOf("name");
    return index;
}

int BlockManager::get_blocks(QSqlQuery * query) {
    QString p_id = QString::number(this->project_id());
    dbm->get_entries(query, "project_id", p_id);
    return 0;
}

bool BlockManager::create_project(QList <model_elm_t *> model) {
    model = this->dbm->get_model();
    dbm->add_entry(model);
    return true;
}

QSqlRecord BlockManager::create_project(QString name, QString type) {
    QList <model_elm_t *> model;
    model = this->dbm->get_model();
    /* workeround */
    model.at(1)->value = QString::number(this->project_id());
    model.at(2)->value = type;
    model.at(3)->value = name;
    return dbm->add_entry(model);
}

QSqlRecord BlockManager::edit_block(QSqlRecord record) {
    dbm->edit_entry(record);
    return record;
}

bool BlockManager::delete_block(QSqlRecord record) {
    bool is_valid = dbm->delete_entry(record);
    return is_valid;
}

bool BlockManager::delete_blocks(int id) {
    bool is_valid = dbm->delete_entries("project_id", QString::number(id));
    return is_valid;
}

int BlockManager::search_block(QSqlQueryModel * model, QString pattern) {
    dbm->get_all_entries(model, pattern, "name");
    int index = model->record().indexOf("name");
    return index;
}

QSqlQueryModel * BlockManager::model(QSqlQueryModel * model) {
    if (model != nullptr) {
        this->mp_model = model;
    }
    return this->mp_model;
}
