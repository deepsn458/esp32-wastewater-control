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
  const COLORS = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b'];

  const [data, setData] = useState({
    ph: {},
    conductivity: {},
    dissolved02: {},
    temperature: {},
    voltage: {}
    //pressure:{}
  });
  const [pHChartData, setpHChartData] = useState([]);
  const [tempChartData, setTempChartData] = useState([]);
  const [condChartData, setCondChartData] = useState([]);
  const [dOChartData, setDOChartData] = useState([]);
  const [voltageChartData, setVoltageChartData] = useState([]);
  //const [pressureChartData, setPressureChartData] = useState([]);

  const updateChartData = (newData) => {
    const date = new Date();
    const newPHEntry = { name: date.toLocaleTimeString() };
    const newTempEntry = { name: date.toLocaleTimeString() };
    const newCondEntry = { name: date.toLocaleTimeString() };
    const newDOEntry = { name: date.toLocaleTimeString() };
    const newVoltageEntry = {name: date.toLocaleTimeString()};
    //const newPressureEntry = { name: date.toLocaleTimeString() };
    console.log(newData.ph)
    Object.entries(newData.ph).forEach(([sensorId, sensorDetails]) =>{
      newPHEntry[sensorId] = sensorDetails.reading;
    });
    setpHChartData((prevData) => {
      const updated = [...prevData, newPHEntry];
      return updated.slice(-60);
    });

    Object.entries(newData.conductivity).forEach(([sensorId, sensorDetails]) =>{
      newCondEntry[sensorId] = sensorDetails.reading;
    });
    setCondChartData((prevData) => {
      const updated = [...prevData, newCondEntry];
      return updated.slice(-100);
    });

    Object.entries(newData.dissolved02).forEach(([sensorId, sensorDetails]) =>{
      newDOEntry[sensorId] = sensorDetails.reading;
    });
    setDOChartData((prevData) => {
      const updated = [...prevData, newDOEntry];
      return updated.slice(-100);
    });

    Object.entries(newData.temperature).forEach(([sensorId, sensorDetails]) =>{
      newTempEntry[sensorId] = sensorDetails.reading;
    });
    setTempChartData((prevData) => {
      const updated = [...prevData, newTempEntry];
      return updated.slice(-100);
    });
    
    Object.entries(newData.voltage).forEach(([sensorId, sensorDetails]) =>{
      newVoltageEntry[sensorId] = sensorDetails.reading;
    });
    setVoltageChartData((prevData) => {
      const updated = [...prevData, newVoltageEntry];
      return updated.slice(-100);
    });
    
    
    /*
    Object.entries(newData).forEach(([sensorId, sensorDetails]) => {
      newEntry[sensorId] = sensorDetails.reading;
    });

    setChartData((prevData) => {
      const updated = [...prevData, newEntry];
      return updated.slice(-10); // Keep only the last 10 entries
    });
    */
  };

  useEffect(() => {
    const db = getDatabase(firebaseConf);
    const sensorDataRef = ref(db, "/devices/esp32/latestReadings");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      const val = snapshot.val() || {};
      console.log(val)
      setData(val);
      updateChartData(val);
    });

    return () => unsubscribe(); // Clean up listener on unmount
  }, []);

  return (
    <div>
      <h1>Latest Sensor Data</h1>
      <ul>
        {Object.entries(data.ph).map(([sensorId, sensorDetails]) => (
          <li key={sensorId}>
            <strong>{sensorId}:</strong> Reading: {sensorDetails.reading}
          </li>
        ))}
        {Object.entries(data.dissolved02).map(([sensorId, sensorDetails]) => (
          <li key={sensorId}>
            <strong>{sensorId}:</strong> Reading: {sensorDetails.reading}
          </li>
        ))}
        {Object.entries(data.conductivity).map(([sensorId, sensorDetails]) => (
          <li key={sensorId}>
            <strong>{sensorId}:</strong> Reading: {sensorDetails.reading}
          </li>
        ))}
        {Object.entries(data.temperature).map(([sensorId, sensorDetails]) => (
          <li key={sensorId}>
            <strong>{sensorId}:</strong> Reading: {sensorDetails.reading}
          </li>
        ))}
        {Object.entries(data.voltage).map(([sensorId, sensorDetails]) => (
          <li key={sensorId}>
            <strong>{sensorId}:</strong> Reading: {sensorDetails.reading}
          </li>
        ))}
      </ul>

      <h2>Sensor Chart (Last 10 updates)</h2>
      <div style={{ display: 'flex', flexWrap: 'wrap', justifyContent: 'center', gap: '2rem' }}>
      <h3>pH Sensor Data</h3>
        <LineChart width={600} height={300} data={pHChartData}>
          <CartesianGrid stroke="#ccc" />
          <XAxis dataKey="name" />
          <YAxis />
          <Tooltip />
          <Legend />
          {Object.keys(data.ph).map((sensorId,index) => (
            <Line
              key={sensorId}
              type="monotone"
              dataKey={sensorId}
              stroke={COLORS[index % COLORS.length]} 
              dot={false}
            />
          ))}
        </LineChart>
        <h3>Dissolved Oxygen</h3>
        <LineChart width={600} height={300} data={dOChartData}>
          <CartesianGrid stroke="#ccc" />
          <XAxis dataKey="name" />
          <YAxis />
          <Tooltip />
          <Legend />
          {Object.keys(data.dissolved02).map((sensorId,index) => (
            <Line
              key={sensorId}
              type="monotone"
              dataKey={sensorId}
              stroke={COLORS[index % COLORS.length]} 
              dot={false}
            />
          ))}
        </LineChart>
        <h3>Conductivity</h3>
        <LineChart width={600} height={300} data={condChartData}>
          <CartesianGrid stroke="#ccc" />
          <XAxis dataKey="name" />
          <YAxis />
          <Tooltip />
          <Legend />
          {Object.keys(data.conductivity).map((sensorId,index) => (
            <Line
              key={sensorId}
              type="monotone"
              dataKey={sensorId}
              stroke={COLORS[index % COLORS.length]} // random color
              dot={false}
            />
          ))}
        </LineChart>
        <h3>Temperature</h3>
        <LineChart width={600} height={300} data={tempChartData}>
          <CartesianGrid stroke="#ccc" />
          <XAxis dataKey="name" />
          <YAxis />
          <Tooltip />
          <Legend />
          {Object.keys(data.temperature).map((sensorId,index) => (
            <Line
              key={sensorId}
              type="monotone"
              dataKey={sensorId}
              stroke={COLORS[index % COLORS.length]} // random color
              dot={false}
            />
          ))}
        </LineChart>
        </div>
    </div>
  );
};
export default Dashboard;
