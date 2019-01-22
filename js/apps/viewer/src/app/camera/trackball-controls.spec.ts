import {PerspectiveCamera} from 'three';
import {TrackballControls} from './trackball-controls';

// TODO: Add more tests!
describe('TrackballControls', () => {
    it('should pass sanity check', () => {
        const container = document.createElement('div');
        const camera = new PerspectiveCamera();
        const controls = new TrackballControls(camera, container);
        expect(controls).toBeDefined();
    });
});
