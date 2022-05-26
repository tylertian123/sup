import firebase from 'firebase/compat/app';
import 'firebase/compat/database';

import { useEffect, useState } from 'react';
import { Button, Container, Form } from 'react-bootstrap';

import Layout from '../components/Layout';
import CollapsedAlert from '../components/CollapsedAlert';


const db = firebase.database();
try {
    db.useEmulator('localhost', 9000);
} catch (e) {}

export default function Config() {
    const [loginUser, setLoginUser] = useState(null);
    const [validated, setValidated] = useState(false);
    const [defaultFormData, setDefaultFormData] = useState({
        dataLocation: null,
    });

    const [successMessage, setSuccessMessage] = useState(null);
    const [errorMessage, setErrorMessage] = useState(null);

    // Make sure the page is updated when auth state is changed, since the auth user starts out null
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            setLoginUser(user);
        }), []
    );

    const handleSubmit = (event) => {
        const form = event.currentTarget;
        event.preventDefault();
        event.stopPropagation();
        if (form.checkValidity() === true) {
            if (!loginUser) {
                setErrorMessage("Error: Not logged in! (This should not happen!)");
                return;
            }
            const data = new FormData(form);
            db.ref("/users/" + loginUser.uid).set({
                writeTo: data.get("dataLocation")
            }).then(() => {
                setSuccessMessage("Configuration updated!");
            }).catch((err) => {
                console.log(err);
                setErrorMessage("Error: Cannot update configuration: " + err.toString());
            });
        }
        setValidated(true);
    };

    if (db && loginUser) {
        if (defaultFormData.dataLocation === null) {
            db.ref("/users/" + loginUser.uid + "/writeTo").get().then((snapshot) => {
                // Update existing settings if the entry exists
                if (snapshot.exists()) {
                    setDefaultFormData({
                        ...defaultFormData,
                        dataLocation: snapshot.val()
                    });
                }
            }).catch((err) => {
                console.log(err);
                setErrorMessage("Error: Cannot fetch current settings: " + err.toString());
            });
        }
    }
    return (
        <Layout title="Configuration">
            <Container>
                <CollapsedAlert variant="success" dismissible timeout={3000} message={successMessage} setMessage={setSuccessMessage}/>
                <CollapsedAlert variant="danger" dismissible message={errorMessage} setMessage={setErrorMessage}/>

                <Form noValidate validated={validated} onSubmit={handleSubmit}>
                    <Form.Group className="mb-3">
                        <Form.Label>Display Data Location</Form.Label>
                        <Form.Control name="dataLocation" required type="text" pattern="[a-zA-Z0-9-]{1,31}" className="w-50 bg-white" placeholder="e.g. myData" defaultValue={defaultFormData.dataLocation}/>
                        <Form.Control.Feedback type="invalid">Location must be alphanumeric and between 1 and 31 characters long.</Form.Control.Feedback>
                        <Form.Text className="text-muted">Where to put the display data in the database. Any device configured to read from this location will display whatever you send!</Form.Text>
                    </Form.Group>
                    <Button type="submit">Submit</Button>
                </Form>
            </Container>
        </Layout>
    );
}
