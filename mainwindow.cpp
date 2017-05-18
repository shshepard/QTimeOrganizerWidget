#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    checkBoxShowSettings_->setChecked(true);
    ui->statusBar->addPermanentWidget(checkBoxShowSettings_);
    checkBoxShowSettings_->setText(tr("Show widget settings"));

    //connections

    connect(ui->timeOrganaizerWidget,&QTimeOrganizerWidget::rangeChanged,
            this,&MainWindow::timeChanged);

    connect(this,&MainWindow::addBusyRange,
            ui->timeOrganaizerWidget,&QTimeOrganizerWidget::addBusyRange);

    connect(checkBoxShowSettings_,&QCheckBox::clicked,
            this,&MainWindow::onCheckBoxShowSettingsClicked);


    //init

    ui->dateTimeEditRangeBegin->setDate(QDate::currentDate());
    ui->dateTimeEditRangeBegin->setTime(QTime(0,0));

    ui->dateTimeEditRangeEnd->setDate(QDate::currentDate());
    ui->dateTimeEditRangeEnd->setTime(QTime(5,0));

    ui->dateTimeEditBusyRangeBegin->setDate(QDate::currentDate());
    ui->dateTimeEditBusyRangeBegin->setTime(QTime(0,0));

    ui->dateTimeEditBusyRangeEnd->setDate(QDate::currentDate());
    ui->dateTimeEditBusyRangeEnd->setTime(QTime(6,0));

    ui->dateEdit->setDate(QDate::currentDate());

    QTimeOrganizerWidget* wgt = ui->timeOrganaizerWidget;

    ui->spinBoxMinItemHeight->setValue(wgt->minimumHeight());
    ui->spinBoxHours->setValue(wgt->hoursDisplayed());
    ui->spinBoxMinItemHeight->setValue(wgt->minItemHeight());
    ui->checkBoxTracking->setChecked(wgt->trackingEnabled());
    ui->checkBoxGrid->setChecked(wgt->gridEnabled());
    ui->checkBoxDrawGrid->setChecked(wgt->drawingGridEnabled());
    ui->checkBoxShowHint->setChecked(wgt->isHintsVisible());

    QTimeOrganizerWidget::GridSeparation sep = wgt->gridSeparation();

    if(sep == QTimeOrganizerWidget::Hour)
        ui->comboBoxGridSep->setCurrentIndex(0);
    else if(sep == QTimeOrganizerWidget::Minute30)
        ui->comboBoxGridSep->setCurrentIndex(1);
    else if(sep == QTimeOrganizerWidget::Minute15)
        ui->comboBoxGridSep->setCurrentIndex(2);
    else if(sep == QTimeOrganizerWidget::Minute10)
        ui->comboBoxGridSep->setCurrentIndex(3);
    else if(sep == QTimeOrganizerWidget::Minute5)
        ui->comboBoxGridSep->setCurrentIndex(4);
    else if(sep == QTimeOrganizerWidget::Minute1)
        ui->comboBoxGridSep->setCurrentIndex(5);
    else if(sep == QTimeOrganizerWidget::Second30)
        ui->comboBoxGridSep->setCurrentIndex(6);

    QTimeOrganizerWidget::ScrollingMode scrMode = wgt->scrollingMode();

    if(scrMode == QTimeOrganizerWidget::ScrollHours)
        ui->comboBoxScrollMode->setCurrentIndex(0);
    else if(scrMode == QTimeOrganizerWidget::OngoingScroll)
        ui->comboBoxScrollMode->setCurrentIndex(1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//METHODS///////////////////////////////////////////////////////////////////////

//SLOTS/////////////////////////////////////////////////////////////////////////

void MainWindow::onCheckBoxShowSettingsClicked(bool checked)
{
    ui->groupBoxSettings->setVisible(checked);
    if(checked)
        ui->statusBar->showMessage("");
}

void MainWindow::timeChanged(const QDateTime& begin, const QDateTime& end)
{
    QString rangeText =  begin.date().toString("dd.MM.yy (") +
                         begin.time().toString("hh:mm:ss)") +
                         " - " +
                         end.date().toString("dd.MM.yy (") +
                         end.time().toString("hh:mm:ss)");

    if(ui->groupBoxSettings->isVisible()) {
        ui->lineEditSelectedRange->setText(rangeText);
    } else {
        ui->statusBar->showMessage(rangeText);
    }

}

void MainWindow::on_checkBoxGrid_clicked()
{
    ui->timeOrganaizerWidget->setGridEnabled(ui->checkBoxGrid->isChecked());

}

void MainWindow::on_spinBoxMinItemHeight_valueChanged(int arg1)
{
    ui->timeOrganaizerWidget->setMinItemHeight(arg1);
}

void MainWindow::on_comboBoxGridSep_currentIndexChanged(int index)
{
    if(index == 0)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Hour);
    else if (index == 1)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Minute30);
    else if (index == 2)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Minute15);
    else if (index == 3)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Minute10);
    else if (index == 4)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Minute5);
    else if (index == 5)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Minute1);
    else if (index == 6)
        ui->timeOrganaizerWidget->setGrid(QTimeOrganizerWidget::Second30);
}

void MainWindow::on_checkBoxTracking_clicked()
{
    ui->timeOrganaizerWidget->setTracking(ui->checkBoxTracking->isChecked());
}

void MainWindow::on_checkBoxDrawGrid_clicked()
{
    ui->timeOrganaizerWidget->setDrawingGrid(ui->checkBoxDrawGrid->isChecked());
}

void MainWindow::on_spinBoxHours_valueChanged(int arg1)
{
    ui->timeOrganaizerWidget->setHoursDisplayed(arg1);
}

void MainWindow::on_pushButtonSetRange_clicked()
{
    ui->timeOrganaizerWidget->setSelectedRange(ui->dateTimeEditRangeBegin->dateTime(),
                                       ui->dateTimeEditRangeEnd->dateTime());
}

void MainWindow::on_pushButtonSetDate_clicked()
{
    ui->timeOrganaizerWidget->setCurrentDate(ui->dateEdit->date());
}

void MainWindow::on_pushButtonClearSelection_clicked()
{
    ui->timeOrganaizerWidget->clearSelection();
}

void MainWindow::on_pushButtonShowHeader_clicked()
{
    ui->timeOrganaizerWidget->setHeaderVisible(true);
}

void MainWindow::on_pushButtonHideHeader_clicked()
{
    ui->timeOrganaizerWidget->setHeaderVisible(false);
}

void MainWindow::on_checkBoxShowHint_clicked()
{
    ui->timeOrganaizerWidget->setHintsVisible(ui->checkBoxShowHint->isChecked());
}

void MainWindow::on_comboBoxScrollMode_currentIndexChanged(int index)
{
    if(index == 0)
        ui->timeOrganaizerWidget->setScrolingMode(QTimeOrganizerWidget::ScrollHours);
    else if(index == 1)
        ui->timeOrganaizerWidget->setScrolingMode(QTimeOrganizerWidget::OngoingScroll);
}

void MainWindow::on_pushButtonSetHour_clicked()
{
    ui->timeOrganaizerWidget->setCurrentHour(ui->spinBoxHour->value());
}

void MainWindow::on_pushButtonSetBusyRange_clicked()
{
    emit addBusyRange(ui->dateTimeEditBusyRangeBegin->dateTime(),
                      ui->dateTimeEditBusyRangeEnd->dateTime(),
                      ui->lineEditBusyText->text());
}

void MainWindow::on_pushButtonClearBusyRange_clicked()
{
    ui->timeOrganaizerWidget->clearBusyRange();
}
