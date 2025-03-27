#ifndef DATABASE_H
#define DATABASE_H
#include <FirebaseClient.h>
// Initialize Firebase once (in setup)
void initFirebase(const char* databaseURL);

// Call this in main loop (or a dedicated task) to handle authentication & async ops
void firebaseLoop();

void processData(AsyncResult &aResult);

//pushes a sensor reading to the database
void pushSensorReading(const String &sensorType,const String &sensorId, float readingValue);

void fireBaseLoop();
#endif