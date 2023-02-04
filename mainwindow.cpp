#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog.h"
#include "fileslisttablemodel.h"
#include "EraseMethod.h"
#include "secureerase.h"
#include "QDirIterator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    ui->progressBar->hide();
    for(int method = EraseMethodFirst; method <= EraseMethodLast; method++)
        ui->comboMethod->addItem(CEraseMethodBase::GetMethodName((EraseMethod)method), method);

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

    if(!dir.isEmpty())
    {
        // add new row
        FilesTableModel->AddItem(dir, true);
    }
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
    bool bNeedStart = !m_bInProgress;
    UpdateControls(bNeedStart);

    m_bCancel = m_bInProgress;
    if(bNeedStart)
        DoEraseFiles();
}

void MainWindow::UpdateControls(bool bStarted)
{
    ui->buttonStart->setText(bStarted? "Stop" : "Start");
    QPixmap icon(bStarted? ":/img/stop.ico" : ":/img/start.ico");
    ui->buttonStart->setIcon(icon);
    if(bStarted)
        ui->progressBar->show();
    else
        ui->progressBar->hide();

    for(int i = 0; i < ui->toolBar->actions().count(); i++)
        ui->toolBar->actions().at(i)->setEnabled(!bStarted);
}

void MainWindow::DoEraseFiles()
{
    m_bInProgress = true;
    m_TotalSize = m_ProcessedSize = 0;
    ui->progressBar->setValue(0);

    // get method according to the combobox selection
    EraseMethod method = static_cast<EraseMethod>(ui->comboMethod->currentData().toInt());
    CEraseMethodBase* pMethod = CEraseMethodBase::GetMethod(method);

    // calculate total size
    QList<FileRemoveInfo> files;
    FilesTableModel->GetFilesToRemove(files);
    for(auto& file : files)
        m_TotalSize += file.Size;

    // process files from the table model
    for(int i = 0; i < FilesTableModel->GetItemsCount(); i++)
    {
        if(m_bCancel)
            break;

        FileRemoveInfo& fileInfo = FilesTableModel->GetItem(i);
        if(fileInfo.Status == FileRemoveInfo::Finished && 0 == fileInfo.LastError)
            continue;

        FilesTableModel->UpdateStatus(i, FileRemoveInfo::Started);
        DoEraseFile(fileInfo, pMethod);
        FilesTableModel->UpdateStatus(i, FileRemoveInfo::Finished);
        qApp->processEvents();

    }

    delete pMethod;

    // finish and update controls
    m_bInProgress = false;
    UpdateControls(false);
}

void MainWindow::DoEraseFile(FileRemoveInfo& file, CEraseMethodBase* pMethod)
{
    if(file.IsDirectory)
    {
        // process folder
        QDirIterator it(file.Path, QDir::AllEntries |QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QFileInfo childFile(it.next());
            if(childFile.isDir())
                continue;
            quint64 size = childFile.size();
            int errorCode = SecureEraseFile(childFile.filePath(), pMethod, m_bCancel);
            if(0 != errorCode)
                file.LastError = errorCode;
            UpdateProgress(size);
        }

        // remove empty directory
        QDir dir(file.Path);
        if(dir.isEmpty())
            dir.removeRecursively();
    }
    else
    {
        // process single file
        file.LastError = SecureEraseFile(file.Path, pMethod, m_bCancel);
        UpdateProgress(file.Size);
    }
}

void MainWindow::UpdateProgress(quint64 addSize)
{
    m_ProcessedSize += addSize;
    if(m_TotalSize)
        ui->progressBar->setValue(100*m_ProcessedSize/m_TotalSize);
}
