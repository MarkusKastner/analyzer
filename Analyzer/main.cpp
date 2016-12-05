#include "mainwindow\MainWindow.h"
#include <QtWidgets/QApplication>

#include <memory>

#include "application\IOActionsImpl.h"
#include "AnalyzerLib\base\AnalyzerBase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::unique_ptr<analyzer::app::IOActions> ioActions(new analyzer::app::IOActionsImpl());
    std::unique_ptr<analyzer::base::AnalyzerBase> analyzerBase(new analyzer::base::AnalyzerBase());
    analyzer::gui::MainWindow w(ioActions.get(), *analyzerBase.get());
    w.show();
    return a.exec();
}
