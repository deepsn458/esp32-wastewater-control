#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <ArduinoJson.h>
#include "database.h"

// CREDENTIALS
static const char* API_KEY       = "AIzaSyCRiDbuGOOqW_q_OKA_-zeCXVWCqW3gJe4";
static const char* USER_EMAIL    = "controlrtdb@gmail.com";
static const char* USER_PASSWORD = "ControlRTDB1!";
//static const int   TOKEN_EXPIRE  = 3000; 

// device ID that must match auth.uid in rtdb rules
static const char* DEVICE_ID     = "esp32";

const int maximumTaskCount = 20;
//check if the task queue has been cleared
bool clearedTasks = true;
// The UserAuth object
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

// Global Firebase Objects
FirebaseApp app;
RealtimeDatabase Database;

WiFiClientSecure ssl_client1, ssl_client2;

using AsyncClient = AsyncClientClass;
AsyncClient async_client1(ssl_client1), async_client2(ssl_client2);
void initFirebase(const char* databaseURL){
    // Skip certificate verification
    ssl_client1.setInsecure();
    ssl_client2.setInsecure();

    ssl_client1.setHandshakeTimeout(5);
    ssl_client2.setHandshakeTimeout(5);


    initializeApp(async_client1, app, getAuth(user_auth), processData, "authTask");

    // Assign the Realtime Database handle to this app
    app.getApp<RealtimeDatabase>(Database);

    // Set the database URL
    Database.url(databaseURL);
  
}
void fireBaseLoop(){
    app.loop();
}
void processData(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

    if (aResult.available())
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}


void pushSensorReading(const String &sensorType,const String &sensorId, float readingValue){
    if (!app.ready()) {
        Serial.println("[Firebase] Not ready yet for pushSensorReading.");
        return;
    }
    //if the task queue is full, we wait till all the tasks have been cleared
    else if (async_client2.taskCount() == maximumTaskCount){
        clearedTasks = false;
        while (async_client2.taskCount() > 9){
            delay(50);
        }
        clearedTasks = true;
    }
    else if (clearedTasks){
        // build a JSON object with sensor info + server timestamp
        object_t readingObj;
        object_t tsObj;
        object_t fullTsObj;
        object_t fullMsgObj;
        String deviceId = "esp32";
        String path = "/devices/" + deviceId + "/latestReadings/"+ sensorType +"/" + sensorId;
        String path2 = "/devices/" + deviceId + "/sensors/" + sensorType + "/" + sensorId;
        JsonWriter writer;
        writer.create(tsObj, ".sv", "timestamp");
        writer.create(readingObj, "reading", number_t(readingValue, 2));
        writer.create(fullTsObj, "timestamp", tsObj);
        writer.join(fullMsgObj, 2,readingObj,fullTsObj);
        Serial.println(fullMsgObj);
        Database.set<object_t>(async_client2, path, fullMsgObj, processData, "pushSensorTask");
        Database.push<object_t>(async_client2, path2, fullMsgObj, processData, "pushSensorTask");
    }
    return;
}

void pushAlert(const String &alert){
    if (!app.ready()) {
        Serial.println("[Firebase] Not ready yet for pushSensorReading.");
        return;
    }
    //if the task queue is full, we wait till all the tasks have been cleared
    else if (async_client2.taskCount() == maximumTaskCount){
        clearedTasks = false;
        while (async_client2.taskCount() != 0){
            delay(100);
        }
        clearedTasks = true;
    }
    else if (clearedTasks){
        // build a JSON object with alert + server timestamp
        object_t alertObj;
        object_t tsObj;
        object_t fullTsObj;
        object_t fullMsgObj;
        String deviceId = "esp32";
        String path = "/devices/" + deviceId + "/alerts/";
        
        JsonWriter writer;
        writer.create(tsObj, ".sv", "timestamp");
        writer.create(alertObj, "alert", string_t(alert));
        writer.create(fullTsObj, "timestamp", tsObj);
        writer.join(fullMsgObj, 2,alertObj,fullTsObj);
        Serial.println(fullMsgObj);
        Database.push<object_t>(async_client2, path, fullMsgObj, processData, "pushAlertTask");
    }
    return;
}

