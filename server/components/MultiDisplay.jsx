import Display from './Display';
import styles from './MultiDisplay.module.css'

export default function MultiDisplay({rows, cols, innerRows = 8, innerCols = 8, style = {}}) {
    return <div style={style} className={styles.outer}>
        <div style={{
            gridTemplateColumns: `repeat(${cols}, auto)`
        }} className={styles.inner}>
            {Array(rows * cols).fill(<Display rows={innerRows} cols={innerCols} />)}
        </div>
    </div>;
}
