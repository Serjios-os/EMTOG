#ifndef ELECMTUGEN_H
#define ELECMTUGEN_H

#include "dashboard.h"
#include <QMainWindow>

#include <configuration.h>


QT_BEGIN_NAMESPACE
namespace Ui { class ElecMtuGen; }
QT_END_NAMESPACE

class ElecMtuGen : public QMainWindow
{
    Q_OBJECT

public:
    ElecMtuGen(QWidget * = nullptr);
    ~ElecMtuGen();
    static ElecMtuGen * instance(QWidget * = nullptr);
    static void terminate(int sig);
private slots:
    void cleanup();
private:
    static ElecMtuGen * _instance;
    Ui::ElecMtuGen *ui;
    Dashboard * _dashboard;
    void resizeEvent(QResizeEvent *);
    void clean();
    void icon(QString);
};
#endif // ELECMTUGEN_H
