const ESPIP = ""; // http://localhost:3000
const connectionStatus = { WIFI_TRY: 2, WIFI_OK: 1, WIFI_KO: 0 }; // Connection status types

// Document ready
$(document).ready(() => {
    init(); // Init function
});

// Init function
const init = () => {
    addEventListeners(); // Add event listeners
    getNetworks(); // Get the networks
};

// Add the event listeners
const addEventListeners = () => {
    $("#inputPassword").keypress(event => {
        const keycode = event.keyCode || event.which;
        if (keycode == "13") // Enter pressed
            connectToWiFi(); // Send the request
    });
};

// Get the SSIDs
const getNetworks = () => {
    setBusy(true); // Busy on
    fetch(`${ESPIP}/networks`).then(response => {
        return response.json(); // Get the JSON
    }).then(data => {
        setBusy(false); // Busy off
        const networks = data.networks || []; // Networks list
        const select = $("#inputSSID");
        select.empty(); // Clear the select options
        if (networks.length === 0) {
            select.append($("<option>", {
                value: "",
                text: "No networks found"
            }));
            return;
        }

        // Add networks to the select
        networks.forEach(item => {
            select.append($("<option>", {
                value: item.ssid,
                text: `${item.ssid} (${item.signal} dBm)`
            }));
        });
    }).catch(error => {
        setBusy(false); // Busy off
        $("#errorModalMessage").text("An error occurred while getting the list of available WiFi networks"); // Set the error message
        $("#modalError").modal("show"); // Open the modal
        $("#inputSSID").prop("disabled", false); // Enable element
    });
};

// Try to connect to WiFi
const connectToWiFi = () => {
    const ssid = $("#inputSSID").val();
    const password = $("#inputPassword").val();
    setBusy(true); // Busy on
    const queryString = new URLSearchParams({ // Create the query strings with parameters
        ssid: ssid || "",
        password: password || ""
    }).toString();
    fetch(`${ESPIP}/connect?${queryString}`).then(response => {
        return response.json(); // Get the JSON
    }).then(data => {
        if (data.status == connectionStatus.WIFI_TRY) {
            checkWiFiConnectionPolling(); // Start polling
            return; // If the status is OK, return
        }
        throw new Error(); // Throw an error
    }).catch(error => {
        setBusy(false); // Busy off
        $("#errorModalMessage").text("An error occurred while connecting to the WiFi network"); // Set the error message
        $("#modalError").modal("show"); // Open the modal
    });
};

// Check the status of the connection
const checkWiFiConnectionPolling = () => {
    const polling = setInterval(() => {
        fetch(`${ESPIP}/checkConnection`).then(response => {
            return response.json(); // Get the JSON
        }).then(data => {
            if (data.status == connectionStatus.WIFI_TRY) {
                return; // If the status is TRY, return
            } else if (data.status == connectionStatus.WIFI_OK) {
                clearInterval(polling); // Stop polling
                setBusy(false); // Busy off
                $("#modalSuccess").modal("show"); // Open the modal
                return; // If the status is OK, return
            }
            throw new Error(); // Throw an error
        }).catch(error => {
            clearInterval(polling); // Stop polling
            setBusy(false); // Busy off
            $("#errorModalMessage").text("An error occurred while connecting to the WiFi network"); // Set the error message
            $("#modalError").modal("show"); // Open the modal
        });
    }, 2000); // Every 2 seconds
};

// Busy
const setBusy = busy => {
    if (busy)
        $("#fullScreenBusy").fadeIn(100);
    else
        $("#fullScreenBusy").fadeOut(100);
};