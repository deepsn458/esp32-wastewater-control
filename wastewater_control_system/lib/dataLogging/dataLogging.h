#ifndef DATALOGGING_H
#define DATALOGGING_H

void dataLogging(void* parameters);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();


#endif