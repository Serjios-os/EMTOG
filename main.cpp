#include "elecmtugen.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ElecMtuGen * w = ElecMtuGen::instance();
    QObject::connect(&a, SIGNAL(destroyed()), w, SLOT(cleanup()));
    w->show();
    return a.exec();
}
