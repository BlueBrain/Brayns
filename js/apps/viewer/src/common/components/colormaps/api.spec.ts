import fetchMock from 'fetch-mock';
import {getColormap, getColormapsList} from './api';

const colormapList = ['viridis'];
const viridis = ['#fff', '#000'];

afterEach(() => {
    fetchMock.restore();
});

describe('getColormapList()', () => {
    it('should fetch available colormaps from colormaps.io', async () => {
        const fetchSpy = jest.fn();

        fetchMock.mock('*', url => {
            fetchSpy(url);
            return {
                body: colormapList,
                status: 200
            };
        });

        const items = await getColormapsList();

        expect(fetchSpy).toHaveBeenCalledWith('https://api.colormaps.io/colormap');
        expect(items).toEqual(colormapList);
    });

    it('should return [] if fetch fails', async () => {
        fetchMock.mock('*', {
            throws: true,
            status: 404
        });

        const items = await getColormapsList();
        expect(items).toEqual([]);
    });
});

describe('getColormap()', () => {
    it('should fetch colormap from colormaps.io with format=hex by default', async () => {
        const fetchColormapSpy = jest.fn();
        fetchMock.mock('*', url => {
            fetchColormapSpy(url);
            return {
                body: [...viridis],
                status: 200
            };
        });

        const colormap = await getColormap('viridis');

        expect(fetchColormapSpy).toHaveBeenCalledWith('https://api.colormaps.io/colormap/viridis?format=hex');
        expect(colormap).toEqual(viridis);
    });

    it('should use specified format', async () => {
        const fetchColormapSpy = jest.fn();
        fetchMock.mock('*', url => {
            fetchColormapSpy(url);
            return {
                body: [...viridis],
                status: 200
            };
        });

        const colormap = await getColormap('viridis', 'gl');

        expect(fetchColormapSpy).toHaveBeenCalledWith('https://api.colormaps.io/colormap/viridis?format=gl');
        expect(colormap).toEqual(viridis);
    });

    it('should return [] if fetch fails', async () => {
        fetchMock.mock('*', {
            throws: true,
            status: 404
        });

        const items = await getColormap('viridis');
        expect(items).toEqual([]);
    });
});
