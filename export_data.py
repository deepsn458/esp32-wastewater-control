import json
import sys
from datetime import datetime, time
import pandas as pd

def convert_file(filename):
    try:
        with open(filename,'r') as data:
            parsed_sensor_data = {}
            data_dict = json.loads(data.read())
            all_sensors = data_dict["devices"]["esp32"]["sensors"]
    
                #continues adding data points until there are no more left
            
            for sensor in all_sensors.keys():
                for probe in all_sensors[sensor].keys():
                    timestamps=[]
                    readings = []
                    for entry in (list(all_sensors[sensor][probe].values())):
                        #print(entry)
                        timestamp = str(datetime.fromtimestamp(entry['timestamp']/1000)).split(".")[0]
                        reading = entry['reading']
                        timestamps.append(timestamp)
                        readings.append(reading)
                    parsed_sensor_data[f"{probe}_timestamps"] = timestamps
                    parsed_sensor_data[f"{probe}_readings"] = readings
            #pads the other arrays with zeros
            max_length = len(max(list(parsed_sensor_data.values()),key=len))
            for key in parsed_sensor_data:
                while (len(parsed_sensor_data[key]) < max_length):
                    parsed_sensor_data[key].append(0)
            df = pd.DataFrame(parsed_sensor_data)
            df.to_csv(f"data_csv/{datetime.now().strftime('%m_%d_%Y_%H_%M_%S')}.csv", index=False)    
    except Exception as e:
        print(e)
    pass

def create_csv(sensor_data):      
    pass

if __name__ == "__main__":
    if len(sys.argv) > 1:
        filename = f"data_raw/{sys.argv[1]}"
        convert_file(filename)
    else:
        print("error: no filename provided")