#include "datamanipulation.h"

#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QIODevice>
#include <QtXml/QtXml>
#include <QtXml/QDomDocument>
#include <QByteArray>
#include <QMap>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

#include "routine.h"

typedef QString RoutineName;
typedef QString ExerciseName;

DataManipulation::DataManipulation() {
    fileLocation();
}

void DataManipulation::fileLocation() {
    QDir dir = QDir::root();
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (!QDir(path).exists())
        dir.mkpath(path);
    filePath = path + "/data.xml";

    file = new QFile(filePath);
    if (!file->open(QIODevice::ReadWrite)) {
    }
    file->close();
    if (file->size()==0) {
        presetLibrary();
    }

    return;
}

void DataManipulation::presetLibrary() {

    if(!file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate) ){
    }

    QByteArray content = file->readAll();
    QDomDocument doc;
    doc.setContent(content);
    QDomProcessingInstruction instr = doc.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"UTF-8\"");

    doc.appendChild(instr);
    QDomElement data = doc.createElement("data");


    //---- writing exercises ----//
    QDomElement exLib = doc.createElement("ExerciseLibrary");

    QList<ExerciseName> exer;
    exer.append("Bench Press");
    exer.append("Bicep Curl");
    exer.append("Crunch");
    exer.append("Deadlift");
    exer.append("Dips");
    exer.append("Leg Press");
    exer.append("Leg Throw");
    exer.append("Lunge");
    exer.append("Plank");
    exer.append("Pull Up");
    exer.append("Russian Twist");
    exer.append("Sit Up");
    exer.append("Squat");
    exer.append("Tricep Extension");

    int numE = exer.count();
    for (int i = 1; i <= numE; i++) {
        QDomElement newEx = doc.createElement("Exercise");
        newEx.setAttribute("drill", i);
        QDomText name = doc.createTextNode(exer[i-1]);
        newEx.appendChild(name);
        exLib.appendChild(newEx);
    }

    data.appendChild(exLib);

    doc.appendChild(data);


    QTextStream out(file);
    out << doc.toString();

    file->close();

    return;
}


void DataManipulation::loadXML() {
    if (!file->open(QIODevice::ReadOnly)) {
        file->close();
        return;
    }
    QByteArray content = file->readAll();

    if (!doc.setContent(content)) {
        file->close();
        return;
    }
    file->close();
}

void DataManipulation::save(QMap<RoutineName, Routine> routinesData,
                            QMap<ExerciseName, Exercise> exercisesData) {

    if( !file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate) )
        return;


    QByteArray content = file->readAll();
    QDomDocument outDoc;
    outDoc.setContent(content);
    QDomProcessingInstruction instr = outDoc.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"UTF-8\"");

    outDoc.appendChild(instr);
    QDomElement data = outDoc.createElement("data");


    //---- saving exercises ----//
    QDomElement exLib = outDoc.createElement("ExerciseLibrary");

    QMap<ExerciseName, Exercise>::iterator e;
    int numE = 1;
    for (e = exercisesData.begin(); e != exercisesData.end(); ++e) {
        QDomElement newEx = outDoc.createElement("Exercise");
        newEx.setAttribute("drill", numE);
        QDomText name = outDoc.createTextNode(e.key());
        newEx.appendChild(name);
        exLib.appendChild(newEx);
        numE++;
    }

    data.appendChild(exLib);


    //---- saving routines and the exercises in them ----//
    QDomElement routineLib = outDoc.createElement("RoutineLibrary");

    QMap<RoutineName, Routine>::iterator r;
    int numR = 1;
    for (r = routinesData.begin(); r != routinesData.end(); ++r) {
        QDomElement newRoutine = outDoc.createElement("Routine");
        newRoutine.setAttribute("num", numR);
        QDomElement nameNode = outDoc.createElement("Name");
        QDomText name = outDoc.createTextNode(r.key());
        nameNode.appendChild(name);
        newRoutine.appendChild(nameNode);
        QStringList exInThisRoutine = r.value().getExercises();
        for (int x = 0; x < exInThisRoutine.count(); x++) {
            QDomElement ex = outDoc.createElement("Exercise");
            ex.setAttribute("num", x+1);
            QDomText exName = outDoc.createTextNode(exInThisRoutine[x]);
            ex.appendChild(exName);
            newRoutine.appendChild(ex);
        }
        routineLib.appendChild(newRoutine);
        numR++;
    }

    //---- saving progress data----//
    QDomElement trackLib = outDoc.createElement("TrackLibrary");
    for (r = routinesData.begin(); r != routinesData.end(); ++r) {
        QDomElement routine = outDoc.createElement("Routine");
        routine.setAttribute("name", r->getName());

        QList<QDate>::iterator d;
        QList<QDate> dates = r->getDates();
        for (d = dates.begin(); d != dates.end(); ++d) {
            QDomElement workout = outDoc.createElement("Workout");
            workout.setAttribute("date", d->toString(Qt::ISODate));
            routine.appendChild(workout);

            WorkoutData thisWorkout = r->getSpecificWorkoutByDate(*d);
            int setInWorkout = thisWorkout.getSets();
            QDomElement numSet = outDoc.createElement("numSet");
            QDomText setNum = outDoc.createTextNode(QString::number(setInWorkout));
            numSet.appendChild(setNum);
            workout.appendChild(numSet);
            QList<indivExerciseData>::iterator i;
            QList<indivExerciseData> allExData = thisWorkout.getWorkoutData();
            for (i = allExData.begin(); i != allExData.end(); ++i) {
                QString exName = i->getName();
                QDomElement ex = outDoc.createElement("Exercise");
                ex.setAttribute("name", exName);

                for (int s = 1; s <= setInWorkout; s++) {
                    indivExerciseData thisIndivEx = thisWorkout.getIndivEx(exName);
                    QList<int> allReps = thisIndivEx.getReps();
                    QList<int> allWeights = thisIndivEx.getWeight();
                    QDomElement set = outDoc.createElement("Set");
                    set.setAttribute("ID", s);
                    QDomElement rep = outDoc.createElement("Rep");
                    QDomText numRep = outDoc.createTextNode(QString::number(allReps[s-1]));
                    rep.appendChild(numRep);
                    QDomElement weight = outDoc.createElement("Weight");
                    QDomText numWeight = outDoc.createTextNode(QString::number(allWeights[s-1]));
                    weight.appendChild(numWeight);

                    set.appendChild(weight);
                    set.appendChild(rep);
                    ex.appendChild(set);
                }
            workout.appendChild(ex);
            }

        trackLib.appendChild(routine);
        }
    }

    data.appendChild(routineLib);

    data.appendChild(trackLib);

    outDoc.appendChild(data);


    QTextStream out(file);
    out << outDoc.toString();

    file->close();

    return;
}

QMap<RoutineName, Routine> DataManipulation::getRoutineInfo(){
    QMap<RoutineName, Routine> routinesInLib;


    //---- loading the name of and the exercises in the routine ----//
    QDomElement parent = doc.documentElement();
    QDomNode routineLib = parent.firstChild().nextSibling();
    QDomNode indivRoutine = routineLib.firstChild();

    while (!indivRoutine.isNull()) {
        QStringList exercisesInRoutine;
        QDomElement name = indivRoutine.firstChild().toElement();
        QDomNode exercise = name.nextSibling();

        while (!exercise.isNull()) {
            QDomElement nameExercise = exercise.toElement();
            exercisesInRoutine.append(nameExercise.text());

            exercise = exercise.nextSibling();
        }
        Routine routine;
        routine.setNameAndExercises(name.text(),exercisesInRoutine);

        routinesInLib[name.text()] = routine;

        indivRoutine = indivRoutine.nextSibling();
    }


    //---- loading the progress data associated with different routines ----//
    QDomNode trackLib = routineLib.nextSibling();
    QDomNode routineData = trackLib.firstChild();

    QList<WorkoutData> listRoutineData;

    while (!routineData.isNull()) {
        QString nameR = routineData.toElement().attributeNode("name").value();
        Routine *thisRoutine = &routinesInLib[nameR];

        QDomElement dateOfWorkout = routineData.firstChildElement();

        while(!dateOfWorkout.isNull()) {
            QDomAttr dateAttr = dateOfWorkout.attributeNode("date");
            QDate date = QDate::fromString(dateAttr.value(),Qt::ISODate);
            int numSet = dateOfWorkout.firstChild().toElement().text().toInt();
            WorkoutData *thisWorkout = thisRoutine->appendToWorkoutDataList
                    (WorkoutData(date, numSet));

            QList<indivExerciseData> exsData;

            QDomElement indivExNameElement = dateOfWorkout.firstChild().nextSiblingElement();
            while (!indivExNameElement.isNull()) {
                QList<QPair<rep, weight> > repWeight;
                QDomAttr nameAttr = indivExNameElement.attributeNode("name");
                QString indivExName = nameAttr.value();

                QDomNode diffSet = indivExNameElement.firstChild();
                while (!diffSet.isNull()) {

                    QDomElement weightElement = diffSet.firstChildElement();
                    int weightValue = weightElement.text().toInt();
                    QDomElement repElement = weightElement.nextSiblingElement();
                    int repValue = repElement.text().toInt();
                    QPair<rep,weight> repWeightPerSet;
                    repWeightPerSet.first = repValue;
                    repWeightPerSet.second = weightValue;
                    repWeight.append(repWeightPerSet);

                    diffSet = diffSet.nextSibling();
                }
                indivExerciseData *thisExercise = new indivExerciseData(indivExName, numSet, repWeight);
                exsData.push_back(*thisExercise);
                indivExNameElement = indivExNameElement.nextSiblingElement();
            }
            thisWorkout->inputWorkoutData(exsData);
            dateOfWorkout = dateOfWorkout.nextSiblingElement();
        }
        routineData = routineData.nextSibling();
    }


    return routinesInLib;
}


QMap<ExerciseName, Exercise> DataManipulation::getExercisesInfo() {
    QMap<ExerciseName, Exercise> exercisesInLib;

    QDomElement parent = doc.documentElement();
    QDomNode exerciseLib = parent.firstChild();
    QDomNode indivEx = exerciseLib.firstChild();
    while (!indivEx.isNull()) {
        QDomElement name = indivEx.toElement();
        Exercise exercise;
        exercisesInLib[name.text()] = exercise;
        indivEx = indivEx.nextSibling();
    }
    return exercisesInLib;
}

DataManipulation::~DataManipulation() {

}
