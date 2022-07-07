const { Server } = require("node-osc");
const robot = require("robotjs");

const port = 3333;

const oscServer = new Server(port, "localhost");

let blocked = false;

oscServer.on("listening", () => {
  console.log("OSC Server is listening.");
});

// Receive OSC/TUIO Bundles

oscServer.on("bundle", function (bundle) {
  bundle.elements.forEach((element, i) => {
    console.log(`Bundle: ${element}`);

    const type = element[1]; // TYP der Nachricht

    //Kieferstellung auslesen
    if (type === "set") {
      const id = element[3]; // MARKER ID
      const x = element[4]; // XPOS
      const y = element[5]; // YPOS
      const r = element[6]; //DREHUNG

      // MAUS ZU MARKER X/Y POSITION BEWEGEN
      //robot.moveMouseSmooth(x * 1440, y * 800);

      // Linke Seite -> Wurde Space noch nicht gedrückt? ist die erkannte id = 7? ist die Position kleiner als die Hälfte?
      if (blocked === false && id === 0 && x < 0.5) {
        robot.keyTap("space");
        blocked = true;
      }

      // Rechte Seite -> Wurde Space gedrückt? ist die erkannte id = 7? ist die Position größer als die Hälfte?
      else if (blocked === true && id === 0 && x > 0.5) {
        robot.keyTap("enter");
        blocked = false;
      }

      // Beispiel wenn ein anderer Marker einen Effekt haben soll
      if (id === 4) {
        robot.keyTap("enter");
      }
    }
  });
});
