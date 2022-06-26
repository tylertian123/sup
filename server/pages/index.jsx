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
    const [displayValues, setDisplayValues] = useState(createDefaultValues(DISPLAY_HEIGHT, DISPLAY_WIDTH));
    const [displayUpdated, setDisplayUpdated] = useState(false);
    const writeTo = useRef(null);
    
    const [successMessage, setSuccessMessage] = useState(null);
    const [errorMessage, setErrorMessage] = useState(null);

    const [showAdvancedOptions, setShowAdvancedOptions] = useState(false);
    const [validated, setValidated] = useState(false);
    const advancedMode = useRef(null);

    // Make sure the page is updated when auth state is changed, since the auth user starts out null
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            setLoginUser(user);
        }), []
    );

    // When user is loaded, try to find where to write the data and show the alert if it's not configured
    useEffect(() => {
        if (loginUser) {
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
    }, [loginUser]);

    const handleDisplaySubmit = (event) => {
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
            setDisplayUpdated(false);
        }).catch((err) => {
            setErrorMessage("Error: Cannot update display: " + err.toString());
        });
    };

    const handleAdvancedSubmit = (event) => {
        const form = event.currentTarget;
        event.preventDefault();
        event.stopPropagation();
        if (form.checkValidity() === true) {
            if (!loginUser) {
                setErrorMessage("Error: Not logged in! (This should not happen!)");
                return;
            }
            const data = new FormData(form);
            const location = data.get("dataLocation");
            if (advancedMode.current === "save") {
                db.ref(`/data/${location}`).update({
                    displayData: `blob,base64,${serializeDisplay(displayValues)}`
                }).then(() => {
                    setSuccessMessage("Saved!");
                }).catch((err) => {
                    setErrorMessage("Error: Cannot save display: " + err.toString());
                });
            }
            else if (advancedMode.current === "load") {
                db.ref(`/data/${location}/displayData`).get().then((snapshot) => {
                    if (snapshot.exists()) {
                        try {
                            setDisplayValues(deserializeDisplay(snapshot.val().slice("blob,base64,".length), 8 * DISPLAY_WIDTH));
                            setSuccessMessage("Loaded!");
                            setDisplayUpdated(true);
                        }
                        catch (_e) {
                            setErrorMessage(`Error: Can't load display data because it's not in the expected format! (data: ${snapshot.val()})`)
                        }
                    }
                    else {
                        setErrorMessage(`No display data is stored at ${location}!`);
                    }
                });
            }
            else if (advancedMode.current === "delete") {
                db.ref(`/data/${location}`).remove()
                .then(() => {
                    setSuccessMessage("Deleted!");
                }).catch((err) => {
                    setErrorMessage("Error: Cannot delete: " + err.toString());
                });
            }
            else {
                setErrorMessage("Internal error: Invalid mode: " + advancedMode.current);
            }
        }
        setValidated(true);
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

                
                <Form onSubmit={handleDisplaySubmit} className="mb-3">
                    <h3>Update Display</h3>
                    <Form.Group>
                        <Form.Label>Click to toggle each pixel. Left-click and drag to draw; right-click and drag to erase.</Form.Label>
                    </Form.Group>
                    <Form.Group className="mb-3">
                        <MultiDisplay values={displayValues} setValues={setDisplayValues} setUpdated={setDisplayUpdated}></MultiDisplay>
                    </Form.Group>
                    <Button type="submit" disabled={!configOk || !displayUpdated}>Save</Button>
                    <Button className="ms-2" variant="danger" disabled={!configOk} onClick={() => {
                        setDisplayValues(createDefaultValues(DISPLAY_HEIGHT, DISPLAY_WIDTH));
                        setDisplayUpdated(true);
                    }}>Clear</Button>
                    <Button className="ms-2" disabled={!configOk} onClick={() => setShowAdvancedOptions(!showAdvancedOptions)}>Advanced Options</Button>
                </Form>
                <Collapse in={showAdvancedOptions}>
                    <div className="rounded border border-secondary p-2">
                        <Form noValidate validated={validated} onSubmit={handleAdvancedSubmit}>
                            <Form.Group className="mb-2">
                                <Form.Label>Save to/Load from</Form.Label>
                                <Form.Control name="dataLocation" required type="text" pattern="[a-zA-Z0-9-]+" className="w-50 bg-white"/>
                                <Form.Control.Feedback type="invalid">Location must be alphanumeric and can't be empty.</Form.Control.Feedback>
                                <Form.Text className="text-muted">Save the current display data to or load the display data from another location.</Form.Text>
                            </Form.Group>
                            <Button type="submit" name="save" onClick={() => advancedMode.current = "save"}>Save</Button>
                            <Button type="submit" name="load" className="mx-2" onClick={() => advancedMode.current = "load"}>Load</Button>
                            <Button type="submit" name="delete" variant="danger" onClick={() => advancedMode.current = "delete"}>Delete</Button>
                        </Form>
                    </div>
                </Collapse>
            </Container>
        </Layout>
    );
}
