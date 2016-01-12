#ifndef ROUTINE_H
#define ROUTINE_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QMultiMap>
#include <QList>
#include <QPair>

typedef int indivSet;
typedef int rep;
typedef int weight;
typedef float averageRep;
typedef float averageWeight;


//----Exercise class----//
class Exercise
{
public:
    Exercise();
    ~Exercise();
};


//----indivExerciseData class----//
class indivExerciseData
{
    QString exerciseName;
    int numSets;
    QList<QPair<rep,weight> > repAndWeight;

public:
    indivExerciseData();
    indivExerciseData(QString name, int sets, QList<QPair<rep,weight> > repWeight);
    ~indivExerciseData();
    int getSets();
    QPair<rep,weight> repWeightPerSet(int set);
    QList<weight> getWeight();
    QList<rep> getReps();
    QString getName();
    void setName(QString name);
    void setSets(int num);
    void setRepAndWeightData(QList<QPair<rep,weight> > data);
    QPair<averageRep, averageWeight> averageRepAndWeightPerExercise();
};



//----WorkoutData class----//
class WorkoutData {

    QDate dateOfWorkout;
    int numSets;
    QList<indivExerciseData> dataSet;

public:
    WorkoutData();
    WorkoutData(QDate date, int sets);
    ~WorkoutData();
    QDate getDate();
    int getSets();
    QList<indivExerciseData> getWorkoutData();
    void inputWorkoutData(QList<indivExerciseData> data);
    void setDate(QDate date);
    void setSets(int num);
    indivExerciseData getIndivEx(QString exName);
};


//----Routine class----//
class Routine
{
    QString routineName;
    QStringList routineExercises;
    QList<WorkoutData> instancesOfRoutine;

public:
    Routine();
    ~Routine();
    QString getName();
    QStringList getExercises();
    QList<QDate> getDates();
    WorkoutData getSpecificWorkoutByDate(QDate date);
    void setNameAndExercises(QString name, QStringList exerciseList);
    WorkoutData* appendToWorkoutDataList(WorkoutData newWorkout);    
    void deleteInstanceOfRoutineByDate(QDate date);
    void sortByDate();
};



#endif // ROUTINE_H
