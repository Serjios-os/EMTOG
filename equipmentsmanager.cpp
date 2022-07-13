#include "equipmentsmanager.h"
#include "ui_equipmentsmanager.h"


QList <model_elm_t> database_model_initializer = {
    {"id", "integer primary key autoincrement", ""},
    {"ref", "varchar(20) NOT NULL", ""},
    {"value", "NUMERIC NOT NULL", ""},
};

EquipmentsManager::EquipmentsManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EquipmentsManager),
    _database_manager_cables(new DatabaseManager("cables", database_model_initializer)),
    _database_manager_fusibles(new DatabaseManager("fusibles", database_model_initializer)),
    _database_manager_disjoncteurs_dom(new DatabaseManager("disjoncteurs_dom", database_model_initializer)),
    _database_manager_disjoncteurs_ind(new DatabaseManager("disjoncteurs_ind", database_model_initializer)),
    _database_model(new QSqlQueryModel()),
    _database_model_index(new QModelIndex())
{
    ui->setupUi(this);
    this->database_fetch(this->database(this->ui->field_equipment_type->currentIndex()));
    this->ui->btn_delete->hide();
}

EquipmentsManager::~EquipmentsManager() {
    delete ui;
}

void EquipmentsManager::on_btn_confirmation_accepted() {
    QString __pattern = this->ui->field_equipment_search->text();
    DatabaseManager * __database_manager = this->database(this->ui->field_equipment_type->currentIndex());
    if (__database_manager) {
        if (this->_database_model_index->isValid()) {
            QSqlRecord __record = this->_database_model->record(this->_database_model_index->row());
            __record.setValue("ref", this->ui->field_equipment_reference->text());
            __record.setValue("value", QString::number(this->ui->field_equipment_value->text().toFloat()));
            this->database_update(__database_manager, __record);
            this->_database_model_index = new QModelIndex();
            this->ui->btn_delete->hide();
        } else {
            QSqlRecord __record = QSqlRecord();
            QSqlField __field = QSqlField();
            __field = QSqlField();
            __field.setName("id");
            __field.setMetaType(QMetaType(QMetaType::QUuid));
            __field.setAutoValue(true);
            __record.append(__field);
            __field = QSqlField();
            __field.setName("ref");
            __field.setMetaType(QMetaType(QMetaType::QString));
            __field.setValue(QVariant(this->ui->field_equipment_reference->text()));
            __record.append(__field);;
            __field = QSqlField();
            __field.setName("value");
            __field.setMetaType(QMetaType(QMetaType::QReal));
            __field.setValue(QVariant(QString::number(this->ui->field_equipment_value->text().toFloat())));
            __record.append(__field);
            this->database_insert(__database_manager, __record);
        }
    }
    this->database_fetch(__database_manager, __pattern);
    this->ui->field_equipment_reference->clear();
    this->ui->field_equipment_value->clear();
    this->close();
}


void EquipmentsManager::on_btn_confirmation_rejected() {
    this->ui->field_equipment_reference->clear();
    this->ui->field_equipment_value->clear();
    this->close();
}

void EquipmentsManager::database_insert(DatabaseManager* database, QSqlRecord record) {
    record = database->add_entry(record);
}

void EquipmentsManager::database_delete(DatabaseManager* database, QSqlRecord record) {
    database->delete_entry(record);
}

void EquipmentsManager::database_update(DatabaseManager* database, QSqlRecord record) {
    database->edit_entry(record);
}

void EquipmentsManager::database_fetch(DatabaseManager* database, QString pattern) {
    int index = (database) ? database->get_all_entries(this->_database_model, pattern, "ref") : 0;
    this->ui->list_compenents->setModel(this->_database_model);
    this->ui->list_compenents->setModelColumn(index);
    this->ui->list_compenents->show();
}


DatabaseManager * EquipmentsManager::database(int index) {
    switch (index)  {
        case 0: /* clable */
            return this->_database_manager_cables;
            break;

        case 1: /* fusible */
            return this->_database_manager_fusibles;
            break;

        case 2: /* disjonteur demostique */
            return this->_database_manager_disjoncteurs_dom;
            break;

        case 3: /* disjoncteur industriel */
            return this->_database_manager_disjoncteurs_ind;
            break;

        default:
            return nullptr;
    }
}


void EquipmentsManager::on_field_equipment_type_currentIndexChanged(int index) {
    this->database_fetch(this->database(index));
    this->_database_model_index = new QModelIndex();
}


void EquipmentsManager::on_list_compenents_doubleClicked(const QModelIndex &index) {
    this->ui->btn_delete->show();
    this->_database_model_index = new QModelIndex(index);
    this->ui->field_equipment_reference->setText(this->_database_model->record(index.row()).value("ref").toString());
    this->ui->field_equipment_value->setText(this->_database_model->record(index.row()).value("value").toString());
}


void EquipmentsManager::on_btn_delete_clicked() {
    DatabaseManager * __database = this->database(this->ui->field_equipment_type->currentIndex());
    QSqlRecord __record = this->_database_model->record(this->_database_model_index->row());
    QString __pattern = this->ui->field_equipment_search->text();
    this->database_delete(__database,__record);
    this->database_fetch(__database,__pattern);
    this->_database_model_index = new QModelIndex();
    this->ui->field_equipment_reference->clear();
    this->ui->field_equipment_value->clear();
    this->ui->btn_delete->hide();
}


void EquipmentsManager::on_field_equipment_search_textEdited(const QString & pattern) {
    this->database_fetch(this->database(this->ui->field_equipment_type->currentIndex()), pattern);
}

