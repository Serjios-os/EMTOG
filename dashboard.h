#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "projectmanager.h"
#include "blockmanager.h"
#include <datavalidator.h>
#include <datawrapper.h>
#include <datacalculator.h>
#include <equipmentsmanager.h>
#include <configuration.h>



namespace Ui {
class Dashboard;
}

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget * = nullptr);
    ~Dashboard();
    static Dashboard * instance(QWidget * = nullptr);
    void set_ui();
    void refresh();
    void resize();

private slots:
    void on_btn_project_search_clicked();
    void on_btn_project_save_clicked();
    void on_list_blocks_doubleClicked(const QModelIndex &);
    void on_list_projects_doubleClicked(const QModelIndex &);
    void on_btn_project_delete_clicked();
    void on_btn_project_create_clicked();
    void on_btn_project_block_add_clicked();
    void on_btn_block_delete_clicked();
    void on_btn_charge_data_apply_clicked();
    void on_btn_project_generate_clicked();
    void on_field_installation_data_name_search_textEdited(const QString &);
    void on_btn_database_toggled(bool checked);
    void on_btn_database_backup_clicked(QAbstractButton *);
    void on_field_installation_data_name_new_returnPressed();
    void on_cbx_installation_data_facteur_extension_stateChanged(int);
    void on_cbx_charge_data_facteur_puissance_stateChanged(int);
    void on_cbx_charge_data_rendement_stateChanged(int);
    void on_cbx_charge_data_facteur_utilisation_stateChanged(int);
    void on_cbx_charge_data_facteur_simultaneite_stateChanged(int);
    void on_field_charge_data_puissance_nominale_edit_textChanged(const QString &);
    void on_field_charge_data_mode_pose_edit_currentIndexChanged(int);
    void on_btn_download_clicked();
    void on_field_charge_data_protection_currentIndexChanged(int);
    void on_cbx_charge_data_current_assigned_stateChanged(int);
    void on_field_charge_data_couplage_edit_currentIndexChanged(int);
    void on_field_charge_data_facteur_puissance_valueChanged(double);
    void on_field_charge_data_rendement_valueChanged(double);
    void on_field_charge_data_facteur_utilisation_valueChanged(double);
    void on_field_charge_data_facteur_simultaneite_valueChanged(double);
    void on_field_charge_data_current_edit_textChanged(const QString &);

private:
    Ui::Dashboard *ui;
    static Dashboard * sp_dashboard;
    ProjectManager * pmgr;
    BlockManager * bmgr;
    QItemSelectionModel * projects_model ;
    QList<project_wrapper_t> project_db_wrapper;
    QSqlQueryModel * projects_db_model;
    QSqlQueryModel * blocks_db_model;
    QSqlRecord * selected_project_record;
    QSqlRecord * selected_block_record;
    DataValidator _data_validator;
    DataWrapper _data_wrapper;
    DataCalculator _data_calculator;
    EquipmentsManager _equipments_manager;
    bool edit;
    void refresh_project_list();
    void refresh_blocks_list();
    bool fill_project_fields(const QSqlRecord);
    bool fill_block_fields(const QSqlRecord);
    bool clear_block_fields();
    void create(QString);
    void update_current();
    void update_current_assigned(bool = false);
};

#endif // DASHBOARD_H
