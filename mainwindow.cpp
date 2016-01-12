#include "mainwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QListWidget>
#include <QFile>
#include <QString>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QStringListIterator>
#include <QLayout>
#include <QInputDialog>
#include <QMap>
#include <QTableWidget>
#include <QDate>
#include <QHeaderView>
#include <QCalendarWidget>
#include <QDateTimeEdit>
#include <qDebug>
#include <QSpacerItem>

#include "datamanipulation.h"
#include "routine.h"

typedef QString RoutineName;
typedef QString ExerciseName;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    //For sizing the window as it starts up
    QDesktopWidget dw;

    int x=dw.width()*0.5;
    int y=dw.height()*0.5;
    setMinimumSize(x,y);

    dataLib = new DataManipulation();
    dataLib->loadXML();
    routines = dataLib->getRoutineInfo();
    exercises = dataLib->getExercisesInfo();
    if (exercises.isEmpty())
        QMessageBox::warning(this, "Error", "Unable to load document. "
                             "Please reopoen the application.");


    QWidget *main = new QWidget();
    setCentralWidget(main);

    mainLayout = new QHBoxLayout();
    main->setLayout(mainLayout);


    //left-divider stuff-------------------------------
    QWidget *left = new QWidget();
    left->setStyleSheet("background-color:#99CCFF");
    mainLayout->addWidget(left, 0, 0);

    QVBoxLayout *menu = new QVBoxLayout();
    left->setLayout(menu);

    QLabel *workoutTracker = new QLabel ("<font size=\"5\">Workout Tracker</font>");
    workoutTracker->setMargin(workoutTracker->width()*0.05);
    menu->addWidget(workoutTracker);


    //menu buttons-------------------------------------
    QPushButton *chooseRoutine = new QPushButton ("Track");
    QPushButton *library = new QPushButton ("Workout Library");
    QPushButton *progress = new QPushButton ("Progress");
    //QPushButton *exportData = new QPushButton ("Export Data");
    QPushButton *reset = new QPushButton ("Reset");
    menu->addWidget(chooseRoutine);
    menu->addWidget(library);
    menu->addWidget(progress);
    //menu->addWidget(exportData);
    menu->addWidget(reset);


    addRoutineButton = new QPushButton("Add");
    choose = new QPushButton("Next");
    addExerciseToRoutineButton = new QPushButton("Add >>");
    removeExerciseFromRoutineButton = new QPushButton("<< Remove");


    //right-divider stuff------------------------------
    homePage = new QWidget;
    QVBoxLayout *home = new QVBoxLayout;
    homePage->setLayout(home);
    home->QLayout::setContentsMargins(20,20,20,20);

    QLabel *homeLabel = new QLabel ("<font size=\"7\"><b>Welcome to the Workout Tracker!</b></font>");
    homeLabel->setAlignment(Qt::AlignCenter);
    home->addWidget(homeLabel, 1, 0);
    mainLayout->addWidget(homePage, 1, 0);

    //keeps track of which widget needs to be displayed
    pastPage = homePage;
    currentPage = homePage;



    //initializes all the widgets
    createRightLayout();


    connect(chooseRoutine, SIGNAL(clicked()), this, SLOT(chooseRoutine()));
    connect(library, SIGNAL(clicked()), this, SLOT(chooseWorkoutLibrary()));
    connect(addRoutineButton, SIGNAL(clicked()), this, SLOT(showAddRoutinePage()));
    connect(progress, SIGNAL(clicked()), this, SLOT(showProgressPage()));

    connect(addExerciseToRoutineButton, SIGNAL(clicked()), this, SLOT(addExerciseToSelected()));
    connect(allExercisesForAddRoutine, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addExerciseToSelected()));
    connect(listChosenExercises, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeSelectedExercise()));
    connect(removeExerciseFromRoutineButton, SIGNAL(clicked()), this, SLOT(removeSelectedExercise()));
    connect(nameRoutine, SIGNAL(textEdited(QString)), this, SLOT(enableCreateRoutine()));
    connect(createRoutine, SIGNAL(clicked()), this, SLOT(CreateRoutine()));
    connect(removeRoutineButton, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(addExerciseToLibrary, SIGNAL(clicked()), this, SLOT(addExToLib()));
    connect(removeExerciseFromLibrary, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(choose, SIGNAL(clicked()), this, SLOT(addAWorkout()));
    connect(saveTrackData, SIGNAL(clicked()), this, SLOT(retrieveDataFromTable()));
    connect(cancelSaveData,SIGNAL(clicked()), this, SLOT(cancelTrackData()));
    connect(listRoutineProgress, SIGNAL(activated(QString)), this, SLOT(updateProgressDates()));
    connect(listRoutineProgress, SIGNAL(activated(QString)), this, SLOT(updateWorkoutDataTable()));
    connect(progressDates, SIGNAL(activated(QString)), this, SLOT(updateWorkoutDataTable()));
    connect(reset, SIGNAL(clicked()), this, SLOT(resetLibrary()));
}


MainWindow::~MainWindow()
{
    dataLib->save(routines, exercises);
}

//---- for navigating to different pages----//
void MainWindow::chooseRoutine() {
    pastPage = currentPage;
    currentPage = routineRight;
    updateRoutineWidgets();
    chooseWidget();

}

void MainWindow::chooseWorkoutLibrary() {
    pastPage = currentPage;
    currentPage = workoutLibPage;
    updateRoutineWidgets();
    updateExerciseWidgets();
    chooseWidget();
}

void MainWindow::showAddRoutinePage() {
    pastPage = currentPage;
    currentPage = addRoutinePage;
    updateExerciseWidgets();
    chooseWidget();
}

void MainWindow::showProgressPage() {
    pastPage = currentPage;
    currentPage = progressPage;
    updateRoutineWidgets();
    updateProgressDates();
    updateWorkoutDataTable();
    chooseWidget();
}
//------------------------------------------


void MainWindow::enableCreateRoutine() {
    if (nameRoutine->text() == "" || listChosenExercises->count() == 0)
        createRoutine->setEnabled(false);
    else
        createRoutine->setEnabled(true);
}

void MainWindow::addExerciseToSelected() {

    QListWidgetItem *chosen = allExercisesForAddRoutine->currentItem();
    for (int i = 0; i < listChosenExercises->count(); i++) {
        if (chosen->text() == listChosenExercises->item(i)->text())
            return;
    }

    listChosenExercises->addItem(chosen->text());
    enableCreateRoutine();

    return;
}

void MainWindow::removeSelectedExercise(){
    listChosenExercises->takeItem(listChosenExercises->currentRow());
}

void MainWindow::CreateRoutine() {
    if (!routines.contains(nameRoutine->text())) {
        int c = listChosenExercises->count();
        if (c != 0) {
            for (int row = 0; row < c; row++)
                chosenExercisesForRoutine.append(listChosenExercises->item(row)->text());
        }
        Routine newRoutine;
        newRoutine.setNameAndExercises(nameRoutine->text(), chosenExercisesForRoutine);
        routines[newRoutine.getName()] = newRoutine;
        allRoutineInfo = routines.values();
        updateRoutineWidgets();

        QMessageBox::information(this, "Success!", "New routine has been added to the library.");
        nameRoutine->clear();
        listChosenExercises->clear();
        chosenExercisesForRoutine.clear();
        allExercisesForAddRoutine->clearSelection();

        enableCreateRoutine();
    } else {
        QMessageBox::warning(this, "Oops!", "\"" + nameRoutine->text() + "\" is already in"
                             " the library. Please use another name and try again.");
        nameRoutine->clear();
    }
}

void MainWindow::removeItem() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Warning", "Are you sure you want to remove "
                                  "selected item?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (QObject::sender() == removeRoutineButton) {
            routines.remove(allRoutines->currentItem()->text());
            allRoutineInfo = routines.values();
            updateRoutineWidgets();
        }
        else {
            exercises.remove(allExercisesForWorkoutLib->currentItem()->text());
            updateExerciseWidgets();
        }
    }
}



void MainWindow::addExToLib() {
    bool okayed;
    QString input = QInputDialog::getText(this, tr("New Exercise"),
                                          tr("Name of new exercise:"), QLineEdit::Normal,
                                          QString(""), &okayed);
    if (okayed && !input.isEmpty()) {
        if (!exercises.contains(input)) {
            Exercise newExercise;
            exercises[input] = newExercise;

            updateExerciseWidgets();

            QMessageBox::information(this, "Success", "\"" + input + "\" has been "
                                 "successfully added to library.");

        }
        else
            QMessageBox::warning(this, "Error", "\"" + input + "\" is already in the library. "
                                 "Please use another name for your new exercise.");

    }
}


void MainWindow::addAWorkout() {

    dateOfWorkout = cal->selectedDate();
    for (int i = 0; i < routines.values().count(); i++) {
        if (routines.values()[i].getName() == dropDownRoutine->currentText()) {
            currentRoutine = &routines[dropDownRoutine->currentText()];
        }
    }

    if (currentRoutine->getDates().contains(dateOfWorkout)) {
        QMessageBox::warning(this, "Oops!", "The indicated date of the workout has already"
                             " been used. Please try with a different date.");
        return;
    }

    numSetPerRoutine = dropDownSets->currentText().QString::toInt();

    pastPage = currentPage;
    currentPage = trackWorkoutDataPage;
    specificRoutineLabel->setText("<font size=\"30\"><font color=\"#3366ff\"><b>" +
                                  currentRoutine->getName() + "</b></font></font>");
    dateEdit->setDate(dateOfWorkout);
    setWorkoutTable();
    chooseWidget();
}



void MainWindow::titleLabel(QLabel *name, QVBoxLayout *layout) {
    name->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(name, 0, Qt::AlignTop);
}


void MainWindow::chooseWidget() {

    if (pastPage != currentPage) {
        pastPage->setVisible(false);
        currentPage->setVisible(true);
    }
}

void MainWindow::updateExerciseWidgets() {
    // update widgets that use data stored in "exercises"

    allExercisesForWorkoutLib->clear();
    allExercisesForAddRoutine->clear();
    QMap<ExerciseName, Exercise>::iterator b;
    for (b = exercises.begin(); b != exercises.end(); ++b) {
        allExercisesForWorkoutLib->addItem(b.key());
        allExercisesForAddRoutine->addItem(b.key());
    }
    allExercisesForWorkoutLib->repaint();
    allExercisesForAddRoutine->repaint();
}

void MainWindow::updateRoutineWidgets() {
    // update widgets that use data stored in routines

    allRoutines->clear();
    dropDownRoutine->clear();
    listRoutineProgress->clear();

    if (routines.values().isEmpty()){
        dropDownRoutine->addItem("You don't have any routines created. Please go to "
                                 "\"Workout Library\" to add routines.");
        dropDownRoutine->setEnabled(false);
        dropDownSets->setEnabled(false);
        choose->setEnabled(false);
        listRoutineProgress->setEnabled(false);
    } else {
        dropDownRoutine->setEnabled(true);
        dropDownSets->setEnabled(true);
        choose->setEnabled(true);
        listRoutineProgress->setEnabled(true);

        allRoutineInfo = routines.values();
        QList<Routine>::iterator i;
        for (i = allRoutineInfo.begin(); i != allRoutineInfo.end(); ++i) {
            allRoutines->addItem(i->getName());
            dropDownRoutine->addItem(i->getName());
            listRoutineProgress->addItem(i->getName());
        }


        allRoutines->repaint();
        dropDownRoutine->repaint();
        listRoutineProgress->repaint();
    }
}

void MainWindow::updateProgressDates() {
    // update the dropdown menu of dates as routine is chosen

    progressDates->clear();

    QList<QDate> dates;
    Routine thisRoutine = routines.value(listRoutineProgress->currentText());
    thisRoutine.sortByDate();

    dates = thisRoutine.getDates();

    QList<QDate>::iterator i;
    for (i = dates.begin(); i != dates.end(); i++ ) {
        progressDates->addItem(i->toString(Qt::ISODate));
    }

}


void MainWindow::setWorkoutTable() {

    trackWorkoutTable->clear();
    trackWorkoutTable->clearContents();

    QStringList trackVertiHeader = currentRoutine->getExercises();
    trackWorkoutTable->setRowCount(trackVertiHeader.count());
    trackWorkoutTable->setColumnCount(numSetPerRoutine);
    trackWorkoutTable->setVerticalHeaderLabels(trackVertiHeader);

    QStringList trackHorizHeader;
    for (int count = 1; count <= numSetPerRoutine; count++) {
        trackHorizHeader.append("Set " + QString::number(count) + ": Rep x Weight");
    }

    trackWorkoutTable->setHorizontalHeaderLabels(trackHorizHeader);
    trackWorkoutTable->resizeColumnsToContents();
    trackWorkoutTable->repaint();
}

void MainWindow::retrieveDataFromTable() {
    int numRows = trackWorkoutTable->rowCount();
    int numCols = trackWorkoutTable->columnCount();

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            // check to see if table has been filled out entirely
            QTableWidgetItem *item = trackWorkoutTable->item(i,j);
            if (!item) {
                QMessageBox::warning(this, "Error", "Please fill out the entire table.");
                return;
            }
            else {
                QString input = item->text();
                if (checkInputFormat(input).isEmpty()) {
                    // check to see if input format is correct
                    QMessageBox::warning(this, "Error", "Invalid data format for certain cells. "
                                         "Please enter data in the following style:"
                                         " \"(Reps) x (Weight)\"");
                    return;
                }
            }
        }
    }


    indivExerciseData exercise;
    QList<indivExerciseData> newWorkoutData;

    dateOfWorkout = dateEdit->date();
    if (currentRoutine->getDates().contains(dateOfWorkout)) {
        QMessageBox::warning(this, "Oops!", "The indicated date of the workout has already"
                             " been used. Please try with a different date.");
        return;
    }
    newWorkout = currentRoutine->appendToWorkoutDataList(WorkoutData(dateOfWorkout, numSetPerRoutine));
    QStringList trackVertiHeader = currentRoutine->getExercises();
    for (int i = 0; i < numRows; i++) {
        QString name = trackVertiHeader.at(i);
        QList<QPair<rep,weight> > exerciseData;
        int repetition, wght;
        for (int j = 0; j < numCols; j++) {
            QTableWidgetItem *item = trackWorkoutTable->item(i,j);
            QString input = item->text();
            QStringList test = checkInputFormat(input);
            repetition = test[0].toInt();
            wght = test[1].toInt();
            QPair<rep,weight> dataPerSet;
            dataPerSet.first = repetition;
            dataPerSet.second = wght;
            exerciseData.append(dataPerSet);
       }
        int numSet = newWorkout->getSets();
        exercise.setName(name);
        exercise.setSets(numSet);
        exercise.setRepAndWeightData(exerciseData);
        newWorkoutData.append(exercise);
    }
    newWorkout->inputWorkoutData(newWorkoutData);
    QMessageBox::information(this, "Success", "Data has been saved successfully.");

    dropDownRoutine->setCurrentIndex(0);
    dropDownSets->setCurrentIndex(0);
    cal->showToday();
}


QStringList MainWindow::checkInputFormat (QString input) {
    // parse the data
    QStringList list;
    if (!input.contains("x")) {
        return list;
    } else {
        QRegExp rv("(\\ | x)");
        QStringList processed = input.split(rv, QString::SkipEmptyParts);
        for (int count = 0; count < processed.count(); count++) {
            bool ok;
            int value = processed.at(count).toInt(&ok);
            if (!ok)
                return list;
        }
        return processed;
    }
}

void MainWindow::cancelTrackData() {

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Cancel", "Are you sure you want to cancel? All the "
                                  "unsaved data will be lost!");
    if (reply == QMessageBox::Yes) {
        routines[currentRoutine->getName()].deleteInstanceOfRoutineByDate(dateOfWorkout);
        pastPage = currentPage;
        currentPage = routineRight;
        chooseWidget();
    }
}

void MainWindow::updateWorkoutDataTable() {

    QList<QDate> dates;
    Routine thisRoutine = routines.value(listRoutineProgress->currentText());
    thisRoutine.sortByDate();

    dates = thisRoutine.getDates();

    if (dates.isEmpty()) {
        showWorkoutDataTable->clear();
        showWorkoutDataTable->horizontalHeader()->setVisible(false);
        showWorkoutDataTable->verticalHeader()->setVisible(false);
        showWorkoutDataTable->setColumnCount(0);
        showWorkoutDataTable->setRowCount(0);

    }
    else {

        WorkoutData targetWorkout = thisRoutine.getSpecificWorkoutByDate(dates[progressDates->currentIndex()]);
        int columnCount = targetWorkout.getSets();
        showWorkoutDataTable->setColumnCount(columnCount);

        QStringList listExercises = thisRoutine.getExercises();
        showWorkoutDataTable->setRowCount(listExercises.count());
        showWorkoutDataTable->setVerticalHeaderLabels(listExercises);
        showWorkoutDataTable->verticalHeader()->setVisible(true);
        showWorkoutDataTable->horizontalHeader()->setVisible(true);

        for (int set = 0; set < columnCount; set++) {
            for (int ex = 0; ex < listExercises.count(); ex++) {
                indivExerciseData targetEx = targetWorkout.getIndivEx(listExercises[ex]);
                QPair<rep,weight> repWeight = targetEx.repWeightPerSet(set);
                QString repWeightText = QString::number(repWeight.first, 'f', 2) +
                        " x " + QString::number(repWeight.second, 'f', 2);
                showWorkoutDataTable->setItem(ex,set,new QTableWidgetItem(repWeightText));
            }
//            QPair<averageRep,averageWeight> repWeight = targetEx.averageRepAndWeightPerExercise();
//            QString avgRepWeight = QString::number(repWeight.first, 'f', 2)
//                    + " x " + QString::number(repWeight.second, 'f', 2);
//            showWorkoutDataTable->setItem(ex,date,new QTableWidgetItem(avgRepWeight));
        }
//        QStringList listDates;
//        for (int numWorkout = 0; numWorkout < dates.count(); numWorkout++)
//            listDates.append(dates.at(numWorkout).toString(Qt::ISODate));

//        showWorkoutDataTable->setColumnCount(dates.count());
//        showWorkoutDataTable->setHorizontalHeaderLabels(listDates);
//        showWorkoutDataTable->horizontalHeader()->setVisible(true);
//        QStringList listExercises = thisRoutine.getExercises();

//        showWorkoutDataTable->setRowCount(listExercises.count());
//        showWorkoutDataTable->setVerticalHeaderLabels(listExercises);
//        showWorkoutDataTable->verticalHeader()->setVisible(true);

//        for (int date = 0; date < dates.count(); date++) {
//            WorkoutData targetWorkout = thisRoutine.getSpecificWorkoutByDate(dates[date]);
//            for (int ex = 0; ex < listExercises.count(); ex++) {
//                indivExerciseData targetEx = targetWorkout.getIndivEx(listExercises[ex]);
//                QPair<averageRep,averageWeight> repWeight = targetEx.averageRepAndWeightPerExercise();
//                QString avgRepWeight = QString::number(repWeight.first, 'f', 2)
//                        + " x " + QString::number(repWeight.second, 'f', 2);
//                showWorkoutDataTable->setItem(ex,date,new QTableWidgetItem(avgRepWeight));
//            }
//        }
        showWorkoutDataTable->setVisible(true);
    }
}

void MainWindow::resetLibrary() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset Data", "Are you sure you want to reset "
                                  "your library? All routines and workouts will be erased!");

    if (reply == QMessageBox::Yes) {
        routines.clear();
        allRoutineInfo.clear();
        updateRoutineWidgets();
        updateProgressDates();
        updateWorkoutDataTable();
    }
}

void MainWindow::createRightLayout() {

    // layout for "Track"
    routineRight = new QWidget();
    QVBoxLayout *routineLayout = new QVBoxLayout();
    routineLayout->QLayout::setContentsMargins(20,20,20,20);
    routineRight->setLayout(routineLayout);

    QLabel *routineLabel = new QLabel("<font size=\"30\"><font color=\"#3366ff\"><b>Routine</b></font></font>");
    titleLabel(routineLabel, routineLayout);


    QVBoxLayout *innerRoutineLayout = new QVBoxLayout();

    QHBoxLayout *row1 = new QHBoxLayout();
    QLabel *text = new QLabel("Choose your routine:");
    dropDownRoutine = new QComboBox();
    dropDownRoutine->setMinimumWidth(routineRight->width()*.3);
    row1->addWidget(text, 0, Qt::AlignLeft | Qt::AlignTop);
    row1->addWidget(dropDownRoutine, 1, Qt::AlignTop | Qt::AlignLeft);
    innerRoutineLayout->addLayout(row1);

    QHBoxLayout *row2 = new QHBoxLayout();
    QLabel *setsLabel = new QLabel("Number of sets for this routine: ");
    dropDownSets = new QComboBox();
    for (int set = 1; set < 6; set++) {
        dropDownSets->addItem(QString::number(set));
    }
    dropDownSets->setMinimumWidth(routineRight->width()*.3);
    row2->addWidget(setsLabel, 0, Qt::AlignTop | Qt::AlignLeft);
    row2->addWidget(dropDownSets, 1, Qt::AlignTop | Qt::AlignLeft);
    innerRoutineLayout->addLayout(row2);

    QHBoxLayout *row3 = new QHBoxLayout();
    QLabel *date = new QLabel("Date Of WorkOut: ");
    cal = new QCalendarWidget();
    cal->setGridVisible(true);
    cal->setMaximumDate(QDate::currentDate());
    row3->addWidget(date, 0, Qt::AlignTop | Qt::AlignLeft);
    row3->addWidget(cal, 1, Qt::AlignTop | Qt::AlignLeft);
    innerRoutineLayout->addLayout(row3);

    routineLayout->addLayout(innerRoutineLayout);
    routineLayout->addWidget(choose, 0, Qt::AlignTop | Qt::AlignCenter);

    mainLayout->addWidget(routineRight, 1, 0);
    routineRight->setVisible(false);


    // layout for specific routine data
    trackWorkoutDataPage = new QWidget();
    QVBoxLayout *trackTemplate = new QVBoxLayout();
    trackTemplate->QLayout::setContentsMargins(20,20,20,20);
    trackWorkoutDataPage->setLayout(trackTemplate);

    specificRoutineLabel = new QLabel();
    titleLabel(specificRoutineLabel, trackTemplate);

    QHBoxLayout *horiRow = new QHBoxLayout();
    dateEdit = new QDateEdit();
    dateEdit->setDisplayFormat("MM.dd.yyyy");
    dateEdit->setMaximumDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    horiRow->addWidget(dateEdit);

    saveTrackData = new QPushButton("Save");
    horiRow->addWidget(saveTrackData, 0, Qt::AlignTop | Qt::AlignRight);

    cancelSaveData = new QPushButton("Cancel");
    horiRow->addWidget(cancelSaveData, 0, Qt::AlignTop | Qt::AlignRight);

    trackTemplate->addLayout(horiRow);

    trackWorkoutTable = new QTableWidget(this);
    trackTemplate->addWidget(trackWorkoutTable);

    mainLayout->addWidget(trackWorkoutDataPage, 1, 0);
    trackWorkoutDataPage->setVisible(false);




    // layout for "Workout Library"
    workoutLibPage = new QWidget();
    QVBoxLayout *workoutLibLayout = new QVBoxLayout();
    workoutLibLayout->QLayout::setContentsMargins(20,20,20,20);
    workoutLibPage->setLayout(workoutLibLayout);

    QLabel *workoutLabel = new QLabel("<font size=\"30\"><font color=\"#3366ff\"><b>Workout Library</b></font></font>");
    QLabel *workoutDescription = new QLabel("Here you can add and removes routines and exercises.");
    titleLabel(workoutLabel, workoutLibLayout);
    workoutLibLayout->addWidget(workoutDescription, 0, Qt::AlignTop);

    QHBoxLayout *subLibLayout = new QHBoxLayout();

    QVBoxLayout *routLibLayout = new QVBoxLayout();
    QLabel *label1 = new QLabel("Routines: ");
    routLibLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignCenter);

    allRoutines = new QListWidget();
    allRoutines->setMaximumWidth(workoutLibPage->width()*0.3);
    routLibLayout->addWidget(allRoutines, 0, Qt::AlignCenter);
    routLibLayout->addWidget(addRoutineButton);

    removeRoutineButton = new QPushButton("Remove");
    routLibLayout->addWidget(removeRoutineButton);

    QVBoxLayout *exerciseLibLayout = new QVBoxLayout();
    QLabel *label2 = new QLabel("Exercises: ");
    exerciseLibLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignCenter);

    allExercisesForWorkoutLib = new QListWidget();
    allExercisesForWorkoutLib->setMaximumWidth((workoutLibPage->width())*0.3);
    exerciseLibLayout->addWidget(allExercisesForWorkoutLib, 0, Qt::AlignCenter);

    addExerciseToLibrary = new QPushButton("Add");
    exerciseLibLayout->addWidget(addExerciseToLibrary);

    removeExerciseFromLibrary = new QPushButton("Remove");
    exerciseLibLayout->addWidget(removeExerciseFromLibrary);

    horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    subLibLayout->addLayout(routLibLayout,0);
    subLibLayout->addLayout(exerciseLibLayout,0);
    subLibLayout->addItem(horizontalSpacer);
    workoutLibLayout->addLayout(subLibLayout);

    mainLayout->addWidget(workoutLibPage, 1, 0);
    workoutLibPage->setVisible(false);



    // layout for "Add routine"
    addRoutinePage = new QWidget();
    QVBoxLayout *addRoutineLayout = new QVBoxLayout();
    addRoutineLayout->QLayout::setContentsMargins(20,20,20,20);
    addRoutinePage->setLayout(addRoutineLayout);
    mainLayout->addWidget(addRoutinePage, 1, 0);

    QLabel *addRoutineLabel = new QLabel("<font size=\"30\"><font color=\"#3366ff\"><b>Add A Routine</b></font></font>");
    titleLabel(addRoutineLabel, addRoutineLayout);

    // routine name line edit
    QLabel *nameRoutineLabel = new QLabel("<font size=\"4\">Name of routine:</font>");
    nameRoutineLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    nameRoutineLabel->setStyleSheet("margin-top:30px");
    addRoutineLayout->addWidget(nameRoutineLabel, 0, Qt::AlignTop);


    nameRoutine = new QLineEdit();
    nameRoutine->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    nameRoutine->setMaxLength(30);
    nameRoutine->setMaximumWidth(addRoutinePage->width()*0.5);
    nameRoutine->setTextMargins(3,3,3,3);
    nameRoutine->setStyleSheet("margin-bottom:30px");
    addRoutineLayout->addWidget(nameRoutine, 0, Qt::AlignTop);


    QHBoxLayout *bottomOuterARL = new QHBoxLayout();
    addRoutineLayout->addLayout(bottomOuterARL);

    QVBoxLayout *leftBottomARL = new QVBoxLayout();
    bottomOuterARL->addLayout(leftBottomARL);


    // display the library of exercises in a QListWidget
    QLabel *exerciseLibrary = new QLabel("<font size=\"4\">Exercise Library</font>");
    exerciseLibrary->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    leftBottomARL->addWidget(exerciseLibrary, 0, Qt::AlignTop);

    allExercisesForAddRoutine = new QListWidget();
    allExercisesForAddRoutine->setMaximumWidth((addRoutinePage->width())*0.3);
    leftBottomARL->addWidget(allExercisesForAddRoutine, 0, Qt::AlignCenter);



    // layout for add & remove buttons
    QVBoxLayout *midLeftBottomARL = new QVBoxLayout();
    bottomOuterARL->addLayout(midLeftBottomARL);

    midLeftBottomARL->addWidget(addExerciseToRoutineButton, 0, Qt::AlignBottom);
    midLeftBottomARL->addWidget(removeExerciseFromRoutineButton, 0, Qt::AlignTop);


    // displays selected exercises in a QListWidget
    QVBoxLayout *midRightBottomARL = new QVBoxLayout();
    bottomOuterARL->addLayout(midRightBottomARL);

    QLabel *selectedExercises = new QLabel("<font size=\"4\">Selected Exercises</font>");
    selectedExercises->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    midRightBottomARL->addWidget(selectedExercises);

    listChosenExercises = new QListWidget();
    listChosenExercises->setMaximumWidth((addRoutinePage->width())*0.3);
    midRightBottomARL->addWidget(listChosenExercises,0, Qt::AlignCenter);


    // "create a new routine" button
    QVBoxLayout *rightBottomARL = new QVBoxLayout();
    bottomOuterARL->addLayout(rightBottomARL);
    createRoutine = new QPushButton("Create new routine!");
    createRoutine->setMaximumWidth(addRoutinePage->width()*0.3);
    rightBottomARL->addWidget(createRoutine, 0, Qt::AlignCenter);
    createRoutine->setEnabled(false);

    addRoutinePage->setVisible(false);




    // layout for "Progress" page------------
    progressPage = new QWidget();
    QVBoxLayout *mainProgressLayout = new QVBoxLayout();
    mainProgressLayout->QLayout::setContentsMargins(20,20,20,20);
    progressPage->setLayout(mainProgressLayout);

    QLabel *progressLabel = new QLabel("<font size=\"30\"><font color=\"#3366ff\"><b>Progress</b></font></font>");
    titleLabel(progressLabel, mainProgressLayout);

    QHBoxLayout *progressOptions = new QHBoxLayout();
    QLabel *option1Label = new QLabel("Routine: ");

    listRoutineProgress = new QComboBox();
    listRoutineProgress->setMinimumWidth(progressPage->width()*.2);

    progressOptions->addWidget(option1Label, 0, Qt::AlignRight);
    progressOptions->addWidget(listRoutineProgress, 0, Qt::AlignLeft);

    QLabel *option2Label = new QLabel("Date: ");

    progressDates = new QComboBox();
    progressDates->setMinimumWidth(progressPage->width()*.2);

    progressOptions->addWidget(option2Label, 0, Qt::AlignRight);
    progressOptions->addWidget(progressDates, 0, Qt::AlignLeft);

    mainProgressLayout->addLayout(progressOptions);

    showWorkoutDataTable = new QTableWidget();
    mainProgressLayout->addWidget(showWorkoutDataTable);


    mainLayout->addWidget(progressPage, 1, 0);
    progressPage->setVisible(false);

}
