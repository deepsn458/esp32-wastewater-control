import { useState, useEffect } from "react";
import firebaseConf from "../firebaseConfig";
import {
    getDatabase,
    ref,
    onValue,
    query,
    orderByChild,
    limitToLast,
} from "firebase/database";

const Dashboard = () => {
    const [data, setData] = useState({});

    useEffect(() => {
        const db = getDatabase(firebaseConf);
        const sensorDataRef = ref(db, "/devices/esp32/latestReadings");
        onValue(sensorDataRef, (snapshot) => {
            setData(snapshot.val() || {});
        });         
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
        </div>
    );
};

export default Dashboard;
