#ifndef DATAMANIPULATION_H
#define DATAMANIPULATION_H

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QXmlStreamReader>
#include <QByteArray>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QMap>

#include "routine.h"

class QString;

typedef QString ExerciseName;
typedef QString RoutineName;


class DataManipulation
{
    QFile* file;
    QDomDocument doc;
    QString filePath;
public:
    DataManipulation();
    ~DataManipulation();
    void fileLocation();
    void presetLibrary();
    void loadXML();
    void save(QMap<RoutineName, Routine> routinesData,
              QMap<ExerciseName, Exercise> exercisesData);

    QMap<ExerciseName, Exercise> getExercisesInfo();
    QMap<RoutineName, Routine> getRoutineInfo();
};

#endif // DATAMANIPULATION_H
