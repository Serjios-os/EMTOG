#ifdef Q_OS_WIN
#include <_mingw_mac.h>
#endif
#include <dashboard.h>
#include "ui_dashboard.h"


Dashboard * Dashboard::sp_dashboard = nullptr;

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Dashboard)
    , pmgr(nullptr)
    , bmgr(nullptr)
    , projects_db_model(nullptr)
    , blocks_db_model(nullptr)
    , selected_project_record(nullptr)
    , _data_calculator(DataCalculator())
#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
    , _equipments_manager(EquipmentsManager(this))
#endif
    , edit(false)
{
#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
    this->_equipments_manager.hide();
#endif
    projects_model = new QItemSelectionModel () ;
    if (this->pmgr == nullptr) {
        this->pmgr = ProjectManager::instance();
    }
    if ((this->bmgr) == nullptr) {
        this->bmgr = BlockManager::instance();
    }
    if ((this->projects_db_model) == nullptr) {
        this->projects_db_model = new QSqlQueryModel(this);
        this->projects_db_model = this->pmgr->model(this->projects_db_model);
    }
    if ((this->blocks_db_model) == nullptr) {
        this->blocks_db_model = new QSqlQueryModel(this);
        this->blocks_db_model = this->bmgr->model(this->blocks_db_model);
    }
    this->project_db_wrapper = {};
}

Dashboard::~Dashboard() {
    delete ui;
}

Dashboard * Dashboard::instance(QWidget * parent) {
    if (Dashboard::sp_dashboard == nullptr) {
        Dashboard::sp_dashboard = new Dashboard(parent);
    }
    return Dashboard::sp_dashboard;
}

void Dashboard::set_ui() {
    ui->setupUi(this);
    /* Configure initial stats */
    this->ui->frame_database_backup->hide();
    this->ui->btn_project_new->hide();
    this->ui->btn_database_backup->buttons().at(2)->setEnabled(false);
    this->ui->frame_installation_data->hide();
    this->ui->frame_charge_data->hide();
    this->ui->frame_charge_data_generated->hide();
    this->ui->btn_project_save->setEnabled(false);
    this->ui->btn_project_delete->setEnabled(false);
    /* Configure validators */
    this->ui->field_charge_data_puissance_nominale_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_PUISSANCE, this));
    this->ui->field_charge_data_cable_length_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_METRIC_LENGTH, this));
    this->ui->field_charge_data_couplage_edit->setItemData(0, "0-0");
    this->ui->field_charge_data_couplage_edit->setItemData(1, "1-127");
    this->ui->field_charge_data_couplage_edit->setItemData(2, "1-230");
    this->ui->field_charge_data_couplage_edit->setItemData(3, "3-230");
    this->ui->field_charge_data_couplage_edit->setItemData(4, "3-380");

    this->ui->field_charge_data_meth_ref_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_METRIC, this));
    this->ui->field_charge_data_temperatur_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_METRIC, this));
    this->ui->field_charge_data_group_cable_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_METRIC, this));
    this->ui->field_charge_data_resistance_sol_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_METRIC, this));
    this->ui->field_charge_data_group_conduit_edit->setValidator(this->_data_validator.validator(VALIDATOR_REGEXP_METRIC, this));

    this->ui->lbl_charge_data_neutre->hide();
    this->ui->lbl_charge_data_temperatur_edit->hide();
    this->ui->lbl_charge_data_group_cable_edit->hide();
    this->ui->lbl_charge_data_resistance_sol_edit->hide();
    this->ui->lbl_charge_data_group_conduit_edit->hide();

    this->ui->field_charge_data_meth_ref_edit->hide();
    this->ui->field_charge_data_temperatur_edit->hide();
    this->ui->field_charge_data_group_cable_edit->hide();
    this->ui->field_charge_data_resistance_sol_edit->hide();
    this->ui->field_charge_data_group_conduit_edit->hide();

    this->ui->field_charge_data_cable_length_edit->hide();
    this->ui->lbl_charge_data_cable_length_edit->hide();
    this->ui->tableView->hide();
}

void Dashboard::refresh_project_list() {
    int index = pmgr->get_projects(projects_db_model);
    this->ui->list_projects->setModel(projects_db_model);
    if (index < 0) {
        this->ui->list_projects->setModelColumn(1);
    } else {
        this->ui->list_projects->setModelColumn(index);
    }
    this->ui->list_projects->show();
}

void Dashboard::refresh_blocks_list() {
    int index = bmgr->get_blocks(blocks_db_model);
    this->ui->list_blocks->setModel(blocks_db_model);
    this->ui->list_blocks->setModelColumn(index);
    this->ui->list_blocks->show();
}

void Dashboard::refresh() {
    this->refresh_project_list();
    this->refresh_blocks_list();
}

void Dashboard::on_btn_project_create_clicked() {
    QString name = this->ui->field_installation_data_name_new->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Ajouter un projet"), tr("Le titre du projet est vide !"));
        return;
    }
    if (this->ui->frame_installation_data->isHidden()) {
        this->ui->frame_installation_data->show();
    }
    this->ui->btn_project_save->setEnabled(true);
    this->ui->btn_project_delete->setEnabled(true);
    this->create(name);
    this->ui->field_installation_data_name_new->clear();
}

void Dashboard::on_btn_project_save_clicked() {
    this->selected_project_record->setValue("name", this->ui->field_installation_data_name_edit->text());
    this->selected_project_record->setValue("facteur_extension", this->ui->field_installation_data_facteur_extension->text());
    this->selected_project_record->setValue("facteur_extension_default", this->ui->cbx_installation_data_facteur_extension->isChecked());
    this->pmgr->edit_project(*(this->selected_project_record));
    this->refresh();
    this->fill_project_fields((QSqlRecord) *this->selected_project_record);
}

void Dashboard::on_btn_project_delete_clicked() {
    this->ui->btn_project_save->setEnabled(false);
    this->ui->btn_project_delete->setEnabled(false);
    this->ui->field_installation_data_name_edit->clear();
    this->bmgr->delete_blocks(this->bmgr->project_id());
    this->pmgr->delete_project(*(this->selected_project_record));
    this->bmgr->project_id(-1);
    this->refresh();
    this->ui->frame_charge_data->hide();
    this->ui->frame_charge_data_generated->hide();
}

void Dashboard::on_btn_project_search_clicked() {
    QString pattern = this->ui->field_installation_data_name_search->text();
    int index = pmgr->search_project(projects_db_model, pattern);
    this->ui->list_projects->setModel(projects_db_model);
    if (index < 0) {
        this->ui->list_projects->setModelColumn(1);
    } else {
        this->ui->list_projects->setModelColumn(index);
    }
    this->ui->list_projects->show();
}

void Dashboard::on_list_blocks_doubleClicked(const QModelIndex &index) {
    this->ui->frame_charge_data->show();
    this->ui->frame_charge_data_generated->hide();
    QSqlRecord record = this->blocks_db_model->record(index.row())                                                                                                                          ;
    this->selected_block_record = new QSqlRecord(record);
    clear_block_fields();
    fill_block_fields((QSqlRecord) *this->selected_block_record);
    this->refresh_blocks_list();
    this->ui->btn_charge_data_apply->setEnabled(true);
    this->ui->btn_block_delete->setEnabled(true);
}

void Dashboard::on_list_projects_doubleClicked(const QModelIndex &index) {
    this->ui->frame_installation_data->show();
    QSqlRecord record = this->projects_db_model->record(index.row());
    this->selected_project_record = new QSqlRecord(record);
    bmgr->project_id(this->selected_project_record->value("id").toInt());
    this->clear_block_fields();
    this->ui->frame_charge_data->hide();
    this->ui->frame_charge_data_generated->hide();
    fill_project_fields((QSqlRecord) *this->selected_project_record);
    this->refresh_blocks_list();
    this->ui->btn_project_save->setEnabled(true);
    this->ui->btn_project_delete->setEnabled(true);
}

bool Dashboard::fill_project_fields(const QSqlRecord record) {
    this->ui->field_installation_data_name_edit->setText(record.value("name").toString());
    QString facteur_extension = QString::number(record.value("facteur_extension").toFloat());
    this->ui->field_installation_data_facteur_extension->setText(facteur_extension);
    int state;
    state = (record.value("facteur_extension_default").toString() == "true") ? 1 : 0;
    this->ui->cbx_installation_data_facteur_extension->setChecked(state);
    return true;
}

bool Dashboard::clear_block_fields() {
    this->ui->field_charge_data_name_edit->clear();
    this->ui->field_charge_data_type_edit->setCurrentIndex(0);
    this->ui->field_charge_data_puissance_nominale_value->clear();
    this->ui->field_charge_data_puissance_nominale_edit->clear();
    this->ui->field_charge_data_rendement->clear();
    this->ui->field_charge_data_couplage_edit->setCurrentIndex(0);
    this->ui->field_charge_data_facteur_puissance->clear();
    this->ui->field_charge_data_facteur_utilisation->clear();
    this->ui->field_charge_data_facteur_simultaneite->clear();
    this->ui->field_charge_data_mode_pose_edit->setCurrentIndex(0);
    this->ui->field_charge_data_cable_length_edit->clear();
    this->ui->field_charge_data_current_edit->clear();
    this->ui->field_charge_data_current_admissible->clear();
    return true;
}

bool Dashboard::fill_block_fields(const QSqlRecord record) {
    this->ui->field_charge_data_name_edit->setText(record.value("name").toString());
    this->ui->field_charge_data_type_edit->setCurrentText(record.value("type").toString());
    QString puissance;
    float puissance_value;
    puissance = record.value("puissance_nominale").toString();
    puissance_value = this->_data_wrapper.wrappe("puissance", puissance);
    this->ui->field_charge_data_puissance_nominale_edit->setText(puissance);
    this->ui->field_charge_data_puissance_nominale_value->setValue(puissance_value);
    int state;
    state = (record.value("rendement_default").toString() == "true") ? 1 : 0;
    this->ui->cbx_charge_data_rendement->setChecked(state);
    state = (record.value("facteur_puissance_default").toString() == "true") ? 1 : 0;
    this->ui->cbx_charge_data_facteur_puissance->setChecked(state);
    state = (record.value("facteur_utilisation_default").toString() == "true") ? 1 : 0;
    this->ui->cbx_charge_data_facteur_utilisation->setChecked(state);
    state = (record.value("facteur_simultaneite_default").toString() == "true") ? 1 : 0;
    this->ui->cbx_charge_data_facteur_simultaneite->setChecked(state);
    this->ui->field_charge_data_rendement->setValue(record.value("rendement").toFloat());
    this->ui->field_charge_data_couplage_edit->setCurrentIndex(record.value("couplage").toInt());
    this->ui->field_charge_data_facteur_puissance->setValue(record.value("facteur_puissance").toFloat());
    this->ui->field_charge_data_facteur_utilisation->setValue(record.value("facteur_utilisation").toFloat());
    this->ui->field_charge_data_facteur_simultaneite->setValue(record.value("facteur_simultaneite").toFloat());
    this->ui->field_charge_data_mode_pose_edit->setCurrentIndex(record.value("mode_pose").toInt());
    this->ui->field_charge_data_meth_ref_edit->setText(record.value("cable_meth_ref").toString());
    this->ui->field_charge_data_temperatur_edit->setText(record.value("cable_temperature").toString());
    this->ui->field_charge_data_group_cable_edit->setText(record.value("cable_group_cable").toString());
    this->ui->field_charge_data_resistance_sol_edit->setText(record.value("cable_resistance_sol").toString());
    this->ui->field_charge_data_group_conduit_edit->setText(record.value("cable_group_conduit").toString());
    this->ui->field_charge_data_current_edit->setText(QString::number(record.value("courant").toFloat()));
    state = record.value("protection").toInt();
    this->ui->field_charge_data_protection->setCurrentIndex(state);
    this->ui->cbx_charge_data_current_assigned->setEnabled(state);
    /* Order important */
    state = (record.value("current_assigned_default").toString() == "true") ? 1 : 0;
    this->ui->cbx_charge_data_current_assigned->setChecked(state);
    this->ui->field_charge_data_current_assigned_edit->setText(QString::number(record.value("current_assigned").toFloat()));
    this->ui->field_charge_data_current_admissible->setText(record.value("current_admissible").toString());
    this->ui->field_charge_data_facteur_conversion_result->setText(QString::number(this->_data_wrapper.wrappe("facteur_conversion", record.value("couplage").toString())));
    this->ui->field_charge_data_name_facteur_extension_result->setText(selected_project_record->value("facteur_extension").toString());
    this->ui->field_charge_data_name_facteur_simultaneite_result->setText(record.value("facteur_simultaneite").toString());
    this->ui->field_charge_data_name_facteur_utilisation_result->setText(record.value("facteur_utilisation").toString());
    this->ui->field_charge_data_name_facteur_puissance_result->setText(record.value("facteur_puissance").toString());
    this->ui->field_charge_data_name_rendement_result->setText(record.value("rendement").toString());
    this->ui->field_charge_data_name_puissance_nominale_result->setText(QString::number(puissance_value));
    this->ui->field_charge_data_name_type_result->setText(record.value("type").toString());
    this->ui->field_charge_data_current_result->setText(QString::number(record.value("courant").toFloat()));
    this->ui->field_charge_data_current_assigned_result->setText(QString::number(record.value("current_assigned").toFloat()));
    return true;
}

void Dashboard::on_btn_project_block_add_clicked() {
    this->ui->field_charge_data_name_new->text();
    QString name = this->ui->field_charge_data_name_new->text();
    QString type = this->ui->field_charge_data_type_new->currentText();
    int type_index = this->ui->field_charge_data_type_new->currentIndex();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Ajouter une charge"), tr("Le nom de la charge est vide !"));
    } else if (type_index < 1) {
        QMessageBox::warning(this, tr("Ajouter une charge"), tr("Seletionne le type de la charge !"));
    } else {
        QSqlRecord record = this->bmgr->create_project(name, type);
        record.setValue("puissance_nominale", "0");
        record.setValue("rendement", 0);
        record.setValue("rendement_default", false);
        record.setValue("couplage", 0);
        record.setValue("facteur_puissance", 0);
        record.setValue("facteur_puissance_default", false);
        record.setValue("facteur_utilisation", 0);
        record.setValue("facteur_utilisation_default", false);
        record.setValue("facteur_simultaneite", 0);
        record.setValue("facteur_simultaneite_default", false);
        record.setValue("cable_length", "0");
        record.setValue("mode_pose", "0");
        record.setValue("cable_meth_ref", 0);
        record.setValue("cable_temperature", 0);
        record.setValue("cable_group_cable", 0);
        record.setValue("cable_resistance_sol", 0);
        record.setValue("cable_group_conduit", 0);
        record.setValue("protection", 0);
        record.setValue("current_assigned_default", false);
        record.setValue("current_assigned", 0);
        this->ui->cbx_charge_data_current_assigned->setEnabled(false);
        record.setValue("courant", 0);
        record.setValue("current_admissible", 0);
        this->selected_block_record = new QSqlRecord(this->bmgr->edit_block(record));
        this->refresh_blocks_list();
        this->ui->frame_charge_data->show();
        this->clear_block_fields();
        this->fill_block_fields((QSqlRecord) *this->selected_block_record);
        this->ui->field_charge_data_name_new->clear();
        this->ui->field_charge_data_type_new->setCurrentIndex(0);
    }
}

void Dashboard::on_btn_block_delete_clicked() {
    this->ui->btn_charge_data_apply->setEnabled(false);
    this->ui->btn_block_delete->setEnabled(false);
    this->ui->field_charge_data_name_edit->clear();
    this->bmgr->delete_block(*(this->selected_block_record));
    this->refresh();
    this->ui->frame_charge_data->hide();
    this->ui->frame_charge_data_generated->hide();
}

void Dashboard::on_btn_charge_data_apply_clicked() {
    bool valide = true;
    int __state = 0;
    this->selected_block_record->setValue("name", this->ui->field_charge_data_name_edit->text());
    this->selected_block_record->setValue("project_id", this->bmgr->project_id());
    this->selected_block_record->setValue("type", this->ui->field_charge_data_type_edit->currentText());
    this->selected_block_record->setValue("puissance_nominale", this->ui->field_charge_data_puissance_nominale_edit->text());
    this->selected_block_record->setValue("rendement", this->ui->field_charge_data_rendement->value());
    this->selected_block_record->setValue("rendement_default", this->ui->cbx_charge_data_rendement->isChecked());
    this->selected_block_record->setValue("couplage", this->ui->field_charge_data_couplage_edit->currentIndex());
    this->selected_block_record->setValue("facteur_puissance", this->ui->field_charge_data_facteur_puissance->value());
    this->selected_block_record->setValue("facteur_puissance_default", this->ui->cbx_charge_data_facteur_puissance->isChecked());
    this->selected_block_record->setValue("facteur_utilisation", this->ui->field_charge_data_facteur_utilisation->value());
    this->selected_block_record->setValue("facteur_utilisation_default", this->ui->cbx_charge_data_facteur_utilisation->isChecked());
    this->selected_block_record->setValue("facteur_simultaneite", this->ui->field_charge_data_facteur_simultaneite->value());
    this->selected_block_record->setValue("facteur_simultaneite_default", this->ui->cbx_charge_data_facteur_simultaneite->isChecked());
    __state = this->ui->field_charge_data_mode_pose_edit->currentIndex();
    this->selected_block_record->setValue("mode_pose", __state);
    float __cable_coef = 1.00;
    float __cable_meth_ref = 0.00;
    float __cable_temperature = 0.00;
    float __cable_group_cable = 0.00;
    float __cable_resistance_sol = 0.00;
    float __cable_group_conduit = 0.00;
    if (__state) {
        __cable_resistance_sol = 1.00;
        __cable_group_conduit = 1.00;
        switch (__state) {
            case 3:
                __cable_coef = 0.80;
                __cable_group_conduit = this->ui->field_charge_data_group_conduit_edit->text().toFloat();
                __cable_resistance_sol = this->ui->field_charge_data_resistance_sol_edit->text().toFloat();
            break;
            case 4:
                __cable_resistance_sol = this->ui->field_charge_data_resistance_sol_edit->text().toFloat();
            break;
        }
        __cable_meth_ref = this->ui->field_charge_data_meth_ref_edit->text().toFloat();
        __cable_temperature = this->ui->field_charge_data_temperatur_edit->text().toFloat();
        __cable_group_cable = this->ui->field_charge_data_group_cable_edit->text().toFloat();
    }
    this->selected_block_record->setValue("cable_meth_ref", QString::number(__cable_meth_ref));
    this->selected_block_record->setValue("cable_temperature", QString::number(__cable_temperature));
    this->selected_block_record->setValue("cable_group_cable", QString::number(__cable_group_cable));
    this->selected_block_record->setValue("cable_resistance_sol", QString::number(__cable_resistance_sol));
    this->selected_block_record->setValue("cable_group_conduit", QString::number(__cable_group_conduit));
    this->selected_block_record->setValue("cable_length", this->ui->field_charge_data_cable_length_edit->text());
    this->bmgr->edit_block(*(this->selected_block_record));
    /* Real Time data model */
    float __courant = this->ui->field_charge_data_current_edit->text().toFloat();
    this->selected_block_record->setValue("courant", __courant);
    /* TBD : Determination du courant In : */
    __state = this->ui->field_charge_data_protection->currentIndex();
    this->selected_block_record->setValue("protection", __state);
    this->selected_block_record->setValue("current_assigned_default", this->ui->cbx_charge_data_current_assigned->isChecked());
    this->update_current_assigned(true);
    float __current_assigned = this->ui->field_charge_data_current_assigned_edit->text().toFloat();
    if (__current_assigned < __courant) {
        QMessageBox::warning(this, tr("Impossible !"),
                             tr("Le courant d'emploi est superieur du courant assigné !"));
        this->ui->cbx_charge_data_current_assigned->setChecked(false);
    }
    this->selected_block_record->setValue("current_assigned", __current_assigned);
    /* TBD : Determination du coefition Iz = k*In : */
    cable_data_t __current_admissible_data;
    __current_admissible_data.courant = __current_assigned;
    __current_admissible_data.cable_coef = __cable_coef;
    __current_admissible_data.cable_meth_ref = __cable_meth_ref;
    __current_admissible_data.cable_temperature = __cable_temperature;
    __current_admissible_data.cable_group_cable = __cable_group_cable;
    __current_admissible_data.cable_resistance_sol = __cable_resistance_sol;
    __current_admissible_data.cable_group_conduit = __cable_group_conduit;
    __current_admissible_data.longeur = 0;
    float __current_admissible = this->_data_calculator.current_admissible(__current_admissible_data);
    this->selected_block_record->setValue("current_admissible", QString::number(__current_admissible));
    this->bmgr->edit_block(*(this->selected_block_record));
    this->refresh();
    fill_block_fields((QSqlRecord) *this->selected_block_record);
    if (valide)
        this->ui->frame_charge_data_generated->show();
}

void Dashboard::on_btn_project_generate_clicked() {
    QMessageBox::warning(this, tr("Future Feathers"), tr("This is a Future Feathers !"));
}

void Dashboard::create(QString name) {

    if (name.isEmpty()) {
        qDebug() << "# Project name is empty #";
    } else {
        QSqlRecord record = this->pmgr->create_project(name);
        record.setValue("nombre_blocks", "0");
        record.setValue("facteur_extension", "0");
        record.setValue("facteur_extension_default", false);
        this->selected_project_record = new QSqlRecord(this->pmgr->edit_project(record));
        this->bmgr->project_id(record.value("id").toInt());
        this->clear_block_fields();
        this->ui->frame_charge_data->hide();
        this->ui->frame_charge_data_generated->hide();
        this->refresh();
        this->fill_project_fields((QSqlRecord) *this->selected_project_record);
    }
}

void Dashboard::on_field_installation_data_name_search_textEdited(const QString &pattern) {
    int index = pmgr->search_project(projects_db_model, pattern);
    this->ui->list_projects->setModel(projects_db_model);
    if (index < 0) {
        this->ui->list_projects->setModelColumn(1);
    } else {
        this->ui->list_projects->setModelColumn(index);
    }
    this->ui->list_projects->show();
}

void Dashboard::on_btn_database_toggled(bool checked) {
    if (checked) {
        this->ui->btn_database_backup->buttons().at(0)->setText("Export");
        this->ui->btn_database_backup->buttons().at(1)->setText("Imoprt");
        this->ui->btn_database_backup->buttons().at(2)->setText("Cancel");
        this->ui->btn_database_backup->buttons().at(3)->setText("Update");
        this->ui->frame_database_backup->show();
    } else {
        this->ui->frame_database_backup->hide();
    }
}

void Dashboard::on_btn_database_backup_clicked(QAbstractButton *button) {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    QString file_orig = dir.filePath(QString("local") + ".db");
    QString file_orig_backup = dir.filePath("backup." + QString("local") + ".db");
    if (button->text() == "Export") {
        QString file_orig = dir.filePath(QString("local") + ".db");
        QString filter = QString("*.db");
        QString file_backup = QFileDialog::getSaveFileName(0, tr("Save the database location"), QDir().homePath(), filter, &filter);
        if (!file_backup.isEmpty()) {
            if (file_backup.endsWith(".db")) {
                QFile(file_backup).remove();
                QFile(file_orig).copy(file_backup);
            } else {
                QFile(file_backup + ".db").remove();
                QFile(file_orig).copy(file_backup + ".db");
            }
        }
    } else if (button->text() == "Imoprt") {
        QString filter = QString("*.db");
        QString file_backup = QFileDialog::getOpenFileName(0, tr("Save the database location"), QDir().homePath(), filter, &filter);
        if (!file_backup.isEmpty()) {
            QFile(file_orig_backup).remove();
            QFile(file_orig).copy(file_orig_backup);
            QFile(file_orig).remove();
            QFile(file_backup).copy(file_orig);
            this->ui->btn_database_backup->buttons().at(2)->setEnabled(true);
            this->pmgr->database()->reload();
            this->refresh();
        }
    } else if (button->text() == "Cancel") {
        QFile(file_orig).remove();
        QFile(file_orig_backup).copy(file_orig);
        this->ui->btn_database_backup->buttons().at(2)->setEnabled(false);
        this->pmgr->database()->reload();
        this->refresh();
    } else if (button->text() == "Update") {
        this->_equipments_manager.show();
    }
}

void Dashboard::resize() {
    QPoint start;
    QSize size;
    QRect geometry;
    start = QPoint(0, 0);
    size = QSize(this->size().width() * 15 / 100, this->size().height() * 90 / 100);
    geometry = QRect(start, size);
    this->ui->frame_installations->setGeometry(geometry);
    this->ui->btn_database->setGeometry(5, this->size().height() * 92 / 100, geometry.width() - 10, this->size().height() * 6 / 100);
    start = QPoint(start.x() + geometry.width(), 0);
    size = QSize(this->size().width() * 30 / 100, this->size().height() * 90 / 100);
    geometry = QRect(start, size);
    this->ui->frame_installation->setGeometry(geometry);
    start = QPoint(start.x(), start.y() + geometry.height());
    size = QSize(this->size().width() * 30 / 100, this->size().height() * 10 / 100);
    geometry = QRect(start, size);
    this->ui->frame_database_backup->setGeometry(geometry);
    start = QPoint(start.x() + geometry.width(), 0);
    size = QSize(this->size().width() * 55 / 100, SIZE_FRAME_CHARGE_DATA);
    geometry = QRect(start, size);
    this->ui->frame_charge_data->setGeometry(geometry);
    start = QPoint(start.x(), start.y() + geometry.height());
    size = QSize(this->size().width() * 55 / 100, this->size().height() * 90 / 100 - SIZE_FRAME_CHARGE_DATA);
    geometry = QRect(start, size);
    this->ui->frame_charge_data_generated->setGeometry(geometry);
    this->ui->btn_download->setGeometry(geometry.right() - this->ui->btn_download->width() - 1, geometry.top() + geometry.height() + 1, this->ui->btn_download->width(), this->ui->btn_download->height());
}

void Dashboard::on_field_installation_data_name_new_returnPressed() {
    if (this->ui->frame_installation_data->isHidden()) {
        this->ui->frame_installation_data->show();
    }
    this->ui->btn_project_save->setEnabled(true);
    this->ui->btn_project_delete->setEnabled(true);
    QString name = this->ui->field_installation_data_name_new->text();
    this->create(name);
    this->ui->field_installation_data_name_new->clear();
}

void Dashboard::on_cbx_installation_data_facteur_extension_stateChanged(int state) {
    if (state) {
        double value;
        value = this->_data_wrapper.default_value("facteur_extension");
        this->ui->field_installation_data_facteur_extension->setText(QString::number(value));
        this->ui->field_installation_data_facteur_extension->setEnabled(false);
    } else {
        this->ui->field_installation_data_facteur_extension->setEnabled(true);
    }
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_cbx_charge_data_facteur_puissance_stateChanged(int state) {
    if (state) {
        double value, v;
        v = this->ui->field_charge_data_puissance_nominale_value->value();
        value = this->_data_wrapper.default_value("facteur_puissance", v);
        this->ui->field_charge_data_facteur_puissance->setValue(value);
        this->ui->field_charge_data_facteur_puissance->setEnabled(false);
    } else {
        this->ui->field_charge_data_facteur_puissance->setEnabled(true);
    }
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_cbx_charge_data_rendement_stateChanged(int state) {
    if (state) {
        double value, v;
        v = this->ui->field_charge_data_puissance_nominale_value->value();
        value = this->_data_wrapper.default_value("rendement", v);
        this->ui->field_charge_data_rendement->setValue(value);
        this->ui->field_charge_data_rendement->setEnabled(false);
    } else {
        this->ui->field_charge_data_rendement->setEnabled(true);
    }
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_cbx_charge_data_facteur_utilisation_stateChanged(int state) {
    if (state) {
        double value;
        value = this->_data_wrapper.default_value("facteur_utilisation");
        this->ui->field_charge_data_facteur_utilisation->setValue(value);
        this->ui->field_charge_data_facteur_utilisation->setEnabled(false);
    } else {
        this->ui->field_charge_data_facteur_utilisation->setEnabled(true);
    }
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_cbx_charge_data_facteur_simultaneite_stateChanged(int state) {
    if (state) {
        double value;
        value = this->_data_wrapper.default_value("facteur_simultaneite");
        this->ui->field_charge_data_facteur_simultaneite->setValue(value);
        this->ui->field_charge_data_facteur_simultaneite->setEnabled(false);
    } else {
        this->ui->field_charge_data_facteur_simultaneite->setEnabled(true);
    }
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_field_charge_data_puissance_nominale_edit_textChanged(const QString & value) {
    double v, dv;
    int state;
    v = this->_data_wrapper.wrappe("puissance", value);
    this->ui->field_charge_data_puissance_nominale_value->setValue(v);
    state = this->ui->cbx_charge_data_facteur_puissance->isChecked();
    if (state) {
        dv = this->_data_wrapper.default_value("facteur_puissance", v);
        this->ui->field_charge_data_facteur_puissance->setValue(dv);
    }
    state = this->ui->cbx_charge_data_rendement->isChecked();
    if (state) {
        dv = this->_data_wrapper.default_value("rendement", v);
        this->ui->field_charge_data_rendement->setValue(dv);
    }
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_field_charge_data_mode_pose_edit_currentIndexChanged(int index) {
    std::string lbl_charge_data_meth_ref_str = "Methode de reference :";
    std::string lbl_charge_data_temperatur_str = "Température :";
    std::string lbl_charge_data_group_cable_str = "Groupement de câbles :";
    std::string lbl_charge_data_resistance_sol_str = "Résistivité thermique du sol :";
    std::string lbl_charge_data_group_conduit_str = "Groupement de conduits :";
    switch (index) {
        case 1: // goto mode_de_pose_13_multi;
        {
            lbl_charge_data_meth_ref_str += " (E) ";
            lbl_charge_data_temperatur_str += " (BF1) ";
            lbl_charge_data_group_cable_str += " (BG1 Ref.4)";
            this->ui->lbl_charge_data_neutre->show();
            this->ui->lbl_charge_data_temperatur_edit->show();
            this->ui->lbl_charge_data_group_cable_edit->show();
            this->ui->lbl_charge_data_resistance_sol_edit->hide();
            this->ui->lbl_charge_data_group_conduit_edit->hide();
            this->ui->field_charge_data_meth_ref_edit->show();
            this->ui->field_charge_data_temperatur_edit->show();
            this->ui->field_charge_data_group_cable_edit->show();
            this->ui->field_charge_data_resistance_sol_edit->hide();
            this->ui->field_charge_data_group_conduit_edit->hide();
            break;
        }
        case 2: // goto mode_de_pose_13_mono;
        {
            lbl_charge_data_meth_ref_str += " (F) ";
            lbl_charge_data_temperatur_str += " (BF1) ";
            lbl_charge_data_group_cable_str += " (BG1 Ref.4)";
            this->ui->lbl_charge_data_neutre->show();
            this->ui->lbl_charge_data_temperatur_edit->show();
            this->ui->lbl_charge_data_group_cable_edit->show();
            this->ui->lbl_charge_data_resistance_sol_edit->hide();
            this->ui->lbl_charge_data_group_conduit_edit->hide();
            this->ui->field_charge_data_meth_ref_edit->show();
            this->ui->field_charge_data_temperatur_edit->show();
            this->ui->field_charge_data_group_cable_edit->show();
            this->ui->field_charge_data_resistance_sol_edit->hide();
            this->ui->field_charge_data_group_conduit_edit->hide();
            break;
        }
        case 3: // goto mode_de_pose_61;
        {
            lbl_charge_data_meth_ref_str += " (D)";
            lbl_charge_data_temperatur_str += " (BF2)";
            lbl_charge_data_group_cable_str += " (BK3)";
            lbl_charge_data_resistance_sol_str += " (BL)";
            lbl_charge_data_group_conduit_str += " (BK2)";
            this->ui->lbl_charge_data_neutre->show();
            this->ui->lbl_charge_data_temperatur_edit->show();
            this->ui->lbl_charge_data_group_cable_edit->show();
            this->ui->lbl_charge_data_resistance_sol_edit->show();
            this->ui->lbl_charge_data_group_conduit_edit->show();
            this->ui->field_charge_data_meth_ref_edit->show();
            this->ui->field_charge_data_temperatur_edit->show();
            this->ui->field_charge_data_group_cable_edit->show();
            this->ui->field_charge_data_resistance_sol_edit->show();
            this->ui->field_charge_data_group_conduit_edit->show();
            break;
        }
        case 4: // goto mode_de_pose_62;
        {
            lbl_charge_data_meth_ref_str += " (D)";
            lbl_charge_data_temperatur_str += " (BF2)";
            lbl_charge_data_group_cable_str += " (BK1)";
            lbl_charge_data_resistance_sol_str += " (BL)";
            this->ui->lbl_charge_data_neutre->show();
            this->ui->lbl_charge_data_temperatur_edit->show();
            this->ui->lbl_charge_data_group_cable_edit->show();
            this->ui->lbl_charge_data_resistance_sol_edit->show();
            this->ui->lbl_charge_data_group_conduit_edit->hide();
            this->ui->field_charge_data_meth_ref_edit->show();
            this->ui->field_charge_data_temperatur_edit->show();
            this->ui->field_charge_data_group_cable_edit->show();
            this->ui->field_charge_data_resistance_sol_edit->show();
            this->ui->field_charge_data_group_conduit_edit->hide();
            break;
        }
        default: // goto mode_de_pose_none;
        {
            this->ui->lbl_charge_data_neutre->hide();
            this->ui->lbl_charge_data_temperatur_edit->hide();
            this->ui->lbl_charge_data_group_cable_edit->hide();
            this->ui->lbl_charge_data_resistance_sol_edit->hide();
            this->ui->lbl_charge_data_group_conduit_edit->hide();
            this->ui->field_charge_data_meth_ref_edit->hide();
            this->ui->field_charge_data_temperatur_edit->hide();
            this->ui->field_charge_data_group_cable_edit->hide();
            this->ui->field_charge_data_resistance_sol_edit->hide();
            this->ui->field_charge_data_group_conduit_edit->hide();
            break;
        }
    }
    this->ui->lbl_charge_data_meth_ref_edit->setText(lbl_charge_data_meth_ref_str.c_str());
    this->ui->lbl_charge_data_temperatur_edit->setText(lbl_charge_data_temperatur_str.c_str());
    this->ui->lbl_charge_data_group_cable_edit->setText(lbl_charge_data_group_cable_str.c_str());
    this->ui->lbl_charge_data_resistance_sol_edit->setText(lbl_charge_data_resistance_sol_str.c_str());
    this->ui->lbl_charge_data_group_conduit_edit->setText(lbl_charge_data_group_conduit_str.c_str());
    this->update_current();
    this->update_current_assigned();
}

void Dashboard::on_btn_download_clicked() {
    Configuration configuration = Configuration();
    QString file_path = configuration.dir_data().filePath(QString(FILE_NORM));
    QFile::copy(":/pdf/NFC-15-105.pdf", file_path);
    QString filter = QString("*.pdf");
    QDir desktop (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    QString copy_path = QFileDialog::getSaveFileName(0, tr("Save the file location"), desktop.filePath("NFC-15-105.pdf"), filter, &filter);
    if (!copy_path.isEmpty()) {
        if (copy_path.endsWith(".pdf")) {
            QFile(copy_path).remove();
            QFile(file_path).copy(copy_path);
        } else {
            QFile(copy_path + ".pdf").remove();
            QFile(file_path).copy(copy_path + ".pdf");
        }
    }
}

void Dashboard::on_field_charge_data_protection_currentIndexChanged(int index) {
    switch (index) {
    case 0:
        this->ui->cbx_charge_data_current_assigned->setChecked(false);
        this->ui->cbx_charge_data_current_assigned->setEnabled(false);
        break;
    default:
        this->ui->cbx_charge_data_current_assigned->setChecked(true);
        this->ui->cbx_charge_data_current_assigned->setEnabled(true);
        break;
    }
    this->update_current_assigned();
}

void Dashboard::on_cbx_charge_data_current_assigned_stateChanged(int state) {
    if (state) {
        this->ui->field_charge_data_current_assigned_edit->setEnabled(false);
        float __current = this->ui->field_charge_data_current_edit->text().toFloat();
        int __dispositif = this->ui->field_charge_data_protection->currentIndex();
        float __current_assigned = this->_data_calculator.current_assigned(__dispositif, __current);
        this->ui->field_charge_data_current_assigned_edit->setText(QString::number(__current_assigned));
    } else {
        this->ui->field_charge_data_current_assigned_edit->setEnabled(true);
        float __current_assigned = this->selected_block_record->value("current_assigned").toFloat();
        this->ui->field_charge_data_current_assigned_edit->setText(QString::number(__current_assigned));
    }
}

void Dashboard::update_current() {
    current_data_t __current_data;
    __current_data.puissance_nominale = this->ui->field_charge_data_puissance_nominale_value->value();
    __current_data.facteur_puissance = this->ui->field_charge_data_facteur_puissance->value();
    __current_data.rendement = this->ui->field_charge_data_rendement->value();
    __current_data.facteur_utilisation = this->ui->field_charge_data_facteur_utilisation->value();
    __current_data.facteur_simultaneite = this->ui->field_charge_data_facteur_simultaneite->value();
    __current_data.facteur_extension = this->ui->field_installation_data_facteur_extension->text().toFloat();
    __current_data.facteur_convertion = this->_data_wrapper.wrappe("facteur_conversion", QString::number(this->ui->field_charge_data_couplage_edit->currentIndex()));
    float __current = this->_data_calculator.current(__current_data);
    this->ui->field_charge_data_current_edit->setText(QString::number(__current));
}

void Dashboard::update_current_assigned(bool alert) {
    int __protection = this->ui->field_charge_data_protection->currentIndex();
    int __state = this->ui->cbx_charge_data_current_assigned->isChecked();
    float __current = this->ui->field_charge_data_current_edit->text().toFloat();
    float __current_assigned = 0.00;
    if (__protection) {
        if (__state) {
            __current_assigned = this->_data_calculator.current_assigned(__protection, __current);
            if (__current_assigned == 0) {
                this->ui->cbx_charge_data_current_assigned->setChecked(false);
            }
        }
        switch (__protection) {
        case 1:
                if (__current_assigned == 0)
                    if (alert)
                        QMessageBox::warning(this, tr("Impossible d'utiliser Disjonteur domestique"),
                                         tr("Les Disjonteur domestique ne peut être utilisés avec cette valeur du courant d'emploi ! \n"
                                            " Veuillez d'utiliser un autre dispositf de protection !"));
            break;
        case 2:
                if (__current_assigned == 0)
                    if (alert)
                        QMessageBox::warning(this, tr("Impossible d'utiliser Fusible gG"),
                                         tr("Les fusible gG ne peut être utilisé avec cette valeur du courant d'emploi ! \n"
                                            " Veuillez d'utiliser un autre dispositf de protection !"));
            break;
        default:
            break;
        }
        this->ui->field_charge_data_current_assigned_edit->setText(QString::number(__current_assigned));
    }
}

void Dashboard::on_field_charge_data_couplage_edit_currentIndexChanged(int) {
    this->update_current();
}

void Dashboard::on_field_charge_data_facteur_puissance_valueChanged(double) {
    this->update_current();
}

void Dashboard::on_field_charge_data_rendement_valueChanged(double) {
    this->update_current();
}

void Dashboard::on_field_charge_data_facteur_utilisation_valueChanged(double) {
    this->update_current();
}

void Dashboard::on_field_charge_data_facteur_simultaneite_valueChanged(double) {
    this->update_current();
}

void Dashboard::on_field_charge_data_current_edit_textChanged(const QString &) {
    this->update_current_assigned();
}
