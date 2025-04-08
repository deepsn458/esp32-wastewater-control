import { useState, useEffect } from "react";
import firebaseConf from "../firebaseConfig";
import {
  getDatabase,
  ref,
  onValue,
} from "firebase/database";
import {
  LineChart, Line, CartesianGrid, XAxis, YAxis, Tooltip, Legend,
} from 'recharts';

const Dashboard = () => {
  const [data, setData] = useState({});
  const [chartData, setChartData] = useState({}); // sensorType -> data array
  const updateChartData = (newData) => {
    const timestamp = new Date().toLocaleTimeString();
  
    // Create a fresh copy of the chartData structure
    const updatedChartData = {};
  
    Object.entries(newData).forEach(([sensorType, sensors]) => {
      const newEntry = { name: timestamp };
  
      Object.entries(sensors).forEach(([sensorId, sensorInfo]) => {
        newEntry[sensorId] = sensorInfo.reading;
      });
  
      // Create a new array for this sensorType
      const prev = chartData[sensorType] || [];
      updatedChartData[sensorType] = [...prev, newEntry]; // Ensure max 10 entries
      console.log(prev)
    });
  
    setChartData(updatedChartData);
  };
  

  useEffect(() => {
    const db = getDatabase(firebaseConf);
    const sensorDataRef = ref(db, "/devices/esp32/latestReadings");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      const val = snapshot.val() || {};
      setData(val);
      updateChartData(val);
    });

    return () => unsubscribe();
  }, []);

  return (
    <div>
      <h1>Latest Sensor Data</h1>

      {Object.entries(data).map(([sensorType, sensors]) => (
        <div key={sensorType} style={{ marginBottom: "2rem" }}>
          <h2>{sensorType.toUpperCase()} Sensors</h2>
          <LineChart
            width={700}
            height={300}
            data={chartData[sensorType] || []}
          >
            <CartesianGrid stroke="#ccc" />
            <XAxis dataKey="name" />
            <YAxis />
            <Tooltip />
            <Legend />
            {Object.keys(sensors).map((sensorId) => (
              <Line
                key={sensorId}
                type="monotone"
                dataKey={sensorId}
                stroke="#8884d8"
                dot={false}
              />
            ))}
          </LineChart>
        </div>
      ))}
    </div>
  );
};

export default Dashboard;
