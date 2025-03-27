// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getDatabase } from "firebase/database";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyCRiDbuGOOqW_q_OKA_-zeCXVWCqW3gJe4",
  authDomain: "waste-water-control.firebaseapp.com",
  databaseURL: "https://waste-water-control-default-rtdb.firebaseio.com",
  projectId: "waste-water-control",
  storageBucket: "waste-water-control.firebasestorage.app",
  messagingSenderId: "639856624447",
  appId: "1:639856624447:web:a2f956b380607939ab898a",
  measurementId: "G-P3LNNXVF5V"
};

// Initialize Firebase
const firebaseConf = initializeApp(firebaseConfig);
const analytics = getAnalytics(firebaseConf);
const database = getDatabase(firebaseConf);

export default firebaseConf;