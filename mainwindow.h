#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <gmpxx.h>
#include <map>
#include <vector>
#include <string>
#include <atomic>

typedef mpz_class BigInt;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void progressUpdated(int value);
    void statusLog(QString msg);
    void calculationFinished();

private slots:
    void on_runButton_clicked();
    void on_stopButton_clicked();
    void updateProgressBar(int value);
    void handleLog(QString msg);
    void onRunComplete();
    void on_actionSave_Session_triggered();
    void on_actionExport_Session_triggered();
    void on_actionLoad_Session_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    std::map<std::string, std::vector<BigInt>> sets;
    int next_set = 23;
    std::atomic<bool> stop_requested;
    void initialize_base_sets();
    bool is_prime(const BigInt& n);
    void process_pair(std::string i, std::string j, int limit);
    void runAutoMode();
    void save_state();
};

#endif // MAINWINDOW_H
