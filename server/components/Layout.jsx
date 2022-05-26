import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';

import { useRouter } from 'next/router';
import { useState, useEffect } from 'react';
import Head from "next/head";
import Link from "next/link";
import { Button, Container, Nav, Navbar, Spinner } from "react-bootstrap";

import styles from './Layout.module.css'


const firebaseConfig = require('../pages/firebase-config.json').result.sdkConfig;
const firebaseApp = firebase.initializeApp(firebaseConfig);

export default function Layout({ children, title = "Sup", requireSignIn = true }) {
    const [loginUser, setLoginUser] = useState(null);

    const router = useRouter();
    // Register auth state callback
    // Update login user and UI if logged in, otherwise redirect to login page
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            if (requireSignIn) {
                if (user) {
                    setLoginUser(user);
                }
                else {
                    setLoginUser(null);
                    router.replace('/login');
                }
            }
        }), []
    );

    return (
        <>
            <Head>
                <title>{title}</title>
            </Head>
            <Navbar bg="dark" variant="dark" className="mb-3">
                <Container>
                    <Navbar.Brand>Sup</Navbar.Brand>
                    <Nav className="me-auto" activeKey={router.asPath}>
                        <Link href="/" passHref><Nav.Link>Home</Nav.Link></Link>
                        <Link href="/config" passHref><Nav.Link>Config</Nav.Link></Link>
                    </Nav>
                    {loginUser ?
                        <Navbar.Collapse className="justify-content-end">
                            <Navbar.Text><u>{loginUser.email}</u></Navbar.Text>
                            <Button className="mx-2" size="sm" onClick={() => firebase.auth().signOut().catch((err) => console.log(err))}>Logout</Button>
                        </Navbar.Collapse>
                    : 
                        <Navbar.Collapse className="justify-content-end">
                            <Link href="/login" passHref><Nav.Link>Log in</Nav.Link></Link>
                        </Navbar.Collapse>
                    }
                </Container>
            </Navbar>

            {requireSignIn && !loginUser
                ? <div className={styles.spinner}><Spinner animation="border" role="status" /></div>
                : children}
        </>
    );
}
