const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const fs = require('fs');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const port = 5000;
const COM_PORT = 'COM21'; // Change this to your actual port

// Middleware
app.use(cors());
app.use(bodyParser.json());
app.use(express.static('public')); // Serve HTML

let latestData = {};

// Setup Serial
const serialPort = new SerialPort({ path: COM_PORT, baudRate: 115200 });
const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', (line) => {
  try {
    if (line.startsWith('{') && line.endsWith('}')) {
      const parsed = JSON.parse(line.trim());
      console.log('Parsed data:', parsed);

      latestData = parsed;

      fs.writeFileSync('data.json', JSON.stringify(parsed, null, 2));
    } else {
      console.log('Non-JSON line:', line);
    }
  } catch (err) {
    console.error('Failed to parse line:', line);
  }
});

// Endpoint to get latest data
app.get('/data', (req, res) => {
  res.json(latestData);
});

// Start server
app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
