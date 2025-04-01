import { useState, useEffect } from "react";
import firebaseConf from "../firebaseConfig";
import { getDatabase, ref, query, orderByChild, limitToLast, onValue } from "firebase/database";

const Dashboard = () => {
    const [data, setData] = useState({});

    useEffect(() => {
        const db = getDatabase(firebaseConf);
        const sensorDataRef = ref(db,"/devices/esp32/sensors");

        const fetchData = onValue(sensorDataRef, (snapshot) => {
            if (!snapshot.exists()) {
                console.log("No data available");
                setData({});
                return;
            }

            const latestData = {};

            snapshot.forEach((sensorSnapshot) => {
                const sensorName = sensorSnapshot.key; // "pH"

                sensorSnapshot.forEach((sensorIdSnapshot) => {
                    const sensorId = sensorIdSnapshot.key; // "pH1"

                    // Get the last taskID using orderByChild + limitToLast(1)
                    const lastTaskQuery = query(sensorIdSnapshot.ref, orderByChild("timestamp"), limitToLast(1));

                    onValue(lastTaskQuery, (taskSnapshot) => {
                        taskSnapshot.forEach((taskData) => {
                            latestData[sensorId] = {
                                id: taskData.key, // Latest taskID
                                reading: taskData.val().reading,
                                timestamp: taskData.val().timestamp,
                            };
                        });

                        setData({ ...latestData });
                    });
                });
            });
        });

        fetchData();
    }, []);

    return (
        <div>
            <h1>Latest Sensor Data</h1>
            <ul>
                {Object.entries(data).map(([sensorId, sensorDetails]) => (
                    <li key={sensorDetails.id}>
                        <strong>{sensorId}:</strong> (Latest Reading:{sensorDetails.reading})
                    </li>
                ))}
            </ul>
        </div>
    );
};

export default Dashboard;
