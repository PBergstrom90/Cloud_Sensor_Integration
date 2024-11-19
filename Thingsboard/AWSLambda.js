const axios = require('axios');
const dotenv = require('dotenv');

// Load environment variables from .env file
dotenv.config();

// Environment variables
const AWS_FUNCTION_URL = process.env.AWS_FUNCTION_URL;
const AWS_AUTH_TOKEN = process.env.AWS_AUTH_TOKEN;
const THINGSBOARD_BASE_URL = process.env.THINGSBOARD_BASE_URL;
const THINGSBOARD_ACCESS_TOKEN = process.env.THINGSBOARD_ACCESS_TOKEN;
const POLL_INTERVAL_MS = 60000;

// Validate environment variables
if (!AWS_FUNCTION_URL || !AWS_AUTH_TOKEN || !THINGSBOARD_BASE_URL || !THINGSBOARD_ACCESS_TOKEN) {
    console.error('Missing required environment variables. Please check your .env file.');
    process.exit(1);
}
const THINGSBOARD_TELEMETRY_URL = `${THINGSBOARD_BASE_URL}/api/v1/${THINGSBOARD_ACCESS_TOKEN}/telemetry`;
const AWS_AUTH_HEADERS = {
    'Content-Type': 'application/json',
    'Authorization': AWS_AUTH_TOKEN
};

// Function to fetch telemetry data from AWS Lambda and send it to ThingsBoard
async function pollDeviceTelemetry(deviceID) {
    try {
        // Fetch latest telemetry data from AWS Lambda for the given deviceID
        const lambdaResponse = await axios.get(`${AWS_FUNCTION_URL}?deviceID=${deviceID}`, { headers: AWS_AUTH_HEADERS });
        const telemetryData = lambdaResponse.data;

        // Validate telemetry data
        if (!telemetryData || !telemetryData.temperature || !telemetryData.humidity) {
            console.warn(`Invalid telemetry data for deviceID ${deviceID}:`, telemetryData);
            return;
        }

        console.log(`Received telemetry data for ${deviceID}:`, telemetryData);

        // Prepare ThingsBoard telemetry payload
        const telemetryPayload = {
            ts: telemetryData.timestamp,
            values: {
                temperature: telemetryData.temperature,
                humidity: telemetryData.humidity
            }
        };

        // Send telemetry data to ThingsBoard
        await axios.post(THINGSBOARD_TELEMETRY_URL, telemetryPayload, {
            headers: { 'Content-Type': 'application/json' }
        });
        console.log(`Telemetry data sent to ThingsBoard for ${deviceID}`);
    } catch (error) {
        if (error.response) {
            console.error(`Error for deviceID ${deviceID}: ${error.response.status} - ${error.response.data}`);
        } else {
            console.error(`Error polling telemetry for deviceID ${deviceID}:`, error.message);
        }
    }
}

// Function to start polling telemetry data
async function startPolling() {
    console.log("Starting telemetry polling...");

    // Define the device ID (optional if you have only one device)
    const deviceID = '4827E2E8B1AC';

    // Poll telemetry data for the specified device at regular intervals
    setInterval(() => {
        pollDeviceTelemetry(deviceID);
    }, POLL_INTERVAL_MS);

    console.log(`Polling AWS Lambda Function URL every ${POLL_INTERVAL_MS / 1000} seconds...`);
}

startPolling();