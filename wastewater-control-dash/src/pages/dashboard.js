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

        const unsubscribe = onValue(sensorDataRef, (snapshot) => {
            console.log(snapshot.val())
            if (!snapshot.exists()) {
                console.log("No data available");
                setData({});
                return;
            }

            const promises = [];

            snapshot.forEach((sensorTypeSnap) => {
                sensorTypeSnap.forEach((sensorIdSnap) => {
                    const sensorId = sensorIdSnap.key;
                    const latestReadingQuery = query(sensorIdSnap.ref, orderByChild("timestamp"), limitToLast(1));

                    const promise = new Promise((resolve) => {
                        onValue(latestReadingQuery, (readingSnap) => {
                            readingSnap.forEach((childSnap) => {
                                resolve({
                                    sensorId,
                                    data: {
                                        id: childSnap.key,
                                        reading: childSnap.val().reading,
                                        timestamp: childSnap.val().timestamp,
                                    },
                                });
                            });
                        }, {
                            onlyOnce: true 
                        });
                    });

                    promises.push(promise);
                });
            });

            Promise.all(promises).then((results) => {
                const newData = {};
                results.forEach(({ sensorId, data }) => {
                    newData[sensorId] = data;
                });
                setData(newData);
            });
        });

        // Optional: return unsubscribe function for cleanup
        return () => unsubscribe();
    }, []);

    return (
        <div>
            <h1>Latest Sensor Data</h1>
            <ul>
                {Object.entries(data).map(([sensorId, sensorDetails]) => (
                    <li key={sensorDetails.id}>
                        <strong>{sensorId}:</strong> Latest Reading: {sensorDetails.reading}
                    </li>
                ))}
            </ul>
        </div>
    );
};

export default Dashboard;
