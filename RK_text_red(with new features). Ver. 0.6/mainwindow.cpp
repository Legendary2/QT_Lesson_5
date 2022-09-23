#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpform.h"
#include "fileview.h"

#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDataStream>
#include <QTranslator>
#include <QLibraryInfo>
#include <QEvent>
#include <QClipboard>
#include <QShortcut>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    openedFileName("")
{
    ui->setupUi(this);
    Init();

    this->aDirList = new QFileInfoList();

    this->model = new QListViewExplorerModel();

    this->currentFolder = QDir(QDir::currentPath());

    this->model->getFolderList(this->currentFolder.absolutePath(),this->aDirList);


    this->ui->listView->setModel(model);

    this->ui->lineEdit->setText(QDir::currentPath());

    this->ui->listView->setWordWrap(true);
}

MainWindow::~MainWindow(){

    delete ui;
}

void MainWindow::Init(){

    ui->textEdit->setReadOnly(true);
    ui->textEdit->setDisabled(true);

    QAction *exitAction = new QAction(tr("Exit"));
    exitAction->setShortcut(tr("CTRL+Q"));
    connect(exitAction, SIGNAL(triggered()),this, SLOT(close()));

}

void MainWindow::changeTranslator(QString postfix)
{
    QApplication::removeTranslator(translator);
    translator = new QTranslator(this);
    translator->load(QApplication::applicationName() + "_" + postfix);
    QApplication::installTranslator(translator);
}

void MainWindow::changeEvent(QEvent *event)
{
    setWindowTitle(tr("Editor"));

    if(event->type() == QEvent::LanguageChange){
        ui->menu->setTitle(tr("Editor"));
        ui->actionEnglish->setText(tr("English"));
        ui->new_pushButton->setText(tr("New"));
        ui->open_pushButton->setText(tr("Open"));
        ui->readOnly_pushButton_2->setText(tr("Read only"));
        ui->close_pushButton->setText(tr("Close"));
        ui->save_pushButton->setText(tr("Save"));
        ui->saveas_pushButton->setText(tr("Save as"));
        ui->pushButton->setText(tr("Exit"));
        ui->help_pushButton->setText(tr("Help"));
        ui->actionRussian->setText(tr("Russian"));

    }else
    {
        QMainWindow::changeEvent(event);
    }
}

void MainWindow::ExtendedFileSave(){

    if(!ui->textEdit->toPlainText().isEmpty())
    {
        QMessageBox::StandardButton reply;
        QString type = openedFileName.isEmpty() ? " as" : "";
        reply = QMessageBox::question(this, "Save file" + type, "Do you want to save file" + type + "?", QMessageBox::Yes | QMessageBox::No);
        if (QMessageBox::Yes == reply)
        {
            if (!openedFileName.isEmpty())
            {
                FileSave();
            }
            else
            {
                FileSaveAs();
            }
        }
    }
}

void MainWindow::FileSave(){

    QString s = QFileDialog::getSaveFileName(this, "Save File", openedFileName, filter);
    if (s.length() > 0)
    {
        QString ext = QString(&(s.data()[s.length() - 4]));
        if (ext == ".txt")
        {
            QFile file(s);
            if (file.open(QFile::WriteOnly))
            {
                QTextStream stream(&file);
                stream << ui->textEdit->toPlainText();
                file.close();
                openedFileName = s;
            }
        }
    }
}

void MainWindow::FileSaveAs(){

    QString s = QFileDialog::getSaveFileName(this, "Save File As", QDir::current().path(), filter);
    if (s.length() > 0)
    {
        QString ext = QString(&(s.data()[s.length() - 4]));
        if (ext == ".txt")
        {
            QFile file(s);
            if (file.open(QFile::WriteOnly))
            {
                QTextStream stream(&file);
                stream << ui->textEdit->toPlainText();
                file.close();
                openedFileName = s;
            }
        }
    }
}

void MainWindow::FileOpen(){

    QString s = QFileDialog::getOpenFileName(this, "Open File", QDir::current().path(), filter);
    if (s.length() > 0)
    {
        int index = s.indexOf(".txt");
        QFile file(s);
        if (file.open(QFile::ReadOnly | QFile::ExistingOnly))
        {
            if (index != -1 && s.length() - 4 == index)
            {
                QTextStream stream(&file);
                ui->textEdit->setText(stream.readAll());
                openedFileName = s;
                file.close();
            }
        }
    }
}

void MainWindow::on_new_pushButton_pressed(){

    ExtendedFileSave();
    ui->textEdit->setReadOnly(false);
    ui->textEdit->setDisabled(false);
    ui->textEdit->clear();
    openedFileName = "";
}

void MainWindow::on_close_pushButton_pressed(){

    ExtendedFileSave();
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setDisabled(true);
    ui->textEdit->clear();
    openedFileName = "";

}

void MainWindow::on_open_pushButton_pressed(){

    ExtendedFileSave();
    FileOpen();

    ui->textEdit->setReadOnly(false);
    ui->textEdit->setDisabled(false);
}

void MainWindow::on_save_pushButton_pressed(){

    FileSave();

}

void MainWindow::on_saveas_pushButton_pressed(){
    FileSaveAs();

}

void MainWindow::on_help_pushButton_pressed(){
    HelpForm* help = new HelpForm(this);
    if(help){

        help->show();
    }
}

void MainWindow::on_pushButton_clicked(){
    close();

}

void MainWindow::on_readOnly_pushButton_2_clicked(bool checked)
{
    QString s = QFileDialog::getOpenFileName(this, "Open File", QDir::current().path(), filter);
    QFile file(":/.txt");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Внимание!", "Режим только для чтения! а значит - файл нельзя изменять!");
    }
    if (s.length() > 0)
    {
        int index = s.indexOf(".txt");
        QFile file(s);
        if (file.open(QFile::ReadOnly))
        {
            if (index != -1 && s.length() - 4 == index)
            {
                QTextStream stream(&file);
                ui->textEdit->setText(stream.readAll());
                openedFileName = s;
                file.close();
            }
        }
    }
}

void MainWindow::on_actionEnglish_triggered()
{
    changeTranslator("en");
}

void MainWindow::on_actionRussian_triggered()
{
    changeTranslator("ru");
}

void MainWindow::on_on_lightPushButton_clicked(bool checked)
{
     qApp->setPalette(style()->standardPalette());
}

void MainWindow::on_on_darkPushButton_clicked(bool checked)
{
    QPalette darkPalette;

        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        qApp->setPalette(darkPalette);
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    if (this->aDirList->at(index.row()).isDir())
    {
        QString tmp = this->aDirList->at(index.row()).absoluteFilePath();
        model->getFolderList(this->aDirList->at(index.row()).absoluteFilePath(),this->aDirList);
        this->ui->lineEdit->setText(tmp);
    } else {
        QDesktopServices::openUrl(QUrl(this->aDirList->at(index.row()).absoluteFilePath()));
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    currentFolder.cdUp();

    this->ui->lineEdit->setText(currentFolder.absolutePath());
    model->getFolderList(currentFolder.absolutePath(),this->aDirList);
}


void MainWindow::on_pushButton_3_clicked()
{
    this->ui->listView->setUniformItemSizes(false);
    this->ui->listView->setViewMode(QListView::ListMode);
}

void MainWindow::on_pushButton_4_clicked()
{
    this->ui->listView->setUniformItemSizes(true);
    this->ui->listView->setViewMode(QListView::IconMode);
}
