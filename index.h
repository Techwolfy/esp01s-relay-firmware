// HTML Index Page Resource
const char *HTML = R"EOF(
<html>
    <head>
        <title>ESP-01S Relay Controller</title>
        <link href='https://cdnjs.cloudflare.com/ajax/libs/bootswatch/4.3.1/darkly/bootstrap.min.css' rel='stylesheet'>
    </head>
    <script type='text/javascript'>
        function controlPin(pin, state) {
            var uri = '/control?pin=' + pin;
            if (state != undefined) {
                uri += '&state=' + state;
            }

            var xhr = new XMLHttpRequest();
            xhr.open('GET', uri);
            xhr.send();

            refreshStatus();
        }

        function refreshStatus() {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/status');
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4) {
                    if (xhr.status == 200) {
                        displayStatus(JSON.parse(xhr.response));
                    } else {
                        displayStatus();
                    }
                }
            }
            xhr.send();
        }

        function displayStatus(status) {
            var statusBadge = document.getElementById('statusBadge');
            if (status == undefined) {
                statusBadge.className = 'btn btn-danger';
                statusBadge.innerText = 'Disconnected';
                return;
            } else {
                statusBadge.className = 'btn btn-success';
                statusBadge.innerText = 'Connected';
            }

            var statusCard = document.getElementById('statusCard');
            var onHtml = '<span class=\'badge badge-success\'>ON</span>';
            var offHtml = '<span class=\'badge badge-danger\'>OFF</span>';
            var statusHtml = '';
            statusHtml += 'Relay state: ' + (status.relayActive ? onHtml : offHtml) + '<br>\n';
            statusHtml += 'LED state: ' + (status.ledActive ? onHtml : offHtml) + '<p>\n';
            statusHtml += 'Hostname: ' + status.hostName + '<br>\n';
            statusHtml += 'MAC address: ' + status.macAddress + '<br>\n';
            statusHtml += 'SSID: ' + status.networkSSID + '<br>\n';
            statusHtml += 'IP address: ' + status.ipAddress + '<br>\n';
            statusHtml += 'Subnet mask: ' + status.subnetMask + '<br>\n';
            statusHtml += 'Default gateway: ' + status.gatewayAddress + '<br>\n';

            statusCard.innerHTML = statusHtml;
        }

        setInterval(refreshStatus, 1000);
    </script>
    <body>
        <div class='navbar navbar-expand-lg navbar-dark bg-primary mb-3'>
            <div class='container'>
                <span class='navbar-brand'>ESP-01S Relay Controller</span>
                <div class='navbar-right'>
                    <span id='statusBadge' class='btn btn-danger' style='pointer-events: none;'>Disconnected</span>
                </div>
            </div>
        </div>
        <div class='container'>
            <div class='card mb-3'>
                <div class='card-body'>
                    <h4 class='card-title'>Status</h4>
                    <div id='statusCard' class='card-text'>Loading...</div>
                </div>
            </div>
            <div class='card mb-3'>
                <div class='card-body'>
                    <h4 class='card-title'>Control</h4>
                    <button class='btn btn-primary mr-3' onclick='controlPin(0);'>Toggle Relay</button>
                    <button class='btn btn-primary mr-3' onclick='controlPin(2);'>Toggle LED</button>
                </div>
            </div>
        </div>
    </body>
</html>
)EOF";
