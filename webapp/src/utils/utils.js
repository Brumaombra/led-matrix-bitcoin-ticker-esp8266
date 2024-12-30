import router from '@/router';
import GlobalStore from '@/stores/global.js';

const ESPIP = 'http://192.168.21.174'; // ESP IP address
const connectionStatus = { WIFI_TRY: 2, WIFI_OK: 1, WIFI_KO: 0 }; // Connection status types

// Set the busy state of the app
export const setBusy = busy => {
    if (GlobalStore.busy === busy) return; // Exit if it's already equal
    GlobalStore.busy = busy;
};

// Navigate to a path
export const navTo = path => {
    const currentPath = router.currentRoute.value.path; // Get the current path
    if (currentPath === path) return; // Exit if it's the same path
    router.push(path); // Navigate to the path
};

// Load the networks
export const getNetworks = async () => {
    try {
        const response = await fetch(`${ESPIP}/networks`);
        const data = await response.json();
        return data.networks || [];
    } catch (error) {
        console.error(error);
        return [];
    }
};

// Get the settings
export const getSettings = async () => {
    try {
        const response = await fetch(`${ESPIP}/settings`);
        const data = await response.json();
        return data || {};
    } catch (error) {
        console.error(error);
        return {};
    }
};

// Save the settings
export const saveSettings = async settings => {
    try {
        const response = await fetch(`${ESPIP}/settings`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(settings)
        });
        const data = await response.json();
        return data.status === 'OK';
    } catch (error) {
        console.error(error);
        throw new Error('An error occurred while saving the settings');
    }
};

// Save the API key
export const saveApiKey = async apiKey => {
    try {
        const response = await fetch(`${ESPIP}/apiKey?apiKey=${apiKey}`);
        const data = await response.json();
        return data.status === 'OK';
    } catch (error) {
        console.error(error);
        throw new Error('An error occurred while saving the API key');
    }
};

// Connect to WiFi
export const connectToWiFi = async (ssid, password) => {
    try {
        const response = await fetch(`${ESPIP}/connect?ssid=${ssid}&password=${password}`);
        const data = await response.json();
        if (data.status === connectionStatus.WIFI_TRY) {
            return await checkWiFiConnectionPolling();
        }
        throw new Error('Failed to connect to WiFi');
    } catch (error) {
        console.error(error);
        throw new Error('An error occurred while connecting to the WiFi network');
    }
};

// Polling to check the WiFi connection
export const checkWiFiConnectionPolling = async () => {
    return new Promise((resolve, reject) => {
        const maxAttempts = 30; // 30 seconds timeout
        let attempts = 0;
        const polling = setInterval(async () => {
            try {
                attempts++;
                if (attempts > maxAttempts) {
                    clearInterval(polling);
                    reject(new Error('Connection timeout'));
                    return;
                }

                // Check the connection status
                const response = await fetch(`${ESPIP}/checkConnection`);
                const data = await response.json();
                switch (data.status) {
                    case connectionStatus.WIFI_TRY:
                        return; // Keep polling
                    case connectionStatus.WIFI_OK:
                        clearInterval(polling);
                        resolve(true);
                        return;
                    case connectionStatus.WIFI_KO:
                        clearInterval(polling);
                        reject(new Error('Connection failed'));
                        return;
                    default:
                        throw new Error('Unknown status');
                }
            } catch (error) {
                clearInterval(polling);
                reject(new Error('An error occurred while checking the WiFi connection'));
            }
        }, 1000);
    });
};