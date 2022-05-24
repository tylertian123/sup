import firebase from 'firebase/compat/app';
import 'firebase/compat/database';
import { useEffect, useState } from 'react';
import { Alert, Button, Container, Form } from 'react-bootstrap';
import Layout from '../components/layout';


const db = firebase.database();


export default function Config() {
    const [loginUser, setLoginUser] = useState(null);
    const [validated, setValidated] = useState(false);
    const [success, setSuccess] = useState(false);
    const [defaultFormData, setDefaultFormData] = useState({
        dataLocation: null,
    });

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
                alert("Error: Not logged in!");
                return;
            }
            const data = new FormData(form);
            db.ref("/users/" + loginUser.uid).set({
                writeTo: data.get("dataLocation")
            }).then(() => {
                setSuccess(true);
                window.setTimeout(() => {
                    setSuccess(false);
                }, 3000);
            }).catch((err) => {
                console.log(err);
            });
        }
        setValidated(true);
    };

    if (db && loginUser) {
        if (defaultFormData.dataLocation === null) {
            db.ref("/users/" + loginUser.uid + "/writeTo").get().then((snapshot) => {
                if (snapshot.exists()) {
                    setDefaultFormData({
                        ...defaultFormData,
                        dataLocation: snapshot.val()
                    });
                }
            });
        }
    }
    return (
        <Layout title="Configuration">
            <Container>
                {success ? <Alert variant="success" dismissible onClose={() => setSuccess(false)}>Success!</Alert> : null}
                <Form noValidate validated={validated} onSubmit={handleSubmit}>
                    <Form.Group className="mb-3">
                        <Form.Label>Display Data Location</Form.Label>
                        <Form.Control name="dataLocation" required type="text" pattern="[a-zA-Z0-9-]+" className="w-50 bg-white" placeholder="e.g. myData" defaultValue={defaultFormData.dataLocation}/>
                        <Form.Control.Feedback type="invalid">Location can only contain alphanumeric characters and dashes and can't be empty.</Form.Control.Feedback>
                        <Form.Text className="text-muted">Where to put the display data in the database. Any device configured to read from this location will display whatever you send!</Form.Text>
                    </Form.Group>
                    <Button type="submit">Submit</Button>
                </Form>
            </Container>
        </Layout>
    );
}
