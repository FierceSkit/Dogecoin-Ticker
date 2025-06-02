var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

var compiledCryptos;
var sortedSymbols;

var cCurrency = "";
var cCrypto = "";

var connected = false;
var firstConnect = true;
var optChanged = false;

const cryptoDictionary = [];

function onLoad(event) {

    // These functions will be performed when the page is first loaded on all clients.

    connected = false; // Set our connected bool to false

    setUpApp(); // Begin our app setup routine

}

function setUpApp() {

/*
    // Set our select list elements to disabled
    $('#target-select').attr('disabled', 'disabled');
    $('#crypto-select').attr('disabled', 'disabled');

    // Set Save Button to Disabled
    $('#saveChangesButton').attr('disabled', 'disabled');
*/

    // Set our select list elements to enabled
    $('#target-select').attr('enabled', 'enabled');
    $('#crypto-select').attr('enabled', 'enabled');

    // Set Save Button to Enabled
    $('#saveChangesButton').attr('enabled', 'enabled');

    parseSymbols(); // Fetch our data from API and parse JSON response

    // If we have the data, start our websocket connection to ESP8266
    if (cryptoDictionary != null) {
        initWebSocket();
    }
}

function initWebSocket() {
    connected = false;
    console.log('Attempting to open a WebSocket connection...');

    $('#saveChangesButton').attr('disabled', 'disabled');
    $('#saveChangesButton').removeClass("btn-primary btn-danger").addClass("btn-warning");
    $('#saveChangesButton').html("Connecting...")

    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {

    connected = true;
    console.log('WebSocket Open');

    websocket.send("getCurrentStates\0");

    $('#saveChangesButton').removeClass("btn-warning btn-danger").addClass("btn-success");
    $('#saveChangesButton').html("Save Changes");
    //  $('#saveChangesButton').removeAttr('true');
}

function onClose(event) {
    connected = false;
    console.log('WebSocket Closed');
    setTimeout(initWebSocket, 2000);
    $('#saveChangesButton').attr('true', 'true');
    $('#saveChangesButton').removeClass("btn-primary btn-warning").addClass("btn-danger");
    $('#saveChangesButton').html("Can't Connect to Device")
}

function onMessage(event) {

    var jsonData = JSON.parse(event.data);

    console.log("===============================\nJSON Message: " + JSON.stringify(jsonData));

    for (i in jsonData.states) {

        var sender = jsonData.states[i].sender;
        var activeCrypto = jsonData.states[i].currentCrypto;
        var activeTarget = jsonData.states[i].currentCurrency;

        console.log("Sender: " + sender);

        if (sender == "esp8266") {

            if (cryptoDictionary == null) {

                parseSymbols(activeCrypto, activeTarget);

            } else {

                cCrypto = activeCrypto;
                console.log("Current Crypto: " + activeCrypto);
                $('#active-crypto').html(activeCrypto)

                cCurrency = activeTarget;
                $('#active-target').html(activeTarget)
                console.log("Current Currency: " + activeTarget);

            }
        }
    }
    console.log(event.data);
}

function updateSelectList(element) {

    //console.log("Element Changed: ", element.id)

    if (element.id == 'crypto-select') {

        var availableOptions = cryptoDictionary[element.value];

        console.log("New Available Options: ", availableOptions)

        updateCurrencies(availableOptions, true);

    }

    optChanged = true;

    if (connected) $('#saveChangesButton').removeAttr('disabled');
}

// Send our changes to server
function saveChanges() {

    var targetCrypto = $('#crypto-select').val().toUpperCase();

    var targetCurrency = $('#target-select').val().toUpperCase();

    console.log("Saving Changes!");
    console.log("New Crypto", targetCrypto, "will be shown in", targetCurrency)

    // Build our JSON to send to ESP8266
    var x = ('{"states":[{"sender":"client","currentCurrency":' + '"' + targetCurrency + '","currentCrypto": "' + targetCrypto + '"}]}\0');

    optChanged = false;

    // Set Save Button to Disabled
    $('#saveChangesButton').attr('disabled', 'disabled');

    // Send the data.
    websocket.send(x);

}

async function getSupportedSymbols() {

    let endpoint = 'https://api.sandbox.gemini.com/v1/symbols';

    try {

        let result = await fetch(endpoint);
        return await result.json();

    } catch (error) {
        console.log("Fetch Symbols Error: ", error);
    }
}

async function parseSymbols(curCrypto, curTarget) {

    // Symbols sample: {"btcusd","ethusd","shibbtc","dogeeth", ... }

    // Set() with filters for target currencies | eg. if "btcusd" => find the usd
    //const targetCurrencies = new Set(["usd", "btc", "eth", "fil", "sgd", "gbp"]);
    var targetCurrencies = ["usd", "btc", "doge", "eth", "fil", "sgd", "gbp", "eur", "dai", "bch", "ltc"]

    // Sets our Symbols variable once we receive the data from the API
    let symbols = await getSupportedSymbols();

    /* 
    // Takes the cryptopairs from our API data and strips the currency from the end
    // to give us our cryptocoin symbol. Then takes each cryptocoin and gets the
    // stripped currency and puts it into an array and adds it to dictionary
    // so we can show the available conversions in the select lists
    */

    const p = crypto => {

        const foundCryptos = targetCurrencies.find(str => crypto.endsWith(str));
        const cryptos = !foundCryptos ? crypto : crypto.slice(0, -foundCryptos.length);

        const foundCurrencies = symbols.find(str => crypto.startsWith(cryptos));
        const currencies = !foundCurrencies ? crypto : crypto.slice(0, foundCryptos.length);

        if (cryptos in cryptoDictionary) {

            cryptoDictionary[cryptos].add(foundCryptos);

        } else {

            cryptoDictionary[cryptos] = new Set([foundCryptos]);

        }

        //console.log(cryptoDictionary);

        return cryptos;
    }

    // Maps the Symbols and puts them in a Set
    compiledCryptos = new Set(symbols.map(p));

    // Creates an array from the Set and sort it
    sortedSymbols = Array.from(compiledCryptos).sort();

    // Creates cryptocoin selectlist option for each symbol in our Array
    if (curCrypto == null) updateCryptos(sortedSymbols, true);
    else updateCryptos(sortedSymbols, false);

    // Creates convert-to selectlist option for each symbol in our targetCurrencies array
    if (curTarget == null) updateCurrencies(targetCurrencies.sort(), true);
    else updateCurrencies(targetCurrencies.sort(), false);

    console.log("Data Parsed:", cryptoDictionary);
}

function updateCurrencies(array, clearList) {

    if (firstConnect) {
        // Creates convert-to selectlist option for each symbol in our targetCurrencies array
        array.forEach(symbol => {
            $('#target-select').append($('<option>')
                .val(symbol)
                .html(symbol.toUpperCase())
            );
        });

        if (connected) {
            $('#crypto-select').removeAttr('disabled');
        }

        // Update our currenty selected cryptocurrency.
        $('#target-select').val(cCurrency);

        // set firstConnect to false
        firstConnect = false;

    } else {

        if (clearList) {
            $('#target-select').empty();
        }
        // Creates convert-to selectlist option for each symbol in our targetCurrencies array
        array.forEach(symbol => {
            $('#target-select').append($('<option>')
                .val(symbol)
                .html(symbol.toUpperCase())
            );
        });

        $('#target-select').removeAttr('disabled');
    }
}

function updateCryptos(array, clearList) {

    if (firstConnect) {

        // Creates convert-to selectlist option for each symbol in our targetCurrencies array
        array.forEach(symbol => {
            $('#crypto-select').append($('<option>')
                .val(symbol)
                .html(symbol.toUpperCase())
            );
        })

        // Update our currenty selected cryptocurrency.
        $('#crypto-select').val(cCrypto);

        // Remove disabled attribute
        $('#crypto-select').removeAttr('disabled');

    } else {

        if (clearList) {
            $('#crypto-select').empty();
        }

        // Creates convert-to selectlist option for each symbol in our targetCurrencies array
        array.forEach(symbol => {
            $('#crypto-select').append($('<option>')
                .val(symbol)
                .html(symbol.toUpperCase())
            );
        })
    }
}