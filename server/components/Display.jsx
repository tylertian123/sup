import styles from './Display.module.css'

export default function Display({rows, cols, style = {}}) {
    return <div style={{
        ...style,
        gridTemplateColumns: `repeat(${cols}, 1fr)`
    }} className={styles.matrix}>
        {Array(rows * cols).fill(<input type="checkbox" className={styles.led}></input>)}
    </div>;
}
