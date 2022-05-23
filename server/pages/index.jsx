import { useRouter } from 'next/router';
import { useState, useEffect } from 'react';
import Head from 'next/head';

import { Oval } from 'react-loader-spinner';

import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';

const firebaseConfig = require('./firebase-config.json').result.sdkConfig;
const firebaseApp = firebase.initializeApp(firebaseConfig);


function Home() {
    return (
        <>
            <p>You are logged in!</p>
        </>
    );
}


export default function HomeLoader() {
    const [loggedIn, setLoggedIn] = useState(false);

    const router = useRouter();
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            if (user) {
                setLoggedIn(true);
            }
            else {
                router.replace('/login');
            }
        }), []
    );

    return (
        <>
            <Head>
                <title>Home</title>
            </Head>
            {!loggedIn ? <div style={{display: 'flex', justifyContent:'center', alignItems:'center', height: '100vh'}}>
                <Oval height={75} width={75} strokeWidth={4} color='blue' secondaryColor='grey'/></div>
                : null}
            <div style={{visibility: loggedIn ? 'visible' : 'hidden'}}><Home/></div>
        </>
    );
}
