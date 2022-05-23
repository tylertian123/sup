import { useRouter } from 'next/router';
import { useState, useEffect } from 'react';
import Head from "next/head";
import { Button, Container, Nav, Navbar, Spinner } from "react-bootstrap";

import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';

const firebaseConfig = require('../pages/firebase-config.json').result.sdkConfig;
const firebaseApp = firebase.initializeApp(firebaseConfig);

export default function Layout({ children, title = "Sup" }) {
    const [loginUser, setLoginUser] = useState(null);

    const router = useRouter();
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            if (user) {
                setLoginUser(user);
            }
            else {
                setLoginUser(null);
                router.replace('/login');
            }
        }), []
    );

    return (
        <>
            <Head>
                <title>{title}</title>
            </Head>
            <Navbar bg="dark" variant="dark">
                <Container>
                    <Navbar.Brand>Navbar</Navbar.Brand>
                    {loginUser ?
                        <Navbar.Collapse className="justify-content-end">
                            <Navbar.Text><u>{loginUser.email}</u></Navbar.Text>
                            <Button className="mx-2" size="sm" onClick={() => firebase.auth().signOut().catch((err) => console.log(err))}>Logout</Button>
                        </Navbar.Collapse>
                    : null}
                </Container>
            </Navbar>

            {!loginUser ? <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100vh' }}>
                <Spinner animation="border" role="status" /></div>
                : null}
            <div style={{ visibility: loginUser ? 'visible' : 'hidden' }}>{children}</div>
        </>
    );
}
