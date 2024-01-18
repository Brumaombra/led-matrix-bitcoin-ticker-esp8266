const ESPIP = "http://localhost:3000";

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
            sendRequest(); // Send the request
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

// Send the request to the server
const sendRequest = () => {
    const ssid = $("#inputSSID").val();
    const password = $("#inputPassword").val();
    const request = { ssid, password };
    setBusy(true); // Busy on
    fetch(`${ESPIP}/connect`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(request)
    }).then(response => {
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