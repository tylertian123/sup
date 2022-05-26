import firebase from 'firebase/compat/app';
import 'firebase/compat/database';

import Link from 'next/link';
import { useEffect, useRef, useState } from 'react';
import { Alert, Button, ButtonGroup, Collapse, Container, Form, ToggleButton, ToggleButtonGroup } from 'react-bootstrap';

import Layout from '../components/Layout';
import MultiDisplay, { createDefaultValues, packValues, unpackValues } from '../components/MultiDisplay'
import CollapsedAlert from '../components/CollapsedAlert';

const DISPLAY_WIDTH = 4;
const DISPLAY_HEIGHT = 2;

const db = firebase.database();
try {
    db.useEmulator('localhost', 9000);
} catch (e) {}

function serializeDisplay(values) {
    // Call unpackValues() to unpack into 2D pixel grid, then flat() to flatten into 1D array
    // reduce() to convert every 8 bits to a numerical byte
    // Use destructuring to convert array of bytes (char codes) into binary string
    // And finally convert to base64
    return window.btoa(String.fromCharCode(...unpackValues(values).flat().reduce((data, bit, i) => {
        if (i % 8 == 0) {
            data.push(bit);
        }
        else {
            data[data.length - 1] = (data[data.length - 1] << 1) + bit;
        }
        return data;
    }, [])));
}

function deserializeDisplay(b64Data, width) {
    // Convert from base64 to binary string, then use destructuring to convert it into a character array
    // map() to convert every character to an array of 8 booleans, making a 2D array, which is then flattened
    // Then reduce to group every width booleans, making a 2D pixel grid, and then pass to packValues()
    return packValues([...window.atob(b64Data)].map(
        (c) => new Array(8).fill(null).map(
            (_e, i) => (c.charCodeAt(0) & (1 << (7 - i))) !== 0
        ))
    .flat()
    .reduce((rows, key, i) => (i % width == 0 ? rows.push([key]) : (rows[rows.length - 1].push(key)), rows), []));
}

export default function Home() {
    const [loginUser, setLoginUser] = useState(null);
    const [configOk, setConfigOk] = useState(true);
    const [drawOrClear, setDrawOrClear] = useState(true);
    const [displayValues, setDisplayValues] = useState(createDefaultValues(DISPLAY_HEIGHT, DISPLAY_WIDTH));
    const writeTo = useRef(null);

    const [successMessage, setSuccessMessage] = useState(null);
    const [errorMessage, setErrorMessage] = useState(null);

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
                // If we have a location to read the display data from, then load the current display data
                db.ref(`/data/${writeTo.current}/displayData`).get().then((snapshot) => {
                    if (snapshot.exists()) {
                        try {
                            setDisplayValues(deserializeDisplay(snapshot.val().slice("blob,base64,".length), 8 * DISPLAY_WIDTH));
                        }
                        catch (_e) {
                            setErrorMessage(`Error: Can't get current display data because it's not in the expected format! (current data: ${snapshot.val()})`)
                        }
                    }
                });
            }
        });
    }

    const handleSubmit = (event) => {
        event.preventDefault();
        event.stopPropagation();
        
        if (!loginUser) {
            setErrorMessage("Error: Not logged in! (This should not happen!)");
            return;
        }
        if (writeTo.current === null) {
            setErrorMessage("Error: Display data location is missing! (This should not happen!)");
            return;
        }
        
        // Try to serialize the data now
        db.ref(`/data/${writeTo.current}`).update({
            displayData: `blob,base64,${serializeDisplay(displayValues)}`
        }).then(() => {
            setSuccessMessage("Display updated!");
        }).catch((err) => {
            setErrorMessage("Error: Cannot update display: " + err.toString());
        });
    };

    return (
        <Layout title="Home" navbarActiveKey="home">
            <Container>
                <Collapse in={!configOk}>
                    <div>
                        <Alert variant="warning">You need to <Link href="/config">configure some settings</Link> before you can use this app.</Alert>
                    </div>
                </Collapse>
                <CollapsedAlert variant="success" dismissible timeout={3000} message={successMessage} setMessage={setSuccessMessage}/>
                <CollapsedAlert variant="danger" dismissible message={errorMessage} setMessage={setErrorMessage}/>

                
                <Form onSubmit={handleSubmit}>
                    <h3>Update Display</h3>
                    <Form.Group className="mb-2">
                        <ToggleButtonGroup type="radio" size="sm" name="mode" defaultValue={true} onChange={(value) => setDrawOrClear(value)}>
                            <ToggleButton value={true} id="mode-radio-1" variant="outline-secondary">Draw</ToggleButton>
                            <ToggleButton value={false} id="mode-radio-2" variant="outline-secondary">Erase</ToggleButton>
                        </ToggleButtonGroup>
                    </Form.Group>
                    <Form.Group className="mb-3">
                        <MultiDisplay values={displayValues} setValues={setDisplayValues} drawOrClear={drawOrClear}></MultiDisplay>
                    </Form.Group>
                    <Button type="submit" disabled={!configOk}>Save</Button>
                    <Button className="ms-2" variant="danger" disabled={!configOk} onClick={() => setDisplayValues(createDefaultValues(DISPLAY_HEIGHT, DISPLAY_WIDTH))}>Clear</Button>
                </Form>
            </Container>
        </Layout>
    );
}
