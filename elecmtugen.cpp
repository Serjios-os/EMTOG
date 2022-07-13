#include "elecmtugen.h"
#include "ui_elecmtugen.h"
#include "dashboard.h"


ElecMtuGen * ElecMtuGen::_instance = nullptr;

ElecMtuGen::ElecMtuGen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ElecMtuGen)
    , _dashboard(nullptr)
{
    signal(SIGINT, ElecMtuGen::terminate);
    signal(SIGTERM, ElecMtuGen::terminate);
    ui->setupUi(this);
    this->icon(APP_ICON);
    this->showMaximized();
    this->_dashboard = new Dashboard(this);
    this->_dashboard->set_ui();
    this->setCentralWidget(this->_dashboard);
    this->_dashboard->resize();
    this->_dashboard->refresh();
}

ElecMtuGen::~ElecMtuGen() {
    delete ui;
}

ElecMtuGen * ElecMtuGen::instance(QWidget *parent) {
    if (ElecMtuGen::_instance == nullptr) {
        ElecMtuGen::_instance = new ElecMtuGen(parent);
    }
    return ElecMtuGen::_instance;
}

void ElecMtuGen::terminate(int sig) {
    qDebug() << " > terminate";
    if (sig == SIGINT)
    {
        qDebug() << " > SIGINT : " << SIGINT;
        ElecMtuGen::instance()->clean();
        qApp->quit();
    }
    else if (sig == SIGTERM)
    {
        qDebug() << " > SIGTERM : " << SIGTERM;
        ElecMtuGen::instance()->clean();
        qApp->quit();
    }
}

void ElecMtuGen::cleanup() {
    qDebug() << " > cleanup";
    this->clean();
}

void ElecMtuGen::clean() {
    qDebug() << " > clean";
}

void ElecMtuGen::icon(QString file) {
    Configuration configuration = Configuration();
    QString icon_path = configuration.dir_data().filePath(QString(file));
    QFile::copy(":/icons/icon.png", icon_path);
    QIcon icon = QIcon(icon_path);
    this->setWindowIcon(icon);
}

void ElecMtuGen::resizeEvent(QResizeEvent *) {
    if (this->_dashboard != nullptr) {
        this->_dashboard->resize();
    }
}
