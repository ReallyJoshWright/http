const wsStatusElement = document.getElementById('ws-status');
let ws;

function connectWebSocket() {
    ws = new WebSocket('ws://localhost:3000/ws');

    ws.onopen = function() {
        console.log('WebSocket connected!');
        wsStatusElement.textContent = 'WebSocket Status: Connected!';
        wsStatusElement.className = 'status connected';
    };

    ws.onmessage = function(event) {
        console.log('Message from server:', event.data);
        if (event.data === 'reload') {
            console.log('Reloading page...');
            ws.close();
            window.location.reload();
        }
    };

    ws.onclose = function(event) {
        console.log(
            'WebSocket disconnected:',
            event.reason,
            event.code
        );

        wsStatusElement.textContent = 'WebSocket Status: ' +
            'Disconnected. Reconnecting...';
        wsStatusElement.className = 'status disconnected';

        if (event.code !== 1000 && event.code !== 1001) {
            setTimeout(connectWebSocket, 3000);
        }
    };

    ws.onerror = function(error) {
        console.error('WebSocket error:', error);
        wsStatusElement.textContent = 'WebSocket Status: Error. ' +
            'Reconnecting...';
        wsStatusElement.className = 'status error';
    };
}

connectWebSocket();
