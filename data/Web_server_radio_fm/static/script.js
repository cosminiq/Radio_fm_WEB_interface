// Funcție pentru a încărca datele CSV și a le afișa în tabel
function loadCSVData(data) {
    const tableBody = document.getElementById("frequencyTable").getElementsByTagName("tbody")[0];
    tableBody.innerHTML = ""; // Șterge datele existente

    data.forEach(row => {
        const tableRow = tableBody.insertRow();

        // Post
        const postCell = tableRow.insertCell();
        postCell.textContent = row[0];

        // Frecvență
        const freqCell = tableRow.insertCell();
        freqCell.textContent = row[1];

        // Descriere
        const descCell = tableRow.insertCell();
        descCell.textContent = row[2];

        // Buton
        const actionCell = tableRow.insertCell();
        const button = document.createElement("button");
        button.textContent = `Select ${row[1]}`;
        button.onclick = () => sendFrequency(row[1]);
        actionCell.appendChild(button);
    });
}

// Funcție pentru a trimite frecvența către ESP32 și a actualiza sliderul
function sendFrequency(frequency) {
    fetch(`/setFrequency?freq=${frequency}`)
        .then(response => {
            if (response.ok) {
                console.log(`Frecvența ${frequency} a fost trimisă.`);

                // Actualizăm valoarea sliderului și eticheta
                const frequencySlider = document.getElementById('frequencySlider');
                const frequencyLabel = document.getElementById('frequencyLabel');

                frequencySlider.value = parseFloat(frequency);
                frequencyLabel.textContent = `Frecvența: ${parseFloat(frequency).toFixed(1)} MHz`;
            } else {
                console.error("Eroare la trimiterea frecvenței.");
            }
        })
        .catch(error => console.error("Eroare:", error));
}

// Solicită datele CSV de la server
fetch('/getCSVData')
    .then(response => response.json())
    .then(data => loadCSVData(data))
    .catch(error => console.error("Eroare la încărcarea datelor CSV:", error));


// Funcție pentru a schimba volumul (folosită la apăsarea butoanelor)
function changeVolume(action) {
    let volumeSlider = document.getElementById("volumeSlider");
    let volumeLabel = document.getElementById("volumeLabel");

    let currentVolume = parseInt(volumeSlider.value);

    if (action === 'up' && currentVolume < 100) {
        currentVolume++;
    } else if (action === 'down' && currentVolume > 0) {
        currentVolume--;
    }

    volumeSlider.value = currentVolume;
    volumeLabel.textContent = "Volum: " + currentVolume;

    // Trimitem volumul la ESP32
    fetch(`/setVolume?volume=${currentVolume}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error("Eroare la schimbarea volumului:", error));
}


// Actualizare volum pe măsură ce slider-ul este mutat
document.getElementById("volumeSlider").addEventListener("input", function() {
    let volumeSlider = document.getElementById("volumeSlider");
    let volumeLabel = document.getElementById("volumeLabel");
    
    volumeLabel.textContent = "Volum: " + volumeSlider.value;

    // Trimitem volumul la ESP32
    fetch(`/setVolume?volume=${volumeSlider.value}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error("Eroare la schimbarea volumului:", error));
});


// Funcție pentru a comuta mute/unmute
function toggleMute() {
    fetch('/toggleMute')
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error("Eroare la comutarea mute/unmute:", error));
}

// Funcție pentru a schimba frecvența
function changeFrequency(direction) {
    fetch(`/setFrequency?direction=${direction}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error("Eroare la schimbarea frecvenței:", error));
}

////////////////////////////////////////////////////////////////
// Funcție pentru a schimba frecvența (folosită la apăsarea butoanelor)
function changeFrequency(direction) {
    const frequencySlider = document.getElementById('frequencySlider');
    let currentFrequency = parseFloat(frequencySlider.value);
    const step = 0.1; // Pasul pentru schimbarea frecvenței (0.1 MHz)

    if (direction === 'up' && currentFrequency < 108) {
        currentFrequency += step;
    } else if (direction === 'down' && currentFrequency > 87) {
        currentFrequency -= step;
    }

    frequencySlider.value = currentFrequency;
    document.getElementById('frequencyLabel').textContent = `Frecvența: ${currentFrequency.toFixed(1)} MHz`;

    // Trimitem frecvența la ESP32
    fetch(`/setFrequency?freq=${currentFrequency.toFixed(1)}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error("Eroare la setarea frecvenței:", error));
}


// Actualizăm eticheta și trimitem frecvența la ESP32 în timp real pe măsură ce slider-ul este mutat
document.getElementById('frequencySlider').addEventListener('input', function() {
    const sliderValue = parseFloat(this.value); // Obținem valoarea slider-ului
    const frequencyLabel = document.getElementById('frequencyLabel');

    // Actualizăm eticheta în timp real
    frequencyLabel.textContent = `Frecvența: ${sliderValue.toFixed(1)} MHz`;

    // Trimitem frecvența la ESP32
    fetch(`/setFrequency?freq=${sliderValue.toFixed(1)}`)
        .then(response => response.text())
        .then(data => console.log(data)) // Afișăm răspunsul de la ESP32 în consolă
        .catch(error => console.error("Eroare la setarea frecvenței:", error));
});

// Funcție pentru a adăuga un post de radio nou
document.getElementById("radioForm").addEventListener("submit", function(event) {
    event.preventDefault(); // Împiedică trimiterea formularului implicit
    addRadioPost(); // Apelează funcția pentru trimiterea datelor
});

document.getElementById("wifiForm").addEventListener("submit", function(event) {
    event.preventDefault(); // Împiedică trimiterea formularului implicit
    addWifiData(); // Apelează funcția pentru trimiterea datelor
});


// Funcție pentru a adăuga un post de radio nou
function addRadioPost() {
    const radioName = document.getElementById('radioName').value;
    const radioFrequency = document.getElementById('radioFrequency').value;
    const radioType = document.getElementById('radioType').value;

    const data = new FormData();
    data.append("radioName", radioName);
    data.append("radioFrequency", radioFrequency);
    data.append("radioType", radioType);

    fetch("/saveCSVData", {
        method: "POST",
        body: data
    })
    .then(response => response.text())
    .then(result => {
        showConfirmationMessage(); // Afișează mesajul de confirmare
    })
    .catch(error => {
        console.error('Eroare:', error);
        alert("Eroare la salvarea datelor!");
    });
}

// Funcție pentru a adăuga date Wi-Fi
function addWifiData() {
    const apartment = document.getElementById("apartment").value;
    const wifiPassword = document.getElementById("wifiPassword").value;

    if (apartment && wifiPassword) {
        fetch("/saveCSVData_wifi", {
            method: "POST",
            headers: {
                "Content-Type": "application/x-www-form-urlencoded"
            },
            body: `apartment=${encodeURIComponent(apartment)}&wifiPassword=${encodeURIComponent(wifiPassword)}`
        })
        .then(response => {
            if (response.ok) {
                showConfirmationMessage(); // Afișează mesajul de confirmare
            } else {
                alert("Eroare la salvarea datelor!");
            }
        })
        .catch(error => {
            console.error("Eroare:", error);
            alert("Eroare la salvarea datelor!");
        });
    } else {
        alert("Te rog să completezi toate câmpurile!");
    }
}

// Funcție pentru afișarea mesajului de confirmare
function showConfirmationMessage() {
    const messageElement = document.getElementById("confirmationMessage");
    messageElement.style.display = "block"; // Afișează mesajul

    setTimeout(() => {
        messageElement.style.display = "none"; // Ascunde mesajul după 5 secunde
    }, 5000);
}


// Funcție pentru a solicita datele de stare de la server
function fetchRadioStatus() {
    fetch("/getStatus")
        .then(response => response.text())
        .then(data => {
            // Exemplu de răspuns de la ESP32:
            // "You are tuned on 105.80 MHz STEREO [SNR: 13 dB, RSSI: 16 dBuV] [Status: FM] [RDS: Radio Station Name] [UTC: 14:30]"

            // Extrage datele din răspuns
            const frequencyMatch = data.match(/[\d.]+ MHz/);
            const modeMatch = data.match(/STEREO|MONO/);
            const snrMatch = data.match(/SNR: [\d.]+ dB/);
            const rssiMatch = data.match(/RSSI: [\d.]+ dBuV/);
            const statusMatch = data.match(/Status: \w+/);
            const rdsMatch = data.match(/RDS: ([^\[]+)/); // Extrage mesajul RDS
            const utcMatch = data.match(/UTC: (\d{2}:\d{2})/); // Extrage ora UTC

            // Verifică dacă datele există în răspuns
            const frequency = frequencyMatch ? frequencyMatch[0] : "N/A";
            const mode = modeMatch ? modeMatch[0] : "N/A";
            const snr = snrMatch ? snrMatch[0].replace("SNR: ", "") : "N/A";
            const rssi = rssiMatch ? rssiMatch[0].replace("RSSI: ", "") : "N/A";
            const status = statusMatch ? statusMatch[0].replace("Status: ", "") : "N/A";
            const rds = rdsMatch ? rdsMatch[1].trim() : "N/A"; // Mesajul RDS
            const utc = utcMatch ? utcMatch[1] : "N/A"; // Ora UTC

            // Actualizează tabelul cu noile valori
            document.getElementById("frequency").innerText = frequency;
            document.getElementById("mode").innerText = mode;
            document.getElementById("snr").innerText = snr;
            document.getElementById("rssi").innerText = rssi;
            document.getElementById("status").innerText = status;
            document.getElementById("rds").innerText = rds; // Afișează mesajul RDS
            document.getElementById("utc").innerText = utc; // Afișează ora UTC
        })
        .catch(error => {
            console.error("Eroare la obținerea statusului radio:", error);
        });
}

// Actualizează la fiecare 10 secunde
setInterval(fetchRadioStatus, 10000);

// Actualizează imediat ce se încarcă pagina
fetchRadioStatus();


