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

export function unpackValues(values) {
    const rows = values.length, cols = values[0].length, innerRows = values[0][0].length, innerCols = values[0][0][0].length;
    return new Array(rows * innerRows).fill(null).map(
        (_r, i) => new Array(cols * innerCols).fill(null).map(
            (_c, j) => values[Math.floor(i / innerRows)][Math.floor(j / innerCols)][i % innerRows][j % innerCols] 
        )); 
}

export function packValues(values, innerRows = 8, innerCols = 8) {
    if (values.length % innerRows != 0 || values[0].length % innerCols != 0) {
        throw new Error("Invalid dimensions for specified inner dimensions");
    }
    const rows = values.length / innerRows, cols = values[0].length / innerCols;
    return new Array(rows).fill(null).map(
        (_r, i) => new Array(cols).fill(null).map(
            (_c, j) => new Array(innerRows).fill(null).map(
                (_ir, k) => new Array(innerCols).fill(null).map(
                    (_ic, l) => values[i * innerRows + k][j * innerCols + l]
                ))));
}

export default function MultiDisplay({ values, setValues, drawOrClear = true, setUpdated = null, style = {} }) {

    const cols = values[0].length;
    
    function setSubValues(i, j, newValues) {
        const copy = structuredClone(values);
        copy[i][j] = newValues;
        setValues(copy);
    }

    const displays = values.map(
        (displayRow, i) => displayRow.map(
            (display, j) =>
                <Display key={[i, j]} drawOrClear={drawOrClear} values={display} setValues={setSubValues.bind(null, i, j)} setUpdated={setUpdated}/>
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
