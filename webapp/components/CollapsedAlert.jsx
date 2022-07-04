import { Alert, Collapse } from "react-bootstrap";

export default function CollapsedAlert({ message, setMessage, timeout = null, ...props }) {
    if (message !== null && timeout) {
        window.setTimeout(() => {
            setMessage(null);
        }, timeout);
    }
    return <Collapse in={message !== null}>
        <div>
            <Alert onClose={() => setMessage(null)} {...props}>
                {message}
            </Alert>
        </div>
    </Collapse>;
}
