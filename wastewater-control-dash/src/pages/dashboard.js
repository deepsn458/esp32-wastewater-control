import { useState, useEffect } from "react";
import firebaseConf from "../firebaseConfig";
import {
  getDatabase,
  ref,
  onValue,
} from "firebase/database";
import { LineChart, Line, CartesianGrid, XAxis, YAxis, Tooltip, Legend } from 'recharts';

const Dashboard = () => {
  const [data, setData] = useState({});
  const [chartData, setChartData] = useState([]);

  const updateChartData = (newData) => {
    const date = new Date();
    const newEntry = { name: date.toLocaleTimeString() };

    Object.entries(newData).forEach(([sensorId, sensorDetails]) => {
      newEntry[sensorId] = sensorDetails.reading;
    });

    setChartData((prevData) => {
      const updated = [...prevData, newEntry];
      return updated.slice(-10); // Keep only the last 10 entries
    });
  };

  useEffect(() => {
    const db = getDatabase(firebaseConf);
    const sensorDataRef = ref(db, "/devices/esp32/latestReadings");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      const val = snapshot.val() || {};
      setData(val);
      updateChartData(val);
    });

    return () => unsubscribe(); // Clean up listener on unmount
  }, []);

  return (
    <div>
      <h1>Latest Sensor Data</h1>
      <ul>
        {Object.entries(data).map(([sensorId, sensorDetails]) => (
          <li key={sensorId}>
            <strong>{sensorId}:</strong> Reading: {sensorDetails.reading}
          </li>
        ))}
      </ul>

      <h2>Sensor Chart (Last 10 updates)</h2>
      <LineChart width={600} height={300} data={chartData}>
        <CartesianGrid stroke="#ccc" />
        <XAxis dataKey="name" />
        <YAxis />
        <Tooltip />
        <Legend />
        {Object.keys(data).map((sensorId) => (
          <Line
            key={sensorId}
            type="monotone"
            dataKey={sensorId}
            stroke={"#8884d8"} // random color
            dot={false}
          />
        ))}
      </LineChart>
    </div>
  );
};

export default Dashboard;
