#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include <databasemanager.h>


class BlockManager
{
public:
    BlockManager();
    ~BlockManager();
    static BlockManager * instance();
    int project_id(int = 0);
    QStringList get_blocks();
    int get_blocks(QSqlQueryModel *);
    bool create_project(QList <model_elm_t *>);
    QSqlRecord create_project(QString, QString);
    QSqlRecord edit_block(QSqlRecord);
    bool delete_block(QSqlRecord);
    bool delete_blocks(int);
    int get_projects(QSqlQueryModel *);
    int search_block(QSqlQueryModel *, QString);
    QSqlQueryModel * model(QSqlQueryModel * = nullptr);
    int get_blocks(QSqlQuery *);
private:
    static BlockManager * sp_instance;
    DatabaseManager * dbm;
    QSqlQueryModel * mp_model;
    int m_projec_id;
};

#endif // BLOCKMANAGER_H
