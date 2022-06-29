import firebase from 'firebase/compat/app';
import 'firebase/compat/database';

import Link from 'next/link';
import { useEffect, useRef, useState } from 'react';
import { Alert, Collapse, Container, Form, ToggleButton, ToggleButtonGroup } from 'react-bootstrap';

import Layout from '../components/Layout';
import MultiDisplay, { createDefaultValues, packValues, unpackValues } from '../components/MultiDisplay';
import CollapsedAlert from '../components/CollapsedAlert';
import TooltipWrapper, { TooltipButton } from '../components/TooltipWrapper';
import * as Icons from 'react-bootstrap-icons';

const DISPLAY_WIDTH = 4;
const DISPLAY_HEIGHT = 2;

const db = firebase.database();
try {
    if (process.env.NODE_ENV === "development") {
        db.useEmulator('localhost', 9000);
    }
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

// TODO: Add a button to reload
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
    const advancedSavePrivate = useRef(true);

    // Make sure the page is updated when auth state is changed, since the auth user starts out null
    useEffect(() =>
        firebase.auth().onAuthStateChanged((user) => {
            setLoginUser(user);
        }), []
    );

    function loadDisplayData() {
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
                            setDisplayUpdated(false);
                        }
                        catch (_e) {
                            setErrorMessage(`Error: Can't get current display data because it's not in the expected format! (current data: ${snapshot.val()})`)
                        }
                    }
                });
            }
        }).catch((err) => {
            setErrorMessage("Error: Can't fetch user data: " + err.toString());
        });
    }

    // When user is loaded, try to find where to write the data and show the alert if it's not configured
    useEffect(() => {
        if (loginUser) {
            loadDisplayData();
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
            // Choose data path based on private/public save
            const path = advancedSavePrivate.current ? `/users/${loginUser.uid}/savedData/${location}` : `/data/${location}`; 
            if (advancedMode.current === "save") {
                db.ref(path).update({
                    displayData: `blob,base64,${serializeDisplay(displayValues)}`
                }).then(() => {
                    setSuccessMessage("Saved!");
                }).catch((err) => {
                    setErrorMessage("Error: Cannot save display: " + err.toString());
                });
            }
            else if (advancedMode.current === "load") {
                db.ref(`${path}/displayData`).get().then((snapshot) => {
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
                db.ref(path).remove()
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
                        <MultiDisplay values={displayValues} setValues={setDisplayValues} setUpdated={setDisplayUpdated} disabled={!configOk}></MultiDisplay>
                    </Form.Group>
                    <TooltipButton type="submit" tooltip="Save these changes and update the physical display." disabled={!configOk || !displayUpdated}>Update</TooltipButton>
                    <TooltipButton className="ms-2" variant="danger" tooltip="Clear the editor contents." disabled={!configOk} onClick={() => {
                        setDisplayValues(createDefaultValues(DISPLAY_HEIGHT, DISPLAY_WIDTH));
                        setDisplayUpdated(true);
                    }}><Icons.XLg/></TooltipButton>
                    <TooltipButton className="ms-2" variant="danger" tooltip="Reload the saved display data and replace the current editor contents." disabled={!configOk} onClick={loadDisplayData}><Icons.ArrowClockwise/></TooltipButton>
                    <TooltipButton className="ms-2" disabled={!configOk} tooltip="Show/hide advanced options." onClick={() => setShowAdvancedOptions(!showAdvancedOptions)}><Icons.ThreeDots/></TooltipButton>
                </Form>
                <Collapse in={showAdvancedOptions}>
                    <div className="rounded border border-secondary p-2">
                        <Form noValidate validated={validated} onSubmit={handleAdvancedSubmit}>
                            <Form.Group className="mb-2">
                                <Form.Label>Save to/Load from another location</Form.Label>
                                <Form.Control name="dataLocation" required type="text" pattern="[a-zA-Z0-9-]+" className="w-50 bg-white"/>
                                <Form.Control.Feedback type="invalid">Location must be alphanumeric and can't be empty.</Form.Control.Feedback>
                                <Form.Text className="text-muted">Save the current display data to or load the display data from another location.</Form.Text>
                            </Form.Group>
                            <TooltipWrapper tooltip="Whether the data can be accessed by only you or everyone.">
                                <ToggleButtonGroup name="save-type-radio" type="radio" defaultValue={true} className="me-2" onChange={(value) => advancedSavePrivate.current = value}>
                                    <ToggleButton value={true} id="save-type-radio-1" variant="outline-secondary">Private</ToggleButton>
                                    <ToggleButton value={false} id="save-type-radio-2" variant="outline-secondary">Public</ToggleButton>
                                </ToggleButtonGroup>
                            </TooltipWrapper>
                            <TooltipButton type="submit" name="save" tooltip="Save to this location." onClick={() => advancedMode.current = "save"}>Save</TooltipButton>
                            <TooltipButton type="submit" name="load" tooltip="Load from this location into the current editor." className="mx-2" onClick={() => advancedMode.current = "load"}>Load</TooltipButton>
                            <TooltipButton type="submit" name="delete" tooltip="Delete the data at this location. This cannot be undone!" variant="danger" onClick={() => advancedMode.current = "delete"}>Delete</TooltipButton>
                        </Form>
                    </div>
                </Collapse>
            </Container>
        </Layout>
    );
}
