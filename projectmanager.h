#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <databasemanager.h>


typedef struct project_wrapper_s {
    int list_id;
    int db_id;
} project_wrapper_t;

class ProjectManager
{
public:
    ProjectManager();
    ~ProjectManager();
    static ProjectManager * instance();
    bool isFirestUse();
    QStringList get_projects();
    bool create_project(QList <model_elm_t *>);
    QSqlRecord create_project(QString);
    QSqlRecord edit_project(QSqlRecord);
    bool delete_project(QSqlRecord);
    int get_projects(QSqlQueryModel *);
    int search_project(QSqlQueryModel *, QString);
    QSqlQueryModel * model(QSqlQueryModel * = nullptr);
    DatabaseManager * database() { return this->dbm ; };
private:
    static ProjectManager * sp_instance;
    DatabaseManager * dbm;
    QSqlQueryModel * mp_model;
};

#endif // PROJECTMANAGER_H
