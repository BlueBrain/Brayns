import {toAxisAngle} from './quaternion';
import {degToRad} from './utils';


// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/examples/index.htm
describe('toAxisAngle()', () => {
    test('q = 1', () => {
        const quaternion = [0, 0, 0, 1];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(1);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(0));
    });

    test('q = 0.7071 + j 0.7071', () => {
        const quaternion = [0, 0.7071, 0, 0.7071];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(1);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(90));
    });

    test('q = j', () => {
        const quaternion = [0, 1, 0, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(1);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = 0.7071 - j 0.7071', () => {
        const quaternion = [0, -0.7071, 0, 0.7071];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(-1);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(90));
    });

    // test('q = -0.7071 + j 0.7071', () => {
    //     const quaternion = [0, 0.7071, 0, -0.7071];

    //     const [x, y, z, angle] = toAxisAngle(quaternion);

    //     expect(x).toBeCloseTo(0);
    //     expect(y).toBeCloseTo(1);
    //     expect(z).toBeCloseTo(0);
    //     expect(angle).toBeCloseTo(degToRad(-90));
    // });

    test('q = 0.7071 + k 0.7071', () => {
        const quaternion = [0, 0, 0.7071, 0.7071];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(1);
        expect(angle).toBeCloseTo(degToRad(90));
    });

    test('q = 0.5 + i 0.5 + j 0.5 + k 0.5', () => {
        const quaternion = [0.5, 0.5, 0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.5774);
        expect(y).toBeCloseTo(0.5774);
        expect(z).toBeCloseTo(0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = i 0.7071 +j 0.7071', () => {
        const quaternion = [0.7071, 0.7071, 0, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.7071);
        expect(y).toBeCloseTo(0.7071);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = 0.5 - i 0.5 - j 0.5 + k 0.5', () => {
        const quaternion = [-0.5, -0.5, 0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(-0.5774);
        expect(y).toBeCloseTo(-0.5774);
        expect(z).toBeCloseTo(0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = 0.7071 - k 0.7071', () => {
        const quaternion = [0, 0, -0.7071, 0.7071];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(-1);
        expect(angle).toBeCloseTo(degToRad(90));
    });

    // test('q = -0.7071 + k 0.7071', () => {
    //     const quaternion = [0, 0, 0.7071, -0.7071];

    //     const [x, y, z, angle] = toAxisAngle(quaternion);

    //     expect(x).toBeCloseTo(0);
    //     expect(y).toBeCloseTo(0);
    //     expect(z).toBeCloseTo(1);
    //     expect(angle).toBeCloseTo(degToRad(-90));
    // });

    test('q = 0.5 - i 0.5 + j 0.5 - k 0.5', () => {
        const quaternion = [-0.5, 0.5, -0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(-0.5774);
        expect(y).toBeCloseTo(0.5774);
        expect(z).toBeCloseTo(-0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = -i 0.7071 + j 0.7071', () => {
        const quaternion = [-0.7071, 0.7071, 0, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(-0.7071);
        expect(y).toBeCloseTo(0.7071);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    // test('q = i 0.7071 - j 0.7071', () => {
    //     const quaternion = [0.7071, -0.7071, 0, 0];

    //     const [x, y, z, angle] = toAxisAngle(quaternion);

    //     expect(x).toBeCloseTo(-0.7071);
    //     expect(y).toBeCloseTo(0.7071);
    //     expect(z).toBeCloseTo(0);
    //     expect(angle).toBeCloseTo(degToRad(180));
    // });

    test('q = 0.5 + i 0.5 - j 0.5 - k 0.5', () => {
        const quaternion = [0.5, -0.5, -0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.5774);
        expect(y).toBeCloseTo(-0.5774);
        expect(z).toBeCloseTo(-0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = 0.7071 + i 0.7071', () => {
        const quaternion = [0.7071, 0, 0, 0.7071];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(1);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(90));
    });

    test('q = 0.5 + i 0.5 + j 0.5 - k 0.5', () => {
        const quaternion = [0.5, 0.5, -0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.5774);
        expect(y).toBeCloseTo(0.5774);
        expect(z).toBeCloseTo(-0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = j 0.7071 - k 0.7071', () => {
        const quaternion = [0, 0.7071, -0.7071, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(0.7071);
        expect(z).toBeCloseTo(-0.7071);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = 0.5 + i 0.5 - j 0.5 + k 0.5', () => {
        const quaternion = [0.5, -0.5, 0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.5774);
        expect(y).toBeCloseTo(-0.5774);
        expect(z).toBeCloseTo(0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = i', () => {
        const quaternion = [1, 0, 0, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(1);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = i 0.7071 - k 0.7071', () => {
        const quaternion = [0.7071, 0, -0.7071, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.7071);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(-0.7071);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = k', () => {
        const quaternion = [0, 0, 1, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(1);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = i 0.7071 + k 0.7071', () => {
        const quaternion = [0.7071, 0, 0.7071, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0.7071);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(0.7071);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = 0.7071 - i 0.7071', () => {
        const quaternion = [-0.7071, 0, 0, 0.7071];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(-1);
        expect(y).toBeCloseTo(0);
        expect(z).toBeCloseTo(0);
        expect(angle).toBeCloseTo(degToRad(90));
    });

    // test('q = -0.7071 + i 0.7071', () => {
    //     const quaternion = [0.7071, 0, 0, -0.7071];

    //     const [x, y, z, angle] = toAxisAngle(quaternion);

    //     expect(x).toBeCloseTo(1);
    //     expect(y).toBeCloseTo(0);
    //     expect(z).toBeCloseTo(0);
    //     expect(angle).toBeCloseTo(degToRad(-90));
    // });

    test('q = 0.5 - i 0.5 + j 0.5 + k 0.5', () => {
        const quaternion = [-0.5, 0.5, 0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(-0.5774);
        expect(y).toBeCloseTo(0.5774);
        expect(z).toBeCloseTo(0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });

    test('q = j 0.7071 + k 0.7071', () => {
        const quaternion = [0, 0.7071, 0.7071, 0];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(0);
        expect(y).toBeCloseTo(0.7071);
        expect(z).toBeCloseTo(0.7071);
        expect(angle).toBeCloseTo(degToRad(180));
    });

    test('q = 0.5 - i 0.5 - j 0.5 - k 0.5', () => {
        const quaternion = [-0.5, -0.5, -0.5, 0.5];

        const [x, y, z, angle] = toAxisAngle(quaternion);

        expect(x).toBeCloseTo(-0.5774);
        expect(y).toBeCloseTo(-0.5774);
        expect(z).toBeCloseTo(-0.5774);
        expect(angle).toBeCloseTo(degToRad(120));
    });
});
