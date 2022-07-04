import { Button, OverlayTrigger, ToggleButton, Tooltip } from "react-bootstrap";


export default function TooltipWrapper({ children, tooltip, triggerProps = {} }) {
    return <OverlayTrigger
        placement="right"
        delay={{ show: 400, hide: 100 }}
        {...triggerProps}
        overlay={(tooltipProps) => {
            return <Tooltip {...tooltipProps}>{tooltip}</Tooltip>;
        }}>
        {/* Wrap in a div so that the tooltip can still show on disabled buttons */}
        <div style={{ display: "inline-block" }}>{children}</div>
    </OverlayTrigger>;
}

export function TooltipButton({ children, tooltip, triggerProps = {}, ...props }) {
    return <TooltipWrapper tooltip={tooltip} triggerProps={triggerProps}>
        <Button {...props}>{children}</Button>
    </TooltipWrapper>;
}
