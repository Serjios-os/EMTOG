#ifndef EQUIPMENTSMANAGER_H
#define EQUIPMENTSMANAGER_H

#include "databasemanager.h"
#include <QDialog>

namespace Ui {
class EquipmentsManager;
}

class EquipmentsManager : public QDialog
{
    Q_OBJECT

public:
    EquipmentsManager(QWidget *parent = nullptr);
    ~EquipmentsManager();

private slots:
    void on_btn_confirmation_accepted();
    void on_btn_confirmation_rejected();
    void on_field_equipment_type_currentIndexChanged(int index);
    void on_list_compenents_doubleClicked(const QModelIndex &index);
    void on_btn_delete_clicked();
    void on_field_equipment_search_textEdited(const QString &arg1);

private:
    Ui::EquipmentsManager *ui;
    DatabaseManager * _database_manager_cables;
    DatabaseManager * _database_manager_fusibles;
    DatabaseManager * _database_manager_disjoncteurs_dom;
    DatabaseManager * _database_manager_disjoncteurs_ind;
    QSqlQueryModel * _database_model;
    QModelIndex * _database_model_index;
    void database_insert(DatabaseManager*, QSqlRecord);
    void database_delete(DatabaseManager*, QSqlRecord);
    void database_update(DatabaseManager*, QSqlRecord);
    void database_fetch(DatabaseManager*, QString = "");
    DatabaseManager * database(int);
};

#endif // EQUIPMENTSMANAGER_H
