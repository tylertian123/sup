import styles from './Display.module.css'
import { useEffect, useRef } from 'react';

export default function Display({ values, setValues, style = {} }) {
    const cols = values[0].length;
    const mouseDown = useRef(false);

    function mouseDownListener() {
        mouseDown.current = true;
    }
    function mouseUpListener() {
        mouseDown.current = false;
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
    }
    const leds = values.map(
        (row, i) => row.map(
            (state, j) =>
                <input key={[i, j]} type="checkbox" className={styles.led} checked={state} 
                    onChange={onChange.bind(null, i, j)}
                    onMouseOver={(event) => {
                        if (mouseDown.current) {
                            event.currentTarget.checked = !event.currentTarget.checked;
                            onChange(i, j, event);
                        }
                    }} />
        )
    );

    return <div style={{
        ...style,
        gridTemplateColumns: `repeat(${cols}, 1fr)`
    }} className={styles.matrix}>
        {leds}
    </div>;
}
