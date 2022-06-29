import firebase from 'firebase/compat/app';
import 'firebase/compat/auth';

import { useRouter } from 'next/router';
import { useEffect } from 'react';
import { Container } from 'react-bootstrap';
import StyledFirebaseAuth from 'react-firebaseui/StyledFirebaseAuth';

import Layout from '../components/Layout';

const firebaseConfig = require('./firebase-config.json').result.sdkConfig;
const firebaseApp = firebase.initializeApp(firebaseConfig);

const uiConfig = {
    signInFlow: 'popup',
    signInSuccessUrl: '/',
    signInOptions: [
        {
            provider: firebase.auth.EmailAuthProvider.PROVIDER_ID,
            requireDisplayName: false,
            disableSignUp: {
                status: true,
                adminEmail: "tylertian123@gmail.com",
            }
        }
    ]
}


export default function Index() {
    const router = useRouter();
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            if (user) {
                router.replace('/');
            }
        }), []
    );
    return (
        <Layout title="Sign In" requireSignIn={false}>
            <Container>
                <StyledFirebaseAuth uiConfig={uiConfig} firebaseAuth={firebase.auth()}/>
            </Container>
        </Layout>
    );
}
