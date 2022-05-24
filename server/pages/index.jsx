import firebase from 'firebase/compat/app';
import 'firebase/compat/database';
import { useEffect, useState } from 'react';
import { Button, Container, Form } from 'react-bootstrap';
import Layout from '../components/layout';

const db = firebase.database();

export default function Home() {
    const [loginUser, setLoginUser] = useState(null);

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
        
        if (!loginUser) {
            alert("Error: Not logged in!");
            return;
        }
        db.ref("/users/" + loginUser.uid + "/writeTo").get().then((snapshot) => {
            if (!snapshot.exists()) {
                alert("Error: Don't know where to write to!");
                return;
            }
            const data = new FormData(form);
            db.ref("/data/" + snapshot.val()).set({
                testData: data.get("testData")
            });
        });
    };

    return (
        <Layout title="Home" navbarActiveKey="home">
            <Container>
                <Form onSubmit={handleSubmit}>
                    <Form.Group className="mb-3">
                        <Form.Label>Send Test Data</Form.Label>
                        <Form.Control name="testData" required type="text" className="w-50 bg-white"/>
                    </Form.Group>
                    <Button type="submit">Submit</Button>
                </Form>
            </Container>
        </Layout>
    );
}
