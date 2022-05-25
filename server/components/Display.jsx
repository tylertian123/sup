import styles from './Display.module.css'
import { useState } from 'react';

export default function Display({ values, setValues, style = {} }) {
    const cols = values[0].length;
    const leds = values.map(
        (row, i) => row.map(
            (state, j) =>
                <input key={[i, j]} type="checkbox" className={styles.led} checked={state} onChange={(event) => {
                    const copy = values.map((e) => [...e]);
                    copy[i][j] = event.currentTarget.checked;
                    setValues(copy);
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
