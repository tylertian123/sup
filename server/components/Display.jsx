import styles from './Display.module.css'
import { useEffect, useRef } from 'react';

export default function Display({ values, setValues, drawOrClear = true, setUpdated = null, style = {} }) {
    const cols = values[0].length;
    const leftDown = useRef(false);
    const rightDown = useRef(false);

    function mouseDownListener(event) {
        if (event.button === 0) {
            leftDown.current = true;
        }
        else if (event.button === 2) {
            rightDown.current = true;
        }
    }
    function mouseUpListener(event) {
        if (event.button === 0) {
            leftDown.current = false;
        }
        else if (event.button === 2) {
            rightDown.current = false;
        }
    }
    useEffect(() => {
        document.addEventListener("mousedown", mouseDownListener);
        document.addEventListener("mouseup", mouseUpListener);
        return () => {
            document.removeEventListener("mousedown", mouseDownListener);
            document.removeEventListener("mouseup", mouseUpListener);
        };
    });

    function onChange(i, j, event) {
        const copy = values.map((e) => [...e]);
        copy[i][j] = event.currentTarget.checked;
        setValues(copy);
        if (setUpdated) {
            setUpdated(true);
        }
    }
    const leds = values.map(
        (row, i) => row.map(
            (state, j) =>
                <input key={[i, j]} type="checkbox" className={styles.led} checked={state}
                    onContextMenu={(event) => {
                        event.currentTarget.checked = !event.currentTarget.checked;
                        event.preventDefault();
                        onChange(i, j, event);
                        return false;
                    }}
                    onChange={onChange.bind(null, i, j)}
                    onMouseMove={(event) => {
                        if (leftDown.current) {
                            event.currentTarget.checked = drawOrClear;
                            onChange(i, j, event);
                        }
                        if (rightDown.current) {
                            event.currentTarget.checked = !drawOrClear;
                            onChange(i, j, event);
                        }
                    }} />
        )
    );

    return <div style={{
        ...style,
        gridTemplateColumns: `repeat(${cols}, 1fr)`
    }} className={styles.matrix} onContextMenu={(e) => {
        e.preventDefault();
        return false;
    }}>
        {leds}
    </div>;
}
