const ESPIP = ""; // http://localhost:3000
let socket; // The web socket

// Document ready
$(document).ready(() => {
    init(); // Init function
});

// Init function
const init = () => {
    addEventListeners(); // Add event listeners
    connectToWebSocket(); // Connect to the web socket
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

// Connect to the web socket
const connectToWebSocket = () => {
    socket = new WebSocket(`ws://${window.location.hostname}/ws`); // Create the WebSocket
    socket.onopen = () => {
        console.log("WebSocket connection opened"); // Log the connection
    };
    socket.onclose = () => {
        console.log("WebSocket connection closed"); // Log the disconnection
    };
    socket.onmessage = event => {
        manageResponse(event.data); // Manage the response
    };
};

// Manage the different types of requests
const manageResponse = response => {
    switch (response) {
        case "WIFI_CON":
            console.log("WiFi connesso...");
            break;
        case "WIFI_DIS":
            console.log("WiFi disconnesso...");
            break;
        case "WIFI_TRY":
            console.log("Tentativo di connessione al WiFi...");
            break;
        default:
            // Default
            break;
    }
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

// Send the request to the server
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
        setBusy(false); // Busy off
        if (data.status === "OK") {
            $("#modalSuccess").modal("show"); // Open the modal
            return; // If the status is OK, return
        }
        throw new Error("Error"); // Throw an error if the status is not OK
    }).catch(error => {
        setBusy(false); // Busy off
        $("#errorModalMessage").text("An error occurred while connecting to the WiFi network"); // Set the error message
        $("#modalError").modal("show"); // Open the modal
    });
};

// Busy
const setBusy = busy => {
    if (busy)
        $("#fullScreenBusy").fadeIn(100);
    else
        $("#fullScreenBusy").fadeOut(100);
};