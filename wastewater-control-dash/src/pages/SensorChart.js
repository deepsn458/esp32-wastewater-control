import { useState, useEffect } from "react";
import firebaseConf from "../firebaseConfig";
import { getDatabase, ref, query, orderByChild, limitToLast, onValue } from "firebase/database";
import { LineChart, Line, XAxis, YAxis, Tooltip, CartesianGrid, ResponsiveContainer } from "recharts";

const SensorChart = ({ sensorType, sensorId }) => {
    const [sensorData, setSensorData] = useState([]); 

    useEffect(() => {
        if (!sensorType || !sensorId) return; // Ensure props are valid

        const db = getDatabase(firebaseConf);
        const sensorRef = ref(db, `/devices/esp32/sensors/${sensorType}/${sensorId}`);

        // Fetch only the last task entry
        const sensorQuery = query(sensorRef, orderByChild("timestamp"), limitToLast(1));

        const unsubscribe = onValue(sensorQuery, (snapshot) => {
            console.log(snapshot.value())
            if (!snapshot.exists()) {
                console.log("No data available");
                setSensorData([]);
                return;
            }

            const newData = [];
            snapshot.forEach((taskSnapshot) => {
                taskSnapshot.forEach((data) => {
                    const val = data.val();
                    if (val && val.timestamp && val.value !== undefined) {
                        newData.push({
                            time: new Date(val.timestamp * 1000).toLocaleTimeString(), // Convert Unix timestamp to readable time
                            value: val.value, // Sensor value
                        });
                    }
                });
            });

            setSensorData(newData);
        });

        return () => unsubscribe(); // Cleanup listener

    }, [sensorType, sensorId]);

    return (
        <div>
            <h2>Sensor Data: {sensorType} - {sensorId}</h2>
            <ResponsiveContainer width="100%" height={300}>
                <LineChart data={sensorData}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="time" />
                    <YAxis />
                    <Tooltip />
                    <Line type="monotone" dataKey="value" stroke="#8884d8" strokeWidth={2} />
                </LineChart>
            </ResponsiveContainer>
        </div>
    );
};

export default SensorChart;
