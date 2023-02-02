#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog.h"
#include "fileslisttablemodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    ui->comboMethod->addItem("Overwrite with random numbers", 1);
    ui->comboMethod->addItem("Overwrite 3-times (DoD method)", 3);
    ui->comboMethod->addItem("Overwrite 6-times (DoD II method)", 6);
    ui->comboMethod->addItem("Overwrite 7-times (VSITR method)", 7);
    ui->comboMethod->addItem("Overwrite 35-times (Peter Gutmann method)", 35);

    FilesTableModel = new FilesListTableModel;
    ui->filesTable->setModel(FilesTableModel);
    ui->filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->filesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->filesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
}

MainWindow::~MainWindow()
{
    delete FilesTableModel;
    delete ui;
}


void MainWindow::on_actionAddFile_triggered()
{
    // show multiple files selection dialog
    QStringList files = QFileDialog::getOpenFileNames(this, "Select files to delete");

    // add rows to table
    for(QString& str : files)
        FilesTableModel->AddItem(str, false);
}


void MainWindow::on_actionAddFolder_triggered()
{
    // show folder selection dialog
    QString dir = QFileDialog::getExistingDirectory(this, "Select folder to delete");

    // add new row
    FilesTableModel->AddItem(dir, true);
}

void MainWindow::on_actionRemove_triggered()
{
    // remove selected rows (from end to begin of selection)
    QModelIndexList indexes =  ui->filesTable->selectionModel()->selectedRows();
    for(auto iter = indexes.rbegin(); iter != indexes.rend(); iter++)
        FilesTableModel->removeRows(iter->row(), 1);
}

void MainWindow::on_buttonStart_clicked()
{
    m_bInProgress = !m_bInProgress;
    ui->buttonStart->setText(m_bInProgress? "Stop" : "Start");
    QPixmap icon(m_bInProgress? ":/img/stop-32.png" : ":/img/start-32.png");
    ui->buttonStart->setIcon(icon);
}

