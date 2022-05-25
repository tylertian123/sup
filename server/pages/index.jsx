import firebase from 'firebase/compat/app';
import 'firebase/compat/database';
import Link from 'next/link';
import { useEffect, useRef, useState } from 'react';
import { Alert, Button, Container, Form } from 'react-bootstrap';
import Layout from '../components/Layout';
import MultiDisplay, { createDefaultValues, packValues, unpackValues } from '../components/MultiDisplay'

const db = firebase.database();
try {
    db.useEmulator('localhost', 9000);
} catch (e) {}

export default function Home() {
    const [loginUser, setLoginUser] = useState(null);
    const [configOk, setConfigOk] = useState(true);
    const [displayValues, setDisplayValues] = useState(createDefaultValues(2, 2));
    const writeTo = useRef(null);

    // Make sure the page is updated when auth state is changed, since the auth user starts out null
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            setLoginUser(user);
        }), []
    );

    // When user is loaded, try to find where to write the data and show the alert if it's not configured
    if (db && loginUser && writeTo.current === null && configOk) {
        db.ref(`/users/${loginUser.uid}/writeTo`).get().then((snapshot) => {
            if (!snapshot.exists()) {
                setConfigOk(false);
            }
            else {
                writeTo.current = snapshot.val();
            }
        });
    }

    const handleSubmit = (event) => {
        const form = event.currentTarget;
        event.preventDefault();
        event.stopPropagation();
        
        if (!loginUser) {
            alert("Error: Not logged in!");
            return;
        }
        if (writeTo.current === null) {
            alert("Error: Don't know where to write to!");
            return;
        }
        const data = new FormData(form);
        db.ref("/data/" + writeTo.current).set({
            testData: data.get("testData")
        });
    };

    return (
        <Layout title="Home" navbarActiveKey="home">
            <Container>
                {!configOk ? <Alert variant="warning">You need to <Link href="/config">configure some settings</Link> before you can use this app.</Alert> : null}
                <Form onSubmit={handleSubmit}>
                    <Form.Group className="mb-3">
                        <Form.Label>Send Test Data</Form.Label>
                        <Form.Control name="testData" required type="text" className="w-50 bg-white"/>
                    </Form.Group>
                    <Button type="submit">Submit</Button>
                </Form>
                <MultiDisplay values={displayValues} setValues={setDisplayValues}></MultiDisplay>
            </Container>
        </Layout>
    );
}
