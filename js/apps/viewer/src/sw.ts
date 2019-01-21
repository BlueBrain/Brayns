// tslint:disable:no-console
// In production, we register a service worker to serve assets from local cache.

// This lets the app load faster on subsequent visits in production, and gives it offline capabilities.
// However, it also means that developers (and users)
// will only see deployed updates on the 'N+1' visit to a page, since previously
// cached resources are updated in the background.

// To learn more about the benefits of this model, read https://goo.gl/KwvDNy.
// This link also includes instructions on opting out of this behavior.
import {dispatchNotification} from './common/events';
import {isProd} from './env';


const isLocalhost = Boolean(
    window.location.hostname === 'localhost' ||
    // [::1] is the IPv6 localhost address.
    window.location.hostname === '[::1]' ||
    // 127.0.0.1/8 is considered localhost for IPv4.
    window.location.hostname.match(/^127(?:\.(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3}$/)
);

export function register() {
    if (isProd() && 'serviceWorker' in navigator) {
        // The URL constructor is available in all browsers that support SW.
        const publicUrl = new URL(process.env.PUBLIC_URL!, window.location.toString());

        if (publicUrl.origin !== window.location.origin) {
            // Our service worker won't work if PUBLIC_URL is on a different origin from what our page is served on.
            // This might happen if a CDN is used to serve assets;
            // See https://github.com/facebookincubator/create-react-app/issues/2374
            return;
        }

        window.addEventListener('load', () => {
            const swUrl = `${process.env.PUBLIC_URL}/service-worker.js`;

            if (!isLocalhost) {
                // Is not local host. Just register service worker
                registerValidSW(swUrl);
            } else {
                // This is running on localhost. Lets check if a service worker still exists or not.
                checkValidServiceWorker(swUrl);
            }
        });
    }
}

function registerValidSW(swUrl: string) {
    navigator.serviceWorker
        .register(swUrl)
        .then(registration => {
            registration.onupdatefound = () => {
                const installingWorker = registration.installing;
                if (installingWorker) {
                    installingWorker.onstatechange = () => {
                        if (installingWorker.state === 'installed') {
                            if (navigator.serviceWorker.controller) {
                                // At this point, the old content will have been purged and the fresh content will have been added to the cache.
                                dispatchNotification('New content is available. Please refresh the page');
                            } else {
                                // At this point, everything has been precached.
                                dispatchNotification('App is cached and ready for offline use');
                            }
                        }
                    };
                }
            };
        })
        .catch(error => {
            console.error('Error during service worker registration:', error);
        });
}

function checkValidServiceWorker(swUrl: string) {
    // Check if the service worker can be found. If it can't reload the page.
    fetch(swUrl)
        .then(response => {
            // Ensure service worker exists, and that we really are getting a JS file.
            if (
                response.status === 404 ||
                response.headers.get('content-type')!.indexOf('javascript') === -1
            ) {
                // No service worker found. Probably a different app. Reload the page.
                navigator.serviceWorker.ready.then(registration => {
                    registration.unregister().then(() => {
                        window.location.reload();
                    });
                });
            } else {
                // Service worker found. Proceed as normal.
                registerValidSW(swUrl);
            }
        })
        .catch(() => {
            dispatchNotification('No network detected. App is running in offline mode');
        });
}

export function unregister() {
    if ('serviceWorker' in navigator) {
        navigator.serviceWorker.ready.then(registration => {
            registration.unregister();
        });
    }
}
