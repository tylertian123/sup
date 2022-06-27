import { Button, OverlayTrigger, Tooltip } from "react-bootstrap";


export default function TooltipButton({ children, tooltip, triggerProps = {}, ...props }) {
    return <OverlayTrigger
        placement="right"
        delay={{ show: 400, hide: 100 }}
        {...triggerProps}
        overlay={(tooltipProps) => {
            return <Tooltip {...tooltipProps}>{tooltip}</Tooltip>;
        }}>
        {/* Wrap in a div so that the tooltip can still show on disabled buttons */}
        <div style={{ display: "inline-block" }}><Button {...props}>{children}</Button></div>
    </OverlayTrigger>;
}
