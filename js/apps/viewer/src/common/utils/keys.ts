/**
 * Check if keyboard key press event has CTRL/CMD key pressed
 * @see https://stackoverflow.com/a/5500536/1092007
 */
export function isCmdKey(evt: KeyboardEvent | MouseEvent) {
    return evt.ctrlKey || evt.metaKey;
}
