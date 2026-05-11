const { SerialPort } = require("serialport");
const readline = require("readline");

const PORT = "COM39";
const BAUDRATE = 115200;

const port = new SerialPort({
  path: PORT,
  baudRate: BAUDRATE,
});

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  prompt: "LED> ",
});

port.on("open", () => {
  console.log(`Verbunden mit ${PORT} @ ${BAUDRATE}`);
  console.log("Beispiele:");
  console.log("  r27  -> LED 27 rot");
  console.log("  y32  -> LED 32 gelb");
  console.log("  g1   -> LED 1 gruen");
  console.log("  o32  -> LED 32 aus");
  console.log("Beenden mit: exit");
  rl.prompt();
});

port.on("data", (data) => {
  process.stdout.write("\nArduino: " + data.toString());
  rl.prompt();
});

port.on("error", (err) => {
  console.error("SerialPort-Fehler:", err.message);
});

rl.on("line", (line) => {
  const command = line.trim();

  if (command === "exit" || command === "quit") {
    console.log("Beende...");
    rl.close();
    port.close();
    return;
  }

  if (command.length > 0) {
    port.write(command + "\n", (err) => {
      if (err) {
        console.error("Fehler beim Senden:", err.message);
      } else {
        console.log("Gesendet:", command);
      }
    });
  }

  rl.prompt();
});

rl.on("close", () => {
  if (port.isOpen) {
    port.close();
  }
  process.exit(0);
});
