import { useRouter } from 'next/router';
import { useState, useEffect } from 'react';
import Head from 'next/head';

import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';

const firebaseConfig = require('./firebase-config.json').result.sdkConfig;
const firebaseApp = firebase.initializeApp(firebaseConfig);


export default function Home() {
    const [loggedIn, setLoggedIn] = useState(false);

    const router = useRouter();
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            if (user) {
                setLoggedIn(true);
            }
            else {
                router.replace("/login");
            }
        }), []
    );

    return (
        <>
            <Head>
                <title>Home</title>
            </Head>
            <p>You {loggedIn ? "are" : "are not"} signed in!</p>
        </>
    );
}
