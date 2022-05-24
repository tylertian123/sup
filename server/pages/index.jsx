import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';
import { Container } from 'react-bootstrap';
import Layout from '../components/layout';

// const firebaseConfig = require('./firebase-config.json').result.sdkConfig;
// const firebaseApp = firebase.initializeApp(firebaseConfig);

export default function Home() {

    return (
        <Layout title="Home" navbarActiveKey="home">
            <Container>
                <p>You are logged in!</p>
            </Container>
        </Layout>
    );
}
