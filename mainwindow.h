#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class FilesListTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAddFile_triggered();
    void on_actionRemove_triggered();
    void on_actionAddFolder_triggered();

    void on_buttonStart_clicked();

private:
    Ui::MainWindow *ui;
    bool m_bInProgress = false;
    FilesListTableModel* FilesTableModel;
};
#endif // MAINWINDOW_H
