const ESPIP = ""; // http://localhost:3000
const connectionStatus = { WIFI_TRY: 2, WIFI_OK: 1, WIFI_KO: 0 }; // Connection status types

// Document ready
$(document).ready(() => {
    setTimeout(() => {
        init(); // Init function
    }, 3000); // Wait 3 seconds (To not overload the ESP)
});

// Init function
const init = () => {
    addEventListeners(); // Add event listeners
    Promise.allSettled([ // Call the pomises
        new Promise((resolve, reject) => { getNetworks(resolve, reject); }), // Get the networks
        new Promise((resolve, reject) => { getValuesSettings(resolve, reject); }) // Get the visibility settings
    ]).then(results => {
        setBusy(false); // Busy off
    });
};

// Open the selected page
const navToPage = page => {
    if ($(window).width() <= 991) $(".navbar-toggler").click(); // If on mobile close the navbar
    $("#navLinkWifi").removeClass("active"); // Deselect the manu item
    $("#navLinkApiKey").removeClass("active"); // Deselect the manu item
    $("#navLinkValues").removeClass("active"); // Deselect the manu item
    $("#pageSectionWifi").removeClass("show"); // Hide the page
    $("#pageSectionApiKey").removeClass("show"); // Hide the page
    $("#pageSectionValues").removeClass("show"); // Hide the page
    switch (page) {
        case "WIFI":
            $("#navLinkWifi").addClass("active"); // Select the manu item
            $("#pageSectionWifi").addClass("show"); // Show the page
            break;
        case "API":
            $("#navLinkApiKey").addClass("active"); // Select the manu item
            $("#pageSectionApiKey").addClass("show"); // Show the page
            break;
        case "VALUES":
            $("#navLinkValues").addClass("active"); // Select the manu item
            $("#pageSectionValues").addClass("show"); // Show the page
            break;
    }
};

/**************************** WIFI SETTINGS ****************************/

// Add the event listeners
const addEventListeners = () => {
    $("#inputPassword").keypress(event => {
        const keycode = event.keyCode || event.which;
        if (keycode == "13") // Enter pressed
            connectToWiFi(); // Send the request
    });
};

// Get the SSIDs
const getNetworks = (successCallback, errorCallback) => {
    !successCallback && setBusy(true); // Busy on only if there's no callback
    fetch(`${ESPIP}/networks`).then(response => {
        return response.json(); // Get the JSON
    }).then(data => {
        successCallback && successCallback(data); // If success call the function
        !successCallback && setBusy(false); // Busy off only if there's no callback
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
        errorCallback && errorCallback(error); // If error call the function
        !errorCallback && setBusy(false); // Busy off only if there's no callback
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
                $("#successModalMessage").text("You have successfully connected the device to the Wi-Fi network! The access point will be disabled. Enjoy the Bitcoin ticker!"); // Set the success message
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
    }, 1000); // Every 1 second
};

/**************************** API KEY SETTINGS ****************************/

// Save the API key
const saveApiKey = () => {
    const apiKey = $("#inputApiKey").val();
    if (!apiKey) { // Check if the API key is empty
        $("#errorModalMessage").text("Insert the API key"); // Set the error message
        $("#modalError").modal("show"); // Open the modal
        return; // If the API key is empty, return
    }
    setBusy(true); // Busy on
    const queryString = new URLSearchParams({ // Create the query strings with parameters
        apiKey: apiKey || ""
    }).toString();
    fetch(`${ESPIP}/apiKey?${queryString}`).then(response => {
        return response.json(); // Get the JSON
    }).then(data => {
        setBusy(false); // Busy off
        $("#inputApiKey").val(""); // Empty the field
        $("#successModalMessage").text("API key saved successfully!"); // Set the success message
        $("#modalSuccess").modal("show"); // Open the modal
    }).catch(error => {
        setBusy(false); // Busy off
        $("#errorModalMessage").text("An error occurred while saving the API key"); // Set the error message
        $("#modalError").modal("show"); // Open the modal
    });
};

/**************************** DISPLAY VALUES SETTINGS ****************************/

// Get the values
const getValuesSettings = (successCallback, errorCallback) => {
    fetch(`${ESPIP}/valuesSettings`).then(response => {
        return response.json(); // Get the JSON
    }).then(data => {
        successCallback && successCallback(data); // If success call the function
        $("#selectCurrentPrice").val(data.currentPrice || "Y").trigger("change"); // Set the visibility
        $("#selectPriceChange").val(data.priceChange || "Y").trigger("change"); // Set the visibility
        $("#selectMarketCap").val(data.marketCap || "Y").trigger("change"); // Set the visibility
        $("#selectDailyHighLow").val(data.dailyHighLow || "Y").trigger("change"); // Set the visibility
        $("#selectYearHighLow").val(data.yearHighLow || "Y").trigger("change"); // Set the visibility
        $("#selectOpenPrice").val(data.openPrice || "Y").trigger("change"); // Set the visibility
        $("#selectVolume").val(data.volume || "Y").trigger("change"); // Set the visibility
        $("#selectBitcoinMined").val(data.bitcoinMined || "Y").trigger("change"); // Set the visibility
        $("#selectFormatType").val(data.formatType || "US").trigger("change"); // Set the visibility
    }).catch(error => {
        errorCallback && errorCallback(error); // If error call the function
        $("#errorModalMessage").text("An error occurred while getting the visibility of the values"); // Set the error message
        $("#modalError").modal("show"); // Open the modal
    });
};

// Save the visibility of the values
const saveValuesSetting = () => {
    setBusy(true); // Busy on
    const queryString = new URLSearchParams({ // Create the query strings with parameters
        currentPrice: $("#selectCurrentPrice").val() || "Y",
        priceChange: $("#selectPriceChange").val() || "Y",
        marketCap: $("#selectMarketCap").val() || "Y",
        dailyHighLow: $("#selectDailyHighLow").val() || "Y",
        yearHighLow: $("#selectYearHighLow").val() || "Y",
        openPrice: $("#selectOpenPrice").val() || "Y",
        volume: $("#selectVolume").val() || "Y",
        bitcoinMined: $("#selectBitcoinMined").val() || "Y",
        formatType: $("#selectFormatType").val() || "US"
    }).toString();
    fetch(`${ESPIP}/saveValuesSettings?${queryString}`).then(response => {
        return response.json(); // Get the JSON
    }).then(data => {
        setBusy(false); // Busy off
        $("#successModalMessage").text("Settings saved successfully!"); // Set the success message
        $("#modalSuccess").modal("show"); // Open the modal
    }).catch(error => {
        setBusy(false); // Busy off
        $("#errorModalMessage").text("An error occurred while saving the settings"); // Set the error message
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