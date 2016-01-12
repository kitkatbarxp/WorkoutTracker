#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QTextStream>
#include <QMap>
#include <QTableWidget>

#include "datamanipulation.h"
#include "routine.h"

typedef QString RoutineName;
typedef QString ExerciseName;


class QHBoxLayout;
class QLabel;
class QWidget;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QString;
class QListWidget;
class QComboBox;
class QDate;
class QCalendarWidget;
class QDateEdit;
class QSpacerItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    // for homepage
    QWidget *homePage;
    QHBoxLayout *mainLayout;


    // to keep track of which widget is being used
    QWidget *currentPage;
    QWidget *pastPage;


    // for "Track" page
    QWidget *routineRight;
    QWidget *trackWorkoutDataPage;
    QPushButton *choose;
    QComboBox *dropDownRoutine;
    QComboBox *dropDownSets;
    QCalendarWidget *cal;
    QLabel *specificRoutineLabel;
    QDateEdit *dateEdit;
    QLabel *dateText;
    QTableWidget *trackWorkoutTable;
    QPushButton *saveTrackData;
    QPushButton *cancelSaveData;

    int numSetPerRoutine;



    // for "Workout Library" page
    QWidget *workoutLibPage;
    QWidget *addRoutinePage;
    QListWidget *allExercisesForWorkoutLib;
    QListWidget *allRoutines;
    QPushButton *removeRoutineButton;
    QPushButton *addExerciseToLibrary;
    QPushButton *removeExerciseFromLibrary;
    QSpacerItem *horizontalSpacer;


    // for "Add Routine" page

    QPushButton *addRoutineButton;
    QLineEdit *nameRoutine;
    QListWidget *allExercisesForAddRoutine;
    QListWidget *listChosenExercises;
    QPushButton *createRoutine;
    QPushButton *addExerciseToRoutineButton;
    QPushButton *removeExerciseFromRoutineButton;

    QStringList chosenExercisesForRoutine;

    // "Progress" page
    QWidget *progressPage;
    QComboBox *listRoutineProgress;
    QTableWidget *showWorkoutDataTable;
    QComboBox *progressDates;




    DataManipulation *dataLib;
    QMap<RoutineName, Routine> routines;        // stores all the routines
    QMap<ExerciseName, Exercise> exercises;     // stores all the exercises
    QList<Routine> allRoutineInfo;              // don't know why routines.values() gives me infinite loop
    Routine *currentRoutine;
    QDate dateOfWorkout;
    WorkoutData *newWorkout;


    // used to determine whether program needs to warn the user of
    // not having saved the data he/she puts in
    bool workoutSaved;


    void titleLabel(QLabel *name, QVBoxLayout *layout);
    void createRightLayout();
    void chooseWidget();
    void updateExerciseWidgets();
    void updateRoutineWidgets();
    void setWorkoutTable();
    QStringList checkInputFormat(QString input);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void chooseRoutine();
    void chooseWorkoutLibrary();
    void showAddRoutinePage();
    void addExerciseToSelected();
    void removeSelectedExercise();
    void enableCreateRoutine();
    void CreateRoutine();
    void removeItem();
    void addExToLib();
    void addAWorkout();
    //void changeDateOfCurrentRoutine();
    void retrieveDataFromTable();
    void cancelTrackData();
    void showProgressPage();
    void updateWorkoutDataTable();
    void updateProgressDates();
    void resetLibrary();
};

#endif // MAINWINDOW_H
