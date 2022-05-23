import { useRouter } from 'next/router';
import { useState, useEffect } from 'react';
import Head from "next/head";
import { Container, Navbar, Spinner } from "react-bootstrap";

import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';

const firebaseConfig = require('../pages/firebase-config.json').result.sdkConfig;
const firebaseApp = firebase.initializeApp(firebaseConfig);

export default function Layout({ children, title = "Sup" }) {
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
                <title>{title}</title>
            </Head>
            <Navbar bg="light" variant="light">
                <Container>
                    <Navbar.Brand>Navbar</Navbar.Brand>
                </Container>
            </Navbar>

            {!loggedIn ? <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100vh' }}>
                <Spinner animation="border" role="status" /></div>
                : null}
            <div style={{ visibility: loggedIn ? 'visible' : 'hidden' }}>{children}</div>
        </>
    );
}
