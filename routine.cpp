#include "routine.h"

#include <QString>
#include <QStringList>
#include <QDate>
#include <QList>
#include <QPair>


//----Exercise class----//
Exercise::Exercise () {}
Exercise::~Exercise() {}



//----indivExerciseData class----//
indivExerciseData::indivExerciseData() {
    numSets = 0;
}

indivExerciseData::indivExerciseData(QString name, int sets, QList<QPair<rep,weight> > repWeight) {
    exerciseName = name;
    numSets = sets;
    repAndWeight = repWeight;
}

indivExerciseData::~indivExerciseData() {

}

int indivExerciseData::getSets() {
    return numSets;
}

QPair<rep,weight> indivExerciseData::repWeightPerSet(int set) {
    return repAndWeight.at(set);
}

QList<weight> indivExerciseData::getWeight() {
    QList<weight> weightData;
    for (int c = 0; c < repAndWeight.count(); c++)
        weightData.append(repAndWeight.at(c).first);
    return weightData;
}

QList<rep> indivExerciseData::getReps() {
    QList<rep> repsData;
    for (int c = 0; c < repAndWeight.count(); c++)
        repsData.append(repAndWeight.at(c).second);
    return repsData;
}

QString indivExerciseData::getName() {
    return exerciseName;
}

void indivExerciseData::setName(QString name) {
    exerciseName = name;
}

void indivExerciseData::setSets(int set) {
    numSets = set;
}

void indivExerciseData::setRepAndWeightData(QList<QPair<rep, weight> > data) {
    repAndWeight = data;
}

QPair<averageRep, averageWeight> indivExerciseData::averageRepAndWeightPerExercise() {
    QPair<averageRep, averageWeight> average;

    float totalRep, totalWeight;
    for (int set = 0; set < numSets; set++) {
        totalRep += repAndWeight[set].first;
        totalWeight += repAndWeight[set].second;
    }

    average.first = totalRep/numSets;
    average.second = totalWeight/numSets;

    return average;
}



//----WorkoutData class----//
WorkoutData::WorkoutData() {

}

WorkoutData::WorkoutData(QDate date, int sets){
    dateOfWorkout = date;
    numSets = sets;
}


WorkoutData::~WorkoutData() {

}

QDate WorkoutData::getDate() {
    return dateOfWorkout;
}

int WorkoutData::getSets(){
    return numSets;
}

QList<indivExerciseData> WorkoutData::getWorkoutData() {
    return dataSet;
}


void WorkoutData::inputWorkoutData(QList<indivExerciseData> data) {
    dataSet = data;
}

void WorkoutData::setDate(QDate date) {
    dateOfWorkout = date;
}

void WorkoutData::setSets(int num) {
    numSets = num;
}

indivExerciseData WorkoutData::getIndivEx(QString exName) {
    QList<indivExerciseData>::iterator i;
    for (i = dataSet.begin(); i != dataSet.end(); ++i) {
        if (i->getName() == exName)
            return *i;
    }
}



//----Routine class----//
Routine::Routine ()
{

}

Routine::~Routine()
{

}

QString Routine::getName() {
    return routineName;
}

QStringList Routine::getExercises(){
    return routineExercises;
}

QList<QDate> Routine::getDates() {
    QList<QDate> dates;
    QList<WorkoutData>::iterator i;
    for (i = instancesOfRoutine.begin(); i != instancesOfRoutine.end(); ++i) {
        dates.append(i->getDate());
    }
    return dates;
}

WorkoutData Routine::getSpecificWorkoutByDate(QDate date) {
    for (int count = 0; count < instancesOfRoutine.count(); count++){
        if (instancesOfRoutine[count].getDate() == date)
            return instancesOfRoutine[count];
    }
}

void Routine::setNameAndExercises(QString name, QStringList exerciseList) {
    routineName = name;
    routineExercises = exerciseList;
}

WorkoutData* Routine::appendToWorkoutDataList(WorkoutData newWorkout) {
    instancesOfRoutine.append(newWorkout);
    return &instancesOfRoutine.back();
}

void Routine::deleteInstanceOfRoutineByDate(QDate date) {
    int specRou;
    for (specRou = 0; specRou < instancesOfRoutine.count(); specRou++) {
        if (instancesOfRoutine[specRou].getDate() == date)
            instancesOfRoutine.takeAt(specRou);
    }
}

void Routine::sortByDate() {
    QMap<QDate,WorkoutData> tempMap;
    QList<WorkoutData>::iterator i;
    for (i = instancesOfRoutine.begin(); i != instancesOfRoutine.end(); ++i) {
        QDate tempDate = i->getDate();
        tempMap[tempDate] = *i;
    }
    instancesOfRoutine.clear();

    QMap<QDate,WorkoutData>::iterator j;
    for (j = tempMap.begin(); j != tempMap.end(); ++j) {
        instancesOfRoutine.append(j.value());
    }
}


