import { useState } from 'react';
import Display from './Display';
import styles from './MultiDisplay.module.css'

export function createDefaultValues(rows, cols, innerRows = 8, innerCols = 8) {
    return new Array(rows).fill(null).map(
        () => new Array(cols).fill(null).map(
            () => new Array(innerRows).fill(null).map(
                () => new Array(innerCols).fill(false)
            )
        ));
}

export default function MultiDisplay({ values, setValues, style = {} }) {

    const cols = values[0].length;
    
    function setSubValues(i, j, newValues) {
        const copy = structuredClone(values);
        copy[i][j] = newValues;
        setValues(copy);
    }

    const displays = values.map(
        (displayRow, i) => displayRow.map(
            (display, j) =>
                <Display key={[i, j]} values={display} setValues={setSubValues.bind(null, i, j)}/>
        )
    );
    
    return <div style={style} className={styles.outer}>
        <div style={{
            gridTemplateColumns: `repeat(${cols}, auto)`
        }} className={styles.inner}>
            {displays}
        </div>
    </div>;
}
