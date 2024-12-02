# Cloud Sensor Integration

## Overview
A sensor project utilizing AWS IoT for secure MQTT data transmission, with data visualization and presentation handled by AWS Amplify and ThingsBoard. November 2024.

This project demonstrates how an ESP32 device collects data from a DHT11 sensor, transmits it securely to AWS IoT Core, and makes the data accessible for visualization through two parallel systems:

1. **AWS Amplify**: For real-time data monitoring and a primary user interface.
2. **ThingsBoard**: For a secondary dashboard using a Dockerized local instance.

The solution incorporates scalability, security best practices, and integrations with external APIs like the SMHI Weather API.

## Flowchart
![Application Flowchart](resources/Cloud_Sensor_Flowchart.png)

## Hardware

### Components:
- **ESP32-S3 DevKit**: Main microcontroller for transmitting sensor data.
- **DHT11 Sensor**: Captures temperature and humidity data.
- Additional supporting components like resistors, wires, and a breadboard.

### Connections:
- The DHT11 sensor is connected to the ESP32, which reads and transmits the data over MQTT.

## Software

### Tools and Technologies:
- **AWS IoT Core**: Handles MQTT communication and message routing.
- **AWS Lambda**: Processes data for storage and API requests.
- **Amazon DynamoDB**: Stores sensor data for long-term persistence.
- **Amazon S3**: Used for any cold storage requirements.
- **AWS Amplify**: Primary user-facing interface for data visualization.
- **ThingsBoard**: A Dockerized instance for additional data visualization.
- **SMHI Public API**: Provides external weather data.

### Codebase:
- ESP32 firmware written in C++ (Arduino framework) with MQTT and DHT11-sensor libraries.
- Custom Lambda-function for polling AWS for Thingsboard is written in Javascript for DynamoDB integration.

## Setup ESP32

### Prerequisites:
1. Install **PlatformIO** in Visual Studio Code.
2. Clone this repository.
3. Configure your Wi-Fi and MQTT broker settings in the ESP32 firmware.

### Steps:
1. Connect the DHT11 sensor to the ESP32 following the wiring diagram.
2. Flash the firmware to the ESP32 using PlatformIO.
3. Monitor the serial console to verify MQTT data transmission.

## Setup AWS

### Prerequisites:
- An active AWS account.
- Permissions to create IoT Core, Lambda, DynamoDB, and Amplify resources.

### Steps:
1. Set up **AWS IoT Core**:
   - Create a thing, certificate, and policy.
   - Configure an MQTT topic for the ESP32 to publish data.
2. Create **AWS Lambda Functions**:
   - One function to process incoming MQTT messages and store them in DynamoDB.
   - Another function to fetch data from DynamoDB and format it for Amplify.
3. Configure **Amazon DynamoDB**:
   - Create a table to store sensor data.
4. Set up **AWS Amplify**:
   - Refer to the linked [Cloud Sensor Integration Frontend-Amplify repository](https://github.com/PBergstrom90/Cloud_Sensor_Integration_Frontend-Amplify).

## Visualization AWS Amplify
AWS Amplify is the primary visualization platform for this project. All the necessary setup files and instructions can be found in the dedicated repository:
[Cloud Sensor Integration Frontend-Amplify](https://github.com/PBergstrom90/Cloud_Sensor_Integration_Frontend-Amplify)

## Visualization ThingsBoard
1. Set up a Docker container for ThingsBoard.
2. Integrate the MQTT broker to visualize data locally.
3. The necessary setup files for ThingsBoard are included in this repository.

## Security
Security is a top priority in this project:
- MQTT communication is secured with TLS certificates.
- AWS IAM policies are used to restrict access to resources, and only grant relevant access when needed.
- Secrets and keys are managed securely in AWS through automatically generated certificates.
- Amplify and ThingsBoard have role-based access controls to prevent unauthorized access.

## Scalability and Future Improvements
- **Multi-sensor support**: Adding more sensor types to the ESP32.
- **Edge computing**: Preprocessing data on the ESP32.
- **Enhanced analytics**: Leveraging AWS QuickSight for data insights.
- **Global deployment**: Using AWS CDK for infrastructure as code.